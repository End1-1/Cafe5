<?php
require __DIR__ . "/index.php";

class KinoparkOrder extends PClass
{
    public function create()
    {

        $this->db->begin_transaction();

        $isnew = empty($this->params->header->id);
        if ($isnew) {
            $this->params->header->id = uuidv4();
            //COUNTER OF ORDER
            $hallcounter = stmtall("select f_counterhall from h_halls where f_id=?", "i", [$this->params->header->hall])->fetch_assoc()["f_counterhall"];
            $counterPrefix = stmtall("select f_counter + 1 as f_counter, f_prefix from h_halls where f_id=? for update", "i", [$hallcounter])->fetch_assoc();
            $this->stmtall("update h_halls set f_counter=? where f_id=?", "ii", [$counterPrefix["f_counter"], $hallcounter]);
            $bind["f_hallid"] = $counterPrefix["f_counter"];
            $bind["f_prefix"] = $counterPrefix["f_prefix"];
        } else {
            $oheader = stmtall("select * from o_header where f_id=?", "s", [$this->params->header->id])->fetch_assoc();
            $counterPrefix = ["f_counter" => $oheader["f_hallid"], "f_prefix" => $oheader["f_prefix"]];
        }

        $bind["f_id"] = $this->params->header->id;
        $bind["f_state"] = $this->params->header->state;
        $bind["f_hall"] = $this->params->header->hall;
        $bind["f_table"] = $this->params->header->table;
        $bind["f_datecash"] = date("Y-m-d");
        if ($isnew) {
            $bind["f_dateopen"] = date("Y-m-d");
            $bind["f_timeopen"] = date("H:i:s");
        }
        if ($this->params->header->state == 2) {
            $bind["f_dateclose"] = date("Y-m-d");
            $bind["f_timeclose"] = date("H:i:s");
        }
        $bind["f_staff"] = getenv("createordercashier");
        $bind["f_cashier"] = getenv("createorderstaff");
        $bind["f_comment"] = $this->params->header->comment;
        $bind["f_amounttotal"] = $this->params->header->amounttotal;
        $bind["f_amountcash"] = $this->params->header->amountcash;
        $bind["f_amountcard"] = $this->params->header->amountcard;
        $bind["f_amountother"] = $this->params->header->amountother;
        $bind["f_amountidram"] = $this->params->header->amountidram;
        $bind["f_servicefactor"] = $this->params->header->servicefactor;
        $bind["f_amountservice"] = $this->params->header->amountservice;
        $bind["f_amountdiscount"] = $this->params->header->amountdiscount;
        $bind["f_discountfactor"] = $this->params->header->discountfactor;
        $bind["f_partner"] = $this->params->header->partner;
        $bind["f_taxpayertin"] = $this->params->header->taxpayertin;
        $bind["f_working_session"] = $this->params->sessionid;
        $this->sinsertupdate("o_header", $bind, $this->params->header->id, $isnew);

        $bind["f_id"] = $this->params->header->id;
        $bind["f_address"] = json_encode($this->params->address);
        $this->sinsertupdate("o_preorder", $bind, $this->params->header->id, $isnew);

        /*
        $bind["f_id"] = $this->params->header->id;
        $bind["f_card"] = $this->params->bhistory->card;
        $bind["f_type"] = $this->params->bhistory->type;
        $bind["f_value"] = $this->params->bhistory->value;
        $bind["f_data"] = $this->params->bhistory->data;
        $this->sinsertupdate("b_history", $bind, $this->params->header->id, $isnew);
        */

        $bind["f_id"] = $this->params->header->id;
        $bind["f_1"] = $this->params->flags->f1;
        if ($isnew) {
            $bind["f_2"] = 0;
        }
        $bind["f_3"] = $this->params->flags->f3;
        $bind["f_4"] = $this->params->flags->f4;
        $bind["f_5"] = $this->params->flags->f5;
        $this->sinsertupdate("o_header_flags", $bind, $this->params->header->id, $isnew);

        $setFlag1 = false;
        foreach ($this->params->dishes as $d) {
            $isbodynew = empty($d->obodyid);
            if ($isbodynew) {
                $d->obodyid = $this->uuidv4();
                $bind["f_appendtime"] = date("Y-m-d H:i:s");
                $bind["f_appenduser"] = $this->userid;
            }
            $bind["f_id"] = $d->obodyid;
            $bind["f_header"] = $this->params->header->id;
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
            $this->sinsertupdate("o_body", $bind, $d->obodyid, $isbodynew);

            if ($d->qty2 < 0) {
                $setFlag1 = true;
            }
        }

        if ($setFlag1) {
            $this->stmtall("update o_header_flags set f_2=1 where f_id=?", "s", [$this->params->header->id]);
        }

        //ORDER CLOSED CREATE CASH DOCUMENTS
        if ($this->params->header->state == 2) {

            if ($this->params->header->amountcash > 0) {
                $cashheaderid = uuidv4();
                $v["f_id"] = $cashheaderid;
                $v["f_userid"] = "Վաճառք " . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
                $v["f_state"] = 1;
                $v["f_type"] = 5;
                $v["f_operator"] = $this->userid;
                $v["f_date"] = date("Y-m-d");
                $v["f_createdate"] = date("Y-m-d");
                $v["f_createtime"] = date("H:i:s");
                $v["f_partner"] = 0;
                $v["f_amount"] = $this->params->header->amountcash;
                $v["f_comment"] = "";
                $v["f_payment"] = 0;
                $v["f_paid"] = 1;
                $v["f_currency"] = 1;
                $v["f_working_session"] = $this->params->sessionid;
                $v["f_body"] = "{}";
                $this->sinsert("a_header", $v);

                $v["f_id"] = $cashheaderid;
                $v["f_cashin"] = 1;
                $v["f_cashout"] = 0;
                $v["f_oheader"] = $this->params->header->id;
                $v["f_related"] = 1;
                $this->sinsert("a_header_cash", $v);

                $v["f_id"] = $cashheaderid;
                $v["f_header"] = $cashheaderid;
                $v["f_cash"] = 1;
                $v["f_sign"] = 1;
                $v["f_remarks"] = "Վաճառք " . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
                $v["f_amount"] = $this->params->header->amountcash;
                $v["f_base"] = $cashheaderid;
                $v["f_row"] = 0;
                $v["f_working_session"] = $this->params->sessionid;
                $this->sinsert("e_cash", $v);
            }

            if ($this->params->header->amountcard > 0 || $this->params->header->amountidram > 0) {
                $cashheaderid = $this->uuidv4();
                $v["f_id"] = $cashheaderid;
                $v["f_userid"] = "Վաճառք" . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
                $v["f_state"] = 1;
                $v["f_type"] = 5;
                $v["f_operator"] = $this->userid;
                $v["f_date"] = date("Y-m-d");
                $v["f_createdate"] = date("Y-m-d");
                $v["f_createtime"] = date("H:i:s");
                $v["f_partner"] = 0;
                $v["f_amount"] = $this->params->header->amountcard + $this->params->header->amountidram;
                $v["f_comment"] = "";
                $v["f_payment"] = 0;
                $v["f_paid"] = 1;
                $v["f_currency"] = 1;
                $v["f_working_session"] = $this->params->sessionid;
                $v["f_body"] = "{}";
                $this->sinsert("a_header", $v);

                $v["f_id"] = $cashheaderid;
                $v["f_cashin"] = 2;
                $v["f_cashout"] = 0;
                $v["f_oheader"] = $this->params->header->id;
                $v["f_related"] = 1;
                $this->sinsert("a_header_cash", $v);

                $v["f_id"] = $cashheaderid;
                $v["f_header"] = $cashheaderid;
                $v["f_cash"] = 2;
                $v["f_sign"] = 1;
                $v["f_remarks"] = "Վաճառք" . $counterPrefix["f_prefix"] . $counterPrefix["f_counter"];
                $v["f_amount"] = $this->params->header->amountcard + $this->params->header->amountidram;
                $v["f_base"] = $cashheaderid;
                $v["f_row"] = 0;
                $v["f_working_session"] = $this->params->sessionid;
                $this->sinsert("e_cash", $v);
            }
        }


        $this->db->commit();

        if ($this->params->header->state == 2) {
            require_once __DIR__ . "/../worker/create-store-out.php";
            $storeout = new CreateStoreOut();
            $storeout->makeOutput($this->params->header->id);
        }

        $this->result["header"] = $this->params->header->id;
        $this->result["state"] = $this->params->header->state;
        $this->echoResult();
    }
}

if (!empty($params->action)) {
    $o = new KinoparkOrder();
    $o->{$params->action}();
}