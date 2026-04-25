<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-28 14:59:24
# Last Modified: 2026-03-14 09:46:32
require_once __DIR__ . "/index.php";

class Menu extends Auth
{
    public function get($params)
    {
        $sql = <<<EOD
        SELECT
        gr.f_id,
        gr.f_parent,
        gr.f_name,
        COALESCE(g.f_qty, 0) AS f_qty,
        CAST(gr.f_class AS INT) AS f_class,
        gr.f_color
        FROM c_groups gr
        LEFT JOIN (
        SELECT f_group, COUNT(*) AS f_qty
        FROM c_goods
        GROUP BY f_group
        ) g ON g.f_group = gr.f_id
        LEFT JOIN (
        SELECT f_parent, COUNT(*) AS f_children
        FROM c_groups
        WHERE f_parent IS NOT NULL
        GROUP BY f_parent
        ) gp ON gp.f_parent = gr.f_id
        WHERE
        (COALESCE(g.f_qty, 0) > 0
        OR COALESCE(gp.f_children, 0) > 0) 
        and gr.f_id in (select f_group from c_goods where f_id in (select f_dish from c_menu where f_state=1))
        order by gr.f_order, gr.f_name
        EOD;
        $groups = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["groups"] = $groups;

        $sql = <<< EOD
        SELECT m.f_id AS f_menu, m.f_name, mm.f_price, gr.f_name as f_group_name,
        mm.f_dish, g.f_group, g.f_name AS f_goods_name, g.f_color,gr.f_taxdept,
        mm.f_store, mm.f_print1, mm.f_print2, mm.f_state,
        if (length(g.f_adg)>0, g.f_adg, gr.f_adgcode) AS f_adgt, gr.f_taxdept,
        mm.f_recent, g.f_emark_required, cast(coalesce(sl.f_qty, -999) AS INT) as f_stoplist,
        g.f_data, g.f_scancode
        FROM c_menu_names m
        LEFT JOIN c_menu mm ON mm.f_menu=m.f_id 
        LEFT JOIN c_goods g ON g.f_id=mm.f_dish
        LEFT JOIN c_groups gr ON gr.f_id=g.f_group
        left join c_stoplist sl on sl.f_dish=mm.f_dish
        WHERE m.f_enabled=1 AND mm.f_state=1
        EOD;
        $dishes = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["dishes"] = $dishes;
        $this->echoResult();
    }

    public function GetComments($params)
    {
        $this->result["data"] = $this->select("select f_name from c_goods_comment")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetRemoveReason($params)
    {
        $this->result["data"] = $this->select("select f_name from o_dish_remove_reason")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetMenuNames($params)
    {
        $this->result["names"] = $this->select("select f_id, f_name from c_menu_names where f_enabled=1")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
