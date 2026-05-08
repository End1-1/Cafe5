<?php
# (C) 2025 Kudryashov Vasili
# Created: 2025-08-12 10:09:17
# Last modified - 2025-08-12 10:09:21

require_once __DIR__ . "/reports.php";

class ComplectationAdditions extends Report
{
    public function columns()
    {
        return [
            $this->tr("ID"),
            $this->tr("Date"),
            $this->tr("Type"),
            $this->tr("Name"),
            $this->tr("Amount")
        ];
    }

    public function hiddenCols()
    {
        return [0];
    }

    public function filter()
    {
        return [
            ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => tr("Date of end"), "field" => "date2"],
            ["type" => "keyvalue", "title" => $this->tr("Document type"), "field" => "doctype", "filter" => "doctype"],
        ];
    }

    public function sumColumns()
    {
        return [
            ["4" => 0]
        ];
    }

    public function widget()
    {
        return [];
    }

    public function where()
    {
        if (empty($this->params->date1)) {
            $this->params->date1 = date("Y-m-01");
            $this->params->date2 = date("Y-m-d");
        }
        $where = "where ah.f_date between '{$this->params->date1}' and '{$this->params->date2}'";
        if (!empty($this->params->doctype)) {
            $where .= " and (ah.f_type in ({$this->params->doctype}))";
        }
        return $where;
    }

    public function rows()
    {
        $sql = <<<EOD
        SELECT ah.f_id, ah.f_date, dt.f_name,  ac.f_name, ac.f_amount
        FROM a_complectation_additions ac
        LEFT JOIN a_header ah ON ah.f_id=ac.f_header
        LEFT JOIN a_type dt ON dt.f_id=ah.f_type
        {$this->where()}
        EOD;
        return $this->db->stmtall($sql)->fetch_all();
    }
}


$report = new ComplectationAdditions();
$report->echoResult();
