<?php
require_once("carwash.php");

$db->begin_transaction();
stmtall("UPDATE o_header SET f_state=2, f_dateclose=CURRENT_DATE(), f_timeclose=CURRENT_TIME(), "
	. "f_amountcash=COALESCE(?, 0), "
	. "f_amountcard=COALESCE(?, 0), "
	. "f_amountidram=COALESCE(?, 0), "
	. "f_working_session=? "
	. "WHERE f_id=?; ", "dddis", 
    [$params->f_amountcash, $params->f_amountcard, $params->f_amountidram, $params->cashsession, $params->f_id]);
stmtall("UPDATE o_body SET f_qty2=f_qty1, f_total=f_qty1*f_price WHERE f_header=?;", "s", [$params->f_id]);
stmtall("UPDATE o_body_process SET f_end=CURRENT_TIMESTAMP() WHERE f_end IS NULL AND f_id IN (SELECT f_id FROM o_body WHERE f_header=?);", "s", [$params->f_id]);
$db->commit();

if ($params->f_amountcash > 0) {
	$uid = uuidv4();
	stmtall("INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency, f_working_session) "
		. "VALUES ('$uid', 1, 5, CURRENT_DATE, 1, $params->f_amountcash, CURRENT_DATE, CURRENT_TIME, 1,$params->cashsession);");
	stmtall("INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES ('$uid', 1, 0, '$params->f_id')");
	stmtall("INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row) VALUES (UUID(), '$uid', 1, $params->f_amountcash, 1, 0);");
}

if ($params->f_amountcard > 0) {
	$uid = uuidv4();
	stmtall("INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency, f_working_session) "
		. "VALUES ('$uid', 1, 5, CURRENT_DATE, 1, $params->f_amountcard, CURRENT_DATE, CURRENT_TIME, 1,$params->cashsession);");
	stmtall("INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES ('$uid', 2, 0, '$params->f_id')");
	stmtall("INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row) VALUES (UUID(), '$uid', 2, $params->f_amountcard, 1, 0);");
}

if ($params->f_amountidram > 0) {
	$uid = uuidv4();
	stmtall("INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency, f_working_session) "
		. "VALUES ('$uid', 1, 5, CURRENT_DATE, 1, $params->f_amountidram, CURRENT_DATE, CURRENT_TIME, 1,$params->cashsession);");
	stmtall("INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES ('$uid', 3, 0, '$params->f_id')");
	stmtall("INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row) VALUES (UUID(), '$uid', 3, $params->f_amountidram, 1, 0);");
}

printResult(1, "");