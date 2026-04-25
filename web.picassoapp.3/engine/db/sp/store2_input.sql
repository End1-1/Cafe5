DROP FUNCTION IF EXISTS sf_store2_input;
DELIMITER $$

CREATE FUNCTION sf_store2_input(params longtext)
    RETURNS longtext
BEGIN
    DECLARE current_version int DEFAULT 0;
    DECLARE current_status int DEFAULT 0;
    DECLARE doc_date datetime DEFAULT JSON_VALUE(params, '$.doc_date');
    DECLARE new_status int DEFAULT CAST(JSON_VALUE(params, '$.doc_status') AS UNSIGNED);
    DECLARE store_in int DEFAULT CAST(JSON_VALUE(params, '$.doc_store_in') AS UNSIGNED);
    DECLARE doc_uuid char(36) COLLATE latin1_general_ci DEFAULT JSON_VALUE(params, '$.doc_uuid');
    DECLARE doc_user_id char(8) DEFAULT JSON_VALUE(params, '$.doc_user_id');
    DECLARE create_user int DEFAULT CAST(JSON_VALUE(params, '$.doc_create_user') AS UNSIGNED);
    DECLARE cashbox_id int DEFAULT CAST(JSON_VALUE(params, '$.cashbox_id') AS unsigned);
    DECLARE payment_type_id int DEFAULT CAST(JSON_VALUE(params, '$.payment_type_id') AS UNSIGNED);
    DECLARE doc_sum decimal(14, 2) DEFAULT CAST(JSON_VALUE(params, '$.doc_sum') AS decimal(14, 2));
    DECLARE currency_id int DEFAULT CAST(JSON_VALUE(params, '$.currency_id') AS unsigned);
    DECLARE partner_id int DEFAULT CAST(JSON_VALUE(params, '$.doc_partner') AS unsigned);

    DECLARE p_item_id INT;
    DECLARE p_qty_arrived DECIMAL(14, 4);
    DECLARE p_price DECIMAL(14, 2);
    DECLARE p_row_uuid CHAR(36);
    DECLARE p_expire_date DATETIME;
    DECLARE done_items INT DEFAULT FALSE;

    DECLARE cur_input CURSOR FOR
        SELECT item_id, qty, price, row_uuid, expire_date
        FROM JSON_TABLE(params, '$.items[*]'
                        COLUMNS (
                            row_uuid CHAR(36) PATH '$.id',
                            item_id INT PATH '$.item_id',
                            qty DECIMAL(14, 4) PATH '$.qty',
                            price DECIMAL(14, 2) PATH '$.price',
                            expire_date DATETIME PATH '$.expire_date'
                            )) AS jt;

    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done_items = TRUE;

    SET sql_safe_updates = 0;

    SELECT f_version, f_status
    INTO current_version, current_status
    FROM store_document
    WHERE f_id = doc_uuid FOR
    UPDATE;

    IF (current_version > 0 AND current_version <> CAST(JSON_VALUE(params, '$.doc_version') AS UNSIGNED)) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'version_conflict');
    END IF;

    DELETE FROM b_clients_debts WHERE f_storedoc = doc_uuid;
    DELETE FROM cash_debts WHERE f_doc_uuid = doc_uuid;
    -- Реверс старого проведения (если был статус 1)
    IF (current_status = 1) THEN
        -- Сначала восстанавливаем минусы, которые этот док лечил
        UPDATE store_stock ss JOIN store_moves sm ON sm.f_batch_id = ss.f_id AND sm.f_doc = doc_uuid
        SET ss.f_qty_left   = -sm.f_qty_in,
            ss.f_qty_in     = 0,
            ss.f_price      = 0,
            ss.f_doc        = NULL,
            ss.f_doc_row_id = NULL
        WHERE ss.f_qty_in > 0
          AND ss.f_qty_left = 0;

        DELETE FROM store_moves WHERE f_doc = doc_uuid;
        DELETE FROM store_stock WHERE f_doc = doc_uuid;
    END IF;

    INSERT INTO store_document (f_id, f_user_id, f_status, f_doc_type, f_doc_date, f_store_in, f_sum, f_partner,
                                f_version, f_data)
    VALUES (doc_uuid, doc_user_id, new_status, CAST(JSON_VALUE(params, '$.doc_type') AS UNSIGNED), doc_date, store_in,
            CAST(JSON_VALUE(params, '$.doc_sum') AS DECIMAL(14, 2)), partner_id, IFNULL(current_version, 0) + 1,
            JSON_EXTRACT(params, '$.doc_data'))
    ON DUPLICATE KEY UPDATE f_status   = VALUES(f_status),
                            f_user_id  = doc_user_id,
                            f_doc_date = VALUES(f_doc_date),
                            f_store_in = store_in,
                            f_sum      = VALUES(f_sum),
                            f_partner  = partner_id,
                            f_version  = f_version + 1,
                            f_data     = VALUES(f_data);

    DELETE FROM store_user WHERE f_doc = doc_uuid;
    INSERT INTO store_user (f_id, f_doc, f_item_id, f_qty, f_price, f_total, f_comment, f_row)
    SELECT row_uuid,
           doc_uuid,
           item_id,
           qty,
           price,
           qty * price,
           comment,
           `row`
    FROM JSON_TABLE(params, '$.items[*]'
                    COLUMNS (row_uuid CHAR(36) PATH '$.id', item_id INT PATH '$.item_id', qty DECIMAL(14, 4) PATH '$.qty', price DECIMAL(14, 2) PATH '$.price', comment varchar(255) PATH '$.comment', `row` int PATH '$.row')) AS jt;

    IF (new_status = 1) THEN
        -- partner debts or cashbox. if cashbox > 0 out from cash, otherwise write to debt
        IF (cashbox_id > 0) THEN
            INSERT INTO cash_operations (f_cashbox_id, f_order_id, f_user, f_operation_type, f_payment_type_id,
                                         f_datetime, f_credit)
            VALUES (cashbox_id, doc_uuid, create_user, 2, payment_type_id, doc_date, doc_sum);
        ELSE
            INSERT INTO cash_debts (f_date, f_partner, f_doc_type, f_doc_uuid, f_credit, f_debit, f_currency_id)
            VALUES (doc_date, partner_id, 1, doc_uuid, doc_sum, 0, currency_id);
        END IF;

        SET done_items = FALSE;
        OPEN cur_input;
        input_loop:
        LOOP
            FETCH cur_input INTO p_item_id, p_qty_arrived, p_price, p_row_uuid, p_expire_date;
            IF done_items THEN LEAVE input_loop; END IF;

            block_neg:
            BEGIN
                DECLARE done_neg INT DEFAULT FALSE;
                DECLARE neg_id CHAR(36);
                DECLARE neg_qty_abs DECIMAL(14, 4);
                DECLARE cur_neg CURSOR FOR SELECT f_id, ABS(f_qty_left)
                                           FROM store_stock
                                           WHERE f_item_id = p_item_id
                                             AND f_store_id = store_in
                                             AND f_qty_left < 0
                                           ORDER BY f_batch_date ASC;
                DECLARE CONTINUE HANDLER FOR NOT FOUND SET done_neg = TRUE;
                OPEN cur_neg;
                neg_loop:
                LOOP
                    FETCH cur_neg INTO neg_id, neg_qty_abs;
                    IF done_neg OR p_qty_arrived <= 0 THEN LEAVE neg_loop; END IF;

                    SET neg_qty_abs = CASE WHEN p_qty_arrived >= neg_qty_abs THEN neg_qty_abs ELSE p_qty_arrived END;

                    -- Записываем связь: приход закрыл этот конкретный минус
                    INSERT INTO store_moves (f_id, f_doc, f_doc_row_id, f_batch_id, f_store_id, f_item_id, f_qty_in,
                                             f_price, f_total)
                    VALUES (UUID(), doc_uuid, p_row_uuid, neg_id, store_in, p_item_id, neg_qty_abs, p_price,
                            neg_qty_abs * p_price);

                    UPDATE store_stock
                    SET f_qty_in     = neg_qty_abs,
                        f_qty_left   = 0,
                        f_price      = p_price,
                        f_doc        = doc_uuid,
                        f_doc_row_id = p_row_uuid
                    WHERE f_id = neg_id;
                    UPDATE store_moves SET f_price = p_price, f_total = f_qty_out * p_price WHERE f_batch_id = neg_id;

                    SET p_qty_arrived = p_qty_arrived - neg_qty_abs;
                END LOOP;
                CLOSE cur_neg;
            END block_neg;

            -- Движение на партию p_row_uuid только на остаток после погашения минусов (иначе 20+1 в store_moves).
            IF p_qty_arrived > 0 THEN
                INSERT INTO store_moves (f_id, f_doc, f_doc_row_id, f_batch_id, f_store_id, f_item_id, f_qty_in,
                                         f_price,
                                         f_total)
                VALUES (UUID(), doc_uuid, p_row_uuid, p_row_uuid, store_in, p_item_id, p_qty_arrived, p_price,
                        p_qty_arrived * p_price);

                INSERT INTO store_stock (f_id, f_doc, f_doc_row_id, f_batch_date, f_expiry_date, f_store_id, f_item_id,
                                         f_qty_in, f_qty_left, f_price)
                VALUES (p_row_uuid, doc_uuid, p_row_uuid, doc_date, p_expire_date, store_in, p_item_id, p_qty_arrived,
                        p_qty_arrived, p_price);
            END IF;
        END LOOP;
        CLOSE cur_input;
    END IF;

    UPDATE c_goods g JOIN JSON_TABLE(params, '$.items[*]'
                                     COLUMNS (item_id INT PATH '$.item_id', price DECIMAL(14, 2) PATH '$.price')) AS jt ON g.f_id = jt.item_id
    SET g.f_lastinputprice = jt.price
    WHERE jt.price > 0;

    RETURN JSON_OBJECT('status', 0, 'version', IFNULL(current_version, 0) + 1);
END$$
DELIMITER ;