<?php
require_once(__DIR__ . "/hotel.php");

$sql = "select r.f_id, r.f_startdate, r.f_enddate, r.f_state,  rm.f_short as f_roomshort, "
. "concat_ws(' ', g.f_firstname, g.f_lastname) as f_guests, coalesce(sum(m.f_amountamd), 0) as f_balance "
."from f_reservation r "
."left join m_register m on m.f_inv=r.f_invoice and m.f_finance=1 and m.f_canceled=0 "
."left join f_guests g on g.f_id=r.f_guest "
."left join f_room rm on rm.f_id=r.f_room "
."where r.f_startdate='$params->date' and r.f_state=2 "
."group by 1 ";
$checkin = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC);

$sql = "select r.f_id, r.f_startdate, r.f_enddate, r.f_state,  rm.f_short as f_roomshort, "
. "concat_ws(' ', g.f_firstname, g.f_lastname) as f_guests, coalesce(sum(m.f_amountamd), 0) as f_balance "
."from f_reservation r "
."left join m_register m on m.f_inv=r.f_invoice and m.f_finance=1 and m.f_canceled=0 "
."left join f_guests g on g.f_id=r.f_guest "
."left join f_room rm on rm.f_id=r.f_room "
."where r.f_state=1 "
."group by 1 ";
$inhouse = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC);

$sql = "select r.f_id, r.f_startdate, r.f_enddate, r.f_state,  rm.f_short as f_roomshort, "
. "concat_ws(' ', g.f_firstname, g.f_lastname) as f_guests, coalesce(sum(m.f_amountamd), 0) as f_balance "
."from f_reservation r "
."left join m_register m on m.f_inv=r.f_invoice and m.f_finance=1 and m.f_canceled=0 "
."left join f_guests g on g.f_id=r.f_guest "
."left join f_room rm on rm.f_id=r.f_room "
."where r.f_enddate='$params->date' and r.f_state=3 "
."group by 1 ";
$alreadycheckout = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC);

$sql = "select r.f_id, r.f_startdate, r.f_enddate, r.f_state,  rm.f_short as f_roomshort, "
. "concat_ws(' ', g.f_firstname, g.f_lastname) as f_guests, coalesce(sum(m.f_amountamd), 0) as f_balance "
."from f_reservation r "
."left join m_register m on m.f_inv=r.f_invoice and m.f_finance=1 and m.f_canceled=0 "
."left join f_guests g on g.f_id=r.f_guest "
."left join f_room rm on rm.f_id=r.f_room "
."where r.f_enddate='$params->date' and r.f_state=1 "
."group by 1 ";
$checkout = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC);

$sql = "select coalesce(sum(f_amountamd), 0) as f_amountamd "
."from m_register where f_wdate=? and f_finance=1 and f_canceled=0 and f_source='RV'";
$payment = hstmtall($sql, "s", [$params->date])->fetch_all(MYSQLI_ASSOC);

$sql = "select f_value from f_global_settings where f_key='Working date' and f_settings=1";
$workingday = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC)[0]["f_value"];

printResult(1, [
    "checkin" => $checkin, 
    "inhouse"=>$inhouse, 
    "checkout"=>$checkout,
    "alreadycheckout"=>$alreadycheckout, 
    "workingday" => $workingday, 
    "payment"=>$payment] );