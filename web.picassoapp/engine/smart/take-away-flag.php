<?php
require_once(__DIR__ . "/smart.php");

if (empty($params->header)) {
    exitError(tr("Empty order not needed flag take away"));
}

stmtall("update o_header_flags set f_3=? where f_id=?", "is", [$params->flag, $params->header]);

printResult(1, "ok");