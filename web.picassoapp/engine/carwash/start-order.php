<?php
require_once("carwash.php");

$db->begin_transaction();
stmtall("UPDATE o_header SET f_state=1, f_table=? WHERE f_id=?", "is", [$params->f_table, $params->f_id]);
stmtall("UPDATE o_body_process SET f_begin=CURRENT_TIMESTAMP() WHERE f_begin IS NULL AND f_id IN (SELECT f_id FROM o_body WHERE f_header=?)", "s", [$params->f_id]);
$db->commit();

printResult(1, '');