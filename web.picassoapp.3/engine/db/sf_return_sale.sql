delimiter $$


DROP procedure if EXISTS sf_create_return_of_sale$$
CREATE procedure sf_create_return_of_sale(params JSON)
BEGIN
DECLARE id CHAR(36) COLLATE UTF8_GENERAL_CI DEFAULT JSON_VALUE(params, '$.id');
DECLARE goods JSON;
DECLARE EXIT HANDLER FOR SQLEXCEPTION 
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

SET @vv = "1.1";
SELECT CURRENT_TIMESTAMP() INTO @t1;

SELECT json_arrayagg(JSON_OBJECT('f_id', g.f_id, 'f_goods', g.f_goods, 'f_price', ad.f_price, 'f_qty', g.f_qty, 'f_returnfrom', g.f_returnfrom)) INTO goods
	FROM o_goods g 
	LEFT JOIN o_goods gr ON gr.f_id=g.f_returnfrom 
	LEFT JOIN a_store_draft ad ON ad.f_id=gr.f_storerec 
	WHERE g.f_header=id;
	
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
		'f_userid', 0,
		'f_state', 1,
		'f_type', 1,
		'f_date', JSON_VALUE(params, '$.date'),
		'f_operator',JSON_VALUE(params, '$.user'),
		'f_partner', 0,
		'f_amount', 0,
		'f_currency', JSON_VALUE(params, '$.currency'),
		'f_storein', cast(JSON_VALUE(params, '$.storein') AS UNSIGNED),
		'f_storeout', 0,
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