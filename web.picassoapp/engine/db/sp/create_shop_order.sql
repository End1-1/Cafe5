DELIMITER $$
DROP PROCEDURE IF EXISTS sf_create_shop_order;
CREATE PROCEDURE sf_create_shop_order(params JSON)
sp:
BEGIN

    DECLARE jfiscal json;
    DECLARE jdata json DEFAULT '{}';

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            ROLLBACK;
            RESIGNAL;
        END;


#27/02/2026 12:48
    SELECT CURRENT_TIMESTAMP() INTO @t1;
    SELECT JSON_EXTRACT(params, '$.header') INTO @jh;
    SELECT JSON_EXTRACT(params, '$.draft') INTO @jd;
    SELECT JSON_EXTRACT(params, '$.goods') INTO @jg;
    SELECT JSON_EXTRACT(params, '$.history') INTO @jhistory;
    SELECT JSON_EXTRACT(params, '$.accumulate') INTO @jaccumulate;
    SELECT JSON_EXTRACT(params, '$.flags') INTO @flags;
    SELECT JSON_OBJECT('status', CAST(1 AS INT), 'message', '') INTO @result;
    SELECT JSON_EXTRACT(params, '$.fiscal') INTO jfiscal;
    SELECT sv.f_value
    INTO @cashid
    FROM s_settings_values sv
    WHERE sv.f_key = 32
      AND f_settings = (SELECT f_settings FROM h_halls WHERE f_id = JSON_VALUE(@jh, '$.f_hall') LIMIT 1);
    SELECT sv.f_value
    INTO @nocashid
    FROM s_settings_values sv
    WHERE sv.f_key = 35
      AND f_settings = (SELECT f_settings FROM h_halls WHERE f_id = JSON_VALUE(@jh, '$.f_hall') LIMIT 1);

    START TRANSACTION;

#clear previuosly saved data
    SELECT f_id INTO @tid FROM o_header WHERE f_id = JSON_VALUE(@jh, '$.f_id');
    IF (@tid IS NOT NULL) THEN
        SELECT f_id INTO @t FROM a_header_cash WHERE f_oheader = JSON_VALUE(@jh, '$.f_id');
        DELETE FROM b_clients_debts WHERE f_order = JSON_VALUE(@jh, '$.f_id');
        DELETE FROM e_cash WHERE f_header = @t;
        DELETE FROM a_header_cash WHERE f_id = @t;
        DELETE FROM a_header WHERE f_id = @t;
        DELETE FROM o_goods WHERE f_header = JSON_VALUE(@jh, '$.f_id');
        DELETE FROM b_history WHERE f_id = JSON_VALUE(@jh, '$.f_id');
        DELETE FROM o_header WHERE f_id = JSON_VALUE(@jh, '$.f_id');
        DELETE FROM o_header_flags WHERE f_id = JSON_VALUE(@jh, '$.f_id');
    END IF;
#delete from o_draft_sale_body where f_header=@tid;
    DELETE FROM o_draft_sale WHERE f_id = JSON_VALUE(@jh, '$.f_id');

#hall id
    IF JSON_VALUE(@jh, '$.f_hallid') = 0 OR JSON_VALUE(@jh, '$.f_hallid') IS NULL THEN
        SELECT f_counter + 1, f_prefix
        INTO @counter, @prefix
        FROM h_halls
        WHERE f_id = JSON_VALUE(@jh, '$.f_hall') FOR
        UPDATE;
        UPDATE h_halls SET f_counter=@counter WHERE f_id = JSON_VALUE(@jh, '$.f_hall');
        SET @jh = JSON_SET(@jh, '$.f_hallid', @counter);
        SET @jh = JSON_SET(@jh, '$.f_prefix', @prefix);
        SET @result = JSON_SET(@result, '$.f_hallid', @counter);
        SET @result = JSON_SET(@result, '$.f_prefix', @prefix);
    END IF;


#o_header
    SET jdata = JSON_SET(jdata, '$.f_fiscal', COALESCE(JSON_COMPACT(jfiscal), JSON_OBJECT()));
    INSERT INTO `o_header` (`f_id`, `f_hallid`, `f_prefix`, `f_state`, `f_hall`,
                            `f_table`, `f_dateOpen`, `f_dateClose`, `f_dateCash`, `f_timeOpen`,
                            `f_timeClose`, `f_cashier`, `f_staff`, `f_comment`, `f_print`,
                            `f_amountTotal`, `f_amountCash`, `f_amountCard`, `f_amountprepaid`, `f_amountBank`,
                            `f_amountcredit`, `f_amountIdram`, `f_amountTelcell`, `f_amountDebt`, `f_amountPayX`,
                            `f_amountOther`, `f_amountService`, `f_amountDiscount`,
                            `f_serviceFactor`, `f_discountFactor`, `f_creditCardId`, `f_otherId`, `f_shift`,
                            `f_source`, `f_saletype`, `f_partner`, `f_currentstaff`, `f_guests`, `f_precheck`,
                            `f_cash`, `f_change`, `f_hotel`, `f_currency`, `f_taxpayertin`, `f_data`)
    VALUES (JSON_VALUE(@jh, '$.f_id'), @counter, @prefix, JSON_VALUE(@jh, '$.f_state'), JSON_VALUE(@jh, '$.f_hall'),
            JSON_VALUE(@jh, '$.f_table'), JSON_VALUE(@jh, '$.f_dateopen'), JSON_VALUE(@jh, '$.f_dateclose'),
            JSON_VALUE(@jh, '$.f_datecash'), JSON_VALUE(@jh, '$.f_timeopen'),
            JSON_VALUE(@jh, '$.f_timeclose'), JSON_VALUE(@jh, '$.f_cashier'), JSON_VALUE(@jh, '$.f_staff'),
            JSON_VALUE(@jh, '$.f_comment'), JSON_VALUE(@jh, '$.f_print'),
            JSON_VALUE(@jh, '$.f_amounttotal'), JSON_VALUE(@jh, '$.f_amountcash'), JSON_VALUE(@jh, '$.f_amountcard'),
            JSON_VALUE(@jh, '$.f_amountprepaid'), JSON_VALUE(@jh, '$.f_amountbank'),
            JSON_VALUE(@jh, '$.f_amountcredit'), JSON_VALUE(@jh, '$.f_amountidram'),
            JSON_VALUE(@jh, '$.f_amounttelcell'), JSON_VALUE(@jh, '$.f_amountdebt'), JSON_VALUE(@jh, '$.f_amountpayx'),
            JSON_VALUE(@jh, '$.f_amountother'), JSON_VALUE(@jh, '$.f_amountservice'),
            JSON_VALUE(@jh, '$.f_amountdiscount'),
            JSON_VALUE(@jh, '$.f_servicefactor'), JSON_VALUE(@jh, '$.f_discountfactor'),
            JSON_VALUE(@jh, '$.f_creditcardid'), JSON_VALUE(@jh, '$.f_otherid'), JSON_VALUE(@jh, '$.f_shift'),
            JSON_VALUE(@jh, '$.f_source'), JSON_VALUE(@jh, '$.f_saletype'), JSON_VALUE(@jh, '$.f_partner'),
            JSON_VALUE(@jh, '$.f_currentstaff'), JSON_VALUE(@jh, '$.f_guests'), JSON_VALUE(@jh, '$.f_precheck'),
            JSON_VALUE(@jh, '$.f_cash'), JSON_VALUE(@jh, '$.f_change'), JSON_VALUE(@jh, '$.f_hotel'),
            JSON_VALUE(@jh, '$.f_currency'), JSON_VALUE(@jh, '$.f_taxpayertin'),
            jdata);


#o_header_flags
    INSERT INTO o_header_flags(f_id, f_1, f_2, f_3, f_4, f_5)
    VALUES (JSON_VALUE(@jh, '$.f_id'),
            JSON_VALUE(@flags, '$.f_1'), JSON_VALUE(@flags, '$.f_2'), JSON_VALUE(@flags, '$.f_3'),
            JSON_VALUE(@flags, '$.f_4'), JSON_VALUE(@flags, '$.f_5'));


    INSERT INTO a_result(f_request, f_result) VALUES ('tuta7', JSON_VALUE(@jh, '$.f_id'));
#o_draft_sale
    INSERT INTO o_draft_sale (f_id, f_state, f_date, f_datefor)
    VALUES (JSON_VALUE(@jh, '$.f_id'), JSON_VALUE(@jh, '$.f_state'), JSON_VALUE(@jh, '$.f_datecash'),
            JSON_VALUE(@jd, 'f_delivery'));

#b_history
    IF (JSON_VALUE(@jhistory, '$.f_type') > 0) THEN
        #kostil
        DELETE FROM b_history WHERE f_id = JSON_VALUE(@jh, '$.f_id');
        INSERT INTO b_history (f_id, f_type, f_card, f_value, f_data)
        VALUES (JSON_VALUE(@jh, '$.f_id'), JSON_VALUE(@jhistory, '$.f_type'), JSON_VALUE(@jhistory, '$.f_card'),
                JSON_VALUE(@jhistory, '$.f_value'), JSON_VALUE(@jhistory, '$.f_data'));
    END IF;


#accumulate card
    IF (@jaccumulate IS NOT NULL) THEN
        IF (JSON_VALUE(@jaccumulate, '$.accumulatespend') > 0) THEN
            INSERT INTO b_gift_card_history (f_card, f_trsale, f_amount)
            VALUES (JSON_VALUE(@jaccumulate, '$.card'), JSON_VALUE(@jh, '$.f_id'),
                    COALESCE(JSON_VALUE(@jaccumulate, '$.accumulatespend'), 0) * -1);
        ELSE
            INSERT INTO b_gift_card_history (f_card, f_trsale, f_amount)
            VALUES (JSON_VALUE(@jaccumulate, '$.card'), JSON_VALUE(@jh, '$.f_id'),
                    COALESCE(JSON_VALUE(@jaccumulate, '$.accumulate'), 0));
        END IF;
    END IF;


#goods
    SET @i = 0;
    SET @needstore = 0;
    WHILE (@i < JSON_LENGTH(@jg))
        DO
            SELECT JSON_EXTRACT(@jg, CONCAT('$[', @i, ']')) INTO @g;
            SET @store = JSON_VALUE(@g, '$.f_store');
            IF (JSON_VALUE(@g, '$.f_isservice') = 0) THEN
                SET @needstore = 1;
            END IF;
            INSERT INTO `o_goods` (`f_id`, `f_header`, `f_body`, `f_store`, `f_goods`, `f_qty`,
                                   `f_price`, `f_total`, `f_tax`, `f_sign`, `f_taxdept`, `f_adgcode`, `f_row`,
                                   `f_storerec`, `f_discountfactor`, `f_discountmode`, `f_return`, `f_returnfrom`,
                                   `f_discountAmount`, `f_isservice`, `f_amountAccumulate`, `f_emarks`)
            VALUES (JSON_VALUE(@g, '$.f_id'), JSON_VALUE(@jh, '$.f_id'), '', JSON_VALUE(@g, '$.f_store'),
                    JSON_VALUE(@g, '$.f_goods'), JSON_VALUE(@g, '$.f_qty'),
                    JSON_VALUE(@g, '$.f_price'), JSON_VALUE(@g, '$.f_total'), JSON_VALUE(@g, '$.f_tax'),
                    JSON_VALUE(@g, '$.f_sign'), JSON_VALUE(@g, '$.f_taxdebt'), JSON_VALUE(@g, '$.f_adgcode'),
                    JSON_VALUE(@g, '$.f_row'),
                    JSON_VALUE(@g, '$.f_id'), JSON_VALUE(@g, '$.f_discountfactor'), JSON_VALUE(@g, '$.f_discountmode'),
                    JSON_VALUE(@g, '$.f_return'), JSON_VALUE(@g, '$.f_returnfrom'),
                    JSON_VALUE(@g, '$.f_discountamount'), JSON_VALUE(@g, '$.f_isservice'),
                    JSON_VALUE(@g, '$.f_amountaccumulate'), JSON_VALUE(@g, '$.f_emarks'));
            UPDATE a_store_sale
            SET f_qty=f_qty - JSON_VALUE(@g, '$.f_qty'),
                f_qtyprogram=f_qtyprogram - JSON_VALUE(@g, '$.f_qty')
            WHERE f_goods = JSON_VALUE(@g, '$.f_goods')
              AND f_store = JSON_VALUE(@g, '$.f_store');

            #sale gift card
            IF (@i = 0 AND JSON_VALUE(@jh, '$.f_saletype') = -1) THEN
                SELECT f_scancode INTO @scancode FROM c_goods WHERE f_id = JSON_VALUE(@g, '$.f_goods');
                SELECT f_id INTO @giftcardid FROM b_gift_card WHERE f_code = CONCAT('2211', @scancode);
                IF (@giftcardid IS NOT NULL) THEN
                    SELECT f_id
                    INTO @costumerid
                    FROM c_partners
                    WHERE f_phone = JSON_VALUE(params, '$.contact')
                    LIMIT 1;
                    IF (@costumerid IS NULL) THEN
                        INSERT INTO c_partners (f_phone, f_taxcode, f_contact, f_name)
                        VALUES (JSON_VALUE(params, '$.contact'), 'giftcard', JSON_VALUE(params, '$.organization'),
                                JSON_VALUE(params, '$.organization'));
                        SELECT LAST_INSERT_ID() INTO @costumerid;
                    END IF;

                    UPDATE b_gift_card
                    SET f_costumer=@costumerid,
                        f_datesaled=JSON_VALUE(@jh, '$.f_datecash'),
                        f_valid_until = DATE_ADD(CURRENT_DATE, INTERVAL 180 DAY)
                    WHERE f_id = @giftcardid;
                    INSERT INTO b_gift_card_history (f_card, f_trsale, f_amount)
                    VALUES (@giftcardid, JSON_VALUE(@jh, '$.f_id'), JSON_VALUE(@jh, '$.f_amounttotal'));
                ELSE
                    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'GIFT ERROR 1';
                END IF;
            END IF;
            SET @i = @i + 1;
        END WHILE;

#gift card used
    SELECT JSON_VALUE(params, '$.giftcard') INTO @giftcard;
    IF (@giftcard > 0) THEN
        IF (JSON_VALUE(@jh, '$.f_amountprepaid') > 0) THEN
            INSERT INTO b_gift_card_history (f_card, f_trsale, f_amount)
            VALUES (@giftcard, JSON_VALUE(@jh, '$.f_id'), JSON_VALUE(@jh, '$.f_amountprepaid') * -1);
        ELSE
            SET @costumerid = NULL;
            SELECT f_id INTO @costumerid FROM c_partners WHERE f_phone = JSON_VALUE(params, '$.contact');
            IF (@costumerid IS NULL) THEN
                INSERT INTO c_partners (f_phone, f_taxcode, f_contact, f_name)
                VALUES (JSON_VALUE(params, '$.contact'), 'giftcard', JSON_VALUE(params, '$.organization'),
                        JSON_VALUE(params, '$.organization'));
                SELECT LAST_INSERT_ID() INTO @costumerid;
            END IF;
            UPDATE b_gift_card SET f_costumer=@costumerid, f_datesaled=CURRENT_DATE WHERE f_id = @giftcard;
            SELECT f_code INTO @cardcode FROM b_gift_card WHERE f_id = @giftcard;
            IF (@cardcode IS NOT NULL) THEN
                SELECT f_value INTO @f_value FROM b_cards_discount WHERE f_code = @cardcode;
                IF (@f_value IS NOT NULL) THEN
                    INSERT INTO b_gift_card_history (f_card, f_trsale, f_amount)
                    VALUES (@giftcard, JSON_VALUE(@jh, '$.f_id'), JSON_VALUE(@jh, '$.f_amounttotal') * @f_value);
                END IF;
            END IF;
        END IF;
    END IF;

    #create cash doc
#cash
    IF (JSON_VALUE(@jh, '$.f_amountcash') > 0) THEN
        SET @cashheader = UUID();
        INSERT INTO a_header (f_id, f_userid, f_state, f_type,
                              f_operator, f_date, f_createdate, f_createtime,
                              f_partner, f_amount, f_comment, f_payment, f_paid, f_currency, f_sessionid, f_body)
        VALUES (@cashheader, CONCAT(@prefix, @counter), 1, 5,
                JSON_VALUE(@jh, '$.f_cashier'), JSON_VALUE(@jh, '$.f_datecash'), CURRENT_DATE(), CURRENT_TIME(),
                0, JSON_VALUE(@jh, '$.f_amountcash'), CONCAT('Վաճառք ', @prefix, @counter), 0,
                0, JSON_VALUE(@jh, '$.f_currency'), NULL, '{}');
        INSERT INTO a_header_cash (f_id, f_cashin, f_cashout, f_oheader, f_related)
        VALUES (@cashheader, @cashid, 0, JSON_VALUE(@jh, '$.f_id'), 1);
        INSERT INTO e_cash (f_id, f_cash, f_sign, f_header, f_remarks, f_amount, f_base, f_row)
        VALUES (UUID(), @cashid, 1, @cashheader, CONCAT('Վաճառք ', @prefix, @counter),
                JSON_VALUE(@jh, '$.f_amountcash'), '', 0);

    END IF;
#nocash
    SET @amountcard = JSON_VALUE(@jh, '$.f_amountcard') + JSON_VALUE(@jh, '$.f_amountidram') +
                      JSON_VALUE(@jh, '$.f_amounttelcell');
    IF (@amountcard > 0) THEN
        SET @cashheader = UUID();
        INSERT INTO a_header (f_id, f_userid, f_state, f_type,
                              f_operator, f_date, f_createdate, f_createtime,
                              f_partner, f_amount, f_comment, f_payment, f_paid, f_currency, f_sessionid, f_body)
        VALUES (@cashheader, CONCAT(@prefix, @counter), 1, 5,
                JSON_VALUE(@jh, '$.f_cashier'), JSON_VALUE(@jh, '$.f_datecash'), CURRENT_DATE(), CURRENT_TIME(),
                0, @amountcard, CONCAT('Վաճառք ', @prefix, @counter), 0,
                0, JSON_VALUE(@jh, '$.f_currency'), NULL, '{}');
        INSERT INTO a_header_cash (f_id, f_cashin, f_cashout, f_oheader, f_related)
        VALUES (@cashheader, @nocashid, 0, JSON_VALUE(@jh, '$.f_id'), 1);
        INSERT INTO e_cash (f_id, f_cash, f_sign, f_header, f_remarks, f_amount, f_base, f_row)
        VALUES (UUID(), @nocashid, 1, @cashheader, CONCAT('Վաճառք ', @prefix, @counter), @amountcard, '', 0);

    END IF;
#debt or bank
    SET @amountdebt = JSON_VALUE(@jh, '$.f_amountbank') + JSON_VALUE(@jh, '$.f_amountdebt');
    IF (@amountdebt > 0) THEN
        INSERT INTO `b_clients_debts` (`f_date`, `f_costumer`, `f_order`,
                                       `f_cash`, `f_amount`, `f_govnumber`, `f_storedoc`,
                                       `f_currency`, `f_source`, `f_flag`)
        VALUES (JSON_VALUE(@jh, '$.f_datecash'), JSON_VALUE(@jh, '$.f_partner'), JSON_VALUE(@jh, '$.f_id'),
                NULL, @amountdebt * -1, NULL, NULL,
                JSON_VALUE(@jh, '$.f_currency'), 1, JSON_VALUE(@jh, '$.f_hall'));

    END IF;


#create store document 
    IF (@needstore = 1) THEN

        SELECT f_value
        INTO @f_donotcheckstore
        FROM s_settings_values s
                 LEFT JOIN s_settings_names n ON n.f_id = s.f_settings
        WHERE s.f_key = 86
          AND n.f_name = JSON_VALUE(params, '$.settings');
        INSERT INTO a_result(f_result) VALUES (params);

        SET @aheader_id = UUID();
        SELECT JSON_OBJECT(
                       'f_accepted', JSON_VALUE(@jh, '$.f_cashier'),
                       'f_passed', JSON_VALUE(@jh, '$.f_cashier'),
                       'f_invoice', '',
                       'f_invoicedate', NULL,
                       'f_storein', 0,
                       'f_storeout', CAST(@store AS INT),
                       'f_complectationcode', 0,
                       'f_complectationqty', 0,
                       'f_cashdoc', '',
                       'f_basedonsale', 1,
                       'f_saleuuid', JSON_VALUE(@jh, '$.f_id')
               )
        INTO @headerbody;
        SELECT JSON_OBJECT(
                       'f_id', @aheader_id,
                       'f_userid', 0,
                       'f_state', 1,
                       'f_type', 2,
                       'f_date', JSON_VALUE(@jh, '$.f_datecash'),
                       'f_operator', JSON_VALUE(@jh, '$.f_cashier'),
                       'f_partner', 0,
                       'f_amount', 0,
                       'f_currency', JSON_VALUE(@jh, '$.f_currency'),
                       'f_storein', 0,
                       'f_comment', CONCAT('Վաճառք ', @prefix, @counter),
                       'f_storeout', CAST(@store AS INT),
                       'f_reason', 4)
        INTO @aheader;
        SET @i = 0;
        SET @draft_goods = JSON_ARRAY();
        WHILE (@i < JSON_LENGTH(@jg))
            DO
                SELECT JSON_EXTRACT(@jg, CONCAT('$[', @i, ']')) INTO @g;
                IF (JSON_VALUE(@g, '$.f_isservice') = 0) THEN
                    SELECT JSON_OBJECT(
                                   'f_id', JSON_VALUE(@g, '$.f_id'),
                                   'f_goods', JSON_VALUE(@g, '$.f_goods'),
                                   'f_qty', JSON_VALUE(@g, '$.f_qty'),
                                   'f_price', 0,
                                   'f_validto', NULL,
                                   'f_comment', '',
                                   'f_row', @i)
                    INTO @d;
                    SELECT JSON_MERGE(@draft_goods, @d) INTO @draft_goods;
                END IF;
                SET @i = @i + 1;
            END WHILE;
        SET @store_session = UUID();
        SELECT JSON_OBJECT(
                       'notransaction', 1,
                       'session', @store_session,
                       'donotcheckstore', @f_donotcheckstore,
                       'header', JSON_DETAILED(@aheader),
                       'body', JSON_DETAILED(@headerbody),
                       'goods', JSON_DETAILED(@draft_goods)
               )
        INTO @headerparams;
        #INSERT INTO a_header_store (f_id, f_saleuuid, f_baseonsale, f_body) VALUES (@aheader_id, JSON_VALUE(@jh, '$.f_id'), 1, @headerparams);
        #CALL sf_create_store_document(@headerparams);
        #SELECT JSON_REPLACE(@headerparams, '$.header.f_state', 1) INTO @headerparams;

        CALL sf_create_store_document(@headerparams);

        #if failed prevoise operation due infucient quantity on store we looking on flag do_no_check_store
        #and if this flag was set to '1', we creating draft document
        IF (@f_donotcheckstore = '1') THEN

            SELECT JSON_DETAILED(f_result) INTO @store_result FROM a_result WHERE f_session = @store_session;
            IF (JSON_VALUE(@store_result, '$.status') = 0) THEN
                SELECT JSON_REPLACE(@headerparams, '$.header.f_state', 2) INTO @headerparams;
                CALL sf_create_store_document(@headerparams);
            END IF;
        END IF;

    END IF;

#finally update draft
    UPDATE o_draft_sale SET f_state=2, f_datefor=JSON_VALUE(@jd, '$.f_delivery') WHERE f_id = JSON_VALUE(@jh, '$.f_id');

    #SELECT CURRENT_TIMESTAMP() INTO @t2;
#INSERT INTO a_result (f_session, f_timestamp, f_done, f_elapsed, f_request, f_result) 
#	VALUES (JSON_VALUE(params, '$.session'), @t1, @t2, TIMESTAMPDIFF(SECOND, @t1, @t2), params, JSON_DETAILED(@result));
#SELECT JSON_VALUE(@result, '$.status') INTO @a;
#SIGNAL SQLSTATE '45000'
#SET MESSAGE_TEXT = @a;
    IF (JSON_VALUE(@result, '$.status') = 1) THEN
        COMMIT;
    ELSE
        ROLLBACK;
    END IF;


    SELECT CURRENT_TIMESTAMP() INTO @t2;
    INSERT INTO a_result (f_session, f_timestamp, f_done, f_elapsed, f_request, f_result)
    VALUES (JSON_VALUE(params, '$.session'), @t1, @t2, TIMESTAMPDIFF(SECOND, @t1, @t2), params,
            JSON_OBJECT('status', CAST(JSON_VALUE(@result, '$.status') AS unsigned), 'message',
                        JSON_VALUE(@result, '$.message')));


END$$
