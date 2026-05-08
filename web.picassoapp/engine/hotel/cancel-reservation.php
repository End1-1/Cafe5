<?php
require_once(__DIR__ . "/hotel.php");

$sql = "select coalesce(sum(f_amountamd), 0) as f_amountamd from m_register where f_inv=? and f_canceled=0 and f_finance=1";
$result = hstmtall($sql, "s", [$params->reservation->f_invoice])->fetch_all(MYSQLI_ASSOC)[0];
if ($result["f_amountamd"] > 0) {
    exitError(tr("Cannot cancel reservation with non zero balance"));
}

$sql = "update f_reservation set f_state=6 where f_state=2 and f_id=?";
hstmtall($sql, "s", [$params->reservation->f_id]);

printResult(1, "");