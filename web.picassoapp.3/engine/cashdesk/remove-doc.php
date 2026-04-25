<?php
require_once(__DIR__ . "/../app.php");

if (empty($params->id)) {
    exitError("Are you hacker?");
}

stmtall("delete from e_cash where f_header=?", "s", [$params->id]);
stmtall("delete from a_header_cash where f_id=?", "s", [$params->id]);
stmtall("delete from a_header where f_id=?", "s", [$params->id]);

if (!empty($params->refresh)) {
    switch ($params->refresh) {
        case 'carwashcashdesk':
            require_once(__DIR__ . "/../carwash/cashreport.php");
            break;
        default:
            exitError("No valid url for refresh");
            break;
    }
} else {
    printResult(1, "ok");
}