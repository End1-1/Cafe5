<?php
require_once(__DIR__ . "/smart.php");

$v["f_comment"] = $params->comment;
supdate("o_body", $v, $params->obodyid);

printResult(1, ["row"=>$params->row, "comment"=>$params->comment]);