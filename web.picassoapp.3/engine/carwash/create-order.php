<?php
require_once("carwash.php");
require_once(__DIR__ . "/../cash/cashsession.php"); 

$cs = new CashSession();
$cs->getSession();
if ($cs->cashsession["f_id"] == 0) {
    exitError("no active session");
}
$params->cashsession = $cs->cashsession["f_id"];
$closed = false;

$db->begin_transaction();
$result = queryStr("select sf_create_order('{" 
    . "\"order\": " . json_encode($params->order, JSON_UNESCAPED_UNICODE) . ","
    . "\"items\": " . json_encode($params->items, JSON_UNESCAPED_UNICODE) . ","
    . "\"f_staff\": $params->f_staff, "
    . "\"f_table\": $params->f_table, "
    . "\"f_working_session\":" . $cs->cashsession["f_id"] . ","
    . "\"car_number\": \"$params->car_number\"}')");

    $row = json_decode($result->fetch_row()[0]);
    $params->f_id = $row->data;
    $params->f_hallid = $row->hallid;

$cashdocs = stmtall("select f_id from a_header_cash where f_oheader=?", "s",  [$params->f_id])->fetch_all(MYSQLI_ASSOC);
foreach ($cashdocs as $cd) {
    stmtall("delete from e_cash where f_header=?", "s", [$cd["f_id"]]);
    stmtall("delete from a_header_cash where f_id=?", "s", [$cd["f_id"]]);
    stmtall("delete from a_header where f_id=?", "s", [$cd["f_id"]]);
}

if ($params->f_amountcash > 0) {
    $closed = true;
    $uid = uuidv4();
    stmtall("INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency, f_comment, f_working_session) "
        . "VALUES ('$uid', 1, 5, CURRENT_DATE, 1, $params->f_amountcash, CURRENT_DATE, CURRENT_TIME, 1, 'Cash $params->f_hallid', $params->cashsession);");
    stmtall("INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES ('$uid', 1, 0, '$params->f_id')");
    stmtall("INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row, f_remarks) VALUES (UUID(), '$uid', 1, $params->f_amountcash, 1, 0, 'Cash $params->f_hallid');");
}

if ($params->f_amountcard > 0) {
    $closed = true;
    $uid = uuidv4();
    stmtall("INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency, f_comment, f_working_session) "
        . "VALUES ('$uid', 1, 5, CURRENT_DATE, 1, $params->f_amountcard, CURRENT_DATE, CURRENT_TIME, 1, 'Card $params->f_hallid', $params->cashsession);");
    stmtall("INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES ('$uid', 2, 0, '$params->f_id')");
    stmtall("INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row, f_remarks) VALUES (UUID(), '$uid', 2, $params->f_amountcard, 1, 0, 'Card $params->f_hallid');");
}

if ($params->f_amountidram > 0) {
    $closed = true;
    $uid = uuidv4();
    stmtall("INSERT INTO a_header(f_id, f_state, f_type, f_date, f_operator, f_amount, f_createdate, f_createtime, f_currency, f_comment, f_working_session) "
        . "VALUES ('$uid', 1, 5, CURRENT_DATE, 1, $params->f_amountidram, CURRENT_DATE, CURRENT_TIME, 1, 'Idram $params->f_hallid', $params->cashsession);");
    stmtall("INSERT INTO a_header_cash(f_id, f_cashin, f_cashout, f_oheader) VALUES ('$uid', 3, 0, '$params->f_id')");
    stmtall("INSERT INTO e_cash (f_id, f_header, f_cash, f_amount, f_sign, f_row, f_remarks) VALUES (UUID(), '$uid', 3, $params->f_amountidram, 1, 0, 'Idram $params->f_hallid');");
}

$row = array_merge((array)$row, ["session"=>$cs->cashsession["f_id"]]);
if ($closed) {
    stmtall("update o_header set f_state=2 where f_id=?", "s", [$row["data"]]);
}
$db->commit();
printResult(1,  $row);