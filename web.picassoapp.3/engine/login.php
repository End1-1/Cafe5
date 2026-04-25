<?php
# (C) 2024-2025 Kudryashov Vasili
#Created 2024-05-01 09:02:21
# Last modified 2025-12-21 10:01:51

define("AUTH", 1);
require_once __DIR__ . "/app.php";
require_once __DIR__ . "/cash/cashsession.php";

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
		exitError("Access denied", 401);
	}
	$row = $result->fetch_assoc();

	$session_key = uuidv4();
	$v["f_session"] = $session_key;
	$v["f_user"] = $row["f_id"];
	$v["f_iplogin"] = json_encode([$_SERVER['HTTP_X_FORWARDED_FOR'] ?? "0.0.0.0", $_SERVER['REMOTE_ADDR']]);
	$v["f_datestart"] = date("Y-m-d");
	$v["f_timestart"] = date("H:i:s");
	sinsert("s_login_session", $v);
} else if ($params->method == 2 || $params->method == 6) {
	if ($params->method == 2) {
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
			exitError("Access denied", 401);
		}
		$row = $result->fetch_assoc();
	} else if ($params->method == 6) {
		$stmt = $db->prepare("select * from s_user where f_altpassword=md5(?) and f_login=?");
		if (!$stmt) {
			exitError($db->error);
		}
		$stmt->bind_param("ss", $params->pin,$params->pass);
		if (!$stmt->execute()) {
			exitError($stmt->error);
		}
		$result = $stmt->get_result();
		if ($result->num_rows == 0) {
			$stmt->close();
			exitError("Access denied", 401);
		}
		$row = $result->fetch_assoc();
	}

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
		exitError("Invalid session key. Login again.", 401);
	}
	$userid = $user["f_id"];
	$row = $user;
	$session_key = $bearerToken;
} else if ($params->method == 4) {
	#guest, register new 
	if (empty($costomergroup = getenv("customergroup"))) {
		exitError("Customer group undefined");
	}
	if (empty($customersettings = getenv("customersettingsid"))) {
		exitError("Customer settings not defined");
	}
	$b["f_group"] = $costomergroup;
	$b["f_state"] = 1;
	$b["f_first"] = "Guest";
	$b["f_last"] = "Name";
	$b["f_config"] = $customersettings;
	$userid = sinsert("s_user", $b);
	$user = stmtall("select * from s_user where f_id=?", "i", [$userid])->fetch_assoc();
	$row = $user;

	$session_key = uuidv4();
	$v["f_session"] = $session_key;
	$v["f_user"] = $userid;
	$v["f_iplogin"] = json_encode([$_SERVER['HTTP_X_FORWARDED_FOR'] ?? "0.0.0.0", $_SERVER['REMOTE_ADDR']]);
	$v["f_datestart"] = date("Y-m-d");
	$v["f_timestart"] = date("H:i:s");
	sinsert("s_login_session", $v);
} else if ($params->method == 5) {
	#signup by phone
	#two step - request and confirmation
	if (empty($params->step)) {
		exitError("You wanna send sms, baby?");
	} else if ($params->step == 1) {
		if (substr($params->phone, 0, 4) != "+374") {
			exitError("Are you international hacker?");
		}
		stmtall("update s_activation set f_state=3 where f_state=1 and f_session=? and f_phone=?", "ss", [$bearerToken, $params->phone]);
		$confirmationcode = random_int(1000, 9999);
		$bind["f_ip"] = get_client_ip();
		$bind["f_state"] = 1;
		$bind["f_session"] = $bearerToken;
		$bind["f_created"] = date("Y-m-d H:i:s");
		$bind["f_code"] = $confirmationcode;
		$bind["f_phone"] = $params->phone;
		sinsert("s_activation", $bind);

		$login = getenv("otp_login");
		$password = getenv("otp_pass");
		$otp_url = getenv("otp_url");

		$data = [
			"messages" => [
				[
					"recipient" => $params->phone,
					"priority" => "2",
					"sms" => [
						"originator" => getenv("otp_ordinator"),
						"content" => [
							"text" => "<#> Your OTP code: \n $confirmationcode \n99Yxa876Gxa"
						]
					],
					"message-id" => "201902280917"
				]
			]
		];

		$ch = curl_init($otp_url);

		curl_setopt($ch, CURLOPT_HTTPHEADER, [
			"Content-Type: application/json; charset=utf-8"
		]);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_USERPWD, "$login:$password");
		curl_setopt($ch, CURLOPT_POST, true);
		curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));

		$response = curl_exec($ch);
		$http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
		curl_close($ch);

		echo json_encode([
			"status" => 1,
			"data" => ["state" => 1]
		]);
		return;
	} else if ($params->step == 2) {
		$data = stmtall(
			"select * from s_activation where f_state=1 and f_session=? and f_phone=? and f_code=?",
			"sss",
			[$bearerToken, $params->phone, $params->code]
		)->fetch_assoc();
		if (empty($data)) {
			exitError(tr("Invalid confirmation code"));
		}
		$l = stmtall("select * from s_login_session where f_session=?", "s", [$bearerToken])->fetch_assoc();
		if (empty($l)) {
			exitError("Program error. No session for this activation");
		}
		$userexists = stmtall("select * from s_user where f_login=?", "s", [$params->phone])->fetch_assoc();
		if (!empty($userexists)) {
			$l["f_user"] = $userexists["f_id"];
			stmtall("update s_login_session set f_user=? where f_id=?", "ii", [$l["f_user"], $l["f_id"]]);
		}
		$session_key = $l["f_session"];
		stmtall("update s_user set f_login=? where f_id=?", "si", [$params->phone, $l["f_user"]]);
		stmtall("update s_activation set f_state=2 where f_id=?", "i", [$data["f_id"]]);
		$user = stmtall("select * from s_user where f_id=?", "i", [$l["f_user"]])->fetch_assoc();
		if (empty($user)) {
			exitError("Program error. No user with registered session id");
		}
		$row = $user;
	} else {
		exitError("Unknown flag");
	}
} else {
	exitError("I think, you wonna hack me");
}

$permissions = stmtall("select f_key from s_user_access where f_group=? and f_value=1", "i", [$row["f_group"]])->fetch_all(MYSQLI_NUM);
$permissions = array_column($permissions, 0);

$settings = stmtall("select f_key, f_value from s_settings_values where f_settings=?", "i", [$row["f_config"]])->fetch_all(MYSQLI_ASSOC);
$row_config = stmtall("select f_config from sys_json_config where f_id=?", "i", [$row["f_config"]])->fetch_assoc();
if (empty($row_config)) {
	$row_config = ["f_config" => ["dashboard" => "none"]];
} else {
	$row_config = ["f_config" => (array) json_decode($row_config["f_config"])];
}

$cashsession = stmtall("select f_id from s_working_sessions where f_close is null")->fetch_assoc();
if (empty($cashsession)) {
	$cashsession = ["f_id" => 0];
}

$menuversion = stmtall("select f_version from s_app where f_app='menu'")->fetch_row()[0];
$row_config["f_config"] = array_merge((array) $row_config["f_config"], ["menuversion" => $menuversion]);

$card_pattern = stmtall("select * from b_card_types")->fetch_all(MYSQLI_ASSOC);
$map = [
    1  => "code_pattern_discount_card",
    4  => "code_pattern_accumulate_card",
    10 => "code_pattern_present_card",
];

foreach ($card_pattern as $cp) {
    $id = (int)$cp["f_id"];
    if (isset($map[$id])) {
        $row_config["f_config"][$map[$id]] = $cp["f_pattern"];
    }
}


unset($row["f_password"]);
unset($row["f_altpassword"]);
echo json_encode([
	"status" => 1,
	"data" => [
		"sessionkey" => $session_key,
		"user" => $row,
		"permissions" => $permissions,
		"config" => $row_config,
		"settings" => $settings,
		"cashsession" => $cashsession,
		"database" => $dbname
	]
]);
