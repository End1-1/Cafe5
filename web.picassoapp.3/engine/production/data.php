<?php
require_once(__DIR__ . "/production.php");

$products = stmtall("SELECT f_id, f_name from mf_actions_group order by f_name")->fetch_all(MYSQLI_ASSOC);

printResult(1, ["r1" => $r1]);