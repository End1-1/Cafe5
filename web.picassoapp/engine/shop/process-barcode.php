<?php
# (C) 2024-2025 Kudryashov Vasili
# Created - ../../2025 :)
# Last modified - 2025-12-13 01:57:10
require_once __DIR__ . "/shop.php";

class ProcessBarCode extends PClass
{
    private $code;
    private $qrcode;
    public function __construct()
    {
        parent::__construct();
        $this->code = $this->params->code;
        $this->qrcode = "";
    }

    public function checkGiftCard()
    {
        $this->code = str_replace(";", "", $this->code);
        $this->code = str_replace("?", "", $this->code);
        $this->result["echocode"] = $this->code;
        $code = $this->code;
        if (substr($code, 0, 4) != "2211") {
            $code = "2211$code";
        }

        $card = $this->stmtall("select * from b_gift_card where f_code=?", "s", [$code])->fetch_assoc();
        if (empty($card)) {
            return false;
        }
        $this->result["card"] = $card;
        $used = ["f_amount" => 0];
        if (empty($card["f_datesaled"])) {
            if (!$this->getGoods(false)) {
                return false;
            }
        } else {
            $used = $this->stmtall("select sum(cast(f_amount as float)) as f_amount from b_gift_card_history where f_card=?", "i", [$card["f_id"]])->fetch_assoc();
            if (empty($used)) {
                $this->exitError($this->tr("Incorrect data for gift card") . "<br>" . $this->code);
            }
        }
        $this->result["used"] = $used["f_amount"];
        $this->result["barcode"] = 3;
        $this->echoResult();
        return true;
    }

    public function checkDiscountCard()
    {
        $card = $this->stmtall("SELECT * from b_cards_discount WHERE f_code=?", "s", [$this->code])->fetch_assoc();
        if (empty($card)) {
            return false;
        }
        if ($this->permissions->check(CP_T5_DISCOUNT)) {
            $this->exitError(Translator::t("You cannot use discount"));
        }
        $partner = $this->stmtall("select * from c_partners where f_id=?", "i", [$card["f_client"]])->fetch_assoc();
        $history = $this->stmtall("select coalesce(SUM(f_amount), 0) as f_amount FROM b_gift_card_history WHERE f_card=?", "i", [$card["f_id"]])->fetch_assoc();
        $this->result["card"] = $card;
        $this->result["partner"] = $partner;
        $this->result["history"] = $history;
        $this->result["barcode"] = 1;
        $this->echoResult();
        return true;
    }

    private function getGoods($echoresult = true)
    {
        $multibarcode = $this->stmtall("select g.f_scancode from c_goods_multiscancode m left join c_goods g on g.f_id=m.f_goods where m.f_id=?", "s", [$this->code])->fetch_assoc();
        if (!empty($multibarcode)) {
            $this->code = $multibarcode["f_scancode"];
        }
        $sql = <<<EOD
        SELECT g.f_id, g.f_name, gp.f_name as f_groupname, g.f_scancode, 
            g.f_qtybox, u.f_name as f_unitname, gp.f_taxdept, gp.f_adgcode, g.f_fiscalname, 
            g.f_candiscount, g.f_autodiscount,
            coalesce(g.f_service, 0) as f_service, coalesce(u.f_defaultqty, 0) as f_defaultqty, 
            if(cp.f_price1disc>0, cp.f_price1disc, cp.f_price1) as f_price1, 
            if(cp.f_price2disc>0, cp.f_price2disc, cp.f_price2) as f_price2 
            FROM c_goods g 
            LEFT JOIN c_groups gp on gp.f_id=g.f_group 
            LEFT JOIN c_goods_prices cp ON cp.f_goods=g.f_id AND cp.f_currency=1 
            LEFT JOIN c_units u ON u.f_id=g.f_unit 
            WHERE g.f_scancode=? and g.f_enabled=1 
        EOD;
        $goods = $this->stmtall($sql, "s", [$this->code])->fetch_assoc();
        if (empty($goods)) {
            return false;
        }

        $store = [];
        if ($goods["f_service"] == 0) {
            $havechanges = stmtall("select f_have_changes from c_storages where f_id=? and f_have_changes=1", "i", [$this->params->store])->fetch_assoc();
            if (!empty($havechanges)) {
                stmtall("delete from a_store_sale where f_store=?", "i", [$this->params->store]);
                stmtall("insert into a_store_sale (f_store, f_goods, f_qty, f_qtyreserve, f_qtyprogram) "
                    . "select ? as f_store, f_goods, sum(f_qty*f_type) as f_qty, 0, 0 "
                    . "from a_store where f_store=? group by 1, 2 having sum(f_qty*f_type)>0", "ii", [$this->params->store, $this->params->store]);
                stmtall("update c_storages set f_have_changes=0 where f_id=?", "i", [$this->params->store]);
            }

            $sql = <<<EOD
            SELECT a.f_qty, COALESCE(r.f_reserved, 0) as f_reserved, coalesce(b.f_draft , 0) AS f_draft 
            from a_store_sale a 

            left JOIN (
            SELECT b.f_goods, sum(cast(b.f_qty as FLOAT)) as f_draft 
            from o_draft_sale_body b 
            where b.f_goods=? and b.f_store=? and b.f_state=1
            ) b ON b.f_goods=a.f_goods 

            left JOIN (
            SELECT r.f_goods, sum(cast(r.f_qty AS FLOAT)) as f_reserved 
            from a_store_reserve r 
            where r.f_goods=? and r.f_store=? and r.f_state=1
            ) r on r.f_goods=a.f_goods 

            left join c_goods gg on gg.f_storeid=a.f_goods
            where gg.f_id=? AND f_store=? 
            EOD;
            $store = $this->stmtall(
                $sql,
                "iiiiii",
                [$goods["f_id"], $this->params->store, $goods["f_id"], $this->params->store, $goods["f_id"], $this->params->store]
            )->fetch_assoc();
            if (empty($store)) {
                $this->exitError($this->tr->tr("Insufficient quantity") . " " . $goods["f_defaultqty"]);
            }
            if ($store["f_qty"] - $store["f_reserved"] - $store["f_draft"] - $goods["f_defaultqty"] < 0) {
                $this->exitError($this->tr->tr("Insufficient quantity") . " " .
                    $store["f_qty"] - $store["f_reserved"] - $store["f_draft"] - $goods["f_defaultqty"]
                    . "\r\n" . $this->tr->tr("Reserved") . " " . $store["f_reserved"]
                    . "\r\n" . $this->tr->tr("In draft") . " " . $store["f_draft"]);
            }
        }

        if (!empty($this->qrcode)) {
            $duplicate = $this->stmtall("select * from o_draft_sale_body where f_emarks=? and f_state=1", "s", [$this->qrcode])->fetch_assoc();
            if (!empty($duplicate)) {
                $this->exitError($this->tr("Duplicate emarks"));
            }
            $goods["f_defaultqty"] = 1;
        }

        $draftid = $this->uuidv4();
        $v["f_id"] = $draftid;
        $v["f_header"] = $this->params->draft_header;
        $v["f_state"] = 1;
        $v["f_dateappend"] = date("Y-m-d");
        $v["f_timeappend"] = date("H:i:s");
        $v["f_store"] = $this->params->store;
        $v["f_goods"] = $goods["f_id"];
        $v["f_qty"] = $goods["f_defaultqty"];
        $v["f_emarks"] = empty($this->qrcode) ? null : $this->qrcode;
        $v["f_price"] = $this->params->retail == 1 ? $goods["f_price1"] : $goods["f_price2"];
        $this->sinsert("o_draft_sale_body", $v);

        $this->result["goods"] = $goods;
        $this->result["store"] = $store;
        $this->result["draftid"] = $draftid;
        $this->result["barcode"] = 2;
        $this->result["emarks"] = $this->qrcode;
        if ($echoresult) {
            $this->echoResult();
        }
        return true;
    }

    public function get()
    {
        if (strlen($this->code) >= 29) {
            //cigarette
            $this->qrcode = $this->code;
            if (substr($this->code, 0, 3) == "010") {
                $this->code = (string) intval(substr($this->code, 3, 13));
            } else if (substr($this->code, 0, 6) == "000000") {
                $this->code = substr($this->code, 6, 8);
            }
            if (strlen($this->code) >= 29) {
                $this->code = substr($this->code, 1, 13);
            }

            if ($this->getGoods()) {
                return;
            }
            $this->exitError($this->tr->tr("Wrong barcode") . "<br>" . $this->code);
        }
        if ($this->checkGiftCard()) {
            return;
        }
        if ($this->checkDiscountCard()) {
            return;
        }
        if ($this->getGoods()) {
            return;
        }
        $this->exitError($this->tr->tr("Wrong barcode") . "<br>" . $this->result["echocode"]);
    }
}

$processBarcode = new ProcessBarCode();
$processBarcode->get();
