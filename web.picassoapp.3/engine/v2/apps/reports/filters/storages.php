<?php
require_once "filters.php";

$cols=["X", tr("Code"), tr("Name")];
$sql = "select '0' as x, f_id, f_name "
    . "from c_storages "
    . "order by 3";

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);