



DROP PROCEDURE if EXISTS sf_create_shop_tax$$
CREATE PROCEDURE sf_create_shop_tax(params JSON)
BEGIN
#12/12/24
	DECLARE order_id CHAR(36) COLLATE latin1_general_ci;
	DECLARE tax_id CHAR(36) COLLATE latin1_general_ci;
	SET order_id = JSON_VALUE(params, '$.f_order');
	UPDATE o_tax_log SET f_state=0 WHERE f_order=order_id;
	SET @reply = JSON_extract(params, '$.f_out');
	INSERT INTO o_tax_log (f_id, f_order, f_date, f_time, f_elapsed, f_in, f_out, f_err, f_result, f_state)
		VALUES (UUID(), order_id, CURRENT_DATE(), CURRENT_TIME(), JSON_VALUE(params, '$.f_elapsed'),
		JSON_extract(params, '$.f_in'), @reply, JSON_VALUE(params, '$.f_err'),
		JSON_VALUE(params, '$.f_result'), JSON_VALUE(params, '$.f_state'));
	if (JSON_VALUE(params, '$.f_state') = 1) then
		SELECT f_id INTO tax_id FROM o_tax WHERE f_id=order_id;
		if (tax_id IS NULL) then
			INSERT INTO  o_tax (f_id, f_receiptnumber) VALUES (order_id, JSON_VALUE(@reply, '$.rseq'));
		ELSE 
			UPDATE o_tax SET f_receiptnumber=JSON_VALUE(@reply, '$.rseq') WHERE f_id=order_id;
		END if;
	END if;
END$$

DROP PROCEDURE if EXISTS sf_shop_process_reserve$$
CREATE PROCEDURE sf_shop_process_reserve(params JSON)
BEGIN
	DECLARE goods_list longtext;
	DECLARE goods longtext;
	DECLARE i INT;
	DECLARE qty DECIMAL(14,2);
	DECLARE store INT;
	DECLARE goods_id INT;
	SELECT JSON_EXTRACT(params, '$.goods_list') INTO goods_list;
	SET i = 0;
	while (i < JSON_LENGTH(goods_list)) DO
		SELECT JSON_EXTRACT(goods_list, CONCAT('$[', i, ']')) INTO goods;
		SELECT JSON_VALUE(goods, '$.qty') INTO qty;
		SELECT JSON_VALUE(goods, '$.store') INTO store;
		SELECT JSON_VALUE(goods, '$.goods') INTO goods_id;
		update a_store_sale set f_qtyreserve=f_qtyreserve+qty where f_store=store and f_goods=goods_id;
		SET i = i + 1; 
	END while;
END$$






DROP FUNCTION if EXISTS sf_shop_add_goods$$
CREATE function sf_shop_add_goods(params JSON)
RETURNS JSON
BEGIN
DECLARE scancode VARCHAR(32) DEFAULT JSON_VALUE(params, '$.scancode');
DECLARE qty DECIMAL(14,3) DEFAULT 0;
DECLARE goods INT DEFAULT 0;
DECLARE store INT DEFAULT JSON_VALUE(params, '$.store');

#18/06/24 11:20
select c.f_id, c.f_code, sum(h.f_amount) as f_amount INTO goods, @f_code, @f_amount
	from b_gift_card_history h 
	inner join b_gift_card c on c.f_id=h.f_card 
	where c.f_code=scancode;
if (goods IS NOT NULL) then
	RETURN JSON_OBJECT('status', 1, 'data', JSON_DETAILED(JSON_OBJECT('type', 2, 'f_id', CAST(goods AS unsigned), 'f_code', @f_code, 'f_amount', @f_amount)));
END if;

SELECT g.f_id, g.f_name, coalesce(g.f_service, 0), coalesce(u.f_defaultqty, 0),
	if (cp.f_price1disc>0, cp.f_price1disc, cp.f_price1), if(cp.f_price2disc>0, cp.f_price2disc, cp.f_price2) 
	INTO goods, @f_name, @f_service, qty, @f_price1, @f_price2
	FROM c_goods g
	left join c_goods_multiscancode m ON g.f_id=m.f_goods 
	LEFT JOIN c_goods_prices cp ON cp.f_goods=g.f_id AND cp.f_currency=1
	LEFT JOIN c_units u ON u.f_id=g.f_unit
	WHERE m.f_id=scancode OR g.f_scancode=scancode
	LIMIT 1;
if (goods IS NULL) then
	RETURN JSON_OBJECT('status', 0, 'data', CONCAT_WS(' ', 'Անհայտ բարկոդ', scancode));
END if;

SELECT f_value INTO @f_donotcheckstore
	FROM s_settings_values s
	LEFT JOIN s_settings_names n ON n.f_id=s.f_settings
	WHERE s.f_key=86 AND n.f_name=JSON_VALUE(params, '$.settings');
	INSERT INTO a_result(f_result) VALUES (params);
	
if (@f_service=0 AND @f_donotcheckstore='0') then
	select coalesce(sum(f_type*f_qty), 0) into @store_qty from a_store a where f_store=store and f_goods=goods;
	select coalesce(sum(f_qty), 0) into @reserve_qty from a_store_reserve where f_state=1 and f_store=store and f_goods=goods;
	SELECT SUM(f_qty) INTO @draft_qty FROM o_draft_sale_body WHERE f_store=store AND f_goods=goods AND f_state=1;
	select @store_qty - @reserve_qty - COALESCE(@draft_qty, 0) - qty into @f_qty;
	#select f_qty-f_qtyreserve-f_qtyprogram-qty INTO @f_qty from a_store_sale where f_store=store and f_goods=goods;
	if(@f_qty < 0 OR @f_qty IS null) then
		RETURN JSON_OBJECT('status', 0, 'data', CONCAT_WS(' ', 'Բացասական քանակ', @f_qty));
	END if;
END if;
	
RETURN JSON_OBJECT('status', 1, 'data', JSON_DETAILED(JSON_OBJECT('type', 1, 'f_id', cast(goods AS unsigned), 'storeqty', CAST(@f_qty + qty AS DECIMAL),
	'price1', CAST(@f_price1 AS DECIMAL), 'price2', cast(@f_price2 AS DECIMAL))));
END$$

DROP FUNCTION if EXISTS sf_check_goods_qty$$
CREATE FUNCTION sf_check_goods_qty (params JSON)
RETURNS JSON
BEGIN
	#04/07/24 13:56
	DECLARE store INT DEFAULT JSON_VALUE(params, '$.store');
	DECLARE goods INT DEFAULT JSON_VALUE(params, '$.goods');
	DECLARE qty DECIMAL(14,2) default JSON_VALUE(params, '$.qty');
	select sum(f_qty) into @store_qty from a_store_sale a where f_store=store and f_goods=goods;
	select coalesce(sum(f_qty), 0) into @reserve_qty from a_store_reserve where f_state=1 and f_store=store and f_goods=goods;
	SELECT SUM(f_qty) INTO @draft_qty FROM o_draft_sale_body WHERE f_store=store AND f_goods=goods AND f_state=1;
	select @store_qty - @reserve_qty - COALESCE(@draft_qty, 0) - qty + JSON_VALUE(params, '$.oldqty') into @f_qty;
	if (@f_qty<0 OR @f_qty IS NULL) then
			RETURN JSON_OBJECT('status', 0, 'data', CONCAT_WS(' ', 'Բացասական քանակ', @f_qty, '\r\n', 'Ամրագրված', @reserve_qty, '\r\nՍևագրում', @draft_qty - JSON_VALUE(params, '$.oldqty')));
	END if;
	RETURN JSON_OBJECT('status', 1, 'storeqty', CAST(@f_qty + qty AS DECIMAL));
END $$











DROP PROCEDURE if EXISTS sf_create_return_of_sale$$
CREATE PROCEDURE `sf_create_return_of_sale`(
	IN `params` JSON
)
LANGUAGE SQL
NOT DETERMINISTIC
CONTAINS SQL
SQL SECURITY DEFINER
COMMENT ''
BEGIN
DECLARE id CHAR(36) COLLATE UTF8_GENERAL_CI DEFAULT JSON_VALUE(params, '$.id');
DECLARE goods JSON;
DECLARE EXIT HANDLER FOR SQLEXCEPTION 
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

#16/10/2024 11:30
SELECT CURRENT_TIMESTAMP() INTO @t1;

SELECT json_arrayagg(JSON_OBJECT('f_id', g.f_id, 'f_goods', g.f_goods, 'f_price', ad.f_price, 'f_qty', g.f_qty, 'f_returnfrom', g.f_returnfrom)) INTO goods
	FROM o_goods g 
	LEFT JOIN o_goods gr ON gr.f_id=g.f_returnfrom 
	LEFT JOIN a_store_draft ad ON ad.f_id=gr.f_storerec 
	WHERE g.f_header=id;
	
	SELECT CONCAT_WS(' ', 'Վերադարձ', CONCAT(oh.f_prefix, oh.f_hallid)), 
		f_amounttotal, f_datecash, f_hall , f_currency, f_id
		INTO @COMMENT, @total, @return_date, @hall, @curr, @orderid
		FROM o_header oh WHERE oh.f_id=id;
	INSERT INTO b_clients_debts (f_order, f_date, f_costumer, f_amount, f_source, f_flag, f_currency) 
		VALUES (@orderid, @return_date, JSON_VALUE(params, '$.partner'), @total*-1, 2, @hall, @curr);
	
	SET @aheader_id = UUID();
	SELECT JSON_OBJECT(
		'f_accepted', JSON_VALUE(params, '$.user'),
		'f_passed', JSON_VALUE(params, '$.user'),
		'f_invoice', '',
		'f_invoicedate', NULL,
		'f_storein', cast(JSON_VALUE(params, '$.storein') AS UNSIGNED), 
		'f_storeout', 0,
		'f_complectationcode', 0,
		'f_complectationqty', 0,
		'f_cashdoc', '',
		'f_basedonsale', 1,
		'f_saleuuid', id
	) INTO @headerbody;
	SELECT JSON_OBJECT(
		'f_id', @aheader_id,
		'f_userid', NULL,
		'f_state', 1,
		'f_nodebtop', 1,
		'f_type', 1,
		'f_date', JSON_VALUE(params, '$.date'),
		'f_operator',JSON_VALUE(params, '$.user'),
		'f_comment', @COMMENT,
		'f_partner', JSON_VALUE(params, '$.partner'),
		'f_amount', 0,
		'f_currency', JSON_VALUE(params, '$.currency'),
		'f_storein', cast(JSON_VALUE(params, '$.storein') AS UNSIGNED),
		'f_storeout', 0,
		'partner', JSON_DETAILED(JSON_OBJECT('partner', JSON_VALUE(params, '$.partner'))),
		'f_reason', 9) INTO @aheader;    
	SET @i = 0;
	SET @draft_goods = JSON_ARRAY();
	while (@i < JSON_LENGTH(goods)) DO
		SELECT JSON_EXTRACT(goods, CONCAT('$[', @i, ']')) INTO @g;
		SELECT JSON_OBJECT(
			'f_id', JSON_VALUE(@g, '$.f_id'),
			'f_goods', JSON_VALUE(@g, '$.f_goods'),
			'f_qty', JSON_VALUE(@g, '$.f_qty'),
			'f_price', JSON_VALUE(@g, '$.f_price'),
			'f_validto', NULL,
			'f_comment', '',
			'f_row', @i) INTO @d;
		SELECT JSON_MERGE(@draft_goods, @d) INTO @draft_goods;
		SET @i = @i + 1;
	END while;
	SET @store_session = UUID();
	SELECT JSON_OBJECT(
		'session', @store_session,
		'donotcheckstore', 0,
		'header', JSON_DETAILED(@aheader),
		'body', JSON_DETAILED(@headerbody),
		'goods', JSON_DETAILED(@draft_goods)
		) INTO @headerparams;
	CALL sf_create_store_document(@headerparams);

SELECT CURRENT_TIMESTAMP() INTO @t2;
INSERT INTO a_result (f_session, f_timestamp, f_done, f_elapsed, f_request, f_result) 
	VALUES (JSON_VALUE(params, '$.session'), @t1, @t2, TIMESTAMPDIFF(SECOND, @t1, @t2), params, 
	JSON_OBJECT('status', 1, 'message', 'ok'));

END$$

DROP FUNCTION if EXISTS sf_bill_info$$
CREATE DEFINER=`root`@`%` FUNCTION `sf_bill_info`(
	`params` JSON
)
RETURNS longtext CHARSET utf8mb4 COLLATE utf8mb4_bin
LANGUAGE SQL
NOT DETERMINISTIC
CONTAINS SQL
SQL SECURITY DEFINER
COMMENT ''
BEGIN
DECLARE id CHAR(36) CHARACTER SET LATIN1 COLLATE LATIN1_GENERAL_CI;
SELECT JSON_VALUE(params, '$.id') INTO id;

SELECT f_config INTO @config FROM s_config WHERE f_name='Sale';

select json_arrayagg(JSON_OBJECT('f_en', f_en, 'f_am', f_am, 'f_ru', f_ru)) INTO @translator from s_translator;

SELECT JSON_OBJECT(
	'f_id', id,
	'f_prefix', h.f_prefix,
	'f_hallid', h.f_hallid,
	'f_currentstaffname', concat_ws(' ', left(u.f_last, 1), '.', u.f_first),
	'f_tablename', t.f_name,
	'f_hallname', hl.f_name,
	'f_amounttotal', h.f_amounttotal, 
	'f_amountcash', h.f_amountcash,
	'f_amountcard', h.f_amountcard,
	'f_amountidram', h.f_amountidram,
	'f_amountbank', h.f_amountbank,
	'f_amounttelcell', h.f_amountTelcell,
	'f_amountpayx', h.f_amountPayX,
	'f_amountservice', h.f_amountservice,
	'f_servicefactor', h.f_servicefactor,
	'f_discountfactor', h.f_discountfactor,
	'f_amountdiscount', h.f_amountdiscount,
	'f_state', h.f_state,
	'f_print', h.f_print,
	'f_comment', h.f_comment) 
	
	INTO @header
	FROM o_header h
	left join h_tables t on t.f_id=h.f_table 
	left join h_halls hl on hl.f_id=t.f_hall 
	LEFT join s_user u on u.f_id=h.f_currentstaff
	WHERE h.f_id=id;
	
SELECT JSON_merge(f_in, f_out) INTO @fiscal FROM o_tax_log WHERE f_state=1 AND f_order=id;

SELECT json_arrayagg(JSON_OBJECT(
	'f_name_am', d.f_name, 
	'f_name_ru', dt.f_ru,
	'f_name_en', dt.f_en,
	'f_adgt', if (length(coalesce(d.f_adgt, '')) > 0, d.f_adgt, p.f_adgcode) , 
   'f_qty1', b.f_qty1, 
   'f_qty2', b.f_qty2,
	'f_price', b.f_price, 
	'f_discount', b.f_discount, 
	'f_canservice', b.f_canservice, 
	'f_candiscount', b.f_candiscount,
	'f_state', b.f_state,
	'f_hourlypayment', d.f_hourlypayment,
	'f_extra', d.f_extra,
	'f_comment', b.f_comment,
	'f_total', b.f_total,
	'f_taxdept', p.f_taxdept, 
	'f_id', cast(d.f_id AS VARCHAR(32)) )) 
                      
	INTO @dishes
   from o_body b 
   left join d_dish d on d.f_id=b.f_dish 
   left join d_part2 p on p.f_id=d.f_part 
   left join d_translator dt on dt.f_id=d.f_id
   left join o_header o on o.f_id=b.f_header 
   WHERE b.f_state=1 and o.f_id=id;
   
SELECT JSON_OBJECT('carnumber', f_govnumber) INTO @car FROM b_car WHERE f_id=id;

RETURN JSON_OBJECT('status', 1, 'data', JSON_OBJECT(
	'config', JSON_DETAILED(@config), 
	'translator', JSON_DETAILED(@translator),
	'dishes', JSON_DETAILED(@dishes),
	'header', JSON_DETAILED(@header),
	'car', JSON_DETAILED(@car),
	'fiscal', JSON_DETAILED(@fiscal)));
END$$