<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-11-23 11:20:20
# Last Modified: 2025-11-26 12:08:42

require_once "report.php";

class MenuReview extends Report
{

    protected function widget()
    {
        return [
            "title" => Translator::t("Review of menu"),
            "icon" => "template.png",
            "version" => 2
        ];
    }

    protected function hiddenCols()
    {
        return [1, 15];
    }
    protected function sumColumns() {}

    protected function rowColors()
    {
        return [15];
    }
    protected function columns()
    {
        return match ($this->params->mode) {
            1 => $this->columns_v1(),
            2 => $this->columns_v1(),
            default => throw new Exception("Invalid mode"),
        };
    }

    protected function childColumns()
    {
        return match ($this->params->mode) {
            1 => [],
            2 => [
                Translator::t("Goods name"),
                Translator::t("Qty"),
                Translator::t("Price"),
                Translator::t("Total"),
            ]
        };
    }

    public function handler()
    {
        return ["65a0e1d4-c843-11f0-9ee3-0a002700000e", "65a0e1d4-c843-11f0-9ee3-0a002700000e"];
    }

    private function columns_v1()
    {
        return [
            Translator::t("Code"),
            Translator::t("Menu id"),
            Translator::t("Status"),
            Translator::t("Menu name"),
            Translator::t("Department"),
            Translator::t("Type"),
            Translator::t("Dish name"),
            Translator::t("Weight"),
            Translator::t("Price"),
            Translator::t("Self cost"),
            Translator::t("Cost Multiplier"),
            Translator::t("Daily"),
            Translator::t("Storage"),
            Translator::t("Printer 1"),
            Translator::t("Printer 2"),
            Translator::t("Color"),
        ];
    }

    protected function filter()
    {
        $modevalues = [
            ["key" => Translator::t("Only dishes"), "value" => 1],
            ["key" => Translator::t("Dishes and recipes"), "value" => 2],
        ];
        return [
            ["type" => "combo", "title" => Translator::t("Report mode"), "field" => "mode", "default" => 2, "values" => $modevalues],
            ["type" => "keyvalue", "title" =>  Translator::t("Status"), "field" => "status", "filter" => "menustatus"],
            ["type" => "keyvalue", "title" =>  Translator::t("Dish group"), "field" => "dishgroup", "filter" => "dishgroup"],
            ["type" => "keyvalue", "title" =>  Translator::t("Dish"), "field" => "dish", "filter" => "dish"],
            ["type" => "keyvalue", "title" =>  Translator::t("Goods"), "field" => "goods", "filter" => "goods"],
        ];
    }

    protected function rows()
    {

        return match ($this->params->mode) {
            1 => $this->rows_v1(),
            2 => $this->rows_v2(),
            default => throw new Exception("Invalid mode"),
        };
    }

    private function rows_v1()
    {
        $where  = "where 1=1  ";
        if (strlen($this->params->status ?? "") > 0) {
            $where .= " and md.f_id in ({$this->params->status}) ";
        }
        if (!empty($this->params->dish)) {
            $where .= " and m.f_dish in ({$this->params->dish}) ";
        }
        if (!empty($this->params->dishgroup)) {
            $where .= " and d.f_part in ({$this->params->dishgroup}) ";
        }

        $sql = <<<EOD
        select  d.f_id, m.f_id as f_mid, md.f_name as f_statename, mn.f_name as f_menuname, dp1.f_name as f_part1, dp2.f_name as f_part2, 
        d.f_name as f_dishname, d.f_netweight,m.f_price, d.f_cost, round(m.f_price/d.f_cost, 2),  d.f_dailyqty, s.f_name as f_storename, m.f_print1, m.f_print2, d.f_color 
        from d_menu m 
        left join d_dish_state md on md.f_id=m.f_state 
        left join d_dish d on d.f_id=m.f_dish 
        left join d_part2 dp2 on dp2.f_id=d.f_part 
        left join d_part1 dp1 on dp1.f_id=dp2.f_part 
        left join d_menu_names mn on mn.f_id=m.f_menu 
        left join c_storages s on s.f_id=m.f_store 
        $where
        order by mn.f_name, dp1.f_name, dp2.f_name 
        EOD;
        $this->debugi = $sql;
        $out = $this->rowsToTree($this->select($sql)->fetch_all());
        return $out;
    }

    private function rows_v2()
    {
        $where  = "where 1=1  ";
        $recipewhere = "where 1=1 ";
        if (strlen($this->params->status ?? "") > 0) {
            $where .= " and md.f_id in ({$this->params->status}) ";
        }
        if (!empty($this->params->dish)) {
            $where .= " and m.f_dish in ({$this->params->dish}) ";
        }
        if (!empty($this->params->dishgroup)) {
            $where .= " and d.f_part in ({$this->params->dishgroup}) ";
        }
        $sql = <<<EOD
        SELECT 
        d.f_id,
        m.f_id as f_mid,
        md.f_name as f_statename,
        mn.f_name as f_menuname,
        dp1.f_name as f_part1,
        dp2.f_name as f_part2,
        d.f_name as f_dishname,
        d.f_netweight,
        m.f_price,
        round(d.f_cost,2) as f_cost,
        round(m.f_price/d.f_cost,2) as f_multiplier,       
        d.f_dailyqty, 
        s.f_name as f_storename,
        m.f_print1,
        m.f_print2,
        d.f_color
        FROM d_menu m
        LEFT JOIN d_dish_state md ON md.f_id=m.f_state
        LEFT JOIN d_dish d ON d.f_id=m.f_dish
        LEFT JOIN d_part2 dp2 ON dp2.f_id=d.f_part
        LEFT JOIN d_part1 dp1 ON dp1.f_id=dp2.f_part
        LEFT JOIN d_menu_names mn ON mn.f_id=m.f_menu
        LEFT JOIN c_storages s ON s.f_id=m.f_store
        $where
        ORDER BY mn.f_name, dp1.f_name, dp2.f_name
        EOD;
        $rows = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $sql = <<<EOD
        SELECT 
        r.f_dish,
        '' as f_1,'' as f_2,'' as f_3,'' as f_4,'' as f_5,
        g.f_name,
        r.f_qty,
        r.f_price,
        r.f_qty * r.f_price AS total,
        r.f_qty*d.f_dailyqty
        FROM d_recipes r
        LEFT JOIN c_goods g ON g.f_id = r.f_goods
        left join d_dish d on d.f_id=r.f_dish
        EOD;
        $recipes = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $mapRecipes = [];

        foreach ($recipes as $r) {
            $dishId = $r["f_dish"];
            if (!isset($mapRecipes[$dishId])) {
                $mapRecipes[$dishId] = [];
            }

            $mapRecipes[$dishId][] = [
                "data" => array_values($r),
                "children" => []
            ];
        }
        $treeRows = [];

        foreach ($rows as $row) {

            $dishId = $row["f_id"];

            $treeRows[] = [
                "data" => array_values($row),
                "children" => $mapRecipes[$dishId] ?? []
            ];
        }
        return $treeRows;
    }

    public  function get($params)
    {
        if (empty($params->mode)) {
            $params->mode = 2;
        }
        parent::get($params);
    }
}
