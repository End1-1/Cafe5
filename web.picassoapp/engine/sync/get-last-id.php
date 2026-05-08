<?php
require_once(__DIR__ . "/sync.php");

$result = stmtall("select f_syncin from $params->db.s_db")->fetch_assoc();



printResult(1, ["result"=>$result]);