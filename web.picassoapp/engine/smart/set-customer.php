<?php
require_once(__DIR__ . "/smart.php");

$v["f_partner"] = $params->customer;
supdate("o_header", $v, $params->header);

printResult(1, "ok");