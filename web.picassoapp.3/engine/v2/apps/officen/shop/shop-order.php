<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-10-06 23:59:29
# Last Modified:2025-11-27 09:27:54
require_once __DIR__ . "/index.php";

class ShopOrder extends Auth {
    public function get($params) {
        $sql = "select * from o_header where f_id=?";
        $oheader = $this->select($sql, "s", [$params->id])->fetch_assoc();
        $this->result["header"] = $oheader;
        $partner = [];
        if ($oheader["f_partner"] > 0) {
            $sql =<<<EOD
            select * from c_partners where f_id=?
            EOD;
            $partner = $this->select($sql, "i", [$oheader["f_partner"]])->fetch_assoc();
            $sql=<<<EOD
            select coalesce(sum(f_amount),0) as f_amount from b_clients_debts where f_costumer=?
            EOD;
            $partner["debt"] = $this->select($sql, "i", [$partner["f_id"]])->fetch_assoc()["f_amount"];
            $partner["f_amountdebt"] = $oheader["f_amountdebt"];
        }
        $this->result["partner"] = $partner;
        $sql =<<<EOD
        select b.f_id, 0 as f_x, g.f_scancode, g.f_name, b.f_qty, 
            u.f_name as f_unit, b.f_price, b.f_total, b.f_goods, b.f_return, 
            b.f_returnedqty, b.f_discountfactor, b.f_discountmode
            from o_goods b 
            inner join c_goods g on g.f_id=b.f_goods 
            inner join c_units u on u.f_id=g.f_unit 
            where b.f_header=? 
            order by b.f_row 
        EOD;
        $this->result["body"] = $this->select($sql, "s", [$params->id])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}