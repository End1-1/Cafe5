<?php
require_once("filters.php");

$cols=["X", tr("Code"), tr("Name"), tr("Taxpayer name"), tr("Taxpayer number"), tr("Phone"), tr("Address")];
$sql = "select '0' as x, p.f_id, p.f_name, p.f_taxname, p.f_taxcode,  p.f_phone, p.f_address from c_partners p order by 3";

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);