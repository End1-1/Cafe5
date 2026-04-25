delimiter $$
DROP PROCEDURE if EXISTS sf_create_store_document$$
CREATE
    DEFINER = `root`@`localhost` PROCEDURE `sf_create_store_document`(
    IN `params` JSON)
    LANGUAGE SQL
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
sp:
BEGIN
    -- 26/01/2026
-- data
    DECLARE aheaderid CHAR(36) CHARACTER SET LATIN1 COLLATE latin1_general_ci;
    declare goods_disassembly json;
    DECLARE
        EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            ROLLBACK;
            RESIGNAL;
        END;

    SET @timestartd = CURRENT_TIMESTAMP();

    select JSON_EXTRACT(params, '$.header') INTO @header;
    SELECT JSON_VALUE(@header, '$.f_id') INTO aheaderid;
    SELECT json_extract(params, '$.body') INTO @header_body;
    SELECT JSON_EXTRACT(params, '$.goods') INTO @goods;
    select json_extract(params, '$.goods_disassembly') into goods_disassembly;
    SELECT JSON_EXTRACT(params, '$.add') INTO @additional;
    SELECT JSON_EXTRACT(params, '$.partner') INTO @partner;

    if (aheaderid is null or LENGTH(aheaderid) = 0) then
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'HEADER ID IS EMPTY';
    END if;

-- delete old document
    SELECT f_id, f_state, f_payment
    INTO @tid, @tstate, @payment
    FROM a_header
    WHERE f_id = aheaderid;
-- check if make doc draft and other documunt depended on  this doc
    if (@tid IS NOT NULL AND @tstate = 1 AND JSON_VALUE(@header, '$.f_state') = 2) then
        SELECT GROUP_CONCAT(DISTINCT (d.f_userid), ' ', d.f_date SEPARATOR '\n')
        INTO @tcheck
        from a_store s
                 inner join a_header d on d.f_id = s.f_document
        where f_basedoc = aheaderid
          and f_document <> aheaderid
          and s.f_type = -1;
        if
            (@tcheck IS NOT NULL) then
            INSERT INTO a_result(f_session, f_document, f_timestamp, f_done, f_elapsed, f_request, f_result)
            VALUES (JSON_VALUE(params, '$.session'), JSON_VALUE(@header, '$.f_id'), @timestartd, @timeendd,
                    TIMESTAMPDIFF(SECOND, @timestartd, @timeendd), params,
                    JSON_OBJECT('status', 0, 'message', CONCAT_WS('\n', 'Փասթաթուղթը կախված է', @tcheck)));
            leave
                sp;
        END if;
    END if;


    if (JSON_VALUE(params, '$.notransaction') IS NULL) then
        START TRANSACTION;
    END if;

    if (@tid IS NOT NULL) then
        DELETE from a_store_valid where f_document = aheaderid;
        DELETE FROM a_store WHERE f_document = aheaderid;
        DELETE FROM a_store_draft WHERE f_document = aheaderid;
        DELETE FROM a_header WHERE f_id = aheaderid;
        DELETE FROM a_header_store WHERE f_id = aheaderid;
        DELETE FROM b_clients_debts WHERE f_storedoc = aheaderid;
        delete from a_complectation_additions where f_header = aheaderid;
        if (@payment IS NOT NULL) then
            SELECT sf_remove_cashdoc(@payment)
            INTO @tmp;
        END if;
    END if;
    SET @message = '';
    SET @STATUS = 1;


-- gen document id
    SET @userid = JSON_VALUE(@header, '$.f_userid');
    if (@userid IS NULL OR @userid = 0) then
        case (JSON_VALUE(@header, '$.f_type'))
            when 1 then begin
                select coalesce(f_inputcounter, 0) + 1
                INTO @userid
                from c_storages
                where f_id = jsON_VALUE(@header, '$.f_storein');
                UPDATE c_storages SET f_inputcounter=@userid WHERE f_id = jsON_VALUE(@header, '$.f_storein');
            END;
            when 2 then begin
                select coalesce(f_outcounter, 0) + 1
                INTO @userid
                from c_storages
                where f_id = JSON_VALUE(@header, '$.f_storeout');
                UPDATE c_storages
                SET f_outcounter=@userid
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            END;
            when 3 then begin
                select coalesce(f_movecounter, 0) + 1
                INTO @userid
                from c_storages
                where f_id = jsON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_movecounter=@userid
                WHERE f_id = jsON_VALUE(@header, '$.f_storein');
            END;
            when 6 then begin
                select coalesce(f_complectcounter, 0) + 1
                INTO @userid
                from c_storages
                where f_id = jsON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_complectcounter=@userid
                WHERE f_id = jsON_VALUE(@header, '$.f_storein');
            END;
            when 8 then begin
                select coalesce(f_complectcounter, 0) + 1
                INTO @userid
                from c_storages
                where f_id = jsON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_complectcounter=@userid
                WHERE f_id = jsON_VALUE(@header, '$.f_storein');
            end;
            END case;
    END if;

-- header
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
    VALUES (aheaderid, JSON_VALUE(@header_body, '$.f_reason'), jsON_VALUE(@header, '$.f_storein'),
            jsON_VALUE(@header, '$.f_storeout'), JSON_VALUE(params, '$.body.f_invoice'),
            JSON_VALUE(@header_body, '$.f_saleuuid'), params, coalesce(JSON_VALUE(@header_body, '$.f_basedonsale'), 0),
            JSON_VALUE(@header_body, '$.f_complectationcode'), JSON_VALUE(@header_body, '$.f_complectationqty'));

    SET @iadd = 0;
    while (@iadd < JSON_LENGTH(@additional))
        DO
            SELECT JSON_extract(@additional, CONCAT('$[', @iadd, ']'))
            INTO @a;
            INSERT INTO a_complectation_additions (f_id, f_header, f_name, f_amount, f_row)
            VALUES (UUID(), aheaderid, JSON_VALUE(@a, '$.f_name'), JSON_VALUE(@a, '$.f_amount'),
                    JSON_VALUE(@a, '$.f_row'));
            SET
                @iadd = @iadd + 1;
        END while;
    if (@iadd > 0) then
        UPDATE s_draft
        SET f_value=aheaderid
        WHERE f_name = 'last_store_additions';
    END if;


-- a_store_draft input
    case (JSON_VALUE(@header, '$.f_type'))
        WHEN 1 THEN BEGIN
            select JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', 1, 'store', JSON_VALUE(@header, '$.f_storein'),
                               'state', JSON_VALUE(@header, '$.f_state'), 'partner', JSON_VALUE(@header, '$.f_partner'))
            INTO @goods;
            call sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET @STATUS = JSON_VALUE(@result, '$.status');
            SET @message = JSON_VALUE(@result, '$.message');
        END;
        WHEN 2 THEN BEGIN
            select JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store',
                               JSON_VALUE(@header, '$.f_storeout'), 'state',
                               JSON_VALUE(@header, '$.f_state'))
            INTO @goods;
            call sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET @STATUS = JSON_VALUE(@result, '$.status');
            SET @message = JSON_VALUE(@result, '$.message');
        END;
        WHEN 3 THEN BEGIN
            select JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store',
                               JSON_VALUE(@header, '$.f_storeout'), 'state',
                               JSON_VALUE(@header, '$.f_state'),
                               'move', 1, 'storein', JSON_VALUE(@header, '$.f_storein'))
            INTO @goods;
            call sf_create_store_goods(@goods);
            SELECT @create_store_goods
            into @result;
            SET @STATUS = JSON_VALUE(@result, '$.status');
            SET @message = JSON_VALUE(@result, '$.message');
        END;
        when 6 then begin
            select JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store',
                               JSON_VALUE(@header, '$.f_storeout'), 'state',
                               JSON_VALUE(@header, '$.f_state'))
            INTO @goods;
            call sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET @STATUS = JSON_VALUE(@result, '$.status');
            SET @message = JSON_VALUE(@result, '$.message');
            if (@STATUS = 1) then
                SELECT JSON_EXTRACT(params, '$.complect')
                INTO @comp;
                SELECT SUM(f_qty * f_price)
                INTO @compprice
                FROM a_store_draft
                WHERE f_document = aheaderid;
                SELECT (coalesce(SUM(f_amount), 0) + @compprice) / JSON_VALUE(@comp, '$.f_qty')
                INTO @compprice
                FROM a_complectation_additions
                WHERE f_header = aheaderid;
                SELECT JSON_OBJECT('f_goods', JSON_VALUE(@comp, '$.f_goods'), 'f_qty', JSON_VALUE(@comp, '$.f_qty'),
                                   'f_price',
                                   @compprice,
                                   'state', JSON_VALUE(@header, '$.f_state'))
                INTO @goods_complect_g;
                select JSON_OBJECT('goods', json_array(JSON_DETAILED(@goods_complect_g)), 'ftype', 1, 'store',
                                   JSON_VALUE(@header, '$.f_storein'), 'state', JSON_VALUE(@header, '$.f_state'))
                INTO @goods_complect;
                call sf_create_store_goods(@goods_complect);
            END if;
        END;
        -- DISASSEMBLY
        when 8 then begin
            select JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store',
                               JSON_VALUE(@header, '$.f_storeout'), 'state',
                               JSON_VALUE(@header, '$.f_state'))
            INTO @goods;
            call sf_create_store_goods(@goods);
            SELECT @create_store_goods
            INTO @result;
            SET @STATUS = JSON_VALUE(@result, '$.status');
            SET @message = JSON_VALUE(@result, '$.message');
            if (@status = 1) then
                select JSON_OBJECT('goods', JSON_DETAILED(goods_disassembly), 'ftype', 1, 'store',
                                   JSON_VALUE(@header, '$.f_storein'), 'state',
                                   JSON_VALUE(@header, '$.f_state'))
                INTO @goods;
                call sf_create_store_goods(@goods);
                SELECT @create_store_goods
                INTO @result;
                SET @STATUS = JSON_VALUE(@result, '$.status');
                SET @message = JSON_VALUE(@result, '$.message');
            end if;
        end;
        ELSE BEGIN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'UNKNOWN DOCUMENT TYPE';
        END;
        END CASE;


    -- partner and paymnent
-- first, payment
    if (JSON_VALUE(@partner, '$.paid') = 1 AND JSON_VALUE(@header, '$.f_state') = 1) then
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
    END if;
-- debts    , ets...
    if ((CAST(@STATUS AS unsigned) = 1) AND (JSON_VALUE(@header, '$.f_state') = 1) AND
        JSON_VALUE(@header, '$.f_nodebtop') IS null) then
        if (JSON_VALUE(@partner, '$.partner') > 0) then
            SELECT f_hall
            INTO @thall
            FROM c_storages
            WHERE f_id = JSON_VALUE(@header, '$.f_storein')
               OR f_id = JSON_VALUE(@header, '$.f_storeout');
            case JSON_VALUE(@header, '$.f_type')
                When 1 then BEGIN
                    # decrease debt if goods return from partner

                    if (JSON_VALUE(@header, '$.f_reason') = 11) then
                        INSERT INTO b_clients_debts (f_date, f_costumer, f_amount, f_storedoc, f_currency, f_source,
                                                     f_flag)
                        VALUES (JSON_VALUE(@header, '$.f_date'), JSON_VALUE(@partner, '$.partner'),
                                JSON_VALUE(@header, '$.f_amount'),
                                aheaderid, JSON_VALUE(@header, '$.f_currency'), 1, @thall);
                    END if;
                    #debt
                    if (JSON_VALUE(@partner, '$.paid') = 0 AND JSON_VALUE(@header, '$.f_reason') <> 11) then
                        INSERT INTO b_clients_debts (f_date, f_costumer, f_amount, f_storedoc, f_currency, f_source,
                                                     f_flag)
                        VALUES (JSON_VALUE(@header, '$.f_date'), JSON_VALUE(@partner, '$.partner'),
                                JSON_VALUE(@header, '$.f_amount') * -1,
                                aheaderid, JSON_VALUE(@header, '$.f_currency'), 2, @thall);
                    END if;
                END;
                when 2 then BEGIN
                    if (JSON_VALUE(@header, '$.f_reason') = 9) then
                        SELECT SUM(f_qty * f_price)
                        INTO @ttotal
                        FROM a_store
                        WHERE f_id = aheaderid;
                        INSERT INTO b_clients_debts (f_date, f_costumer, f_amount, f_storedoc, f_currency, f_source,
                                                     f_flag)
                        VALUES (JSON_VALUE(@header, '$.f_date'), JSON_VALUE(@partner, '$.partner'), @ttotal,
                                aheaderid, JSON_VALUE(@header, '$.f_currency'), 2, @thall);
                    END if;
                END;
                ELSE BEGIN
                END;
                END case;
        END if;
        case JSON_VALUE(@header, '$.f_type')
            WHEn 1 then begin
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
            end;
            when 2 then begin
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            end;
            when 3 then begin
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            END;
            when 6 then begin
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            END;
            when 8 then begin
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storein');
                UPDATE c_storages
                SET f_have_changes=1
                WHERE f_id = JSON_VALUE(@header, '$.f_storeout');
            END;
            END case;
    END if;

    if (CAST(@STATUS AS unsigned) = 1) then
        UPDATE a_store
        SET f_total=f_qty * f_price
        WHERE f_document = aheaderid;
        UPDATE a_store_draft
        SET f_total=f_qty * f_price
        WHERE f_document = aheaderid;
        case JSON_VALUE(@header, '$.f_type')
            when 1 then UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total) FROM a_store_draft WHERE f_document = aheaderid)
                        WHERE f_id = aheaderid;
            when 2 then UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total) FROM a_store_draft WHERE f_document = aheaderid)
                        WHERE f_id = aheaderid;
            when 3 then UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total)
                                      FROM a_store_draft
                                      WHERE f_document = aheaderid
                                        AND f_type = -1)
                        WHERE f_id = aheaderid;
            when 6 then UPDATE a_header
                        SET f_amount=(SELECT SUM(f_total)
                                      FROM a_store_draft
                                      WHERE f_document = aheaderid
                                        AND f_type = -1)
                        WHERE f_id = aheaderid;
            END case;
        if (JSON_LENGTH(@additional) > 0) then
            UPDATE a_header
            SET f_amount=f_amount +
                         (SELECT COALESCE(SUM(f_amount), 0) FROM a_complectation_additions WHERE f_header = aheaderid)
            WHERE f_id = aheaderid;
        END if;
        if (JSON_VALUE(params, '$.notransaction') IS NULL) then
            COMMIT;
        END if;
    ELSE
        if (JSON_VALUE(params, '$.notransaction') IS NULL) then
            ROLLBACK;
        END if;
    END if;

    SET @timeendd = CURRENT_TIMESTAMP;
    INSERT INTO a_result(f_session, f_document, f_timestamp, f_done, f_elapsed, f_request, f_result)
    VALUES (JSON_VALUE(params, '$.session'), JSON_VALUE(@header, '$.f_id'), @timestartd, @timeendd,
            TIMESTAMPDIFF(SECOND, @timestartd, @timeendd), params,
            JSON_OBJECT('status', CAST(@STATUS AS unsigned), 'message', @message, 'state',
                        cast(JSON_VALUE(@header, '$.f_state') AS unsigned)));
END$$

delimiter ;