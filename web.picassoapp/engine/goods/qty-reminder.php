<?php
# (C) 2024-2026 Kudryashov Vasili
# Last modified - 2026-02-26 03:20:13
require_once __DIR__ . "/goods.php";

class QtyReminder extends Report
{

    public function __construct()
    {
        parent::__construct();
    }
    protected function widget()
    {
        return ["icon" => "cash.png", "title" => $this->tr("Reminder of qty")];
    }

    protected function hiddenCols()
    {
        return [0];
    }

    protected function handler()
    {
        return ["78dd7b1e-12d5-11f1-9245-8a884be02f31", "78dd7b1e-12d5-11f1-9245-8a884be02f31"];
    }

    protected function columns()
    {
        return [
            $this->tr("Goods code"),
            $this->tr("Storage"),
            $this->tr("Class"),
            $this->tr("Group"),
            $this->tr("Goods"),
            $this->tr("Partner"),
            $this->tr("Scancode"),
            $this->tr("Price"),
            $this->tr("Cost"),
            $this->tr("Min qty"),
            $this->tr("Qty")
        ];
    }

    protected function filter()
    {
        return [
            ["type" => "keyvalue", "title" => $this->tr("Partner"), "field" => "partner", "filter" => "partners"],
            ["type" => "keyvalue", "title" => $this->tr("Store"), "field" => "store", "filter" => "storages"],
            ["type" => "keyvalue", "title" => $this->tr("Group of goods"), "field" => "group", "filter" => "goodsgroup"],
            ["type" => "text", "title" => $this->tr("Class"), "field" => "groupclass", "filter" => ""]
        ];
    }

    protected function sumColumns()
    {
        return [];
    }

    protected function where()
    {
        $cond = "";
        if (!empty($this->params->partner)) {
            $cond .= " and g.f_partner in (" . $this->params->patner . ") ";
        }
        if (!empty($this->params->store)) {
            $cond .= " and a.f_store in (" . $this->params->store . ") ";
        }
        if (!empty($this->params->group)) {
            $cond .= " and g.f_group in (" . $this->params->group . ") ";
        }
        if (!empty($this->params->groupclass)) {
            $cond .= " and gr.f_class='{$this->params->groupclass}' ";
        }
        return $cond;
    }

    protected function rows()
    {
        $where = $this->where();
        return $this->db->stmtall("select a.f_goods, s.f_name as f_storename, gr.f_class, p.f_taxname as f_partner, gr.f_name as f_groupname, "
            . "g.f_name as f_goodsname, g.f_scancode, cast(gp.f_price1 as float), cast(g.f_lastinputprice as float), "
            . "cast(g.f_lowlevel as float), cast(sum(a.f_qty*a.f_type) as float) "
            . "from a_store a "
            . "left join c_storages s on s.f_id=a.f_store "
            . "left join c_goods g on g.f_id=a.f_goods "
            . "left join c_groups gr on gr.f_id=g.f_group "
            . "left join a_header ah on ah.f_id=a.f_document  "
            . "left join c_partners p on p.f_id=g.f_supplier "
            . "left join c_goods_prices gp on gp.f_goods=g.f_id and gp.f_currency=1 "
            . "where g.f_enabled=1 $where "
            . "group by s.f_name, g.f_name, g.f_lowlevel "
            . "having sum(a.f_qty*a.f_type)=0  or sum(a.f_qty*a.f_type)<g.f_lowlevel")->fetch_all();
    }
}


$qtyReminder = new QtyReminder();
$qtyReminder->echoResult();
