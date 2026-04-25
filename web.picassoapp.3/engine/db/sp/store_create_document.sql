DROP PROCEDURE IF EXISTS sf_create_store_document;
DELIMITER $$
CREATE
    DEFINER = `root`@`localhost` PROCEDURE `sf_create_store_document`(
    IN `params` JSON
)
    LANGUAGE SQL
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
sp:
BEGIN
    DECLARE
        aheaderid CHAR(36) CHARACTER SET latin1 COLLATE latin1_general_ci;
    DECLARE
        EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            ROLLBACK;
            RESIGNAL;
        END;

    SET
        @timestartd = CURRENT_TIMESTAMP();
    -- 26/02/2026 31.03.2025
-- data
    SELECT JSON_EXTRACT(params, '$.header') INTO @header;
    SELECT JSON_VALUE(@header, '$.f_id') INTO aheaderid;
    SELECT JSON_EXTRACT(params, '$.body') INTO @header_body;
    SELECT JSON_EXTRACT(params, '$.goods') INTO @goods;
    SELECT JSON_EXTRACT(params, '$.goods_decomplectation') INTO @goods_decomplectation;
    SELECT JSON_EXTRACT(params, '$.add') INTO @additional;
    SELECT JSON_EXTRACT(params, '$.partner') INTO @partner;

    IF (aheaderid IS NULL OR LENGTH(aheaderid) = 0) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'HEADER ID IS EMPTY';
    END IF;

-- delete old document
    SELECT f_id, f_state, f_payment
    INTO @tid, @tstate, @payment
    FROM a_header
    WHERE f_id = aheaderid;
-- check if make doc draft and other documunt depended on  this doc
    IF (@tid IS NOT NULL AND @tstate = 1 AND JSON_VALUE(@header, '$.f_state') = 2) THEN
        SELECT GROUP_CONCAT(DISTINCT (d.f_userid), ' ', d.f_date SEPARATOR '\n')
        INTO @tcheck
        FROM a_store s
                 INNER JOIN a_header d ON d.f_id = s.f_document
        WHERE f_basedoc = aheaderid
          AND f_document <> aheaderid
          AND s.f_type = -1;
        IF
            (@tcheck IS NOT NULL) THEN
            INSERT INTO a_result(f_session, f_document, f_timestamp, f_done, f_elapsed, f_request, f_result)
            VALUES (JSON_VALUE(params, '$.session'), JSON_VALUE(@header, '$.f_id'), @timestartd, @timeendd,
                    TIMESTAMPDIFF(SECOND, @timestartd, @timeendd), params,
                    JSON_OBJECT('status', 0, 'message', CONCAT_WS('\n', 'Փասթաթուղթը կախված է', @tcheck)));
            LEAVE
                sp;
        END IF;
    END IF;


    IF (JSON_VALUE(params, '$.notransaction') IS NULL) THEN
        START TRANSACTION;
    END IF;

    IF (@tid IS NOT NULL) THEN
        DELETE FROM a_store_valid WHERE f_document = aheaderid;
        DELETE FROM a_store WHERE f_document = aheaderid;
        DELETE FROM a_store_draft WHERE f_document = aheaderid;
        DELETE
        FROM a_header
        WHERE f_id = aheaderid;
        DELETE
        FROM a_header_store
        WHERE f_id = aheaderid;
        DELETE
        FROM b_clients_debts
        WHERE f_storedoc = aheaderid;
        DELETE
        FROM a_complectation_additions
        WHERE f_header = aheaderid;
        IF (@payment IS NOT NULL) THEN
            SELECT sf_remove_cashdoc(@payment)
            INTO @tmp;
        END IF;
    END IF;
    SET
        @message = '';
    SET
        @status = 1;


-- gen document id
    SET @userid = JSON_VALUE(@header, '$.f_userid');
    IF
        (@userid IS NULL OR @userid = 0) THEN
        CASE (JSON_VALUE(@header, '$.f_type'))
            WHEN 1 THEN BEGIN
                SELECT COALESCE(f_inputcounter, 0) + 1
                INTO @userid
                FROM c_storages
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_inputcounter=@userid
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
            END;
            WHEN 2 THEN BEGIN
                SELECT COALESCE(f_outcounter, 0) + 1
                INTO @userid
                FROM c_storages
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
                UPDATE c_storages
                SET f_outcounter=@userid
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            END;
            WHEN 3 THEN BEGIN
                SELECT COALESCE(f_movecounter, 0) + 1
                INTO @userid
                FROM c_storages
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_movecounter=@userid
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
            END;
            WHEN 6 THEN BEGIN
                SELECT COALESCE(f_complectcounter, 0) + 1
                INTO @userid
                FROM c_storages
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_complectcounter=@userid
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
            END;
            END CASE;
    END IF;

#header
    INSERT INTO a_header (f_id, f_userid, f_state, f_type,
                          f_operator, f_date, f_createdate, f_createtime,
                          f_partner, f_amount, f_comment, f_payment, f_paid, f_currency, f_working_session, f_body)
    VALUES (JSON_VALUE(@header, '$.f_id'), @userid, JSON_VALUE(@header, '$.f_state'), JSON_VALUE(@header, '$.f_type'),
            JSON_VALUE(@header, '$.f_operator'), JSON_VALUE(@header, '$.f_date'), CURRENT_DATE(), CURRENT_TIME(),
            JSON_VALUE(@header, '$.f_partner'), JSON_VALUE(@header, '$.f_amount'), JSON_VALUE(@header, '$.f_comment'),
            JSON_VALUE(@header, '$.f_payment'),
            JSON_VALUE(@header, '$.f_paid'), JSON_VALUE(@header, '$.f_currency'),
            JSON_VALUE(@header, '$.f_working_session'), JSON_DETAILED(@header_body));

    INSERT INTO a_header_store (f_id, f_reason, f_storein, f_storeout, f_invoice, f_saleuuid, f_body, f_baseonsale,
                                f_complectation, f_complectationqty)
    VALUES (aheaderid, JSON_VALUE(@header_body, '$.f_reason'), JSON_VALUE(@header, '$.f_storein'),
            JSON_VALUE(@header, '$.f_storeout'), JSON_VALUE(params, '$.body.f_invoice'),
            JSON_VALUE(@header_body, '$.f_saleuuid'), params, COALESCE(JSON_VALUE(@header_body, '$.f_basedonsale'), 0),
            JSON_VALUE(@header_body, '$.f_complectationcode'), JSON_VALUE(@header_body, '$.f_complectationqty'));

    SET
        @iadd = 0;
    WHILE (@iadd < JSON_LENGTH(@additional))
        DO
            SELECT JSON_EXTRACT(@additional, CONCAT('$[', @iadd, ']'))
            INTO @a;
            INSERT INTO a_complectation_additions (f_id, f_header, f_name, f_amount, f_row)
            VALUES (UUID(), aheaderid, JSON_VALUE(@a, '$.f_name'), JSON_VALUE(@a, '$.f_amount'),
                    JSON_VALUE(@a, '$.f_row'));
            SET
                @iadd = @iadd + 1;
        END WHILE;
    IF
        (@iadd > 0) THEN
        UPDATE s_draft
        SET f_value=aheaderid
        WHERE f_name = 'last_store_additions';
    END IF;


-- a_store_draft input
    CASE (JSON_VALUE(@header, '$.f_type'))
        WHEN 1 THEN BEGIN
            SELECT JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', 1, 'store', JSON_VALUE(@header, '$.f_storein'),
                               'state', JSON_VALUE(@header, '$.f_state'), 'partner', JSON_VALUE(@header, '$.f_partner'))
            INTO @goods;
            CALL sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET
                @status = JSON_VALUE(@result, '$.status');
            SET
                @message = JSON_VALUE(@result, '$.message');
        END;
        WHEN 2 THEN BEGIN
            SELECT JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store',
                               JSON_VALUE(@header, '$.f_storeout'), 'state',
                               JSON_VALUE(@header, '$.f_state'))
            INTO @goods;
            CALL sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET
                @status = JSON_VALUE(@result, '$.status');
            SET
                @message = JSON_VALUE(@result, '$.message');
        END;
        WHEN 3 THEN BEGIN
            SELECT JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store',
                               JSON_VALUE(@header, '$.f_storeout'), 'state',
                               JSON_VALUE(@header, '$.f_state'),
                               'move', 1, 'storein', JSON_VALUE(@header, '$.f_storein'))
            INTO @goods;
            CALL sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET
                @status = JSON_VALUE(@result, '$.status');
            SET
                @message = JSON_VALUE(@result, '$.message');
        END;
        WHEN 6 THEN BEGIN
            SELECT JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store',
                               JSON_VALUE(@header, '$.f_storeout'), 'state',
                               JSON_VALUE(@header, '$.f_state'))
            INTO @goods;
            CALL sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET
                @status = JSON_VALUE(@result, '$.status');
            SET
                @message = JSON_VALUE(@result, '$.message');
            IF
                (@status = 1) THEN
                SELECT JSON_EXTRACT(params, '$.complect')
                INTO @comp;
                SELECT SUM(f_qty * f_price)
                INTO @compprice
                FROM a_store_draft
                WHERE f_document = aheaderid;
                SELECT (COALESCE(SUM(f_amount), 0) + @compprice) / JSON_VALUE(@comp, '$.f_qty')
                INTO @compprice
                FROM a_complectation_additions
                WHERE f_header = aheaderid;
                SELECT JSON_OBJECT('f_goods', JSON_VALUE(@comp, '$.f_goods'), 'f_qty', JSON_VALUE(@comp, '$.f_qty'),
                                   'f_price',
                                   @compprice,
                                   'state', JSON_VALUE(@header, '$.f_state'))
                INTO @goods_complect_g;
                SELECT JSON_OBJECT('goods', JSON_ARRAY(JSON_DETAILED(@goods_complect_g)), 'ftype', 1, 'store',
                                   JSON_VALUE(@header, '$.f_storein'), 'state', JSON_VALUE(@header, '$.f_state'))
                INTO @goods_complect;
                CALL sf_create_store_goods(@goods_complect);
            END IF;
        END;
        ELSE BEGIN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'UNKNOWN DOCUMENT TYPE';
        END;
        END CASE;


    -- partner and paymnent
-- first, payment
    IF (JSON_VALUE(@partner, '$.paid') = 1 AND JSON_VALUE(@header, '$.f_state') = 1) THEN
        SELECT JSON_OBJECT('f_cashout', JSON_VALUE(@partner, '$.cash'), 'f_currency',
                           JSON_VALUE(@header, '$.f_currency'),
                           'f_amount', JSON_VALUE(@header, '$.f_amount'), 'f_purpose',
                           CONCAT('Պահեստի մուտքի վճարում ', @userid), 'f_comment',
                           CONCAT('Պահեստի մուտքի վճարում ', @userid),
                           'f_operator', JSON_VALUE(@header, '$.f_operator'), 'f_date', JSON_VALUE(@header, '$.f_date'))
        INTO @cashdoc;
        SELECT sf_create_cashdoc(@cashdoc)
        INTO @cashdocresult;
        UPDATE a_header
        SET f_payment=JSON_VALUE(@cashdocresult, '$.cashdoc')
        WHERE f_id = aheaderid;
    END IF;
-- debts    , ets...
    IF ((CAST(@status AS unsigned) = 1) AND (JSON_VALUE(@header, '$.f_state') = 1) AND
        JSON_VALUE(@header, '$.f_nodebtop') IS NULL) THEN
        IF (JSON_VALUE(@partner, '$.partner') > 0) THEN
            SELECT f_hall
            INTO @thall
            FROM c_storages
            WHERE f_id = JSON_VALUE(@header, '$.f_storein')
               OR f_id = JSON_VALUE(@header, '$.f_storeout');
            CASE JSON_VALUE(@header, '$.f_type')
                WHEN 1 THEN BEGIN
                    # decrease debt if goods return from partner

                    IF (JSON_VALUE(@header, '$.f_reason') = 9) THEN
                        INSERT INTO b_clients_debts (f_date, f_costumer, f_amount, f_storedoc, f_currency, f_source,
                                                     f_flag)
                        VALUES (JSON_VALUE(@header, '$.f_date'), JSON_VALUE(@partner, '$.partner'),
                                JSON_VALUE(@header, '$.f_amount'),
                                aheaderid, JSON_VALUE(@header, '$.f_currency'), 1, @thall);
                    END IF;
                    #debt
                    IF (JSON_VALUE(@partner, '$.paid') = 0 AND JSON_VALUE(@header, '$.f_reason') <> 11) THEN
                        INSERT INTO b_clients_debts (f_date, f_costumer, f_amount, f_storedoc, f_currency, f_source,
                                                     f_flag)
                        VALUES (JSON_VALUE(@header, '$.f_date'), JSON_VALUE(@partner, '$.partner'),
                                JSON_VALUE(@header, '$.f_amount') * -1,
                                aheaderid, JSON_VALUE(@header, '$.f_currency'), 2, @thall);
                    END IF;
                END;
                WHEN 2 THEN BEGIN
                    IF (JSON_VALUE(@header, '$.f_reason') = 9) THEN
                        SELECT f_amount
                        INTO @ttotal
                        FROM a_header
                        WHERE f_id = aheaderid;
                        INSERT INTO b_clients_debts (f_date, f_costumer, f_amount, f_storedoc, f_currency, f_source,
                                                     f_flag)
                        VALUES (JSON_VALUE(@header, '$.f_date'), JSON_VALUE(@partner, '$.partner'), @ttotal,
                                aheaderid, JSON_VALUE(@header, '$.f_currency'), 2, @thall);
                    END IF;
                END;
                ELSE BEGIN
                END;
                END CASE;
        END IF;
        CASE JSON_VALUE(@header, '$.f_type')
            WHEN 1 THEN UPDATE c_storages
                        SET f_have_changes=1
                        WHERE f_id = JSON_VALUE(@header, '$.f_storein');
            WHEN 2 THEN UPDATE c_storages
                        SET f_have_changes=1
                        WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            WHEN 3 THEN BEGIN
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            END;
            WHEN 6 THEN BEGIN
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            END;
            END CASE;
    END IF;

    IF (CAST(@status AS unsigned) = 1) THEN

        UPDATE a_store
        SET f_total=f_qty * f_price
        WHERE f_document = aheaderid;
        UPDATE a_store_draft
        SET f_total=f_qty * f_price
        WHERE f_document = aheaderid;
        CASE JSON_VALUE(@header, '$.f_type')
            WHEN 1 THEN UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total) FROM a_store_draft WHERE f_document = aheaderid)
                        WHERE f_id = aheaderid;
            WHEN 2 THEN UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total) FROM a_store_draft WHERE f_document = aheaderid)
                        WHERE f_id = aheaderid;
            WHEN 3 THEN UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total)
                                      FROM a_store_draft
                                      WHERE f_document = aheaderid
                                        AND f_type = -1)
                        WHERE f_id = aheaderid;
            WHEN 6 THEN UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total)
                                      FROM a_store_draft
                                      WHERE f_document = aheaderid
                                        AND f_type = -1)
                        WHERE f_id = aheaderid;
            END CASE;
        IF
            (JSON_LENGTH(@additional) > 0) THEN
            UPDATE a_header
            SET f_amount=f_amount +
                         (SELECT COALESCE(SUM(f_amount), 0) FROM a_complectation_additions WHERE f_header = aheaderid)
            WHERE f_id = aheaderid;
        END IF;
        IF
            (JSON_VALUE(params, '$.notransaction') IS NULL) THEN
            COMMIT;
        END IF;
    ELSE
        IF (JSON_VALUE(params, '$.notransaction') IS NULL) THEN
            ROLLBACK;
        END IF;
    END IF;

    SET
        @timeendd = CURRENT_TIMESTAMP;
    INSERT INTO a_result(f_session, f_document, f_timestamp, f_done, f_elapsed, f_request, f_result)
    VALUES (JSON_VALUE(params, '$.session'), JSON_VALUE(@header, '$.f_id'), @timestartd, @timeendd,
            TIMESTAMPDIFF(SECOND, @timestartd, @timeendd), params,
            JSON_OBJECT('status', CAST(@status AS unsigned), 'message', @message, 'state',
                        CAST(JSON_VALUE(@header, '$.f_state') AS unsigned)));
END$$

DELIMITER ;