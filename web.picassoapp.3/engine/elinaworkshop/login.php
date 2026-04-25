<?php
define("AUTH", 1);
require_once(__DIR__ . "/../app.php");
require_once(__DIR__ . "/../cash/cashsession.php");
if (!defined("AUTH")) {
	die("NOT DEFINED HORAS ARREV");
}

if (!property_exists($params, "method")) {
	exitError("How are you want to login, baby?");
}

if ($params->method == 1) {
	$stmt = $db->prepare("select * from s_user where f_login=? and f_password=md5(?)");
	if (!$stmt) {
		exitError($db->error);
	}
	$stmt->bind_param("ss", $params->username, $params->password);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$result = $stmt->get_result();
	if ($result->num_rows == 0) {
		$stmt->close();
		exitError("Unauthorized");
	}
	$row = $result->fetch_assoc();

	$session_key = uuidv4();
	$v["f_session"] = $session_key;
	$v["f_user"] = $row["f_id"];
	$v["f_iplogin"] = json_encode([$_SERVER['HTTP_X_FORWARDED_FOR'] ?? "0.0.0.0", $_SERVER['REMOTE_ADDR']]);
	$v["f_datestart"] = date("Y-m-d");
	$v["f_timestart"] = date("H:i:s");
	sinsert("s_login_session", $v);

} else if ($params->method == 2) {
	$stmt = $db->prepare("select * from s_user where f_altpassword=md5(?)");
	if (!$stmt) {
		exitError($db->error);
	}
	$stmt->bind_param("s", $params->pin);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$result = $stmt->get_result();
	if ($result->num_rows == 0) {
		$stmt->close();
		exitError("Unauthorized");
	}
	$row = $result->fetch_assoc();


	$session_key = uuidv4();
	$v["f_session"] = $session_key;
	$v["f_user"] = $row["f_id"];
	$v["f_iplogin"] = json_encode([$_SERVER['HTTP_X_FORWARDED_FOR'] ?? "0.0.0.0", $_SERVER['REMOTE_ADDR']]);
	$v["f_datestart"] = date("Y-m-d");
	$v["f_timestart"] = date("H:i:s");
	sinsert("s_login_session", $v);
} else if ($params->method == 3) {
	$user = stmtall("select u.* from s_user u "
		. "left join s_login_session s on s.f_user=u.f_id "
		. "where f_session=?", "s", [$bearerToken])->fetch_assoc();
	if (empty($user)) {
		exitError("Invalid session key. Login again.");
	}
	$userid = $user["f_id"];
	$row = $user;
	$session_key = $bearerToken;
} else {
	exitError("I think, you wonna hack me");
}

if (!$stmt = $db->prepare("select f_config from sys_json_config where f_id=?")) {
	exitError($db->error);
}
$stmt->bind_param("i", $row["f_config"]);
if (!$stmt->execute()) {
	exitError($stmt->error);
}
$result = $stmt->get_result();
if ($row_config = $result->fetch_assoc()) {
} else {
	$row_config = ["f_config" => ["dashboard" => "none"]];
}

$cashsession = stmtall("select f_id from s_working_sessions where f_close is null")->fetch_assoc();
if (empty($cashsession)) {
	$cashsession = ["f_id" => 0];
}

unset($row["f_password"]);
unset($row["f_altpassword"]);
unset($row["f_altPassword"]);
echo json_encode([
	"status" => 1,
	"data" => [
		"sessionkey" => $session_key,
		"user" => $row,
		"config" => $row_config,
		"cashsession" => $cashsession
	]
]);