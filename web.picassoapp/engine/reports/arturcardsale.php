<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-04-07 01:02:50
 * Last Modified: 2025-04-07 13:33:38
 * License: MIT
 * Description: Specialy for Artur, sale of dishes by payment methods
 */



require_once __DIR__ . "/reports.php";

class DraftSale extends Report
{
    protected function columns()
    {
        return [
            $this->tr("UUID"),
            $this->tr("Date"),
            $this->tr("Group"),
            $this->tr("Dish"),
            $this->tr("Quantity"),
            $this->tr("Price"),
            $this->tr("Total"),
        ];
    }

    protected function filter()
    {
        return [
            ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => tr("Date of end"), "field" => "date2"],
            ["type" => "keyvalue", "title" => $this->tr("Group"), "field" => "dishgroup", "filter" => "dishgroup"],
            ["type" => "keyvalue", "title" => $this->tr("Dish"), "field" => "dish", "filter" => "dish"],
        ];
    }

    protected function where() {
        if (empty($this->params->date1)) {
            $this->params->date1 = date("Y-m-d");
            $this->params->date2 = date("Y-m-d");
        }
        $where = "where oh.f_state=2 and oh.f_datecash between '{$this->params->date1}' and '{$this->params->date2}'";
        $where .= " and ob.f_state=1 ";
        $where .= " and oh.f_amountcard>0 ";
       
        if (!empty($this->params->dishgroup)) {
            $where .= " and dg.f_id in ({$this->params->dishgroup}) ";
        }
        if(!empty($this->params->dish)) {
            $where .= " and dd.f_id in ({$this->params->dish}) ";
        }
        return $where;
    }
    protected function rows()
    {
        
        $sql = <<<EOD
        select oh.f_id, oh.f_datecash, dg.f_name, dd.f_name, 
        sum(ob.f_qty1) , ob.f_price, sum(ob.f_total)
        from o_body ob
        left join o_header oh on oh.f_id=ob.f_header
        left join d_dish dd on dd.f_id=ob.f_dish
        left join d_part2 dg on dg.f_id=dd.f_part
        {$this->where()}
        group by 1,2,3,4
        order by 3,2,4
        EOD;
        return $this->db->stmtall($sql)->fetch_all();
    }

    protected function sumColumns()
    {
        return [["4"=>0], ["6" => 0]];
    }

    protected function widget()
    {
        return [
            "title" => $this->tr("Dish sales by payment type"),
            "icon" => "template.png",
        ];
    }
    protected function hiddenCols()
    {
        return [
            0
        ];
    }

    protected function handler() {
        return [
            "39617ca7-8fa4-11ed-8ad3-1078d2d2b808","39617ca7-8fa4-11ed-8ad3-1078d2d2b808"
        ];
    }
}

$s = new DraftSale();
$s->echoResult();