<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-27 11:40:20
# Last Modified: 2026-01-27 11:40:25
require_once __DIR__ . "/index.php";

class Complect extends Auth {
    public function get($params) {
        $sql =<<<EOD
        select g.f_id,  gr.f_name as f_group_name, g.f_name, g.f_scancode,
        u.f_name as f_unit_name, gc.f_qty
        from c_goods_complectation gc
        left join c_goods g on g.f_id=gc.f_goods
        left join c_groups gr on gr.f_id=g.f_group
        left join c_units u on u.f_id=g.f_unit
        where gc.f_base=?
        order by g.f_name
        EOD;
        $this->result["complect_items"] = $this->select($sql, "i", [$params->complect_id])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}