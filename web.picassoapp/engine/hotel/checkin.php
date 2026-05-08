<?php
require_once(__DIR__ . "/hotel.php");

$sql = "update f_reservation set f_state=1 where f_state=2 and f_id=?";
hstmtall($sql, "s", [$params->reservation->f_id]);
$sql = "update f_room set f_state=1 where f_id=?";
hstmtall($sql, "i", [$params->reservation->f_room]);


printResult(1, "");