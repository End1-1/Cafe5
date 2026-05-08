<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-30 11:41:21
# Last modified - 2025-03-30 11:41:24
require_once __DIR__ . "/../shop/shop.php";
class saledoc extends PClass
{
    public function createDraft()
    {
        $saleid = $this->params->header->f_id;
        $this->db->begin_transaction();
        $sho = new ShopOrder($saleid);
        $sho->remove(false, true);
        $h = $this->params->header;
        $v["f_id"] = $h->f_id;
        $v["f_state"] = 1;
        $v["f_saletype"] = $h->f_saletype;
        $v["f_date"] = date("Y-m-d");
        $v["f_time"] = date("H:i:s");
        $v["f_staff"] = $h->f_staff;
        $v["f_amount"] = $h->f_amounttotal;
        $v["f_comment"] = $h->f_comment;
        $v["f_payment"] = 1;
        $v["f_partner"] = $h->f_partner;
        $v["f_debt"] = 0;
        $v["f_discount"] = 0;
        $v["f_datefor"] = $h->f_deliverydate;
        $v["f_cashier"] = $this->userid;
        $this->sinsert("o_draft_sale", $v);
        foreach ($this->params->goods as $g) {
            $v["f_id"] = $g->f_id;
            $v["f_header"] = $saleid;
            $v["f_state"] = 1;
            $v["f_store"] = $g->f_store;
            $v["f_dateappend"] = date("Y-m-d");
            $v["f_timeappend"] = date("H:i:s");
            $v["f_goods"] = $g->f_goods;
            $v["f_qty"] = $g->f_qty;
            $v["f_back"] = 0;
            $v["f_price"] = $g->f_price;
            $v["f_discount"] = $g->f_discountfactor;
            $this->sinsert("o_draft_sale_body", $v);
        }
        $this->db->commit();
        $this->result["id"] = $saleid;
        $this->echoResult();
    }
}