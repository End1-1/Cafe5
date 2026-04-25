<?php
require_once(__DIR__ . "/smart.php");

$db->begin_transaction();

$isnew = empty($params->header->id);
if ($isnew) {
    $params->header->id = uuidv4();
    //COUNTER OF ORDER
    $hallcounter = stmtall("select f_counterhall from h_halls where f_id=?", "i", [$config->hall])->fetch_assoc()["f_counterhall"];
    $counterPrefix = stmtall("select f_counter + 1 as f_counter, f_prefix from h_halls where f_id=? for update", "i", [$hallcounter])->fetch_assoc();
    stmtall("update h_halls set f_counter=? where f_id=?", "ii", [$counterPrefix["f_counter"], $hallcounter]);
    $bind["f_hallid"] = $counterPrefix["f_counter"];
    $bind["f_prefix"] = $counterPrefix["f_prefix"];
} else {
    $oheader = stmtall("select * from o_header where f_id=?", "s", [$params->header->id])->fetch_assoc();
    $counterPrefix = ["f_counter" => $oheader["f_hallid"], "f_prefix" => $oheader["f_prefix"]];
}


$bind["f_id"] = $params->header->id;
$bind["f_state"] = $params->header->state;
$bind["f_hall"] = $config->hall;
$bind["f_table"] = $params->header->table;
$bind["f_datecash"] = date("Y-m-d");
if ($isnew) {
    $bind["f_dateopen"] = date("Y-m-d");
    $bind["f_timeopen"] = date("H:i:s");
}
if ($params->header->state == 2) {
    $bind["f_dateclose"] = date("Y-m-d");
    $bind["f_timeclose"] = date("H:i:s");
}
$bind["f_staff"] = $userid;
$bind["f_cashier"] = $userid;
$bind["f_comment"] = $params->header->phone;
$bind["f_amounttotal"] = $params->header->amounttotal;
$bind["f_amountcash"] = $params->header->amountcash;
$bind["f_amountcard"] = $params->header->amountcard;
$bind["f_amountother"] = $params->header->amountother;
$bind["f_amountidram"] = $params->header->amountidram;
$bind["f_amountdiscount"] = $params->header->amountdiscount;
$bind["f_discountfactor"] = $params->header->discountfactor;
$bind["f_partner"] = $params->header->partner;
$bind["f_taxpayertin"] = $params->header->taxpayertin;
$bind["f_working_session"] = $params->header->sessionid;
sinsertupdate("o_header", $bind, $params->header->id, $isnew);

/*margar*/
if ($params->bhistory->data > 0) {
    $card = $this->stmtall("select * from b_cards_discount where f_id=?", "i", [$params->bhistory->card])->fetch_assoc();
    if (empty($card)) {
        die("Invalid code");
    }
    $balance = $this->stmtall("select cast(sum(f_data) as float) as f_balance from b_history where f_card=?", "i", [$params->bhistory->card])->fetch_assoc();
    if ($balance["f_balance"] > 0) {
        if ($balance["f_balance"] > 0) {
            $bi["f_id"] = uuidv4();
            $bi["f_card"] = $card["f_id"];
            $bi["f_value"] = 0;
            $bi["f_data"] = -1 * $balance["f_balance"];
            $bi["f_comment"] = "expired";
            $this->sinsert("b_history", $bi);
        }
    }
}
/* end margar */

$bind["f_id"] = $params->header->id;
$bind["f_card"] = $params->bhistory->card;
$bind["f_type"] = $params->bhistory->type;
$bind["f_value"] = $params->bhistory->value;
$bind["f_data"] = $params->bhistory->data;


sinsertupdate("b_history", $bind, $params->header->id, $isnew);

$bind["f_id"] = $params->header->id;
$bind["f_1"] = $params->flags->f1;
if ($isnew) {
    $bind["f_2"] = 0;
}
$bind["f_3"] = $params->flags->f3;
$bind["f_4"] = $params->flags->f4;
$bind["f_5"] = $params->flags->f5;
sinsertupdate("o_header_flags", $bind, $params->header->id, $isnew);

$setFlag1 = false;
foreach ($params->dishes as $d) {
    $isbodynew = empty($d->obodyid);
    if ($isbodynew) {
        $d->obodyid = uuidv4();
    }
    $bind["f_id"] = $d->obodyid;
    $bind["f_header"] = $params->header->id;
    $bind["f_state"] = $d->state;
    $bind["f_dish"] = $d->dishid;
    $bind["f_qty1"] = $d->qty;
    $bind["f_qty2"] = $d->qty2;
    $bind["f_price"] = $d->price;
    $bind["f_total"] = $d->total;
    $bind["f_grandtotal"] = $d->total;
    $bind["f_service"] = $d->service;
    $bind["f_discount"] = $d->discount;
    $bind["f_store"] = $d->store;
    $bind["f_print1"] = $d->print1;
    $bind["f_print2"] = $d->print2;
    $bind["f_comment"] = $d->comment;
    $bind["f_remind"] = $d->remind;
    $bind["f_adgcode"] = $d->adgcode;
    $bind["f_removereason"] = $d->removereason;
    $bind["f_timeorder"] = $d->timeorder;
    $bind["f_package"] = $d->package;
    $bind["f_row"] = $d->row;
    $bind["f_emarks"] = $d->emarks;
    sinsertupdate("o_body", $bind, $d->obodyid, $isbodynew);


    if ($d->qty2 < 0) {
        $setFlag1 = true;
    }
}

if ($setFlag1) {
    stmtall("update o_header_flags set f_2=1 where f_id=?", "s", [$params->header->id]);
}

//ORDER CLOSED CREATE CASH DOCUMENTS
if ($params->header->state == 2) {

    if ($params->header->amountcash > 0) {
        $cashheaderid = uuidv4();
        $v["f_id"] = $cashheaderid;
        $v["f_userid"] = "Վաճառք " . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
        $v["f_state"] = 1;
        $v["f_type"] = 5;
        $v["f_operator"] = $userid;
        $v["f_date"] = date("Y-m-d");
        $v["f_createdate"] = date("Y-m-d");
        $v["f_createtime"] = date("H:i:s");
        $v["f_partner"] = 0;
        $v["f_amount"] = $params->header->amountcash;
        $v["f_comment"] = "";
        $v["f_payment"] = 0;
        $v["f_paid"] = 1;
        $v["f_currency"] = 1;
        $v["f_working_session"] = $params->header->sessionid;
        $v["f_body"] = "{}";
        sinsert("a_header", $v);

        $v["f_id"] = $cashheaderid;
        $v["f_cashin"] = $config->cash_cash_id;
        $v["f_cashout"] = 0;
        $v["f_oheader"] = $params->header->id;
        $v["f_related"] = 1;
        sinsert("a_header_cash", $v);

        $v["f_id"] = $cashheaderid;
        $v["f_header"] = $cashheaderid;
        $v["f_cash"] = $config->cash_cash_id;
        $v["f_sign"] = 1;
        $v["f_remarks"] = "Վաճառք " . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
        $v["f_amount"] = $params->header->amountcash;
        $v["f_base"] = $cashheaderid;
        $v["f_row"] = 0;
        $v["f_working_session"] = $params->header->sessionid;
        sinsert("e_cash", $v);
    }

    if ($params->header->amountcard > 0 || $params->header->amountidram > 0) {
        $cashheaderid = uuidv4();
        $v["f_id"] = $cashheaderid;
        $v["f_userid"] = "Վաճառք" . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
        $v["f_state"] = 1;
        $v["f_type"] = 5;
        $v["f_operator"] = $userid;
        $v["f_date"] = date("Y-m-d");
        $v["f_createdate"] = date("Y-m-d");
        $v["f_createtime"] = date("H:i:s");
        $v["f_partner"] = 0;
        $v["f_amount"] = $params->header->amountcard + $params->header->amountidram;
        $v["f_comment"] = "";
        $v["f_payment"] = 0;
        $v["f_paid"] = 1;
        $v["f_currency"] = 1;
        $v["f_working_session"] = $params->header->sessionid;
        $v["f_body"] = "{}";
        sinsert("a_header", $v);

        $v["f_id"] = $cashheaderid;
        $v["f_cashin"] = $config->cash_card_id;
        $v["f_cashout"] = 0;
        $v["f_oheader"] = $params->header->id;
        $v["f_related"] = 1;
        sinsert("a_header_cash", $v);

        $v["f_id"] = $cashheaderid;
        $v["f_header"] = $cashheaderid;
        $v["f_cash"] = $config->cash_card_id;
        $v["f_sign"] = 1;
        $v["f_remarks"] = "Վաճառք" . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
        $v["f_amount"] = $params->header->amountcard + $params->header->amountidram;
        $v["f_base"] = $cashheaderid;
        $v["f_row"] = 0;
        $v["f_working_session"] = $params->header->sessionid;
        sinsert("e_cash", $v);
    }
}


$db->commit();

if ($params->header->state == 2) {
    require_once(__DIR__ . "/../worker/create-store-out.php");
    $storeout = new CreateStoreOut();
    $storeout->makeOutput($params->header->id);
}

printResult(1, ["header" => $params->header->id, "state" => $params->header->state]);
