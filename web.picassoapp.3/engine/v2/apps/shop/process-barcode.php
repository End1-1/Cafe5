<?php
# (C) 2024-2025 Kudryashov Vasili
# Created - ../../2025 :)
# Last modified - 2025-12-13 01:57:10
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class ProcessBarCode extends Auth
{
    private $code = "";
    private $qrcode = "";
    private $params;


    private function getGoods($goods, $echoresult = true)
    {


        $store = [];


        $sql = <<<EOD
            SELECT coalesce(SUM(f_qty_left),0) AS f_qty FROM store_stock WHERE  f_store_id=? and f_item_id=?
            EOD;
        $store = $this->select($sql, "ii", [$goods["f_dish"], $this->params->store])->fetch_assoc();
        if (empty($store)) {
            dieWithCode(Translator::t("Insufficient quantity") . " " . $goods["f_defaultqty"]);
        }



        $goods["f_qty"] = $store["f_qty"];


        $this->result["goods"] = $goods;
        $this->result["store"] = $store;
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
        $this->params = $params;
        if (strlen($this->code) >= 29) {
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
            dieWithCode(Translator::t("Wrong barcode") . "<br>" . $this->code);
        }

        $sql = <<<SQL
         SELECT g.f_id as f_dish, g.f_name as f_dish_name, gp.f_name as f_groupname, g.f_scancode,  g.f_lowlevel,
            g.f_qtybox, u.f_name as f_unit_name, gp.f_taxdept, gp.f_adgcode as f_adgt, g.f_fiscalname as f_fiscal_name, 
            g.f_candiscount, g.f_autodiscount,
            coalesce(g.f_service, 0) as f_is_service, coalesce(u.f_defaultqty, 0) as f_default_qty, 
            if(cp.f_price1disc>0, cp.f_price1disc, cp.f_price1) as f_price1, 
            if(cp.f_price2disc>0, cp.f_price2disc, cp.f_price2) as f_price2 
            FROM c_goods g 
            left join c_goods_multiscancode m on m.f_goods=g.f_id 
            LEFT JOIN c_groups gp on gp.f_id=g.f_group 
            LEFT JOIN c_goods_prices cp ON cp.f_goods=g.f_id AND cp.f_currency=1 
            LEFT JOIN c_units u ON u.f_id=g.f_unit 
            WHERE m.f_id=? and g.f_enabled=1 
        SQL;
        $goods = $this->select($sql, "s", [$this->code])->fetch_assoc();
        if (!$goods) {
            dieWithCode(Translator::t("Wrong barcode") . "<br>" . $this->code);
        }
        if ($goods["f_is_service"] > 0) {
            $this->result["goods"] = $goods;
            $this->echoResult();
            return;
        }
        $goods["f_barcode"] = $this->code;
        if ($this->getGoods($goods)) {
            return;
        }
        dieWithCode(Translator::t("Wrong barcode") . "<br>" . $this->result["echocode"]);
    }
}
