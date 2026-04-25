<?php
# @2025 Kudryashov Vasili
# Last modified 2025-08-08 11:57:50
require_once __DIR__ . "/cashdesk.php";

$isnew = empty($params->id);
if ($isnew) {
    $id = uuidv4();
} else {
    $id = $params->id;
}

$sql = <<<EOD
        select * from sys_json_config where f_id=?
        EOD;

$configArray = stmtall($sql, "i", [$params->config])->fetch_assoc();
if (!$configArray) {
    exitError("Config not found");
}
$config = json_decode($configArray["f_config"]);
if (json_last_error() !== JSON_ERROR_NONE) {
    exitError("Invalid config JSON");
}

if ($params->daily_check > 0) {
    stmtall("update users_elina_day_end set f_check=? where f_date=? and f_hall=?", "isi", 
    [$params->daily_check ,$params->date, $config->hall]);
}

if ($params->amount > 0) {
    $db->begin_transaction();
    $bind["f_type"] = 5;
    $bind["f_state"] = 1;
    $bind["f_date"] = $params->date;
    $bind["f_operator"] = empty($params->operator) ?  $userid : $params->operator;
    $bind["f_currency"] = 1;
    $bind["f_comment"] = $params->remarks;
    $bind["f_amount"] = $params->amount;
    if ($isnew) {
        $bind["f_id"] = $id;
        $bind["f_createdate"] = date("Y-m-d");
        $bind["f_createtime"] = date("H:i:s");
        sinsert("a_header", $bind);
    } else {
        supdate("a_header", $bind, $id);
    }

    $bind["f_id"] = $id;
    $bind["f_cashin"] = $params->cashin;
    $bind["f_cashout"] = $params->cashout;
    if ($isnew) {
        sinsert("a_header_cash", $bind);
    } else {
        supdate("a_header_cash", $bind, $id);
    }

    $bind["f_id"] = $id;
    $bind["f_header"] = $id;
    $bind["f_cash"] = $params->cashin > 0 ? $params->cashin : $params->cashout;
    $bind["f_sign"] = $params->cashin > 0 ? 1 : -1;
    $bind["f_remarks"] = $params->remarks;
    $bind["f_amount"] = $params->amount;
    $bind["f_row"] = 0;
    sinsertupdate("e_cash", $bind, $id, $isnew);

    $db->commit();
}

if (empty($params->refresh)) {
    printResult(1, "ok");
} else {
    switch ($params->refresh) {
        case 'carwashcashdesk':
            require_once(__DIR__ . "/../carwash/cashreport.php");
            break;
        default:
            exitError("No valid url for refresh");
            break;
    }
}
