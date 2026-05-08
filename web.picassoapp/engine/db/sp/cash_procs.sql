DROP FUNCTION if EXISTS sf_create_cashdoc$$
CREATE FUNCTION sf_create_cashdoc(params JSON)
RETURNS JSON
BEGIN
-- 2025-10-12 13:41:38
DECLARE cashdoc CHAR(36) COLLATE LATIN1_GENERAL_CI;
SET cashdoc = JSON_VALUE(params, '$.f_id');
if (cashdoc IS NULL) then
	SELECT UUID() INTO cashdoc;
ELSE 
	SELECT sf_remove_cashdoc(cashdoc) INTO @tmp;
END if;
INSERT INTO a_header (f_id, f_type, f_state, f_date, f_operator, f_currency, f_comment, f_amount) 
VALUES (cashdoc, 5, 1, JSON_VALUE(params, '$.f_date'), JSON_VALUE(params, '$.f_operator'), JSON_VALUE(params, '$.f_currency'), JSON_VALUE(params, '$.f_comment'), JSON_VALUE(params, '$.f_amount'));
INSERT INTO a_header_cash (f_id, f_cashin, f_cashout) VALUES (cashdoc, JSON_VALUE(params, '$.f_cashin'), JSON_VALUE(params, '$.f_cashout'));
if (coalesce(JSON_VALUE(params, '$.f_cashin'), 0)>0) then
	INSERT INTO e_cash (f_id, f_header, f_cash, f_sign, f_remarks, f_amount, f_row) VALUES (UUID(), cashdoc, JSON_VALUE(params, '$.f_cashin'), 1, JSON_VALUE(params, '$.f_purpose'), JSON_VALUE(params, '$.f_amount'), 0);
END if;
if (COALESCE(JSON_VALUE(params, '$.f_cashout'), 0)>0) then
	INSERT INTO e_cash (f_id, f_header, f_cash, f_sign, f_remarks, f_amount, f_row) VALUES (UUID(), cashdoc, JSON_VALUE(params, '$.f_cashout'), -1, JSON_VALUE(params, '$.f_purpose'), JSON_VALUE(params, '$.f_amount'), 0);
END if;
RETURN JSON_OBJECT('status', 1, 'cashdoc', cashdoc);
END$$

DROP FUNCTION if EXISTS sf_remove_cashdoc$$
CREATE FUNCTION sf_remove_cashdoc(cashdoc CHAR(36) COLLATE LATIN1_GENERAL_CI)
RETURNS JSON
BEGIN
-- 2025-10-12 13:41:44
	delete from e_cash where f_header=cashdoc;
	delete from a_header_cash where f_id=cashdoc;
	delete from a_header where f_id=cashdoc;
	RETURN JSON_OBJECT('status', 1);
END$$