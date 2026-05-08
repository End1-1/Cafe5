DROP FUNCTION IF EXISTS sf_store2_doc_delete;
DELIMITER $$

CREATE FUNCTION sf_store2_doc_delete(p_doc_uuid CHAR(36))
    RETURNS longtext
BEGIN
    DECLARE v_doc_type INT;
    DECLARE v_status INT;
    DECLARE v_res longtext;

    -- 1. Определяем, что это за зверь
    SELECT f_doc_type, f_status
    INTO v_doc_type, v_status
    FROM store_document
    WHERE f_id = p_doc_uuid FOR
    UPDATE;

    IF v_doc_type IS NULL THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'document_not_found');
    END IF;

    -- 2. Распределяем логику в зависимости от типа
    CASE v_doc_type
        -- ПРИХОД (Input)
        WHEN 1 THEN SET v_res = sf_store2_input_delete(p_doc_uuid);

        -- РАСХОД (Output)
        WHEN 2 THEN SET v_res = sf_store2_output_delete(p_doc_uuid);

        -- ПЕРЕМЕЩЕНИЕ (Transfer) - если оно у тебя есть
        WHEN 3 THEN -- Перемещение обычно удаляется как расход с одного склада
        -- и приход на другой. Если логика сложнее — напишем отдельно.
            SET v_res = sf_store2_input_delete(p_doc_uuid);

        ELSE RETURN JSON_OBJECT('status', 3, 'msg', 'unknown_doc_type');
        END CASE;

    RETURN v_res;
END$$
DELIMITER ;