delimiter $$
drop function sf_vip_init$$
CREATE  FUNCTION `sf_vip_init`(
	`params` JSON
)
RETURNS longtext CHARSET utf8mb4 COLLATE utf8mb4_bin
LANGUAGE SQL
NOT DETERMINISTIC
CONTAINS SQL
SQL SECURITY DEFINER
COMMENT ''
BEGIN
declare part1 json;
declare part2 json;
declare dish longtext;
declare translation json;
declare tables json;
select json_arrayagg(JSON_OBJECT('f_id', f_id, 'f_name', f_name)) INTO part1 from d_part1;

select json_arrayagg(JSON_OBJECT('f_id', p.f_id, 'f_part', p.f_part, 'f_name', p.f_name, 'f_image', coalesce(s.f_data, ''))) INTO part2
                  from d_part2 p 
                  left join s_images s on s.f_id=p.f_image;

select json_arrayagg(JSON_OBJECT('f_part', d.f_part, 'f_dish', m.f_dish, 'f_name', d.f_name, 
	'f_price', m.f_price, 'f_print1', m.f_print1, 'f_print2', m.f_print2, 
   'f_image', coalesce(s.f_data, ''), 'f_comment', coalesce(d.f_comment, ''), 'f_cookingtime', d.f_cookingtime,
	'f_adgt', COALESCE(d.f_adgt, p2.f_adgCode) ))
   INTO dish
                  from d_menu m 
                  left join d_dish d on d.f_id=m.f_dish 
                  left join s_images s on s.f_id=d.f_image 
                  LEFT JOIN d_part2 p2 ON p2.f_id=d.f_part
                  where m.f_state=1 and m.f_menu=JSON_VALUE(params, '$.f_menu');
                  
SELECT json_arrayagg(JSON_OBJECT('f_en', f_en, 'f_am', f_am, 'f_ru', f_ru)) INTO translation
FROM s_translator;    

SELECT json_arrayagg(JSON_OBJECT('f_id', t.f_id, 'f_order', coalesce(h.f_id, ''), 'f_name', t.f_name)) INTO tables
FROM h_tables t
LEFT JOIN o_header h ON h.f_table=t.f_id AND h.f_state=1
WHERE t.f_hall=1;              
						
RETURN JSON_OBJECT('part1', json_detailed(part1), 'part2', json_detailed(part2), 'dish', json_detailed(dish), 'translator', json_detailed(translation),
	'tables', json_detailed(TABLES));						                 
END$$

DROP FUNCTION if EXISTS sf_create_dish$$
CREATE FUNCTION sf_create_dish(params JSON)
RETURNS JSON
BEGIN
RETURN JSON_OBJECT('a', '');
END$$


drop function if exists sf_print_complete $$
create function sf_print_complete(params json)
returns JSON
SQL SECURITY definer
BEGIN
declare id char(36);
declare ids JSON;
declare printer varchar(32) default json_value(params, '$.printer');
SELECT JSON_EXTRACT(params, '$.ids') INTO ids;
set @i = 0;
INSERT INTO a_result (f_request, f_result) VALUES (params, ids);
while (@i < json_length(ids)) do 
    select json_unquote(json_extract(ids, concat('$[', @i, ']'))) into @id;
    insert into a_result(f_result) values (@id);
    insert into a_result(f_result) values (json_value(params, '$.side'));
    if (json_value(params, '$.side')=1) then
    insert into a_result(f_result) values ('side1');
        update o_body set f_print1=concat(f_print1, '--PRINTED') where f_id=@id;
    else 
    insert into a_result(f_result) values ('side2');
        update o_body set f_print2=concat(f_print2, '--PRINTED') where f_id=@id;                  
    end if;
    set @i = @i + 1;
end while;
return json_object('status', 1, 'data', 'Success');
END$$



DROP FUNCTION if EXISTS sf_create_message$$
CREATE FUNCTION sf_create_message(params JSON)
RETURNS JSON
BEGIN
INSERT INTO s_notifications (f_created, f_message) VALUES (CURRENT_TIMESTAMP, params);
RETURN JSON_OBJECT('status', 1, 'data', 'Success');
END$$




delimiter $$
DROP FUNCTION if EXISTS sf_set_messages_complete$$
CREATE FUNCTION sf_set_messages_complete(params JSON)
RETURNS JSON
BEGIN
SELECT JSON_EXTRACT(params, '$.ids') INTO @ids;
SET @i = 0;
while (@i < JSON_LENGTH(@ids)) DO
SELECT JSON_UNQUOTE(JSON_EXTRACT(@ids, CONCAT('.[', @i, ']'))) INTO @id;
UPDATE s_notifications SET f_delivered=CURRENT_TIMESTAMP, f_readed=CURRENT_TIMESTAMP WHERE f_id=@id;
END while;
RETURN JSON_OBJECT('status', 1, 'data', 'Success');
END$$




DROP FUNCTION if EXISTS sf_get_messages$$
CREATE FUNCTION sf_get_messages(params JSON)
RETURNS JSON
BEGIN
DECLARE printer VARCHAR(64) DEFAULT JSON_VALUE(params, '$.f_destination');
SELECT json_arrayagg(JSON_OBJECT('f_id', n.f_id, 'f_data', n.f_message))
INTO @messages
FROM s_notifications  n
WHERE f_destination=printer AND f_delivered IS NULL;
RETURN JSON_OBJECT('status', 1, 'data', JSON_DETAILED(@messages));
END$$






DROP FUNCTION if EXISTS sf_print_service$$
CREATE FUNCTION sf_print_service(params JSON)
RETURNS JSON
BEGIN
SELECT JSON_OBJECT('font_family', 'Arial LatArm Unicode', 'font_size', 20) INTO @config;
SELECT  json_arrayagg(JSON_OBJECT('f_id', h.f_id, 
	'f_amountcash', h.f_amountcash,
	'f_amountcard', h.f_amountcard,
	'f_amountidram', h.f_amountidram,
	'f_hallid', CONCAT(h.f_prefix, h.f_hallid), 
	'f_tablename', t.f_name,
	'f_staffname', CONCAT_WS(' ', LEFT(u.f_last, 1), '.', u.f_first),
	'items', json_detailed(ob.items)))
INTO @orders
FROM o_header h 
LEFT JOIN h_tables t ON t.f_id=h.f_table
LEFT JOIN s_user u ON u.f_id=h.f_staff
LEFT JOIN (SELECT b.f_header, json_arrayagg(JSON_OBJECT('f_id', b.f_id, 
	'f_dishname', d.f_name, 
	'f_qty1', b.f_qty1,
	'f_print1', b.f_print1,
	'f_print2', b.f_print2,
	'f_comment', b.f_comment)) AS items
	FROM o_body b
	LEFT JOIN d_dish d ON d.f_id=b.f_dish
	LEFT JOIN o_header oh ON oh.f_id=b.f_header
	WHERE oh.f_state IN (1, 5) AND (b.f_print1=JSON_VALUE(params, '$.printer') OR b.f_print2=JSON_VALUE(params, '$.printer'))
	GROUP BY 1) ob ON ob.f_header=h.f_id
WHERE h.f_state IN (1, 5);
RETURN JSON_OBJECT('status', 1, 'data', JSON_OBJECT('config', JSON_DETAILED(@config), 'orders', JSON_DETAILED(@orders)));
END$$

DROP FUNCTION if EXISTS sf_open_draft$$

DROP FUNCTION if EXISTS sf_get_process_list $$
CREATE DEFINER=`root`@`localhost` FUNCTION `sf_get_process_list`(
	`params` JSON
)
RETURNS longtext CHARSET utf8mb4 COLLATE utf8mb4_bin
LANGUAGE SQL
NOT DETERMINISTIC
CONTAINS SQL
SQL SECURITY DEFINER
COMMENT ''
BEGIN
SELECT coalesce(json_arrayagg(
JSON_OBJECT('f_id', o.f_id, 
'progress', ohp.f_state, 
'f_state', o.f_state, 
'f_washdate', ohp.f_startwash,
'f_washtime', ohp.f_washtime,
'f_drydate', ohp.f_startdry,
'f_drytime', ohp.f_drytime,
'f_parkingdate', ohp.f_parking,
'f_parkingtime', ohp.f_freeparking,
'f_table', o.f_table, 
'f_tablename', t.f_name, 
'f_items', i.f_items,
'f_carnumber', coalesce(c.f_govnumber, ''), 
'f_amounttotal', o.f_amounttotal,
'f_amountcash', o.f_amountcash,
'f_amountcard', o.f_amountcard,
'f_amountidram', o.f_amountidram) ORDER BY ohp.f_state, o.f_dateopen DESC, o.f_timeopen desc
), JSON_ARRAY()) INTO @result1
FROM o_header_progress ohp
LEFT JOIN o_header o ON ohp.f_id=o.f_id
LEFT JOIN h_tables t ON t.f_id=o.f_table
LEFT JOIN b_car c ON c.f_id=o.f_id
LEFT JOIN (SELECT b.f_header, COALESCE(json_arrayagg(JSON_OBJECT('f_id', b.f_id, 'f_part1name', p1.f_name, 'f_part2name', p2.f_name, 'f_dishname', d.f_name, 
'f_cookingtime', bp.f_cookingtime, 'f_begin', bp.f_begin)), JSON_ARRAY()) AS f_items
FROM o_body b
LEFT JOIN o_body_process bp ON bp.f_id=b.f_id
LEFT JOIN d_dish d ON d.f_id=b.f_dish
LEFT JOIN d_part2 p2 ON p2.f_id=d.f_part
LEFT JOIN d_part1 p1 ON p1.f_id=p2.f_part
LEFT JOIN o_header o ON o.f_id=b.f_header 
GROUP BY 1) i ON i.f_header=o.f_id
WHERE ohp.f_state BETWEEN 1 AND 5;

SELECT coalesce(json_arrayagg(
JSON_OBJECT('f_id', o.f_id, 
'progress', ohp.f_state, 
'f_state', o.f_state, 
'f_washdate', ohp.f_startwash,
'f_washtime', ohp.f_washtime,
'f_drydate', ohp.f_startdry,
'f_drytime', ohp.f_drytime,
'f_parkingdate', ohp.f_parking,
'f_parkingtime', ohp.f_freeparking,
'f_table', o.f_table, 
'f_tablename', t.f_name, 
'f_items', i.f_items,
'f_carnumber', coalesce(c.f_govnumber, ''), 
'f_amounttotal', o.f_amounttotal,
'f_amountcash', o.f_amountcash,
'f_amountcard', o.f_amountcard,
'f_amountidram', o.f_amountidram) ORDER BY ohp.f_state, o.f_dateopen DESC, o.f_timeopen asc
), JSON_ARRAY()) INTO @result2
FROM o_header o
LEFT JOIN o_header_progress ohp ON ohp.f_id=o.f_id
LEFT JOIN h_tables t ON t.f_id=o.f_table
LEFT JOIN b_car c ON c.f_id=o.f_id
LEFT JOIN (SELECT b.f_header, COALESCE(json_arrayagg(JSON_OBJECT('f_id', b.f_id,'f_part1name', p1.f_name, 'f_part2name', p2.f_name, 'f_dishname', d.f_name, 
'f_cookingtime', bp.f_cookingtime, 'f_begin', bp.f_begin)), JSON_ARRAY()) AS f_items
FROM o_body b
LEFT JOIN o_body_process bp ON bp.f_id=b.f_id
LEFT JOIN d_dish d ON d.f_id=b.f_dish
LEFT JOIN d_part2 p2 ON p2.f_id=d.f_part
LEFT JOIN d_part1 p1 ON p1.f_id=p2.f_part
LEFT JOIN o_header o ON o.f_id=b.f_header AND (o.f_state IN (1,5) OR (o.f_state=2 AND f_amountcash+f_amountcard+f_amountidram<f_amounttotal))
GROUP BY 1) i ON i.f_header=o.f_id
WHERE ((o.f_state  IN (1,5)) OR (o.f_state=2 AND f_amountcash+f_amountcard+f_amountidram<f_amounttotal)) AND ohp.f_state = 1;

RETURN JSON_OBJECT('status', 1, 'data', JSON_DETAILED(JSON_MERGE(@result1, @result2)));
END$$


DROP FUNCTION if EXISTS sf_create_order$$
CREATE DEFINER=`root`@`localhost` FUNCTION `sf_create_order`(
	`params` JSON
)
RETURNS longtext CHARSET utf8mb4 COLLATE utf8mb4_bin
LANGUAGE SQL
NOT DETERMINISTIC
CONTAINS SQL
SQL SECURITY DEFINER
COMMENT ''
BEGIN
#18/09/24 21:42
DECLARE table_id INTEGER DEFAULT JSON_VALUE(params, '$.f_table');
DECLARE hall_id INTEGER;
DECLARE hall_prefix CHAR(8);
DECLARE hall_counter INTEGER;
DECLARE user_id INTEGER DEFAULT JSON_VALUE(params, '$.f_staff');
DECLARE id CHAR(36)  COLLATE latin1_general_ci;
DECLARE state_id INT DEFAULT JSON_VALUE(params, '$.f_state');

DECLARE cash DECIMAL(14,2) DEFAULT COALESCE(JSON_VALUE(params, '$.order.f_amountcash'), 0);
DECLARE card decimal(14,2) DEFAULT COALESCE(JSON_VALUE(params, '$.order.f_amountcard'), 0);
DECLARE idram DECIMAL(14,2) DEFAULT COALESCE(JSON_VALUE(params, '$.order.f_amountidram'), 0);

SELECT FALSE into @isnew;
set id = JSON_VALUE(params, '$.order.f_id');
if (id IS NULL) then
	SET id = UUID();
	SELECT TRUE INTO @isnew;
END if;


if (table_id IS NULL) then
SELECT f_id INTO table_id FROM h_tables WHERE f_hall=1 AND f_id NOT IN (SELECT f_table FROM o_header WHERE f_state=1) LIMIT 1;
END if ;

/* if table_id still null, create preorder */
if (table_id IS NULL) then
set table_id = 1;
SET state_id = 5;
END if;
SET state_id=5;

SELECT f_hall INTO hall_id FROM h_tables WHERE f_id=table_id;


SELECT t.f_hall, h.f_prefix, h.f_counter+1 
	into hall_id, hall_prefix, hall_counter
	FROM h_tables t 
	INNER JOIN h_halls h ON h.f_id=t.f_hall
	WHERE t.f_id=table_id
	FOR update;
UPDATE h_halls SET f_counter=hall_counter WHERE f_id=hall_id;

SELECT json_unquote(JSON_EXTRACT(params, '$.order')) INTO @ord;
INSERT INTO o_header (f_id, f_state, f_hall, f_table, f_prefix, f_hallid, 
	f_dateopen, f_timeopen, f_dateclose, f_timeclose, f_datecash, 
	f_cashier, f_staff, f_print, f_comment, 
	f_amounttotal, f_amountcash, f_amountcard, f_amountbank, f_amountcredit, f_amountidram, f_amounttelcell, f_amountpayx, f_amountdebt, f_amountother,
	f_amountservice, f_amountdiscount, f_servicefactor, f_discountfactor, 
	f_creditcardid, f_otherid, f_shift, f_source, f_saletype, f_partner, f_currentstaff,
	f_guests, f_precheck, f_cash, f_change, f_hotel, f_currency, f_taxpayertin, f_working_session)
	VALUES (
	id, COALESCE(state_id, 1), hall_id, table_id, hall_prefix, hall_counter, 
	CURRENT_DATE(), CURRENT_TIME(), CURRENT_DATE(), CURRENT_TIME(), CURRENT_DATE(),
	user_id, user_id, 0, '',
	0,JSON_VALUE(@ord, '$.f_amountcash'),JSON_VALUE(@ord, '$.f_amountcard'),0,0,JSON_VALUE(@ord, '$.f_amountidram'),0,0,0,0,
	0,0,0,0,
	
	0,0,0,1,1,0,user_id,
	0,0,0,0,0,1,'', JSON_VALUE(params, '$.f_working_session'))
	ON DUPLICATE KEY UPDATE 
		f_amountcash=JSON_VALUE(@ord, '$.f_amountcash'),
		f_amountcard=JSON_VALUE(@ord, '$.f_amountcard'),
		f_amountidram=JSON_VALUE(@ord, '$.f_amountidram');
	
if (@isnew) then
	INSERT INTO o_header_options (f_id) VALUES (id);
	INSERT INTO o_tax (f_id) VALUES (id);
	INSERT INTO o_pay_cl (f_id) VALUES (id);
	INSERT INTO o_pay_room (f_id) VALUES (id);
	INSERT INTO o_header_flags (f_id) VALUES (id);
	INSERT INTO o_payment (f_id) VALUES (id);
	INSERT INTO o_preorder (f_id) VALUES (id);
	INSERT INTO o_header_hotel (f_id) VALUES (id);
	INSERT INTO b_car (f_id, f_govnumber) VALUES (id, JSON_VALUE(params,'$.car_number'));
	INSERT INTO o_header_progress (f_id, f_state, f_washtime, f_drytime, f_freeparking) VALUES (id, 1, 30, 30, 60);
END if;

/* order items */
DELETE FROM o_body WHERE f_header=id;
SELECT JSON_extract(params, '$.items') INTO @items;
SET @i = 0;
while @i < JSON_LENGTH(@items) DO
	
	SET @bodyid = UUID();
	SELECT JSON_EXTRACT(@items, CONCAT('$[', @i, ']')) INTO @item;
	
	#set cooking time
	if (@i = 0) then 
		UPDATE o_header_progress SET f_washtime=JSON_VALUE(@item, '$.f_cookingtime')/2, f_drytime=JSON_VALUE(@item, '$.f_cookingtime')/2 WHERE f_id=id;
	END if;
	
	INSERT INTO o_body (f_id, f_header, f_state, f_dish, f_qty1, f_qty2, f_price, f_service, f_discount, f_total,
	f_store, f_print1, f_print2) 
	VALUES 
	(@bodyid, id, 1, JSON_VALUE(@item, '$.f_dish'), JSON_VALUE(@item, '$.f_qty'), 0, JSON_VALUE(@item, '$.f_price'), 
	COALESCE(JSON_VALUE(@item, '$.f_service'), 0), COALESCE(JSON_VALUE(@item, '$.f_discount'), 0), 0,
	COALESCE(JSON_VALUE(@item, '$.f_store'), 1), JSON_VALUE(@item, '$.f_print1'), JSON_VALUE(@item, '$.f_print2'));
	UPDATE o_body SET f_total=f_qty1*f_price WHERE f_id=@bodyid;
	SET @i = @i + 1;
END while;

UPDATE o_header SET f_amounttotal=(SELECT SUM(f_qty1*f_price) FROM o_body WHERE f_header=id) WHERE f_id=id;

/*
SET @uid = UUID();
if (cash > 0) then
INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency)
	VALUEs (@uid, 1,5, CURRENT_DATE, 1, cash, CURRENT_DATE, CURRENT_TIME, 1);
INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES (@uid, 1, 0, id);
INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row) VALUES (UUID(), @uid, 1, cash, 1, 0);
END if;

if (card > 0) then
INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency)
	VALUEs (@uid,1, 5, CURRENT_DATE, 1, card, CURRENT_DATE, CURRENT_TIME, 1);
INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES (@uid, 2, 0, id);
INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row) VALUES (UUID(), @uid, 2, card, 1, 0);
END if;

if (idram > 0) then
INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency)
	VALUEs (@uid, 1, 5, CURRENT_DATE, 1, idram, CURRENT_DATE, CURRENT_TIME, 1);
INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES (@uid, 3, 0, id);
INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row) VALUES (UUID(), @uid, 3, idram, 1, 0);
END if;
*/

RETURN JSON_OBJECT('status', 1, 'data', id,'hallid', CONCAT(hall_prefix,hall_counter));

END$$





