<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-11-10 09:48:08
# Last Modified: 2025-12-20 12:22:22
require_once __DIR__ . "/index.php";

class Actions extends Auth
{
    public function list($params)
    {
        $where = "";
        if (!empty($params->filter)) {
            $where = "where f_name like '%{$params->filter}%'";
        }
        $sql = <<<EOD
        select f_id, f_name 
        from mf_actions_group
        $where
        EOD;
        $this->result["data"] = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function get($params)
    {
        
        $this->result["d1"] = $this->select("select * from mf_actions_group where f_id=?", "i", [$params->f_id])->fetch_assoc();
        $sql = <<<EOD
        select ma.f_material, m.f_name as f_materialname, ma.f_code, ma.f_qty, ma.f_comment
        from mf_materials_in_actions ma 
        left join c_goods m on m.f_id=ma.f_material
        where ma.f_action=?
        EOD;
        $this->result["materials"] = $this->select($sql, "i", [$params->f_id])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function Start($params) {
        $v = [
            "f_date" => date("Y-m-d"),
            "f_product" => $params->f_id,
            "f_status" => 1,
            "f_width" => 0,
            "f_height" => 0,
            "f_34" => 0,
            "f_36" => 0,
            "f_38" => 0,
            "f_40" => 0,
            "f_42" => 0,
            "f_44" => 0,
            "f_46" => 0,
        ];
        $id = $this->insert("m_goal_product", $v);
        $sql = <<<EOD
        insert into m_goal_product_material (f_product, f_material, f_code, f_color,f_qty1, f_qty2, f_totalqty, f_qtyperone, f_colorqty, f_reason)
        select ?, f_material, f_code, '', 0, 0, 0, 0, 0, 1
        from mf_materials_in_actions mm
        where mm.f_action=?
        EOD;
        $this->select($sql, "ii", [$id, $params->f_id], true);
        $this->echoResult();
    }
}
