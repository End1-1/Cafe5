<?php
# (C) 2024-2025 Kudryashov Vasili
# Created 2024-05-01 09:02:21
# Last modified 2026-01-01 00:30:37
defined('APP') or die('Die, vampire!');
error_reporting(E_ALL);
ini_set('display_errors', 'On');
ini_set('memory_limit', '4512M');
date_default_timezone_set('Asia/Yerevan');
header('Content-Type: application/json; charset=utf-8');
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods:  GET, POST, PATCH, PUT, DELETE, OPTIONS");
header("Access-Control-Allow-Headers: Access-Control-Allow-Origin, Access-Control-Allow-Headers,  X-Auth-Token, Origin, Content-Type, X-Amz-Date, Authorization, X-Api-Key, X-Amz-Security-Token, locale");

define("CLEANER", 1);

require_once __DIR__ . "/functions/common.php";
require_once __DIR__ . "/cnf.php";

$codeVersion = "1.0";
$db = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
if ($db->connect_errno) {
	echo $db->connect_error;
	exit();
}
if (!$db->set_charset("utf8mb4")) {
	echo $db->error;
	exit();
};

function stacktrace_error_handler($errno,$message,$file,$line)
{
    if($errno === E_WARNING) {
        debug_print_backtrace();
    }
    return false; 
}
set_error_handler("stacktrace_error_handler");
