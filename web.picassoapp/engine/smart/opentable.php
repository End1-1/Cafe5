<?php
#(C) 01/02/25, Kudryashov Vasili
#Last modified - 2025-11-05 23:34:37
require_once __DIR__ . "/smart.php";

$db->begin_transaction();
$table = stmtall("select * from h_tables where f_id=? for update", "i", [$params->table])->fetch_assoc();

if (strcmp($table["f_locksrc"] ?? "", $params->host) && strlen($table["f_locksrc"] ?? "") > 0) {
    $db->commit();
    printResult(1, ["locked"=>$table["f_locksrc"], "previousetable"=>$params->table]);
    return;
} else {
    stmtall("update h_tables set f_locksrc=null where f_locksrc=?", "s", [$params->host]);
    stmtall("update h_tables set f_locksrc=? where f_id=?", "si", [$params->host, $params->table]);
}
$db->commit();

$header = stmtall("select * from o_header where f_table=? and f_state=?", "ii", [$params->table, 1])->fetch_assoc();
if (empty($header)) {
    printResult(1, ["empty" => true, "table" => $params->table]);
    return;
}

$bhistory = stmtall("select * from b_history where f_id=?", "s", [$header["f_id"]])->fetch_assoc();

$flags = stmtall("select * from o_header_flags where f_id=?", "s", [$header["f_id"]])->fetch_assoc();

$dishes = stmtall("select b.f_id as obodyid, b.f_dish, b.f_adgcode, d.f_name, b.f_qty1, b.f_qty2, b.f_price, b.f_comment, "
    . "b.f_print1, b.f_print2, b.f_store, b.f_id, b.f_emarks, b.f_state, d.f_extra "
    . "from o_body b "
    . "left join d_dish d on d.f_id=b.f_dish "
    . "where b.f_header=? and b.f_state>0 "
    . "order by b.f_row", "s", [$header["f_id"]])->fetch_all(MYSQLI_ASSOC);

if ($header["f_partner"] > 0) {
    $partner = stmtall("select * from c_partners where f_id=?", "i", [$header["f_partner"]])->fetch_assoc();
} else {
    $partner = ["f_id" => 0];
}

printResult(1, ["empty" => false, "table" => $params->table, "header" => $header, "bhistory" => $bhistory, "flags" => $flags, "dishes" => $dishes, "customer" => $partner]);


/* debug 
$__query_times = []; // глобальный лог таймингов

function timed_stmtall(string $sql, string $types = "", array $params = []) {
    global $__query_times, $db;

    $start = microtime(true);
    $result = stmtall($sql, $types, $params);
    $duration = (microtime(true) - $start) * 1000; // в миллисекундах

    // Сохраняем: SQL + время + короткая форма
    $__query_times[] = [
        "sql" => $sql,
        "params" => $params,
        "time_ms" => round($duration, 3)
    ];

    return $result;
}

$db->begin_transaction();

$table = timed_stmtall(
    "select * from h_tables where f_id=? for update",
    "i",
    [$params->table]
)->fetch_assoc();

if (strcmp($table["f_locksrc"] ?? "", $params->host) && strlen($table["f_locksrc"] ?? "") > 0) {
    $db->commit();
    printResult(1, [
        "locked" => $table["f_locksrc"],
        "previousetable" => $params->table,
        "timing" => $__query_times
    ]);
    return;
} else {
    timed_stmtall("update h_tables set f_locksrc=null where f_locksrc=?", "s", [$params->host]);
    timed_stmtall("update h_tables set f_locksrc=? where f_id=?", "si", [$params->host, $params->table]);
}

$db->commit();

$header = timed_stmtall(
    "select * from o_header where f_table=? and f_state=?",
    "ii",
    [$params->table, 1]
)->fetch_assoc();

if (empty($header)) {
    printResult(1, [
        "empty" => true,
        "table" => $params->table,
        "timing" => $__query_times
    ]);
    return;
}

$bhistory = timed_stmtall(
    "select * from b_history where f_id=?",
    "s",
    [$header["f_id"]]
)->fetch_assoc();

$flags = timed_stmtall(
    "select * from o_header_flags where f_id=?",
    "s",
    [$header["f_id"]]
)->fetch_assoc();

$dishes = timed_stmtall(
    "select b.f_id as obodyid, b.f_dish, b.f_adgcode, d.f_name, b.f_qty1, b.f_qty2, b.f_price, b.f_comment, "
    . "b.f_print1, b.f_print2, b.f_store, b.f_id, b.f_emarks, b.f_state, d.f_extra "
    . "from o_body b "
    . "left join d_dish d on d.f_id=b.f_dish "
    . "where b.f_header=? and b.f_state>0 "
    . "order by b.f_row",
    "s",
    [$header["f_id"]]
)->fetch_all(MYSQLI_ASSOC);

if ($header["f_partner"] > 0) {
    $partner = timed_stmtall("select * from c_partners where f_id=?", "i", [$header["f_partner"]])->fetch_assoc();
} else {
    $partner = ["f_id" => 0];
}

printResult(1, [
    "empty" => false,
    "table" => $params->table,
    "header" => $header,
    "bhistory" => $bhistory,
    "flags" => $flags,
    "dishes" => $dishes,
    "customer" => $partner,
    "timing" => $__query_times
]);
*/