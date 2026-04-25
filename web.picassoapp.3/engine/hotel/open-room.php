<?php
require_once(__DIR__ . "/hotel.php");

$sql = "select f_id, f_short, f_state from f_room where f_id=?";
$room = hstmtall($sql, "i", [$params->room])->fetch_all(MYSQLI_ASSOC)[0];

$reservation = [];
if( $params->state == 1) {
    $sql = "select * from f_reservation where f_state=1 and f_room=?";
    $reservation = hstmtall($sql, "i", [$params->room])->fetch_all(MYSQLI_ASSOC);
}

$guests = [];
$folio = [];
if (!empty($reservation)) {
    $sql = "select gg.f_firstname, gg.f_lastname, gg.f_tel1 "
        . "from f_reservation_guests rg "
        . "left join f_guests gg on gg.f_id=rg.f_guest "
        . "where rg.f_reservation=?";
    $guests = hstmtall($sql, "s", [$reservation[0]["f_id"]])->fetch_all(MYSQLI_ASSOC);

    $sql = "select f_wdate, f_amountamd, f_finalname, f_sign  "
    . "from m_register m "
    . "where m.f_inv=? and m.f_canceled=0 and m.f_finance=1 "
    . "order by f_wdate ";
    $folio = hstmtall($sql, "s", [$reservation[0]["f_invoice"]])->fetch_all(MYSQLI_ASSOC);
}


printResult(1, ["room" => $room, "reservation"=>empty($reservation) ? [] : $reservation[0], "guests" => $guests, "folio"=>$folio]);