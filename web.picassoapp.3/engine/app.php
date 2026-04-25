<?php
# (C) 2024-2026 Kudryashov Vasili
# Created - 2024-05-01 09:00:00
# Last modified - 2026-02-06 10:00:40


if ($_SERVER['REQUEST_METHOD'] == 'OPTIONS') {
	header("Access-Control-Allow-Origin: *");
	header("Access-Control-Allow-Methods: GET, POST, PATCH, PUT, DELETE, OPTIONS");
	header("Access-Control-Allow-Headers: Access-Control-Allow-Origin, Access-Control-Allow-Headers,  X-Auth-Token, Origin, Content-Type, X-Amz-Date, Authorization, X-Api-Key, X-Amz-Security-Token, locale");
	header("Access-Control-Max-Age: 86400");
	http_response_code(204);
	exit;
}

define("APP", 1);

require_once __DIR__ . "/config.php";
require_once __DIR__ . "/translator.php";
require_once __DIR__ . "/functions/db.php";
require_once __DIR__ . "/functions/pclass.php";
require_once __DIR__ . "/reports/report.php";
require_once __DIR__ . "/functions/dbtranslator.php";
require_once __DIR__ . "/functions/print.php";

$requestStr = file_get_contents("php://input");
$params = json_decode($requestStr);
$engineVersion = "1.0.0.10";

if (!isset($params)) {
	exitError("Holy shit!");
}

if (!empty($params->debug)) {
	if ($params->debug == false) {
		unset($params->debug);
	}
}

require_once __DIR__ . "/v2/check-app.php";

$bearerToken = "";
if (isset($_SERVER['HTTP_AUTHORIZATION'])) {

	$authHeader = $_SERVER['HTTP_AUTHORIZATION'];
} elseif (function_exists('getallheaders')) {

	$headers = getallheaders();
	$authHeader = $headers['Authorization'] ?? $headers['authorization'] ?? null;
}

if (isset($authHeader) && preg_match('/Bearer\s(\S+)/i', $authHeader, $matches)) {
	$bearerToken = $matches[1];
}

if (!empty($params->dick)) {
	define("AUTH", 1);
}
if (!defined("AUTH")) {
	if (empty($bearerToken)) {
		exitError("How to authorize you?");
	} else {
		$user = stmtall("select u.* from s_user u "
			. "left join s_login_session s on s.f_user=u.f_id "
			. "where f_session=?", "s", [$bearerToken])->fetch_assoc();

		if (empty($user)) {
			exitError("Invalid session key. Login again.");
		}
		$userid = $user["f_id"];
		define("AUTH", 1);
	}
}

if (!defined("AUTH")) {
	header("HTTP/1.1 401 Unauthorized", true, 401);
	die("Unauthorized");
}

$v["f_session"] = $bearerToken ?? '';
$v["f_user"] = empty($userid) ? null : $userid;
$v["f_path"] = $_SERVER['REQUEST_URI'];
$v["f_params"] = $requestStr;
sinsert("s_web_log", $v);
