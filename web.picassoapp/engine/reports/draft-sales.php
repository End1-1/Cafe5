<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-30 13:55:06
# Last modified - 2025-03-30 13:55:08

require_once __DIR__ . "/reports.php";

class DraftSale extends Report
{
    protected function columns()
    {
        return [
            $this->tr("UUID"),
            $this->tr("Manager"),
            $this->tr("Date"),
            $this->tr("Time"),
            $this->tr("Taxpayer code"),
            $this->tr("Taxpayer name"),
            $this->tr("Address"),
            $this->tr("Comment"),
            $this->tr("Amount")
        ];
    }

    protected function filter()
    {
        return [
            
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
        SELECT d.f_id , concat_ws(' ', u.f_last, u.f_first), d.f_date , d.f_time, 
        p.f_taxcode, p.f_taxname, p.f_address,
        f_comment, d.f_amount 
        FROM o_draft_sale d
        left join c_partners p on p.f_id=d.f_partner
        left join s_user u on u.f_id=d.f_staff
        WHERE d.f_state=1
        order by d.f_date desc, d.f_time desc
        EOD;
        return $this->db->stmtall($sql)->fetch_all();
    }

    protected function sumColumns()
    {
        return [["7" => 0]];
    }

    protected function widget()
    {
        return [
            "title" => $this->tr("Draft sales"),
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