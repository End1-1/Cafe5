<?php
require_once(__DIR__ . "/filters.php");

$cols=["X", tr("Code"), tr("Name")];
$sql = "select '0' as x, g.f_id, g.f_name from c_groups g order by 3 ";

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);