<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-21 10:41:18
# Last Modified: 2025-12-21 10:41:22
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class ProcessBarcode extends Auth
{

    private $code;
    private $qrcode;
    private $store;
    private $retail;
    private $draft_header;
    private function getGoods($echoresult = true)
    {
        $multibarcode = $this->select("select g.f_scancode from c_goods_multiscancode m left join c_goods g on g.f_id=m.f_goods where m.f_id=?", "s", [$this->code])->fetch_assoc();
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
        $goods = $this->select($sql, "s", [$this->code])->fetch_assoc();
        if (empty($goods)) {
            return false;
        }

        $store = [];
        if ($goods["f_service"] == 0) {
            $havechanges = $this->select("select f_have_changes from c_storages where f_id=? and f_have_changes=1", "i", [$this->store])->fetch_assoc();
            if (!empty($havechanges)) {
                $this->select("delete from a_store_sale where f_store=?", "i", [$this->store]);
                $this->select("insert into a_store_sale (f_store, f_goods, f_qty, f_qtyreserve, f_qtyprogram) "
                    . "select ? as f_store, f_goods, sum(f_qty*f_type) as f_qty, 0, 0 "
                    . "from a_store where f_store=? group by 1, 2 having sum(f_qty*f_type)>0", "ii", [$this->store, $this->params->store]);
                $this->select("update c_storages set f_have_changes=0 where f_id=?", "i", [$this->store]);
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
            $store = $this->select(
                $sql,
                "iiiiii",
                [$goods["f_id"], $this->store, $goods["f_id"], $this->store, $goods["f_id"], $this->store]
            )->fetch_assoc();
            if (empty($store)) {
                dieWithCode(Translator::t("Insufficient quantity") . " " . $goods["f_defaultqty"]);
            }
            if ($store["f_qty"] - $store["f_reserved"] - $store["f_draft"] - $goods["f_defaultqty"] < 0) {
                dieWithCode(Translator::t("Insufficient quantity") . " " .
                    $store["f_qty"] - $store["f_reserved"] - $store["f_draft"] - $goods["f_defaultqty"]
                    . "\r\n" . Translator::t("Reserved") . " " . $store["f_reserved"]
                    . "\r\n" . Translator::t("In draft") . " " . $store["f_draft"]);
            }
        }

        if (!empty($this->qrcode)) {
            $duplicate = $this->select("select * from o_draft_sale_body where f_emarks=? and f_state=1", "s", [$this->qrcode])->fetch_assoc();
            if (!empty($duplicate)) {
                dieWithCode(Translator::t("Duplicate emarks"));
            }
            $goods["f_defaultqty"] = 1;
        }

        $this->result["goods"] = $goods;
        $this->result["store"] = $store;
        $this->result["draftid"] = "";
        $this->result["barcode"] = 2;
        $this->result["emarks"] = $this->qrcode;
        if ($echoresult) {
            $this->echoResult();
        }
        return true;
    }

    public function get($params)
    {
        $this->code = $params->barcode;
        $this->store = $params->store;
        $this->retail = $params->retail;
        $this->draft_header = $params->draft_header;
        if (strlen($params->barcode) >= 29) {
            $this->qrcode = $params->barcode;
            if (substr($params->barcode, 0, 3) == "010") {
                $this->code = (string) intval(substr($params->barcode, 3, 13));
            } else if (substr($params->barcode, 0, 6) == "000000") {
                $this->code = substr($params->barcode, 6, 8);
            } else {
                $this->code = substr($params->barcode, 1, 13);
            }
        }

        if (!$this->getGoods()) {
            dieWithCode(Translator::t("Wrong barcode") . "<br>" . $this->code);
        }
    }
}
