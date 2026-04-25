DROP FUNCTION IF EXISTS sf_store2_output_delete;
DELIMITER $$

CREATE FUNCTION sf_store2_output_delete(p_doc_uuid CHAR(36))
    RETURNS JSON
BEGIN
    DECLARE v_status INT;
    SELECT f_status INTO v_status FROM store_document WHERE f_id = p_doc_uuid FOR UPDATE;
    IF v_status IS NULL THEN RETURN JSON_OBJECT('status', 1, 'msg', 'document_not_found'); END IF;

    IF v_status = 1 THEN
        SET sql_safe_updates = 0;

        -- Партия «минус», закрытая приходом: расход аннулируем — не восстанавливаем дыру (-1), а возвращаем
        -- покрытое количество на основную партию прихода (f_batch_id = f_doc_row_id в движении прихода),
        -- иначе остаётся 19 + (-1) = 18 вместо 20.
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

        -- Обычные партии (без погашения минуса приходом): вернуть списанное в f_qty_left.
        -- Делать до DELETE погашений, иначе hx пустой и к партии «минус» снова прибавится qty_out.
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

        -- Удаление минусов (только если они не привязаны к живому Приходу)
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
    RETURN JSON_OBJECT('status', 0, 'msg', 'ok');
END$$
DELIMITER ;