delimiter $$
DROP FUNCTION if EXISTS sf_open_store_document$$
CREATE FUNCTION sf_open_store_document(params JSON)
RETURNS JSON 
BEGIN
-- 2025-10-12 13:45:28
	DECLARE docid CHAR(36) CHARACTER SET LATIN1 COLLATE LATIN1_GENERAL_CI DEFAULT JSON_VALUE(params, '$.f_id');
	DECLARE doc_header JSON;
	DECLARE doc_body JSON;
	DECLARE doc_client JSON;
	DECLARE doc_header_store JSON;
	DECLARE doc_add JSON;
	SELECT JSON_OBJECT('f_date', f_date, 'f_partner', f_partner,  'f_userid', f_userid, 'f_type', f_type, 'f_state', f_state, 'f_payment', f_payment,
		'f_amount', f_amount, 'f_comment', f_comment, 'f_paid', f_paid, 'f_body', f_body, 'f_currency', f_currency) INTO doc_header
		FROM a_header
		WHERE f_id=docid;
	if (doc_header IS NULL) then
		RETURN JSON_OBJECT('status', 0, 'data', 'Փասթաթուղթը գոյություն չունի');
	END if;
	
	select json_arrayagg(json_object(
		'f_id', d.f_id, 
		'f_goods', d.f_goods, 
		'f_goodsname', concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))), 
		'f_adgt', if (length(coalesce(g.f_adg, '')) > 0, g.f_adg, gr.f_adgcode) ,
		'f_qty', d.f_qty,
		'f_unitname', u.f_name, 
		'f_price', d.f_price, 
		'f_total', d.f_total, 
		'f_reason', d.f_reason, 
		'f_type', d.f_type, 
		'f_base', d.f_base, 
		'f_comment', d.f_comment, 
		'f_validdate', v.f_date)) INTO doc_body
		from a_store_draft d 
		left join c_goods g on g.f_id=d.f_goods
		left join c_units u on u.f_id=g.f_unit
		left join a_store_valid v on v.f_id=d.f_id 
		left join c_groups gr on gr.f_id=g.f_group
		where d.f_document=docid 
		order by d.f_row;
		
	SELECT JSON_OBJECT(
		'f_storein', f_storein, 
		'f_storeout', f_storeout)
		INTO doc_header_store
		FROM a_header_store WHERE f_id=docid;
		
	select JSON_OBJECT(
		'f_id', f_id, 
		'f_date', f_date, 
		'f_costumer', f_costumer,
		'f_order', f_order,
		'f_cash', f_cash,
		'f_amount', f_amount,
		'f_storedoc', f_storedoc, 
		'f_currency', f_currency,
		'f_flag', f_flag,
		'f_source', f_source) INTO doc_client		
		from b_clients_debts where f_storedoc=docid;
	
	RETURN JSON_OBJECT('status', 1, 'data', 
		JSON_OBJECT(
			'doc_header', JSON_DETAILED(doc_header), 
			'doc_header_store', JSON_DETAILED(doc_header_store),
			'doc_body', JSON_DETAILED(doc_body),
			'doc_client', JSON_DETAILED(doc_client)));
	
END$$
delimiter ;

