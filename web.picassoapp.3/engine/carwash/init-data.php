<?php
require_once(__DIR__ . "/carwash.php");
$result = queryStr("select sf_vip_init('{\"f_menu\":$params->f_menu}')");

if ($row = $result->fetch_row()) {
    printResult(1, $row);
} else {
    printResult(1, []);
}