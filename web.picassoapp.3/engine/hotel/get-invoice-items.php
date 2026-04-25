<?php
require_once(__DIR__ . "/hotel.php");

$sql = "select f_id, f_vaucher as f_voucher,  f_en as f_name "
. "from f_invoice_item "
. "where length(f_vaucher)>0 ";
$result = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC);
printResult(1, ["items" => $result]);