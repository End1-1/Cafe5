<?php
# (C) 2025 Kudryashov Vasili
# Created - 2025-03-18 08:46:54
# Last modified - 2025-03-18 08:47:02

require_once __DIR__ . "/reports.php";

class Sale1 extends Report
{
    protected function columns()
    {
        return [
            $this->tr("UUID"),
            $this->tr("Salepoint"),
            $this->tr("Order number"),
            $this->tr("Date close"),
            $this->tr("Time close"),
            $this->tr("Count")
        ];
    }

    protected function filter()
    {
        return [
            ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => tr("Date of end"), "field" => "date2"],
            ["type" => "keyvalue", "title" => $this->tr("Salepoint"), "field" => "salepos", "filter" => "halls"],
        ];
    }

    protected function where() {
        if (empty($this->params->date1)) {
            $this->params->date1 = date("Y-m-d");
            $this->params->date2 = date("Y-m-d");
        }
        $where = "where oh.f_state=2 and oh.f_datecash between '{$this->params->date1}' and '{$this->params->date2}'";
        if (!empty($this->params->salepos)) {
            $where .= " and oh.f_hall={$this->params->salepos} ";   
        }
        return $where;
    }
    protected function rows()
    {
        
        $sql = <<<EOD
        select oh.f_id, h.f_name as f_hallname, concat(oh.f_prefix, oh.f_hallid) as f_hallid,
        oh.f_dateclose, oh.f_timeclose, count(oh.f_id) as f_count
        from o_header oh 
        left join h_halls h on h.f_id=oh.f_hall
        {$this->where()}
        group by 1,2
        EOD;
        return $this->db->stmtall($sql)->fetch_all();
    }

    protected function sumColumns()
    {
        return [["5" => 0]];
    }

    protected function widget()
    {
        return [];
    }
    protected function hiddenCols()
    {
        return [
            0
        ];
    }
}

$s = new Sale1();
$s->echoResult();