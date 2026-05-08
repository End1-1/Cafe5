DROP FUNCTION IF EXISTS sf_store2_output;
DELIMITER $$

CREATE FUNCTION sf_store2_output(params longtext)
    RETURNS longtext
BEGIN
    DECLARE current_version int DEFAULT 0;
    DECLARE current_status int DEFAULT 0;
    DECLARE doc_uuid char(36) COLLATE latin1_general_ci DEFAULT JSON_VALUE(params, '$.doc_uuid');
    DECLARE store_id int DEFAULT JSON_VALUE(params, '$.doc_store_out');
    DECLARE doc_date datetime DEFAULT JSON_VALUE(params, '$.doc_date');
    DECLARE new_status int DEFAULT CAST(JSON_VALUE(params, '$.doc_status') AS UNSIGNED);

    DECLARE curr_item_id INT;
    DECLARE curr_qty_needed DECIMAL(14, 4);
    DECLARE curr_row_id CHAR(36);
    DECLARE done INT DEFAULT FALSE;

    DECLARE v_neg_id CHAR(36);
    DECLARE take_qty DECIMAL(14, 4);
    DECLARE total_cost DECIMAL(14, 2) DEFAULT 0;
    DECLARE last_input_price DECIMAL(14, 2);

    -- Основной курсор по товарам в JSON
    DECLARE item_cursor CURSOR FOR
        SELECT item_id, qty, row_id
        FROM JSON_TABLE(params, '$.items[*]'
                        COLUMNS (
                            item_id INT PATH '$.item_id',
                            qty DECIMAL(14, 4) PATH '$.qty',
                            row_id CHAR(36) PATH '$.id'
                            )
             ) AS jt;

    -- Обработчик для основного курсора
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

    -- 1. Получаем текущее состояние документа
    SET current_status = -1;
    SELECT f_version, f_status
    INTO current_version, current_status
    FROM store_document
    WHERE f_id = doc_uuid FOR
    UPDATE;

    -- Проверка версии
    IF (current_version > 0 AND current_version <> CAST(JSON_VALUE(params, '$.doc_version') AS UNSIGNED)) THEN
        RETURN JSON_COMPACT(JSON_OBJECT('status', 1, 'msg', 'version_error'));
    END IF;

    -- 2. РЕВЕРС (как sf_store2_output_delete): минус+приход — вернуть qmerge на основную партию; остальное — в f_qty_left
    IF (current_status = 1) THEN
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

    -- 3. Сохраняем/Обновляем заголовок
    INSERT INTO store_document (f_id, f_status, f_doc_type, f_doc_date, f_store_out, f_user_id, f_version, f_data)
    VALUES (doc_uuid, new_status, 2, doc_date, store_id, JSON_VALUE(params, '$.doc_user_id'), 1,
            JSON_EXTRACT(params, '$.doc_data'))
    ON DUPLICATE KEY UPDATE f_status    = VALUES(f_status),
                            f_doc_date  = VALUES(f_doc_date),
                            f_store_out = VALUES(f_store_out),
                            f_version   = f_version + 1;

    -- 4. Заполняем пользовательские строки
    DELETE FROM store_user WHERE f_doc = doc_uuid;
    INSERT INTO store_user (f_id, f_doc, f_item_id, f_qty, f_comment, f_row)
    SELECT jt.row_uuid, doc_uuid, jt.item_id, jt.qty, jt.comment, jt.row
    FROM JSON_TABLE(params, '$.items[*]'
                    COLUMNS (row_uuid CHAR(36) PATH '$.id', item_id INT PATH '$.item_id',
                        qty DECIMAL(14, 4) PATH '$.qty', comment varchar(255) PATH '$.comment', row int PATH '$.row')
         ) AS jt;

    -- 5. ЦИКЛ СПИСАНИЯ
    IF (new_status = 1) THEN
        SET total_cost = 0;
        SET done = FALSE; -- Сброс перед началом
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
                      AND f_store_id = store_id
                      AND f_qty_left > 0
                    ORDER BY f_batch_date ASC;

                -- Локальный обработчик для FIFO курсора
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
                    VALUES (UUID(), doc_uuid, curr_row_id, b_id, store_id, curr_item_id, take_qty, b_price,
                            take_qty * b_price);

                    SET curr_qty_needed = curr_qty_needed - take_qty;
                    SET total_cost = total_cost + (take_qty * b_price);
                END LOOP;
                CLOSE batch_cursor;
            END fifo_block;

            -- ВАЖНО: сбрасываем основной флаг done, так как b_done мог его триггернуть
            SET done = FALSE;

            -- Обработка минусов
            IF curr_qty_needed > 0 THEN
                SET v_neg_id = UUID();
                SELECT IFNULL(f_lastinputprice, 0) INTO last_input_price FROM c_goods WHERE f_id = curr_item_id;

                INSERT INTO store_stock (f_id, f_doc, f_doc_row_id, f_batch_date, f_store_id, f_item_id, f_qty_in,
                                         f_qty_left, f_price)
                VALUES (v_neg_id, doc_uuid, curr_row_id, doc_date, store_id, curr_item_id, 0, -curr_qty_needed,
                        last_input_price);

                INSERT INTO store_moves (f_id, f_doc, f_doc_row_id, f_batch_id, f_store_id, f_item_id, f_qty_out,
                                         f_price, f_total)
                VALUES (UUID(), doc_uuid, curr_row_id, v_neg_id, store_id, curr_item_id, curr_qty_needed,
                        last_input_price, curr_qty_needed * last_input_price);

                SET total_cost = total_cost + (curr_qty_needed * last_input_price);
            END IF;
        END LOOP;
        CLOSE item_cursor;

        -- Считаем цены для интерфейса
        UPDATE store_user su
            JOIN (SELECT f_doc_row_id, SUM(f_total) AS row_total, SUM(f_qty_out) AS row_qty
                  FROM store_moves
                  WHERE f_doc = doc_uuid
                  GROUP BY f_doc_row_id) AS sm
            ON su.f_id = sm.f_doc_row_id
        SET su.f_price = sm.row_total / NULLIF(sm.row_qty, 0),
            su.f_total = sm.row_total
        WHERE su.f_doc = doc_uuid;
    END IF;

    -- Финальное обновление суммы документа
    UPDATE store_document SET f_sum = total_cost WHERE f_id = doc_uuid;

    RETURN JSON_COMPACT(JSON_OBJECT('status', 0, 'cost', total_cost));
END$$
DELIMITER ;