<?php
require_once(__DIR__ . "/hotel.php");

$entry = strtotime($params->entry);
$departure = strtotime($params->departure);
$today = strtotime(date("Y-m-d", time()));

if ($entry < $today){
    exitError(tr("Invalid entry date"));
}
if ($entry > $departure) {
    exitError(tr("Date of entry cannot be greater than date of departure"));
}

$filter_category = empty($params->category) ? "" : "and c.f_short='$params->category'";

$sql = "select f_id, f_short, f_description "
."from f_room_classes order by f_queue";
$categories = hqueryStr($sql)->fetch_all();

$sql =  "select r.f_id, r.f_short, r.f_state, c.f_short as f_cshort, c.f_description, r.f_bed, r.f_smoke, "
    . "r.f_rate, r.f_floor, v.f_en as f_view, r.f_building, r.f_class, rs.f_en as f_statename, "
    . "r.f_description as room_description, r.f_donotdisturbe, rss.f_id as f_reservation "
    . "from f_room r "
    . "left join f_reservation rss on rss.f_room=r.f_id and rss.f_state=1 "
    . "inner join f_room_classes c on c.f_id=r.f_class "
    . "inner join f_room_view v on v.f_id=r.f_view "
    . "inner join f_room_state rs on rs.f_id=r.f_state "
    . "where 1=1 $filter_category "
    . "order by r.f_building, r.f_id  ";
$rooms = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC);

$sql =  "select r.f_id, r.f_state, r.f_reserveState, r.f_room, rm.f_short, r.f_cardex, r.f_startDate, r.f_endDate, "
    . "concat(g.f_firstName, ' ', g.f_lastName) as f_guest_name, c.f_name as f_cardex_name, r.f_invoice,  "
    . "rs.f_en as f_reserveStateName, a.amount, coalesce(cc.credit, 0) as credit, coalesce(cd.debet, 0) as debet, "
    . "r.f_remarks, r.f_arrangement, r.f_author, r.f_group, "
    . "r.f_created, r.f_createTime, rm.f_donotdisturbe, r.f_chmstatus "
    ."from f_reservation r "
    . "left join f_room rm on rm.f_id=r.f_room "
    . "left join f_guests g on r.f_guest=g.f_id "
    . "inner join f_reservation_status rs on rs.f_id=r.f_reserveState "
    . "left join f_cardex c on c.f_cardex=r.f_cardex "
    . "left join (select f_inv, sum(f_amountAmd) as amount from m_register where f_source='AV' and f_canceled=0 and f_finance=1) a on  a.f_inv=r.f_invoice "
    ."left join (select f_inv, sum(f_amountAmd) as credit from m_register where f_sign=1 and f_canceled=0 and f_finance=1 group by 1) cc on cc.f_inv=r.f_invoice "
    . "left join (select f_inv, sum(f_amountAmd) as debet from m_register where f_sign=-1 and f_canceled=0 and f_finance=1 group by 1) cd on cd.f_inv=r.f_invoice "
    . "where (r.f_state in (1,2) or (r.f_state in (4, 9, 7) and r.f_endDate>='$today')) ";
$reservations = hqueryStr($sql)->fetch_all(MYSQLI_ASSOC);

printResult(1, ["rooms"=>$rooms, "categories"=>$categories]);