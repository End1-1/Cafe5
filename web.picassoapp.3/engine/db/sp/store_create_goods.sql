delimiter $$

DROP FUNCTION if EXISTS sf_create_store_goods$$
DROP PROCEDURE if EXISTS sf_create_store_goods$$
CREATE procedure sf_create_store_goods(params JSON)
sf_create_store_goods:BEGIN
-- 16/10/24 10:12
DECLARE storerec CHAR(36) CHARACTER SET LATIN1 COLLATE LATIN1_GENERAL_CI;
SET @ftype = JSON_VALUE(params, '$.ftype');
SET @store = JSON_VALUE(params, '$.store');
SET @state = JSON_VALUE(params, '$.state');
SET @partnerid = JSON_VALUE(params, '$.partner');
SELECT JSON_EXTRACT(params, '$.goods') INTO @tgoods;



SET @result_code = 1;
SET @result_message = '';
SET @i_csg = 0;
label1: while (@i_csg < JSON_LENGTH(@tgoods)) DO 
SELECT JSON_EXTRACT(@tgoods, CONCAT('$[', @i_csg, ']')) INTO @gg;
-- draft
if (JSON_VALUE(@gg, '$.f_id') IS NULL) then
	SET @draftid = UUID();
ELSE 
	SET @draftid = JSON_VALUE(@gg, '$.f_id');
END if;
INSERT INTO a_store_draft (f_id, f_document, f_store, f_type, f_goods, 
	f_qty, f_price, f_reason, f_base, f_row, f_comment)
	VALUES (@draftid, JSON_VALUE(@header, '$.f_id'),   @store, @ftype, JSON_VALUE(@gg, '$.f_goods'), 
	JSON_VALUE(@gg, '$.f_qty'), JSON_VALUE(@gg, '$.f_price'), JSON_VALUE(@header, '$.f_reason'), @draftid, JSON_VALUE(@gg, '$.f_row'), JSON_VALUE(@gg, '$.f_comment'));
	
	SET storerec = JSON_VALUE(@gg, '$.f_storerec');
	if (storerec IS NOT NULL) then
	UPDATE o_goods SET f_storerec=@draftid WHERE f_id=storerec;
	END if;
	
if (@state = 1 AND @ftype = 1) then	
	INSERT INTO a_store (f_id, f_document, f_store, f_type, f_goods, 
		f_qty, f_price, f_reason, f_base, f_basedoc, f_draft)
		VALUES (@draftid, JSON_VALUE(@header, '$.f_id'), @store, @ftype, JSON_VALUE(@gg, '$.f_goods'), 
		JSON_VALUE(@gg, '$.f_qty'), JSON_VALUE(@gg, '$.f_price'), JSON_VALUE(@header, '$.f_reason'), @draftid, JSON_VALUE(@header, '$.f_id'), @draftid);
		
	UPDATE c_goods SET f_lastinputprice=JSON_VALUE(@gg, '$.f_price') , f_supplier=@partnerid WHERE f_id=JSON_VALUE(@gg, '$.f_goods');
END if;

if (@state=1 AND @ftype=-1) then
	SELECT JSON_OBJECT('goods', JSON_DETAILED(@gg), 'date', JSON_VALUE(params, '$.date'), 
		'store', @store, 'draftid', @draftid, 'f_document', JSON_VALUE(@header, '$.f_id'), 'move', JSON_VALUE(params, '$.move'), 'storein', JSON_VALUE(params, '$.storein')) INTO @o;
	call sf_create_store_out(@o);
	SELECT @create_store_out INTO @result;
	if (JSON_VALUE(@result, '$.status') = 0) then
		if (@result_code = 1) then
			SET @result_message = 'Բացասական քանակ\n';
		END if;
		SET @result_code=0;
		SELECT CONCAT_WS('\n', @result_message, JSON_VALUE(@result, '$.message')) INTO @result_message;
		#select JSON_OBJECT('status', 0, 'message', JSON_VALUE(@result, '$.message')) INTO @create_store_goods;
		#leave sf_create_store_goods;
	END if;
END if;
	
SET @i_csg = @i_csg + 1;
END while label1;

select JSON_OBJECT('status', @result_code, 'message', @result_message) INTO @create_store_goods;

END$$

delimiter ;