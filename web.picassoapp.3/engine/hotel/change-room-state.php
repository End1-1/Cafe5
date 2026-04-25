<?php
require_once(__DIR__ . "/hotel.php");

$currentState = hstmtall("select * from f_room where f_id=?", "i", [$params->room])->fetch_assoc();

if ($currentState["f_state"] == 2 && $params->newState == 0) {
    hstmtall("update f_room set f_state=0 where f_id=?", "i", [$params->room]);
    printResult(1, "ok");
} else {
    exitError(tr("Not available operation"));
}