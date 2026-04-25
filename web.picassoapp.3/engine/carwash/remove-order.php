<?php
require_once("carwash.php");

$db->begin_transaction();
stmtall("update o_header set f_state=3 where f_id=?", "s", [$params->f_id]);
stmtall("update o_body set f_state=2 where f_header=?", "s", [$params->f_id]);
$cashs = stmtall("select f_id from a_header_cash where f_oheader=?", "s", [$params->f_id])->fetch_all(MYSQLI_ASSOC);
foreach ($cashs as $id) {
    $id = $id["f_id"];
    stmtall("delete from e_cash where f_header=?", "s", [$id]);
    stmtall("delete from a_header_cash where f_id=?", "s", [$id]);
    stmtall("delete from a_header where f_id=?", "s", [$id]);
}
$db->commit();

printResult(1, "ok");