<?php
require_once(__DIR__ . "/sync.php");

$data = stmtall("select * from s_syncronize where f_id>?  order by f_id limit 100", "i", [$params->id])->fetch_all(MYSQLI_ASSOC);
if (empty($data)) {
    printResult(1, ["empty" => true]);
    return;
}
$out = [];
foreach ($data as $d) {
    if ($d["f_op"] == 1 || $d["f_op"] == 2) {
        $row =  stmtall("select * from {$d["f_table"]} where f_id='{$d["f_recid"]}'")->fetch_assoc();
    } else {
        $row = [];
    }
    array_push($out, array_merge($d, ["row" => $row]));
}
       

printResult(1, $out);