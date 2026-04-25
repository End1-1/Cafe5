<?php
require_once __DIR__ . "/production.php";

$workshop= stmtall("SELECT f_id, f_name from mf_stage order by f_name")->fetch_all(MYSQLI_ASSOC);
$product = stmtall("SELECT f_id, coalesce(f_name , '???') as f_name from mf_actions_group where length(coalesce(f_name , ''))>0 order by f_name")->fetch_all(MYSQLI_ASSOC);
$stages = stmtall("SELECT f_id, f_name from mf_actions_state order by f_name")->fetch_all(MYSQLI_ASSOC);
$storages = stmtall("SELECT f_id, f_name from c_storages order by f_name")->fetch_all(MYSQLI_ASSOC);

$data = ["workshop" => $workshop, "product"=>$product, "stages"=>$stages, "storages"=>$storages];
printResult(1, $data);