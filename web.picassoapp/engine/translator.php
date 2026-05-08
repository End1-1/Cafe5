<?php
defined('APP') or die('Die, vampire!');

$data = file_get_contents(__DIR__ . "/res/translator_hy.txt");
$tr=explode("\r\n", $data);
$translator = [];
foreach ($tr as $tt) {
	$ta = explode("=", $tt);
	if (count($ta)>1) {
		$translator[$ta[0]] = $ta[1];
	}
}

function tr($s) {
	global $translator;
	if (array_key_exists($s, $translator)) {
		return $translator[$s];
	}
	return $s;
}