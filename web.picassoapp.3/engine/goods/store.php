<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modified - 2025-09-12 11:32:34
require_once __DIR__ . "/goods.php";

class GoodsInStore extends Report
{

    public function widget()
    {
        return ["icon" => "cash.png", "title" => "Ապրանքների առկայություն"];
    }
    public function columns()
    {
        return [
            $this->tr("Code"),
            $this->tr("Store"),
            $this->tr("Goods group"),
            $this->tr("Goods name"),
            $this->tr("Barcode"),
            $this->tr("Partner"),
            $this->tr("Qty"),
            $this->tr("Low level"),
            $this->tr("Unit"),
            $this->tr("Last input price"),
            $this->tr("Total cost"),
            $this->tr("Retail price"),
            $this->tr("Amount retail"),
            $this->tr("Whosale price"),
            $this->tr("Amount whosale"),
            $this->tr("Discounted retail"),
            $this->tr("Amount discounted retail"),
            $this->tr("Discounted whosale"),
            $this->tr("Amount discounted whosale")
        ];
    }

    public function sumColumns()
    {
        return [
            ["6" =>0],
            ["10"=>0],
            ["12"=>0],
            ["14"=>0],
            ["16"=>0],
            ["18"=>0]
        ];
    }

    public function filter()
    {
        $modevalues = [
            ["key" => $this->tr("AMD"), "value" => 0],
            ["key" => $this->tr("USD"), "value" => 1]
        ];
        return [
            ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
            ["type" => "keyvalue", "title" => $this->tr("Store"), "field" => "store", "filter" => "storages"],
            ["type" => "keyvalue", "title" => $this->tr("Partner"), "field" => "partner", "filter" => "partners"],
            ["type" => "keyvalue", "title" => $this->tr("Group of goods"), "field" => "group", "filter" => "goodsgroup"],
            ["type" => "keyvalue", "title" => $this->tr("Unit"), "field" => "unit", "filter" => "unit"],
            ["type" => "combo", "title" => tr("Currency"), "field" => "mode", "default" => 0, "values" => $modevalues]
        ];
    }

    public function where()
    {
        if (empty($this->params->date)) {
            $this->params->date = date('Y-m-d');
        }

        $where = "where h.f_date<'{$this->params->date}' ";
        $where .= "and (h.f_state=2 or h.f_state=1) ";
        if (!empty($this->params->store)) {
            $where .= " and s.f_store in ({$this->params->store}) ";
        }
        return $where;
    }

    public function rows()
    {
        return stmtall("select g.f_id as f_code,"
            . "ss.f_name as f_storage,"
            . "gg.f_name as f_group,"
            . "g.f_name as f_goods, "
            . "g.f_scancode,"
            . "p.f_taxname,"
            . "cast(sum(s.f_qty*s.f_type) as float),"
            . "cast(g.f_lowlevel as float), "
            . "u.f_name as f_unit, "
            . "cast(g.f_lastinputprice as float),"
            . "cast(sum(g.f_lastinputprice*s.f_type*s.f_qty) as float),"
            . "cast(gpr.f_price1 as float), "
            . "cast(sum(s.f_qty*s.f_type)*gpr.f_price1 as float),"
            . "cast(gpr.f_price2 as float), "
            . "cast(sum(s.f_qty*s.f_type)*gpr.f_price2 as float),"
            ."cast(gpr.f_price1disc as float),"
            . "cast(sum(s.f_qty*s.f_type)*gpr.f_price1disc as float),"
            ."cast(gpr.f_price2disc as float), "
            . "cast(sum(s.f_qty*s.f_type)*gpr.f_price2disc as float) "
            . "from a_store_draft s "
            . "left join c_goods g on g.f_id=s.f_goods "
            . "inner join c_storages ss on ss.f_id=s.f_store "
            . "inner join c_groups gg on gg.f_id=g.f_group "
            . "inner join a_header h on h.f_id=s.f_document "
            . "left join c_partners p on p.f_id=g.f_supplier "
            . "left join c_units u on u.f_id=g.f_unit "
            . "LEFT JOin c_goods_prices gpr on gpr.f_goods=g.f_id "
            . $this->where()
            . "group by g.f_id,ss.f_name,gg.f_name,g.f_name,g.f_scancode,"
            . "p.f_taxname,u.f_name,g.f_lastinputprice,g.f_lowlevel,"
            . "gpr.f_price1,gpr.f_price2 "
            . "having sum(s.f_qty*s.f_type) <> 0  ")->fetch_all();
    }

}

$goodsInStore = new GoodsInStore();
$goodsInStore->echoResult();