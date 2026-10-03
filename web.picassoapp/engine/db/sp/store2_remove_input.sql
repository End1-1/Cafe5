DROP FUNCTION IF EXISTS sf_store2_input_delete;
DELIMITER $$

CREATE FUNCTION sf_store2_input_delete(p_doc_uuid CHAR(36))
    RETURNS JSON
BEGIN
    DECLARE v_status INT;
    -- Probe without a lock first: FOR UPDATE on a missing PK value takes a gap lock.
    IF NOT EXISTS (SELECT 1 FROM store_document WHERE f_id = p_doc_uuid) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'document_not_found');
    END IF;
    SELECT f_status INTO v_status FROM store_document WHERE f_id = p_doc_uuid FOR UPDATE;
    IF v_status IS NULL THEN RETURN JSON_OBJECT('status', 1, 'msg', 'document_not_found'); END IF;

    IF v_status = 1 THEN
        SET sql_safe_updates = 0;

        -- Проверка на реальные продажи (кроме самого прихода)
        IF EXISTS (SELECT 1
                   FROM store_moves sm
                            JOIN store_stock ss ON sm.f_batch_id = ss.f_id
                   WHERE ss.f_doc = p_doc_uuid
                     AND sm.f_doc <> p_doc_uuid) THEN
            RETURN JSON_OBJECT('status', 2, 'msg', 'already_sold_cannot_delete');
        END IF;

        -- Откатываем схлопнутые минусы: снимаем ровно погашенное этим документом,
        -- минус мог быть закрыт частично или несколькими приходами.
        UPDATE store_stock ss
            JOIN (SELECT sm.f_batch_id, SUM(sm.f_qty_in) AS healed
                  FROM store_moves sm
                  WHERE sm.f_doc = p_doc_uuid
                    AND sm.f_qty_in > 0
                    AND sm.f_doc_row_id <> sm.f_batch_id
                  GROUP BY sm.f_batch_id) h ON h.f_batch_id = ss.f_id
        SET ss.f_qty_in     = ss.f_qty_in - h.healed,
            ss.f_qty_left   = ss.f_qty_left - h.healed,
            ss.f_price      = IF(ss.f_qty_in > 0, ss.f_price, 0),
            ss.f_doc        = IF(ss.f_qty_in > 0 AND ss.f_doc <> p_doc_uuid, ss.f_doc, NULL),
            ss.f_doc_row_id = IF(ss.f_doc IS NOT NULL, ss.f_doc_row_id, NULL);

        DELETE FROM store_stock WHERE f_doc = p_doc_uuid;
        DELETE FROM store_moves WHERE f_doc = p_doc_uuid;
    END IF;

    DELETE FROM store_user WHERE f_doc = p_doc_uuid;

    -- Reverse cash-float from purchase payment before deleting ops.
    BEGIN
        DECLARE done_rev INT DEFAULT FALSE;
        DECLARE rev_session_id INT DEFAULT 0;
        DECLARE rev_float_credit DECIMAL(14, 2) DEFAULT 0;
        DECLARE cur_rev CURSOR FOR
            SELECT f_session_id, COALESCE(SUM(f_credit), 0)
            FROM cash_operations
            WHERE f_order_id = p_doc_uuid
              AND f_session_id > 0
              AND f_affects_float = 1
            GROUP BY f_session_id;
        DECLARE CONTINUE HANDLER FOR NOT FOUND SET done_rev = TRUE;
        OPEN cur_rev;
        rev_loop:
        LOOP
            FETCH cur_rev INTO rev_session_id, rev_float_credit;
            IF done_rev THEN LEAVE rev_loop; END IF;
            UPDATE cash_session
            SET f_amount_expected = f_amount_expected + rev_float_credit
            WHERE f_id = rev_session_id;
        END LOOP;
        CLOSE cur_rev;
    END;

    DELETE FROM cash_operations WHERE f_order_id = p_doc_uuid;
    DELETE FROM cash_debts WHERE f_doc_uuid = p_doc_uuid;
    DELETE FROM b_clients_debts WHERE f_storedoc = p_doc_uuid;
    DELETE FROM store_document WHERE f_id = p_doc_uuid;
    RETURN JSON_OBJECT('status', 0, 'msg', 'ok');
END$$
DELIMITER ;