<?php
require_once(__DIR__ . "/hotel.php");

if (empty($params->item)) {
    exitError(tr("Nothing to save"));
}

if (empty($params->reservation->f_invoice)) {
    exitError(tr("Empty invoice"));
}

$bind["f_source"] = $params->item->f_voucher;
$bind["f_res"] = $params->reservation->f_id;
$bind["f_inv"] = $params->reservation->f_invoice;
$bind["f_wdate"] = $params->wdate;
$bind["f_rdate"] = date("Y-m-d");
$bind["f_time"] = date("H:i:s");
$bind["f_user"] = 1;
$bind["f_room"] = $params->reservation->f_room;
$bind["f_guest"] = $params->reservation->f_guest;
$bind["f_itemCode"] = $params->item->f_id;
$bind["f_finalName"] = $params->item->f_name;
$bind["f_amountAMD"] = $params->price;
$bind["f_usedPrepaid"] = 0;
$bind["f_amountUSD"] = 0;
$bind["f_amountVAT"] = 0;
$bind["f_vatmode"] = 1;
$bind["f_fiscal"] = 0;
$bind["f_paymentMode"] = 1;
$bind["f_creditCard"] = 0;
$bind["f_cityLedger"] = 0;
$bind["f_paymentComment"] = "";
$bind["f_dc"] = $params->item->f_voucher == "RV" ? "DEBIT" : "CREDIT";
$bind["f_sign"] = $params->item->f_voucher == "RV" ? -1 : 1;
$bind["f_doc"] = "";
$bind["f_rec"] = "";
$bind["f_remarks"] = $params->comment;
$bind["f_finance"] = 1;
$bind["f_canceled"] = 0;
$bind["f_cancelUser"] = 0;
$bind["f_cancelReason"] = "";
$bind["f_cancelDate"] = null;
$bind["f_side"] = 0;
$bind["f_rb"] = 0;
$bind["f_session"] = 0;
$bind["f_fiscalmachine"] = 0;

if (empty($params->id)) {
    $params->id = voucherNum($params->item->f_voucher);
    $bind["f_id"] = $params->id;
    hinsert("m_register", $bind);
} else {
    hupdate("m_register", $bind, $params->id);
}

printResult(1, "");