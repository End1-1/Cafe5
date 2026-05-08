<?php
require_once (__DIR__ . "/hotel.php");

if (empty($params->guests)) {
    exitError(tr("Guest list empty"));
}

$cross = hstmtall(
    "select f_id, f_startdate, f_enddate from f_reservation "
    . "where f_startdate between '$params->entry' and '$params->departure' "
    . "and f_state in (1,2,4,7,9) and f_room=? and f_id<>?",
    "is",
    [$params->room, empty($params->reservation->f_id) ? "XXXX" : $params->reservation->f_id]
)->fetch_all(MYSQLI_ASSOC);
if (!empty($cross)) {
    $err = "\r\n";
    foreach ($cross as $c) {
        $err .= "{$c["f_id"]} {$c["f_startdate"]} - {$c["f_enddate"]} \r\n";
    }
    exitError(tr("A reservation exists on your selected entry and departure dates" . $err));
}

$hdb->begin_transaction();

foreach ($params->guests as $guest) {
    if (empty($guest->f_id) || $guest->f_id == 0) {
        $bind = [];
        $bind["f_firstname"] = $guest->f_firstname;
        $bind["f_lastname"] = $guest->f_lastname;
        $bind["f_title"] = "MR.";
        $bind["f_tel1"] = $guest->f_tel1;
        $guest->f_id = hinsert("f_guests", $bind);
    }
    if ($params->guestid == 0) {
        $params->guestid = $guest->f_id;
    }
}

$isnew = empty($params->reservation->f_id);
if ($isnew) {
    $rsId = voucherNum("RS");
    $inId = voucherNum("IN");
    $inId = "IN-" . $rsId;
    $rsId = "RS-" . $rsId;
    $params->reservation->f_state = 2;
    $params->reservation->f_id=$rsId;
} else {
    $rsId = $params->reservation->f_id;
    $inId = $params->reservation->f_invoice;
}



$bind = [];
$bind["f_id"] = $rsId;
$bind["f_invoice"] = $inId;
$bind["f_created"] = date("Y-m-d");
$bind["f_author"] = 1;
$bind["f_state"] = $params->reservation->f_state;
$bind["f_reservestate"] = 4;
$bind["f_arrangement"] = 1;
$bind["f_mealincluded"] = 0;
$bind["f_chm"] = "";
$bind["f_chmstatus"] = 1;
$bind["f_room"] = $params->room;
$bind["f_group"] = 0;
$bind["f_guest"] = $params->guestid;
$bind["f_man"] = 1;
$bind["f_woman"] = 0;
$bind["f_child"] = 0;
$bind["f_baby"] = 0;
$bind["f_cardex"] = "ID001";
$bind["f_cityledger"] = 0;
$bind["f_booking"] = "";
$bind["f_startdate"] = $params->entry;
$bind["f_enddate"] = $params->departure;
$bind["f_roomfee"] = $params->price;
$bind["f_paymenttype"] = 1;
$bind["f_mealqty"] = 1;
$bind["f_mealprice"] = 0;
$bind["f_extrabed"] = 0;
$bind["f_extrabedfee"] = 0;
$bind["f_pricepernight"] = $params->price;
$bind["f_vat"] = 20;
$bind["f_vatamount"] = 0;
$bind["f_vatmode"] = 1;
$bind["f_total"] = $params->total;
$bind["f_grandTotal"] = $params->total;
$bind["f_totalusd"] = 0;
$bind["f_remarks"] = $params->remarks;
$bind["f_createtime"] = date("H:i:s");

if ($isnew) {
    hinsert("f_reservation", $bind);
} else {
    hupdate("f_reservation", $bind, $params->reservation->f_id);
}

hstmtall("delete from f_reservation_guests where f_reservation=?", "s", [$rsId]);
foreach ($params->guests as $guest) {
    $bind = [];
    $bind["f_reservation"] = $rsId;
    $bind["f_guest"] = $guest->f_id;
    $bind["f_first"] = intval($guest->f_id == $params->guestid);
    hinsert("f_reservation_guests", $bind);
}

$hdb->commit();

$params->f_id = $params->reservation->f_id;
$folio = new Folio();
$folio->open();