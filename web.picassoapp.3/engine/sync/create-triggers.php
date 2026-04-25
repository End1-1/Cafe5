<?php
require_once (__DIR__ . "/../app.php");

#remove after 1 year
$tables = $tables = stmtall("show tables")->fetch_all(MYSQLI_NUM);
foreach ($tables as $t) {
    $db->query("drop trigger if exists {$t[0]}_ai");
    $db->query("drop trigger if exists {$t[0]}_bu");
    $db->query("drop trigger if exists {$t[0]}_bd");
}


$tables = [
    "c_partners",
    "c_groups",
    "c_units",
    "c_goods",
    "c_goods_prices",
    "a_header",
    "a_header_store",
    "a_store_draft",
    "a_store",
    "o_header",
    "o_tax_log",
    "o_tax",
    "o_goods"
];

foreach ($tables as $t) {
    $db->query("drop trigger if exists {$t}_ai");
    $db->query("drop trigger if exists {$t}_bu");
    $db->query("drop trigger if exists {$t}_bd");
}

if ($params->clearonly == 1) {
    printResult(1, "ok");
    return;
}

foreach ($tables as $t) {

    stmtall("CREATE  TRIGGER `{$t}_ai` "
        . "AFTER INSERT ON `{$t}`  "
        . "FOR EACH ROW "
        . "BEGIN "
        . "insert into s_syncronize (f_table, f_recid, f_op, f_date) values ('{$t}', new.f_id, 1, current_timestamp); "
        . "END");

    stmtall("CREATE  TRIGGER `{$t}_bu` "
        . "BEFORE update ON `{$t}` "
        . "FOR EACH ROW  "
        . "BEGIN "
        . "insert into s_syncronize (f_table, f_recid, f_op, f_date) values ('{$t}', new.f_id, 2, current_timestamp); "
        . "END ");

    stmtall("CREATE  TRIGGER `{$t}_bd` "
        . "BEFORE delete ON `{$t}` "
        . "FOR EACH ROW  "
        . "BEGIN "
        . "insert into s_syncronize (f_table, f_recid, f_op, f_date) values ('{$t}', old.f_id, 3, current_timestamp); "
        . "END ");
}

printResult(1, "ok");