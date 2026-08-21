DROP FUNCTION IF EXISTS sf_store2_input_set_paid;
DELIMITER $$

-- Update paid/debt split on a posted store input without touching stock.
CREATE FUNCTION sf_store2_input_set_paid(params longtext)
    RETURNS longtext
BEGIN
    DECLARE doc_uuid char(36) COLLATE latin1_general_ci DEFAULT JSON_VALUE(params, '$.doc_uuid');
    DECLARE create_user int DEFAULT CAST(JSON_VALUE(params, '$.doc_create_user') AS UNSIGNED);
    DECLARE cashbox_id int DEFAULT CAST(JSON_VALUE(params, '$.cashbox_id') AS unsigned);
    DECLARE payment_type_id int DEFAULT CAST(JSON_VALUE(params, '$.payment_type_id') AS UNSIGNED);
    DECLARE currency_id int DEFAULT CAST(JSON_VALUE(params, '$.currency_id') AS unsigned);
    DECLARE paid_amount decimal(14, 2) DEFAULT CAST(JSON_VALUE(params, '$.paid_amount') AS decimal(14, 2));
    DECLARE doc_version int DEFAULT CAST(JSON_VALUE(params, '$.doc_version') AS UNSIGNED);

    DECLARE current_version int DEFAULT 0;
    DECLARE current_status int DEFAULT 0;
    DECLARE doc_sum decimal(14, 2) DEFAULT 0;
    DECLARE partner_id int DEFAULT 0;
    DECLARE doc_date datetime;
    DECLARE debt_amount decimal(14, 2) DEFAULT 0;
    DECLARE doc_data longtext;

    SET sql_safe_updates = 0;

    IF NOT EXISTS (SELECT 1 FROM store_document WHERE f_id = doc_uuid) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'document_not_found');
    END IF;

    SELECT f_version, f_status, f_sum, f_partner, f_doc_date, f_data
    INTO current_version, current_status, doc_sum, partner_id, doc_date, doc_data
    FROM store_document
    WHERE f_id = doc_uuid FOR UPDATE;

    IF (current_status <> 1) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'document_not_posted');
    END IF;

    IF (current_version <> doc_version) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'version_conflict');
    END IF;

    IF (paid_amount IS NULL OR paid_amount < 0) THEN
        SET paid_amount = 0;
    END IF;
    IF (paid_amount > doc_sum) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'paid_exceeds_sum');
    END IF;
    SET debt_amount = doc_sum - paid_amount;

    IF (paid_amount > 0 AND (IFNULL(cashbox_id, 0) <= 0 OR IFNULL(payment_type_id, 0) <= 0)) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'cashbox_required_for_paid');
    END IF;
    IF (debt_amount > 0 AND IFNULL(partner_id, 0) <= 0) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'partner_required_for_debt');
    END IF;

    DELETE FROM cash_operations WHERE f_order_id = doc_uuid;
    DELETE FROM cash_debts WHERE f_doc_uuid = doc_uuid;

    IF (paid_amount > 0) THEN
        INSERT INTO cash_operations (f_cashbox_id, f_order_id, f_user, f_operation_type, f_payment_type_id,
                                     f_datetime, f_credit, f_currency_id)
        VALUES (cashbox_id, doc_uuid, create_user, 3, payment_type_id, doc_date, paid_amount,
                IFNULL(currency_id, 1));
    END IF;
    IF (debt_amount > 0) THEN
        INSERT INTO cash_debts (f_date, f_partner, f_doc_type, f_doc_uuid, f_credit, f_debit, f_currency_id)
        VALUES (doc_date, partner_id, 1, doc_uuid, debt_amount, 0, IFNULL(currency_id, 1));
    END IF;

    SET doc_data = JSON_SET(
            IFNULL(doc_data, '{}'),
            '$.paid_amount', paid_amount,
            '$.cashbox_id', IFNULL(cashbox_id, 0),
            '$.payment_type_id', IFNULL(payment_type_id, 0),
            '$.currency_id', IFNULL(currency_id, 1)
        );

    UPDATE store_document
    SET f_data    = doc_data,
        f_version = f_version + 1
    WHERE f_id = doc_uuid;

    RETURN JSON_OBJECT('status', 0, 'version', current_version + 1, 'paid_amount', paid_amount);
END$$
DELIMITER ;
