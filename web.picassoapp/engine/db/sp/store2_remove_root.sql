DROP FUNCTION IF EXISTS sf_store2_doc_delete;
DELIMITER $$

CREATE FUNCTION sf_store2_doc_delete(p_doc_uuid CHAR(36))
    RETURNS longtext
BEGIN
    DECLARE v_doc_type INT;
    DECLARE v_res longtext;

    SELECT f_doc_type
    INTO v_doc_type
    FROM store_document
    WHERE f_id = p_doc_uuid FOR UPDATE;

    IF v_doc_type IS NULL THEN
        RETURN JSON_COMPACT(JSON_OBJECT('status', 1, 'msg', 'document_not_found'));
    END IF;

    CASE v_doc_type
        WHEN 1 THEN SET v_res = sf_store2_input_delete(p_doc_uuid);
        WHEN 2 THEN SET v_res = sf_store2_output_delete(p_doc_uuid);
        WHEN 3 THEN SET v_res = sf_store2_move_delete(p_doc_uuid);
        WHEN 4 THEN SET v_res = sf_store2_complect_delete(p_doc_uuid);
        ELSE
            RETURN JSON_COMPACT(JSON_OBJECT('status', 3, 'msg', 'unknown_doc_type', 'doc_type', v_doc_type));
    END CASE;

    RETURN v_res;
END$$
DELIMITER ;
