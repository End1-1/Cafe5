<?php
# (C) 2025 Kudryashov Vasili
# Created: 2025-08-12 10:09:17
# Last modified - 2025-08-12 10:09:21

require_once __DIR__ . "/reports.php";

class ElinaDailyReport extends Report
{
    public function columns()
    {
        return [
            $this->tr("ID"),
            $this->tr("Hall"),
            $this->tr("Date"),
            $this->tr("Previous day"),
            $this->tr("Income"),
            $this->tr("Income other"),
            $this->tr("Returns"),
            $this->tr("Sale"),
            $this->tr("Output"),
            $this->tr("Final"),
            $this->tr("Check")
        ];
    }

    public function hiddenCols() {
        return [0];
    }

    public function filter()
    {
        return [
            ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => tr("Date of end"), "field" => "date2"],
            ["type" => "keyvalue", "title" => $this->tr("Salepoint"), "field" => "salepos", "filter" => "halls"],
        ];
    }

    public function sumColumns()
    {
        return [
            ["2" => 0],
            ["3" => 0],
            ["4" => 0],
            ["5" => 0],
            ["6" => 0],
            ["7" => 0],
            ["8" => 0],
            ["9" => 0],
            ["10" => 0]
        ];
    }

    public function widget() {
        return [];
    }

    public function where() {
        if (empty($this->params->date1)) {
            $this->params->date1 = date("Y-m-01");
            $this->params->date2 = date("Y-m-d");
        }
        $where = "where ed.f_date between '{$this->params->date1}' and '{$this->params->date2}'";
        if (!empty($this->params->salepos)) {
            $where .= " and (ed.f_hall in {$this->params->salepos})";   
        }
        return $where;
    }

    public function rows(){
        $sql = <<<EOD
        select ed.f_id, h.f_name as f_hallname, ed.f_date,
        ed.f_prevday, ed.f_income, ed.f_inputother, ed.f_return, 
        ed.f_sale, ed.f_output, ed.f_final, ed.f_check
        from users_elina_day_end ed
        left join h_halls h on h.f_id=ed.f_hall
        {$this->where()}
        order by ed.f_date, h.f_name
        EOD; 
        return $this->db->stmtall($sql)->fetch_all();
    }
}


$report = new ElinaDailyReport();
$report->echoResult();