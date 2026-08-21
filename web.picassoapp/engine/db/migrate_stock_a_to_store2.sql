-- =============================================================================
-- Migrate stock: cafe5.a_store → valshin.store_* (posted input docs)
-- =============================================================================
-- Standalone / manual. Does NOT go through run_update.php.
--
-- Databases:
--   cafe5   — old warehouse (source: a_store)
--   valshin — new warehouse (target: store_document / store_user / store_stock / store_moves)
--
-- What it does:
--   • For each store with positive qty (SUM(f_qty*f_type) > 0), creates one
--     posted input document in valshin (doc_type=1, status=1).
--   • Qty = SUM(f_qty * f_type) from cafe5.a_store (NOT f_qtyleft).
--   • Price = weighted avg of all input rows (f_type=1) by f_qty; else 0.
--   • f_user_id = MIGRATE1, f_partner = NULL, comment = Migration from a_store.
--   • store_user.f_id = store_stock.f_id = store_moves.f_batch_id = same UUID
--     (charset latin1_general_ci).
--   • Re-runnable: deletes previous MIGRATE1 result in valshin, then migrates again.
--
-- How to run (MariaDB client; user needs SELECT on cafe5 + INSERT/DELETE on valshin):
--   mysql -u USER -p valshin < migrate_stock_a_to_store2.sql
--   -- or inside client:
--   USE valshin;
--   SOURCE /path/to/migrate_stock_a_to_store2.sql;
--
-- All permanent tables are fully qualified (cafe5.* / valshin.*), so the default
-- schema mainly affects where the function is created (valshin below).
--
-- Expected result JSON from the final SELECT:
--   {"status":0,"msg":"ok","docs":N,"lines":M,"cleared_docs":K}
--   {"status":0,"msg":"nothing_to_migrate","docs":0,"lines":0,"cleared_docs":K}
--   {"status":0,"msg":"a_store_missing"}
--   {"status":1,"msg":"store_stock_missing"}
-- =============================================================================

USE valshin;

DROP FUNCTION IF EXISTS sf_migrate_stock_a_to_store2;
DELIMITER $$

CREATE FUNCTION sf_migrate_stock_a_to_store2()
    RETURNS LONGTEXT
    NOT DETERMINISTIC
    MODIFIES SQL DATA
BEGIN
    DECLARE a_store_exists INT DEFAULT 0;
    DECLARE store_tbl_exists INT DEFAULT 0;
    DECLARE docs_created INT DEFAULT 0;
    DECLARE lines_created INT DEFAULT 0;
    DECLARE cleared_docs INT DEFAULT 0;

    SELECT COUNT(*)
    INTO store_tbl_exists
    FROM information_schema.tables
    WHERE table_schema = 'valshin'
      AND table_name = 'store_stock';

    IF store_tbl_exists = 0 THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'store_stock_missing');
    END IF;

    SELECT COUNT(*)
    INTO a_store_exists
    FROM information_schema.tables
    WHERE table_schema = 'cafe5'
      AND table_name = 'a_store';

    IF a_store_exists = 0 THEN
        RETURN JSON_OBJECT('status', 0, 'msg', 'a_store_missing');
    END IF;

    -- Remove previous migration result so the script can be re-run
    SELECT COUNT(*) INTO cleared_docs
    FROM valshin.store_document
    WHERE f_user_id = 'MIGRATE1';

    DELETE m
    FROM valshin.store_moves m
             INNER JOIN valshin.store_document d ON d.f_id = m.f_doc
    WHERE d.f_user_id = 'MIGRATE1';

    DELETE s
    FROM valshin.store_stock s
             INNER JOIN valshin.store_document d ON d.f_id = s.f_doc
    WHERE d.f_user_id = 'MIGRATE1';

    DELETE u
    FROM valshin.store_user u
             INNER JOIN valshin.store_document d ON d.f_id = u.f_doc
    WHERE d.f_user_id = 'MIGRATE1';

    DELETE FROM valshin.store_document WHERE f_user_id = 'MIGRATE1';

    DROP TEMPORARY TABLE IF EXISTS tmp_migrate_stock;
    DROP TEMPORARY TABLE IF EXISTS tmp_migrate_docs;
    DROP TEMPORARY TABLE IF EXISTS tmp_migrate_rows;

    CREATE TEMPORARY TABLE tmp_migrate_stock
    (
        store_id INT            NOT NULL,
        item_id  INT            NOT NULL,
        qty      DECIMAL(14, 4) NOT NULL,
        price    DECIMAL(14, 2) NOT NULL,
        PRIMARY KEY (store_id, item_id)
    );

    CREATE TEMPORARY TABLE tmp_migrate_docs
    (
        store_id INT                                              NOT NULL,
        doc_uuid CHAR(36) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
        doc_sum  DECIMAL(14, 2)                                   NOT NULL,
        PRIMARY KEY (store_id),
        UNIQUE KEY uk_doc (doc_uuid)
    );

    CREATE TEMPORARY TABLE tmp_migrate_rows
    (
        store_id INT                                              NOT NULL,
        item_id  INT                                              NOT NULL,
        qty      DECIMAL(14, 4)                                   NOT NULL,
        price    DECIMAL(14, 2)                                   NOT NULL,
        row_uuid CHAR(36) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
        row_num  INT                                              NOT NULL,
        doc_uuid CHAR(36) CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
        PRIMARY KEY (store_id, item_id),
        UNIQUE KEY uk_row (row_uuid)
    );

    -- Qty = SUM(f_qty*f_type); price = weighted avg of inputs (f_type=1) by f_qty
    INSERT INTO tmp_migrate_stock (store_id, item_id, qty, price)
    SELECT s.f_store,
           s.f_goods,
           SUM(s.f_qty * s.f_type),
           COALESCE(
                   SUM(CASE
                           WHEN s.f_type = 1 THEN s.f_qty * IFNULL(s.f_price, 0)
                           ELSE 0 END)
                       / NULLIF(SUM(CASE WHEN s.f_type = 1 THEN s.f_qty ELSE 0 END), 0),
                   0
           )
    FROM cafe5.a_store s
    GROUP BY s.f_store, s.f_goods
    HAVING SUM(s.f_qty * s.f_type) > 0;

    IF (SELECT COUNT(*) FROM tmp_migrate_stock) = 0 THEN
        DROP TEMPORARY TABLE IF EXISTS tmp_migrate_stock;
        DROP TEMPORARY TABLE IF EXISTS tmp_migrate_docs;
        DROP TEMPORARY TABLE IF EXISTS tmp_migrate_rows;
        RETURN JSON_OBJECT('status', 0, 'msg', 'nothing_to_migrate', 'docs', 0, 'lines', 0,
                           'cleared_docs', cleared_docs);
    END IF;

    -- One posted input document per store → valshin
    INSERT INTO tmp_migrate_docs (store_id, doc_uuid, doc_sum)
    SELECT store_id,
           CAST(UUID() AS CHAR(36) CHARACTER SET latin1) COLLATE latin1_general_ci,
           ROUND(SUM(qty * price), 2)
    FROM tmp_migrate_stock
    GROUP BY store_id;

    INSERT INTO tmp_migrate_rows (store_id, item_id, qty, price, row_uuid, row_num, doc_uuid)
    SELECT t.store_id,
           t.item_id,
           t.qty,
           t.price,
           CAST(UUID() AS CHAR(36) CHARACTER SET latin1) COLLATE latin1_general_ci,
           ROW_NUMBER() OVER (PARTITION BY t.store_id ORDER BY t.item_id),
           d.doc_uuid
    FROM tmp_migrate_stock t
             INNER JOIN tmp_migrate_docs d ON d.store_id = t.store_id;

    INSERT INTO valshin.store_document (f_id, f_user_id, f_status, f_doc_type, f_doc_date,
                                        f_store_in, f_sum, f_partner, f_version, f_data)
    SELECT d.doc_uuid,
           'MIGRATE1',
           1,
           1,
           NOW(),
           d.store_id,
           IFNULL(d.doc_sum, 0),
           NULL,
           1,
           JSON_OBJECT('comment', 'Migration from a_store')
    FROM tmp_migrate_docs d;

    INSERT INTO valshin.store_user (f_id, f_doc, f_item_id, f_qty, f_price, f_total, f_comment, f_row)
    SELECT r.row_uuid,
           r.doc_uuid,
           r.item_id,
           r.qty,
           r.price,
           ROUND(r.qty * r.price, 2),
           'Migration from a_store',
           r.row_num
    FROM tmp_migrate_rows r;

    INSERT INTO valshin.store_stock (f_id, f_doc, f_doc_row_id, f_batch_date, f_expiry_date,
                                     f_store_id, f_item_id, f_qty_in, f_qty_left, f_price)
    SELECT r.row_uuid,
           r.doc_uuid,
           r.row_uuid,
           NOW(),
           NULL,
           r.store_id,
           r.item_id,
           r.qty,
           r.qty,
           r.price
    FROM tmp_migrate_rows r;

    INSERT INTO valshin.store_moves (f_id, f_doc, f_doc_row_id, f_batch_id,
                                     f_store_id, f_item_id, f_qty_in, f_qty_out, f_price, f_total)
    SELECT CAST(UUID() AS CHAR(36) CHARACTER SET latin1) COLLATE latin1_general_ci,
           r.doc_uuid,
           r.row_uuid,
           r.row_uuid,
           r.store_id,
           r.item_id,
           r.qty,
           0,
           r.price,
           ROUND(r.qty * r.price, 2)
    FROM tmp_migrate_rows r;

    SELECT COUNT(*) INTO docs_created FROM tmp_migrate_docs;
    SELECT COUNT(*) INTO lines_created FROM tmp_migrate_rows;

    DROP TEMPORARY TABLE IF EXISTS tmp_migrate_stock;
    DROP TEMPORARY TABLE IF EXISTS tmp_migrate_docs;
    DROP TEMPORARY TABLE IF EXISTS tmp_migrate_rows;

    RETURN JSON_OBJECT('status', 0, 'msg', 'ok', 'docs', docs_created, 'lines', lines_created,
                       'cleared_docs', cleared_docs);
END$$

DELIMITER ;

-- Run migration now (standalone script)
SELECT sf_migrate_stock_a_to_store2() AS migrate_result;
