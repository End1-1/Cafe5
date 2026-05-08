delimiter $$
DROP FUNCTION if EXISTS sf_create_store_out$$
DROP PROCEDURE if EXISTS sf_create_store_out$$
CREATE procedure sf_create_store_out(params JSON)
BEGIN
DECLARE adraftid CHAR(36) CHARACTER SET latin1 COLLATE latin1_general_ci;
DECLARE goods_id INT;
DECLARE goods_store_id INT;
DECLARE store_id INT;
DECLARE qty DECIMAL(14,4);

#26/04/2025 14:27
SET adraftid = JSON_VALUE(params, '$.draftid');
SELECT JSON_EXTRACT(params, '$.goods') INTO @g; 
SELECT JSON_VALUE(@g, '$.f_goods') INTO goods_id;
SELECT JSON_VALUE(params, '$.store') INTO store_id;
SET qty = JSON_VALUE(@g, '$.f_qty');
SET @baseqty=qty;
SELECT f_storeid INTO goods_store_id FROM c_goods WHERE f_id=goods_id;
select coalesce(json_arrayagg(JSON_OBJECT('f_id', s.f_id, 'f_goods', s.f_goods, 'f_qty', sm.f_qty, 
	'f_price', sm.f_price, 'f_total', sm.f_total, 'f_document', s.f_document, 'f_base', s.f_base, 
	'f_basedoc', s.f_basedoc, 'f_date', d.f_date )), JSON_ARRAY()) INTO @sa
   from a_store s 
	left join ( 
	  select sum(ss.f_qty*ss.f_type) as f_qty, ss.f_price,  sum(ss.f_total*ss.f_type) as f_total, ss.f_base 
	  from a_store ss 
	  WHERE ss.f_store=store_id AND ss.f_goods=goods_store_id
	  group by ss.f_base  
	  having sum(ss.f_qty*ss.f_type)>0 
	  ) sm on sm.f_base=s.f_id
	inner join a_header d on d.f_id=s.f_document 
		where s.f_goods=goods_store_id and s.f_store=store_id and s.f_type=1 and sm.f_qty>0 
		order by d.f_date;
		
SET @i = 0;
SET @amount = 0;
label1: while (@i < JSON_LENGTH(@sa)) DO
	SELECT JSON_EXTRACT(@sa, CONCAT('$[', @i, ']')) INTO @s; 
	if (cast(JSON_VALUE(@s, '$.f_qty') AS DOUBLE) > qty) then
		INSERT INTO a_store (f_id, f_document, f_type, f_store, f_goods, f_qty, f_price, f_base, f_basedoc, f_draft, f_reason) 
			VALUES (UUID(), JSON_VALUE(params, '$.f_document'), -1, store_id, 
			goods_store_id, qty, JSON_VALUE(@s, '$.f_price'), 
			JSON_VALUE(@s, '$.f_base'), JSON_VALUE(@s, '$.f_basedoc'), JSON_VALUE(params, '$.draftid'), JSON_VALUE(@header, '$.f_reason'));
		SET @amount = @amount + (qty * JSON_VALUE(@s, '$.f_price'));
		SET qty = 0;
	ELSE 
		INSERT INTO a_store (f_id, f_document, f_type, f_store, f_goods, f_qty, f_price, f_base, f_basedoc, f_draft, f_reason) 
			VALUES (UUID(), json_value(params, '$.f_document'), -1, store_id, 
			goods_store_id, JSON_VALUE(@s, '$.f_qty'), JSON_VALUE(@s, '$.f_price'), 
			JSON_VALUE(@s, '$.f_base'), JSON_VALUE(@s, '$.f_basedoc'), JSON_VALUE(params, '$.draftid'), JSON_VALUE(@header, '$.f_reason'));
		SET @amount = @amount + (JSON_VALUE(@s, '$.f_qty') * JSON_VALUE(@s, '$.f_price'));		
		SET qty = qty - JSON_VALUE(@s, '$.f_qty');	
	END if;
	if (qty<0.001) then
		leave label1;
	END if;
	SET @i = @i + 1;
END while label1;	

UPDATE a_store_draft SET f_price=@amount/@baseqty WHERE f_id=adraftid;

if (JSON_VALUE(params, '$.move')=1) then
	SET @newdraft = UUID();
	INSERT INTO a_store_draft (f_id, f_document, f_store, f_type, f_goods, 
		f_qty, f_price, f_reason, f_base, f_row, f_comment)
		VALUES (@newdraft, JSON_VALUE(params, '$.f_document'), JSON_VALUE(params, '$.storein'), 1, JSON_VALUE(@g, '$.f_goods'), 
		JSON_VALUE(@g, '$.f_qty'), @amount/@baseqty, JSON_VALUE(@header, '$.f_reason'), @newdraft, JSON_VALUE(@g, '$.f_row'), JSON_VALUE(@g, '$.f_comment'));
	
	INSERT INTO a_store (f_id, f_document, f_store, f_type, f_goods, 
		f_qty, f_price, f_reason, f_base, f_basedoc, f_draft)
		VALUES (@newdraft, JSON_VALUE(params, '$.f_document'), JSON_VALUE(params, '$.storein'), 1, goods_store_id, 
		JSON_VALUE(@g, '$.f_qty'), @amount/@baseqty, JSON_VALUE(@header, '$.f_reason'), @newdraft, JSON_VALUE(params, '$.f_document'), @newdraft);
END if;
	

if (qty > 0) then
	SELECT f_name, f_scancode INTO @goods_name, @goods_scancode FROM c_goods WHERE f_id=goods_id;
	select JSON_OBJECT('status', 0, 'message', CONCAT_WS(' ', @goods_name, @goods_scancode,'-:', qty)) INTO @create_store_out;
ELSE 
	SELECT JSON_OBJECT('status', 1) INTO @create_store_out;
END if ;
		
END$$

delimiter ;