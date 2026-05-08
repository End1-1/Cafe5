DROP FUNCTION IF EXISTS sf_store2_input_delete;
DELIMITER $$

CREATE FUNCTION sf_store2_input_delete(p_doc_uuid CHAR(36))
    RETURNS JSON
BEGIN
    DECLARE v_status INT;
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

        -- Откатываем схлопнутые минусы (возвращаем им исходный отрицательный вид)
        UPDATE store_stock ss JOIN store_moves sm ON sm.f_batch_id = ss.f_id AND sm.f_doc = p_doc_uuid
        SET ss.f_qty_left   = -sm.f_qty_in,
            ss.f_qty_in     = 0,
            ss.f_price      = 0,
            ss.f_doc        = NULL,
            ss.f_doc_row_id = NULL
        WHERE ss.f_qty_in > 0
          AND ss.f_qty_left = 0;

        DELETE FROM store_stock WHERE f_doc = p_doc_uuid;
        DELETE FROM store_moves WHERE f_doc = p_doc_uuid;
    END IF;

    DELETE FROM store_user WHERE f_doc = p_doc_uuid;
    DELETE FROM store_document WHERE f_id = p_doc_uuid;
    RETURN JSON_OBJECT('status', 0, 'msg', 'ok');
END$$
DELIMITER ;