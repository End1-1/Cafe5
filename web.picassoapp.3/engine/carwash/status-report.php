<?php
require_once(__DIR__ . "/carwash.php");

$sql = "select sum(f_amounttotal), sum(f_amountcash), sum(f_amountcard), sum(f_amountidram) "
."from o_header h "
."where h.f_datecash between '$params->start' and '$params->end' "
."and h.f_state=2 ";
$result = queryStr($sql);
$total = $result->fetch_all();

$sql = "select sum(f_amounttotal), sum(f_amountcash), sum(f_amountcard), sum(f_amountidram) "
."from o_header h "
."inner join o_tax_log t on t.f_order=h.f_id and t.f_state=1 "
."where h.f_datecash between '$params->start' and '$params->end' "
."and h.f_state=2 ";
$result = queryStr($sql);
$totalfiscal = $result->fetch_all();

printResult(1, ["daterange"=>[$params->start, $params->end], "total" => $total, "totalfiscal"=>$totalfiscal]);