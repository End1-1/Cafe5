<?php
# © 2025, Kudryashov Vasili
# Modified - 2025-05-13 13:47:51
require_once __DIR__ . "/smart.php";

stmtall("update o_body set f_qty1=?, f_price=? where f_id=?", "dds", [$params->qty, $params->price, $params->obodyid]);

printResult(1, ["obodyid"=> $params->obodyid, "row"=>$params->row, "qty"=>$params->qty]);