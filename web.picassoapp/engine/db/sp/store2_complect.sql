DROP FUNCTION IF EXISTS sf_store2_complect_delete;
DROP FUNCTION IF EXISTS sf_store2_complect;
DELIMITER $$

-- Complectation: one store_document (f_doc_type=4).
-- Materials FIFO write-off from store_out + finished goods input to store_in (stores may be equal).
CREATE FUNCTION sf_store2_complect(params longtext)
    RETURNS longtext
BEGIN
    DECLARE current_version int DEFAULT 0;
    DECLARE current_status int DEFAULT 0;
    DECLARE doc_uuid char(36) COLLATE latin1_general_ci DEFAULT JSON_VALUE(params, '$.doc_uuid');
    DECLARE store_out int DEFAULT CAST(JSON_VALUE(params, '$.doc_store_out') AS UNSIGNED);
    DECLARE store_in int DEFAULT CAST(JSON_VALUE(params, '$.doc_store_in') AS UNSIGNED);
    DECLARE doc_date datetime DEFAULT JSON_VALUE(params, '$.doc_date');
    DECLARE new_status int DEFAULT CAST(JSON_VALUE(params, '$.doc_status') AS UNSIGNED);
    DECLARE complect_goods int DEFAULT CAST(JSON_VALUE(params, '$.complect_goods') AS UNSIGNED);
    DECLARE complect_qty decimal(14, 4) DEFAULT CAST(JSON_VALUE(params, '$.complect_qty') AS DECIMAL(14, 4));
    DECLARE complect_row_id char(36) COLLATE latin1_general_ci DEFAULT JSON_VALUE(params, '$.complect_row_id');
    DECLARE doc_user_id char(8) DEFAULT JSON_VALUE(params, '$.doc_user_id');

    DECLARE curr_item_id INT;
    DECLARE curr_qty_needed DECIMAL(14, 4);
    DECLARE curr_row_id CHAR(36);
    DECLARE done INT DEFAULT FALSE;

    DECLARE take_qty DECIMAL(14, 4);
    DECLARE total_cost DECIMAL(14, 2) DEFAULT 0;
    DECLARE unit_price DECIMAL(14, 2) DEFAULT 0;
    DECLARE p_qty_arrived DECIMAL(14, 4);

    DECLARE item_cursor CURSOR FOR
        SELECT item_id, qty, row_id
        FROM JSON_TABLE(params, '$.items[*]'
                        COLUMNS (
                            item_id INT PATH '$.item_id',
                            qty DECIMAL(14, 4) PATH '$.qty',
                            row_id CHAR(36) PATH '$.id'
                            )
             ) AS jt;

    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

    SET sql_safe_updates = 0;
    SET current_status = -1;

    SELECT f_version, f_status
    INTO current_version, current_status
    FROM store_document
    WHERE f_id = doc_uuid FOR UPDATE;

    IF (current_version > 0 AND current_version <> CAST(JSON_VALUE(params, '$.doc_version') AS UNSIGNED)) THEN
        RETURN JSON_COMPACT(JSON_OBJECT('status', 1, 'msg', 'version_error'));
    END IF;

    IF (IFNULL(complect_row_id, '') = '') THEN
        SET complect_row_id = UUID();
    END IF;

    -- ===== REVERSE previous posting (product first, then materials) =====
    IF (current_status = 1) THEN
        -- Product sold to other docs?
        IF EXISTS (SELECT 1
                   FROM store_moves sm
                            JOIN store_stock ss ON sm.f_batch_id = ss.f_id
                   WHERE ss.f_doc = doc_uuid
                     AND ss.f_qty_in > 0
                     AND sm.f_doc <> doc_uuid) THEN
            RETURN JSON_COMPACT(JSON_OBJECT('status', 2, 'msg', 'already_sold_cannot_edit'));
        END IF;

        -- Restore negatives closed by product input
        UPDATE store_stock ss
            JOIN store_moves sm ON sm.f_batch_id = ss.f_id AND sm.f_doc = doc_uuid AND sm.f_qty_in > 0
                AND sm.f_doc_row_id <> sm.f_batch_id
        SET ss.f_qty_left   = -sm.f_qty_in,
            ss.f_qty_in     = 0,
            ss.f_price      = 0,
            ss.f_doc        = NULL,
            ss.f_doc_row_id = NULL
        WHERE ss.f_qty_in > 0
          AND ss.f_qty_left = 0;

        -- Remove product stock batches of this document
        DELETE FROM store_stock WHERE f_doc = doc_uuid AND f_qty_in > 0;
        DELETE FROM store_moves WHERE f_doc = doc_uuid AND f_qty_in > 0;

        -- Reverse materials write-off (same as sf_store2_output reverse)
        UPDATE store_moves sm_main
            INNER JOIN (SELECT b.rid, SUM(b.qmerge) AS qmerge
                        FROM (SELECT am.f_doc_row_id                                     AS rid,
                                     LEAST(o.qty_out, ss.f_qty_in, am.abs_q)             AS qmerge
                              FROM store_stock ss
                                  JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                                        FROM store_moves
                                        WHERE f_doc = doc_uuid
                                        GROUP BY f_batch_id) o ON o.f_batch_id = ss.f_id
                                  JOIN (SELECT DISTINCT sm_in.f_batch_id
                                        FROM store_moves sm_in
                                        WHERE sm_in.f_qty_in > 0
                                          AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
                                  JOIN (SELECT f_batch_id, f_doc_row_id, SUM(f_qty_in) AS abs_q
                                        FROM store_moves
                                        WHERE f_qty_in > 0
                                          AND f_doc_row_id <> f_batch_id
                                        GROUP BY f_batch_id, f_doc_row_id) am ON am.f_batch_id = ss.f_id
                              WHERE ss.f_qty_left = 0
                                AND ss.f_qty_in > 0) b
                        GROUP BY b.rid) x ON sm_main.f_batch_id = x.rid AND sm_main.f_doc_row_id = x.rid
        SET sm_main.f_qty_in = sm_main.f_qty_in + x.qmerge,
            sm_main.f_total  = (sm_main.f_qty_in + x.qmerge) * sm_main.f_price;

        UPDATE store_stock sr
            INNER JOIN (SELECT b.rid, SUM(b.qmerge) AS qmerge
                        FROM (SELECT am.f_doc_row_id                                     AS rid,
                                     LEAST(o.qty_out, ss.f_qty_in, am.abs_q)             AS qmerge
                              FROM store_stock ss
                                  JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                                        FROM store_moves
                                        WHERE f_doc = doc_uuid
                                        GROUP BY f_batch_id) o ON o.f_batch_id = ss.f_id
                                  JOIN (SELECT DISTINCT sm_in.f_batch_id
                                        FROM store_moves sm_in
                                        WHERE sm_in.f_qty_in > 0
                                          AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
                                  JOIN (SELECT f_batch_id, f_doc_row_id, SUM(f_qty_in) AS abs_q
                                        FROM store_moves
                                        WHERE f_qty_in > 0
                                          AND f_doc_row_id <> f_batch_id
                                        GROUP BY f_batch_id, f_doc_row_id) am ON am.f_batch_id = ss.f_id
                              WHERE ss.f_qty_left = 0
                                AND ss.f_qty_in > 0) b
                        GROUP BY b.rid) x ON sr.f_id = x.rid
        SET sr.f_qty_in = sr.f_qty_in + x.qmerge,
            sr.f_qty_left = sr.f_qty_left + x.qmerge;

        UPDATE store_stock ss
            INNER JOIN (SELECT am.f_batch_id                                       AS nid,
                               SUM(LEAST(o.qty_out, ss.f_qty_in, am.abs_q))          AS qmerge
                        FROM store_stock ss
                            JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                                  FROM store_moves
                                  WHERE f_doc = doc_uuid
                                  GROUP BY f_batch_id) o ON o.f_batch_id = ss.f_id
                            JOIN (SELECT DISTINCT sm_in.f_batch_id
                                  FROM store_moves sm_in
                                  WHERE sm_in.f_qty_in > 0
                                    AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
                            JOIN (SELECT f_batch_id, f_doc_row_id, SUM(f_qty_in) AS abs_q
                                  FROM store_moves
                                  WHERE f_qty_in > 0
                                    AND f_doc_row_id <> f_batch_id
                                  GROUP BY f_batch_id, f_doc_row_id) am ON am.f_batch_id = ss.f_id
                        WHERE ss.f_qty_left = 0
                          AND ss.f_qty_in > 0
                        GROUP BY am.f_batch_id) d ON ss.f_id = d.nid
        SET ss.f_qty_in = ss.f_qty_in - d.qmerge,
            ss.f_qty_left = 0;

        UPDATE store_stock ss
            JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                  FROM store_moves
                  WHERE f_doc = doc_uuid
                  GROUP BY f_batch_id) sm ON sm.f_batch_id = ss.f_id
            LEFT JOIN (SELECT DISTINCT sm_in.f_batch_id
                       FROM store_moves sm_in
                       WHERE sm_in.f_qty_in > 0
                         AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
        SET ss.f_qty_left = ss.f_qty_left + sm.qty_out
        WHERE hx.f_batch_id IS NULL;

        DELETE sm_abs
        FROM store_moves sm_abs
            INNER JOIN (SELECT DISTINCT f_batch_id AS bid
                        FROM store_moves
                        WHERE f_doc = doc_uuid) om ON om.bid = sm_abs.f_batch_id
        WHERE sm_abs.f_qty_in > 0
          AND sm_abs.f_doc_row_id <> sm_abs.f_batch_id;
    END IF;

    DELETE FROM store_moves WHERE f_doc = doc_uuid;
    DELETE FROM store_stock WHERE f_doc = doc_uuid AND f_qty_in = 0 AND f_qty_left < 0;

    IF (current_status = 1) THEN
        DELETE ss
        FROM store_stock ss
        WHERE ss.f_qty_in = 0
          AND ss.f_qty_left = 0
          AND NOT EXISTS (SELECT 1 FROM store_moves sm WHERE sm.f_batch_id = ss.f_id);
    END IF;

    -- ===== Header =====
    INSERT INTO store_document (f_id, f_status, f_doc_type, f_doc_date, f_store_out, f_store_in, f_user_id, f_version,
                                f_sum, f_data)
    VALUES (doc_uuid, new_status, 4, doc_date, store_out, store_in, doc_user_id, 1, 0,
            JSON_EXTRACT(params, '$.doc_data'))
    ON DUPLICATE KEY UPDATE f_status    = VALUES(f_status),
                            f_doc_date  = VALUES(f_doc_date),
                            f_store_out = VALUES(f_store_out),
                            f_store_in  = VALUES(f_store_in),
                            f_user_id   = VALUES(f_user_id),
                            f_data      = VALUES(f_data),
                            f_version   = f_version + 1;

    DELETE FROM store_user WHERE f_doc = doc_uuid;
    INSERT INTO store_user (f_id, f_doc, f_item_id, f_qty, f_comment, f_row)
    SELECT jt.row_uuid, doc_uuid, jt.item_id, jt.qty, jt.comment, jt.row
    FROM JSON_TABLE(params, '$.items[*]'
                    COLUMNS (row_uuid CHAR(36) PATH '$.id', item_id INT PATH '$.item_id',
                        qty DECIMAL(14, 4) PATH '$.qty', comment varchar(255) PATH '$.comment', row int PATH '$.row')
         ) AS jt;

    -- ===== Post: materials out + product in =====
    IF (new_status = 1) THEN
        SET total_cost = 0;
        SET done = FALSE;
        OPEN item_cursor;
        item_loop:
        LOOP
            FETCH item_cursor INTO curr_item_id, curr_qty_needed, curr_row_id;
            IF done THEN
                LEAVE item_loop;
            END IF;

            fifo_block:
            BEGIN
                DECLARE b_done INT DEFAULT FALSE;
                DECLARE b_id CHAR(36);
                DECLARE b_qty_left DECIMAL(14, 4);
                DECLARE b_price DECIMAL(14, 2);

                DECLARE batch_cursor CURSOR FOR
                    SELECT f_id, f_qty_left, f_price
                    FROM store_stock
                    WHERE f_item_id = curr_item_id
                      AND f_store_id = store_out
                      AND f_qty_left > 0
                    ORDER BY f_batch_date ASC;

                DECLARE CONTINUE HANDLER FOR NOT FOUND SET b_done = TRUE;

                OPEN batch_cursor;
                batch_loop:
                LOOP
                    FETCH batch_cursor INTO b_id, b_qty_left, b_price;
                    IF b_done OR curr_qty_needed <= 0 THEN
                        LEAVE batch_loop;
                    END IF;

                    SET take_qty = LEAST(curr_qty_needed, b_qty_left);
                    UPDATE store_stock SET f_qty_left = f_qty_left - take_qty WHERE f_id = b_id;

                    INSERT INTO store_moves (f_id, f_doc, f_doc_row_id, f_batch_id, f_store_id, f_item_id, f_qty_out,
                                             f_price, f_total)
                    VALUES (UUID(), doc_uuid, curr_row_id, b_id, store_out, curr_item_id, take_qty, b_price,
                            take_qty * b_price);

                    SET curr_qty_needed = curr_qty_needed - take_qty;
                    SET total_cost = total_cost + (take_qty * b_price);
                END LOOP;
                CLOSE batch_cursor;
            END fifo_block;

            SET done = FALSE;

            IF curr_qty_needed > 0 THEN
                RETURN JSON_COMPACT(JSON_OBJECT(
                        'status', 6,
                        'msg', 'insufficient_stock',
                        'item_id', curr_item_id,
                        'shortage', curr_qty_needed
                                    ));
            END IF;
        END LOOP;
        CLOSE item_cursor;

        UPDATE store_user su
            JOIN (SELECT f_doc_row_id, SUM(f_total) AS row_total, SUM(f_qty_out) AS row_qty
                  FROM store_moves
                  WHERE f_doc = doc_uuid
                    AND f_qty_out > 0
                  GROUP BY f_doc_row_id) AS sm
            ON su.f_id = sm.f_doc_row_id
        SET su.f_price = sm.row_total / NULLIF(sm.row_qty, 0),
            su.f_total = sm.row_total
        WHERE su.f_doc = doc_uuid;

        SET unit_price = CASE WHEN complect_qty > 0 THEN total_cost / complect_qty ELSE 0 END;
        SET p_qty_arrived = complect_qty;

        -- Close negatives of finished goods on store_in
        neg_block:
        BEGIN
            DECLARE done_neg INT DEFAULT FALSE;
            DECLARE neg_id CHAR(36);
            DECLARE neg_qty_abs DECIMAL(14, 4);
            DECLARE cur_neg CURSOR FOR SELECT f_id, ABS(f_qty_left)
                                       FROM store_stock
                                       WHERE f_item_id = complect_goods
                                         AND f_store_id = store_in
                                         AND f_qty_left < 0
                                       ORDER BY f_batch_date ASC;
            DECLARE CONTINUE HANDLER FOR NOT FOUND SET done_neg = TRUE;
            OPEN cur_neg;
            neg_loop:
            LOOP
                FETCH cur_neg INTO neg_id, neg_qty_abs;
                IF done_neg OR p_qty_arrived <= 0 THEN
                    LEAVE neg_loop;
                END IF;
                SET neg_qty_abs = CASE WHEN p_qty_arrived >= neg_qty_abs THEN neg_qty_abs ELSE p_qty_arrived END;

                INSERT INTO store_moves (f_id, f_doc, f_doc_row_id, f_batch_id, f_store_id, f_item_id, f_qty_in,
                                         f_price, f_total)
                VALUES (UUID(), doc_uuid, complect_row_id, neg_id, store_in, complect_goods, neg_qty_abs, unit_price,
                        neg_qty_abs * unit_price);

                UPDATE store_stock
                SET f_qty_in     = neg_qty_abs,
                    f_qty_left   = 0,
                    f_price      = unit_price,
                    f_doc        = doc_uuid,
                    f_doc_row_id = complect_row_id
                WHERE f_id = neg_id;
                UPDATE store_moves SET f_price = unit_price, f_total = f_qty_out * unit_price WHERE f_batch_id = neg_id;

                SET p_qty_arrived = p_qty_arrived - neg_qty_abs;
            END LOOP;
            CLOSE cur_neg;
        END neg_block;

        IF p_qty_arrived > 0 THEN
            INSERT INTO store_moves (f_id, f_doc, f_doc_row_id, f_batch_id, f_store_id, f_item_id, f_qty_in, f_price,
                                     f_total)
            VALUES (UUID(), doc_uuid, complect_row_id, complect_row_id, store_in, complect_goods, p_qty_arrived,
                    unit_price, p_qty_arrived * unit_price);

            INSERT INTO store_stock (f_id, f_doc, f_doc_row_id, f_batch_date, f_store_id, f_item_id, f_qty_in,
                                     f_qty_left, f_price)
            VALUES (complect_row_id, doc_uuid, complect_row_id, doc_date, store_in, complect_goods, p_qty_arrived,
                    p_qty_arrived, unit_price);
        END IF;

        IF unit_price > 0 THEN
            UPDATE c_goods SET f_lastinputprice = unit_price WHERE f_id = complect_goods;
        END IF;
    END IF;

    UPDATE store_document SET f_sum = total_cost WHERE f_id = doc_uuid;

    RETURN JSON_COMPACT(JSON_OBJECT(
            'status', 0,
            'cost', total_cost,
            'unit_price', unit_price,
            'complect_row_id', complect_row_id,
            'version', IFNULL(current_version, 0) + 1
                        ));
END$$

CREATE FUNCTION sf_store2_complect_delete(p_doc_uuid CHAR(36))
    RETURNS longtext
BEGIN
    DECLARE v_status INT;
    SELECT f_status INTO v_status FROM store_document WHERE f_id = p_doc_uuid FOR UPDATE;
    IF v_status IS NULL THEN
        RETURN JSON_COMPACT(JSON_OBJECT('status', 1, 'msg', 'document_not_found'));
    END IF;

    IF v_status = 1 THEN
        SET sql_safe_updates = 0;

        IF EXISTS (SELECT 1
                   FROM store_moves sm
                            JOIN store_stock ss ON sm.f_batch_id = ss.f_id
                   WHERE ss.f_doc = p_doc_uuid
                     AND ss.f_qty_in > 0
                     AND sm.f_doc <> p_doc_uuid) THEN
            RETURN JSON_COMPACT(JSON_OBJECT('status', 2, 'msg', 'already_sold_cannot_delete'));
        END IF;

        -- Reverse product input
        UPDATE store_stock ss
            JOIN store_moves sm ON sm.f_batch_id = ss.f_id AND sm.f_doc = p_doc_uuid AND sm.f_qty_in > 0
                AND sm.f_doc_row_id <> sm.f_batch_id
        SET ss.f_qty_left   = -sm.f_qty_in,
            ss.f_qty_in     = 0,
            ss.f_price      = 0,
            ss.f_doc        = NULL,
            ss.f_doc_row_id = NULL
        WHERE ss.f_qty_in > 0
          AND ss.f_qty_left = 0;

        DELETE FROM store_stock WHERE f_doc = p_doc_uuid AND f_qty_in > 0;
        DELETE FROM store_moves WHERE f_doc = p_doc_uuid AND f_qty_in > 0;

        -- Reverse materials output
        UPDATE store_moves sm_main
            INNER JOIN (SELECT b.rid, SUM(b.qmerge) AS qmerge
                        FROM (SELECT am.f_doc_row_id                                     AS rid,
                                     LEAST(o.qty_out, ss.f_qty_in, am.abs_q)             AS qmerge
                              FROM store_stock ss
                                  JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                                        FROM store_moves
                                        WHERE f_doc = p_doc_uuid
                                        GROUP BY f_batch_id) o ON o.f_batch_id = ss.f_id
                                  JOIN (SELECT DISTINCT sm_in.f_batch_id
                                        FROM store_moves sm_in
                                        WHERE sm_in.f_qty_in > 0
                                          AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
                                  JOIN (SELECT f_batch_id, f_doc_row_id, SUM(f_qty_in) AS abs_q
                                        FROM store_moves
                                        WHERE f_qty_in > 0
                                          AND f_doc_row_id <> f_batch_id
                                        GROUP BY f_batch_id, f_doc_row_id) am ON am.f_batch_id = ss.f_id
                              WHERE ss.f_qty_left = 0
                                AND ss.f_qty_in > 0) b
                        GROUP BY b.rid) x ON sm_main.f_batch_id = x.rid AND sm_main.f_doc_row_id = x.rid
        SET sm_main.f_qty_in = sm_main.f_qty_in + x.qmerge,
            sm_main.f_total  = (sm_main.f_qty_in + x.qmerge) * sm_main.f_price;

        UPDATE store_stock sr
            INNER JOIN (SELECT b.rid, SUM(b.qmerge) AS qmerge
                        FROM (SELECT am.f_doc_row_id                                     AS rid,
                                     LEAST(o.qty_out, ss.f_qty_in, am.abs_q)             AS qmerge
                              FROM store_stock ss
                                  JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                                        FROM store_moves
                                        WHERE f_doc = p_doc_uuid
                                        GROUP BY f_batch_id) o ON o.f_batch_id = ss.f_id
                                  JOIN (SELECT DISTINCT sm_in.f_batch_id
                                        FROM store_moves sm_in
                                        WHERE sm_in.f_qty_in > 0
                                          AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
                                  JOIN (SELECT f_batch_id, f_doc_row_id, SUM(f_qty_in) AS abs_q
                                        FROM store_moves
                                        WHERE f_qty_in > 0
                                          AND f_doc_row_id <> f_batch_id
                                        GROUP BY f_batch_id, f_doc_row_id) am ON am.f_batch_id = ss.f_id
                              WHERE ss.f_qty_left = 0
                                AND ss.f_qty_in > 0) b
                        GROUP BY b.rid) x ON sr.f_id = x.rid
        SET sr.f_qty_in = sr.f_qty_in + x.qmerge,
            sr.f_qty_left = sr.f_qty_left + x.qmerge;

        UPDATE store_stock ss
            INNER JOIN (SELECT am.f_batch_id                                       AS nid,
                               SUM(LEAST(o.qty_out, ss.f_qty_in, am.abs_q))          AS qmerge
                        FROM store_stock ss
                            JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                                  FROM store_moves
                                  WHERE f_doc = p_doc_uuid
                                  GROUP BY f_batch_id) o ON o.f_batch_id = ss.f_id
                            JOIN (SELECT DISTINCT sm_in.f_batch_id
                                  FROM store_moves sm_in
                                  WHERE sm_in.f_qty_in > 0
                                    AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
                            JOIN (SELECT f_batch_id, f_doc_row_id, SUM(f_qty_in) AS abs_q
                                  FROM store_moves
                                  WHERE f_qty_in > 0
                                    AND f_doc_row_id <> f_batch_id
                                  GROUP BY f_batch_id, f_doc_row_id) am ON am.f_batch_id = ss.f_id
                        WHERE ss.f_qty_left = 0
                          AND ss.f_qty_in > 0
                        GROUP BY am.f_batch_id) d ON ss.f_id = d.nid
        SET ss.f_qty_in = ss.f_qty_in - d.qmerge,
            ss.f_qty_left = 0;

        UPDATE store_stock ss
            JOIN (SELECT f_batch_id, SUM(f_qty_out) AS qty_out
                  FROM store_moves
                  WHERE f_doc = p_doc_uuid
                  GROUP BY f_batch_id) sm ON sm.f_batch_id = ss.f_id
            LEFT JOIN (SELECT DISTINCT sm_in.f_batch_id
                       FROM store_moves sm_in
                       WHERE sm_in.f_qty_in > 0
                         AND sm_in.f_doc_row_id <> sm_in.f_batch_id) hx ON hx.f_batch_id = ss.f_id
        SET ss.f_qty_left = ss.f_qty_left + sm.qty_out
        WHERE hx.f_batch_id IS NULL;

        DELETE sm_abs
        FROM store_moves sm_abs
            INNER JOIN (SELECT DISTINCT f_batch_id AS bid
                        FROM store_moves
                        WHERE f_doc = p_doc_uuid) om ON om.bid = sm_abs.f_batch_id
        WHERE sm_abs.f_qty_in > 0
          AND sm_abs.f_doc_row_id <> sm_abs.f_batch_id;

        DELETE ss
        FROM store_stock ss
        WHERE ss.f_doc = p_doc_uuid
          AND ss.f_qty_in = 0
          AND NOT EXISTS (SELECT 1 FROM store_moves WHERE f_batch_id = ss.f_id AND f_doc <> p_doc_uuid);

        DELETE FROM store_moves WHERE f_doc = p_doc_uuid;

        DELETE ss
        FROM store_stock ss
        WHERE ss.f_qty_in = 0
          AND ss.f_qty_left = 0
          AND NOT EXISTS (SELECT 1 FROM store_moves sm WHERE sm.f_batch_id = ss.f_id);
    END IF;

    DELETE FROM store_user WHERE f_doc = p_doc_uuid;
    DELETE FROM store_document WHERE f_id = p_doc_uuid;
    RETURN JSON_COMPACT(JSON_OBJECT('status', 0, 'msg', 'ok'));
END$$
DELIMITER ;
