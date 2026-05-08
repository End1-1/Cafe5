<?php
require_once __DIR__ . "/../../../../app.php";

function getFilterResult($sql) {
	global $db;
	if (!$result = $db->query($sql)) {
		exitError($db->error);
	}
	$rows = [];
	while ($row = $result->fetch_row()){
		array_push($rows, $row);
	}
	return $rows;
}