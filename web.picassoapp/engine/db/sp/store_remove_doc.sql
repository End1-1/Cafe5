delimiter $$
DROP FUNCTION IF EXISTS sf_remove_store_doc $$
CREATE function sf_remove_store_doc (params JSON) RETURNS json BEGIN 
-- 2025-10-12 13:38:20
DECLARE removeid CHAR(36) CHARACTER SET LATIN1 COLLATE LATIN1_GENERAL_CI; 
DECLARE cashdoc CHAR(36) CHARACTER SET LATIN1 COLLATE LATIN1_GENERAL_CI;
SET removeid = JSON_VALUE(params, '$.removeid'); 

#check related doc if this is a input
SELECT  json_arrayagg( JSON_OBJECT( 'doc',s.f_document,'date',d.f_date,'user',d.f_userid ) ) INTO @relateddocs
FROM a_store s
INNER JOIN a_header d
ON d.f_id = s.f_document
WHERE f_basedoc = removeid
AND f_document <> removeid
AND s.f_type = -1; 

IF(@relateddocs IS NOT NULL) THEN 
       RETURN JSON_OBJECT( 'status', 0, 'reason', JSON_DETAILED(@relateddocs) ); 
END if; 

#select cash doc
SELECT  f_baseonsale
       ,f_cashuuid INTO @baseonsale
       ,cashdoc
FROM a_header_store
WHERE f_id = removeid; 
DELETE FROM a_store WHERE f_document = removeid; 
delete FROM a_store_draft WHERE f_document = removeid; 
delete FROM b_clients_debts WHERE f_storedoc = removeid; 
delete FROM a_header WHERE f_id = removeid; 
delete FROM a_header_store WHERE f_id = removeid; 
IF(cashdoc IS NOT null) THEN 
       delete FROM e_cash WHERE f_header = cashdoc; 
       delete FROM a_header_cash WHERE f_id = cashdoc; 
       delete FROM a_header  WHERE f_id = cashdoc; 
END if; 
delete FROM a_store_dish_waste WHERE f_document = removeid; 
RETURN JSON_OBJECT('status', 1); END $$
delimiter ;