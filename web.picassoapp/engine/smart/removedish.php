<?php
#(C) 01/02/2025  Kudryashov Vasili
require_once __DIR__ . "/smart.php";

$dish = stmtall("select * from o_body where f_id=?", "s", [$params->obodyid])->fetch_assoc();
$state = $dish["f_qty2"] > 0 ? 2 : 0;
stmtall(
    "update o_body set f_state=?, f_emarks=null, f_removetime=current_timestamp(), f_removeuser=? where f_id=?",
    "iis",
    [$state, $userid, $params->obodyid]
);
if ($state == 2) {
    stmtall("update o_header_flags set f_2=1 where f_id=?", "s", [$params->header]);
    #stmtall("update o_body set f_qty2=0 where f_header=? and f_state=1", "s", [$params->header]);
}

printResult(1, ["obodyid" => $params->obodyid, "state" => $state]);