<?php
require_once (__DIR__ . "/../app.php");

#remove after 1 year
$tables = $tables = stmtall("show tables")->fetch_all(MYSQLI_NUM);
foreach ($tables as $t) {
    $db->query("drop trigger if exists {$t[0]}_ai");
    $db->query("drop trigger if exists {$t[0]}_bu");
    $db->query("drop trigger if exists {$t[0]}_bd");
}

printResult(1, "ok");