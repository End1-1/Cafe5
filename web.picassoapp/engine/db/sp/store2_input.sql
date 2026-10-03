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
    DECLARE doc_user_id char(16) DEFAULT JSON_VALUE(params, '$.doc_user_id');
    DECLARE create_user int DEFAULT CAST(JSON_VALUE(params, '$.doc_create_user') AS UNSIGNED);
    DECLARE cashbox_id int DEFAULT CAST(JSON_VALUE(params, '$.cashbox_id') AS unsigned);
    DECLARE payment_type_id int DEFAULT CAST(JSON_VALUE(params, '$.payment_type_id') AS UNSIGNED);
    DECLARE doc_sum decimal(14, 2) DEFAULT CAST(JSON_VALUE(params, '$.doc_sum') AS decimal(14, 2));
    DECLARE currency_id int DEFAULT CAST(JSON_VALUE(params, '$.currency_id') AS unsigned);
    DECLARE partner_id int DEFAULT CAST(JSON_VALUE(params, '$.doc_partner') AS unsigned);
    DECLARE paid_raw longtext DEFAULT JSON_VALUE(params, '$.paid_amount');
    DECLARE paid_amount decimal(14, 2) DEFAULT 0;
    DECLARE debt_amount decimal(14, 2) DEFAULT 0;
    DECLARE session_id int DEFAULT 0;

    DECLARE p_user_item_id INT;
    DECLARE p_item_id INT;
    DECLARE p_qty_arrived DECIMAL(14, 4);
    DECLARE p_price DECIMAL(14, 2);
    DECLARE p_row_uuid CHAR(36);
    DECLARE p_expire_date DATETIME;
    DECLARE done_items INT DEFAULT FALSE;

    -- Resolve f_storeid inside the loop (JOIN inside CURSOR is unreliable on some MariaDB builds)
    DECLARE cur_input CURSOR FOR
        SELECT jt.item_id, jt.qty, jt.price, jt.row_uuid, jt.expire_date
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

    IF (paid_raw IS NULL OR paid_raw = '') THEN
        SET paid_amount = IF(IFNULL(cashbox_id, 0) > 0, doc_sum, 0);
    ELSE
        SET paid_amount = CAST(paid_raw AS decimal(14, 2));
    END IF;
    IF (paid_amount < 0) THEN
        SET paid_amount = 0;
    END IF;
    IF (paid_amount > doc_sum) THEN
        RETURN JSON_OBJECT('status', 1, 'msg', 'paid_exceeds_sum');
    END IF;
    SET debt_amount = doc_sum - paid_amount;

    -- Materialize the header first: locking a missing PK value would take a gap lock and
    -- deadlock against a parallel document inserting into the same gap.
    INSERT IGNORE INTO store_document (f_id, f_status, f_version) VALUES (doc_uuid, -1, 0);

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
    DELETE FROM cash_operations WHERE f_order_id = doc_uuid;
    -- Реверс старого проведения (если был статус 1)
    IF (current_status = 1) THEN
        -- Снимаем погашение чужих минусов (f_doc_row_id <> f_batch_id — признак погашения).
        -- Вычитаем ровно то, что погасил этот документ: минус мог быть закрыт частично
        -- или несколькими приходами, и обнулять партию нельзя.
        UPDATE store_stock ss
            JOIN (SELECT sm.f_batch_id, SUM(sm.f_qty_in) AS healed
                  FROM store_moves sm
                  WHERE sm.f_doc = doc_uuid
                    AND sm.f_qty_in > 0
                    AND sm.f_doc_row_id <> sm.f_batch_id
                  GROUP BY sm.f_batch_id) h ON h.f_batch_id = ss.f_id
        SET ss.f_qty_in     = ss.f_qty_in - h.healed,
            ss.f_qty_left   = ss.f_qty_left - h.healed,
            ss.f_price      = IF(ss.f_qty_in > 0, ss.f_price, 0),
            ss.f_doc        = IF(ss.f_qty_in > 0 AND ss.f_doc <> doc_uuid, ss.f_doc, NULL),
            ss.f_doc_row_id = IF(ss.f_doc IS NOT NULL, ss.f_doc_row_id, NULL);

        -- Из своих партий могли уже списать другие документы. Такую партию нельзя просто
        -- удалить: их расходы остались бы без партии. Переносим списанное на отдельный
        -- минус, а саму партию удаляем — строка прихода перепроведётся заново.
        own_batches:
        BEGIN
            DECLARE done_own INT DEFAULT FALSE;
            DECLARE o_id CHAR(36);
            DECLARE o_consumed DECIMAL(14, 4);
            DECLARE o_store INT;
            DECLARE o_item INT;
            DECLARE o_date DATETIME;
            DECLARE o_hole CHAR(36);
            DECLARE cur_own CURSOR FOR
                SELECT ss.f_id, ss.f_qty_in - ss.f_qty_left, ss.f_store_id, ss.f_item_id, ss.f_batch_date
                FROM store_stock ss
                WHERE ss.f_doc = doc_uuid
                  AND ss.f_id = ss.f_doc_row_id
                  AND EXISTS (SELECT 1 FROM store_moves sm
                              WHERE sm.f_batch_id = ss.f_id AND sm.f_doc <> doc_uuid);
            DECLARE CONTINUE HANDLER FOR NOT FOUND SET done_own = TRUE;

            OPEN cur_own;
            own_loop:
            LOOP
                FETCH cur_own INTO o_id, o_consumed, o_store, o_item, o_date;
                IF done_own THEN LEAVE own_loop; END IF;
                IF o_consumed > 0 THEN
                    SET o_hole = UUID();
                    INSERT INTO store_stock (f_id, f_doc, f_doc_row_id, f_batch_date, f_store_id, f_item_id,
                                             f_qty_in, f_qty_left, f_price)
                    VALUES (o_hole, NULL, NULL, o_date, o_store, o_item, 0, -o_consumed, 0);
                    UPDATE store_moves SET f_batch_id = o_hole
                    WHERE f_batch_id = o_id AND f_doc <> doc_uuid;
                END IF;
            END LOOP;
            CLOSE cur_own;
            SET done_items = FALSE;
        END own_batches;

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
                            f_doc_type = VALUES(f_doc_type),
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
        IF (paid_amount > 0) THEN
            IF (IFNULL(cashbox_id, 0) <= 0 OR IFNULL(payment_type_id, 0) <= 0) THEN
                RETURN JSON_OBJECT('status', 1, 'msg', 'cashbox_required_for_paid');
            END IF;
            SET session_id = IFNULL((SELECT f_id
                                     FROM cash_session
                                     WHERE f_state = 1
                                       AND f_cashbox_id = cashbox_id
                                     ORDER BY f_id DESC
                                     LIMIT 1), 0);
            INSERT INTO cash_operations (f_cashbox_id, f_session_id, f_order_id, f_user, f_operation_type,
                                         f_payment_type_id, f_datetime, f_credit, f_currency_id, f_affects_float)
            VALUES (cashbox_id, NULLIF(session_id, 0), doc_uuid, create_user, 3, payment_type_id, doc_date,
                    paid_amount, IFNULL(currency_id, 1), IF(payment_type_id = 1, 1, 0));
            -- Cash purchase reduces drawer float when a shift is open on this cashbox.
            IF (payment_type_id = 1 AND IFNULL(session_id, 0) > 0) THEN
                UPDATE cash_session
                SET f_amount_expected = f_amount_expected - paid_amount
                WHERE f_id = session_id;
            END IF;
        END IF;
        IF (debt_amount > 0) THEN
            IF (IFNULL(partner_id, 0) <= 0) THEN
                RETURN JSON_OBJECT('status', 1, 'msg', 'partner_required_for_debt');
            END IF;
            INSERT INTO cash_debts (f_date, f_partner, f_doc_type, f_doc_uuid, f_credit, f_debit, f_currency_id)
            VALUES (doc_date, partner_id, 1, doc_uuid, debt_amount, 0, IFNULL(currency_id, 1));
        END IF;

        SET done_items = FALSE;
        OPEN cur_input;
        input_loop:
        LOOP
            FETCH cur_input INTO p_user_item_id, p_qty_arrived, p_price, p_row_uuid, p_expire_date;
            IF done_items THEN LEAVE input_loop; END IF;

            SELECT IFNULL(NULLIF(f_storeid, 0), f_id)
            INTO p_item_id
            FROM c_goods
            WHERE f_id = p_user_item_id;
            IF (IFNULL(p_item_id, 0) = 0) THEN
                SET p_item_id = p_user_item_id;
            END IF;
            SET done_items = FALSE;

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

                    -- Прибавляем, а не присваиваем: минус мог быть больше прихода,
                    -- тогда непокрытый остаток должен сохраниться отрицательным.
                    UPDATE store_stock
                    SET f_qty_in     = f_qty_in + neg_qty_abs,
                        f_qty_left   = f_qty_left + neg_qty_abs,
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

    -- c_goods.f_lastinputprice is refreshed by the caller after COMMIT: keeping the catalog
    -- out of this transaction removes a hot lock shared with menu/goods editors.

    RETURN JSON_OBJECT('status', 0, 'version', IFNULL(current_version, 0) + 1, 'paid_amount', paid_amount);
END$$
DELIMITER ;
