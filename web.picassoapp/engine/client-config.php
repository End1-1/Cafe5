<?php
# (C) 2025 Kudryashov Vasili
# Last modified - 2025-08-29 01:16:03
define("AUTH", 1);
require_once __DIR__ . "/app.php";
$res_version = 44;

$res = [];
$tr = "";
if (empty($params->res_version)) {
	$params->res_version = 0;
}
if ($params->res_version != $res_version) {
	array_push($res, ["available" => getBase64OfFile("available.svg")]);
	array_push($res, ["checkstoreinput" => getBase64OfFile("checkstoreinput.svg")]);
	array_push($res, ["logout" => getBase64OfFile("logout.svg")]);
	array_push($res, ["allcheckstoreinput" => getBase64OfFile("allcheckstoreinput.svg")]);
	array_push($res, ["config"=>getBase64OfFile("config.svg")]);
	array_push($res, ["forecast"=>getBase64OfFile("forecast.svg")]);
	array_push($res, ["checkin"=>getBase64OfFile("checkin.svg")]);
	array_push($res, ["draftsale"=>getBase64OfFile("draftsale.svg")]);
	array_push($res, ["minibar"=>getBase64OfFile("minibar.svg")]);
	$tr=file_get_contents("res/translator_hy.txt");
	
}

function getBase64OfFile($f) {
	$data = file_get_contents("res/$f");
	return base64_encode($data);
}

echo json_encode(["status"=>1, "data"=>["res"=>$res, "res_version"=>$res_version, "translator_hy"=>$tr]]);
