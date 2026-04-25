<?php
require_once(__DIR__ . "/../app.php");

class Shifts extends Report
{
    public function __construct()
    {
        parent::__construct();
        if (empty($this->params->date1)) {
            $this->params->date1 = date("Y-m-") . "01";
            $this->params->date2 = date("Y-m-") . cal_days_in_month(CAL_GREGORIAN, 11, 2009);
        }
    }

    function columns()
    {
        return [
            $this->tr("Code"),
            $this->tr("Date start"),
            $this->tr("Date end"),
            $this->tr("Amount"),
            $this->tr("Cash"),
            $this->tr("Card"),
            $this->tr("Other"),
            $this->tr("Out"),
            $this->tr("Remain")
        ];
    }
    function filter()
    {
        return [
            ["type" => "date", "title" => $this->tr("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => $this->tr("Date of end"), "field" => "date2"],
        ];
    }
    function rows()
    {
        return $this->db->stmtall(
            "select s.f_id, f_open, f_close, h.f_amounttotal, h.f_amountcash, h.f_card, "
            . "h.f_other, cast(ah.f_outamount as float), "
            ."h.f_amountcash-ah.f_outamount "
            . "from s_working_sessions s "
            . "left join (select f_working_session, sum(f_amounttotal) as f_amounttotal, sum(f_amountcash) f_amountcash, "
            . "sum(f_amountcard+f_amountidram) as f_card, sum(f_amountdebt+f_amountbank) as f_other "
            . "from o_header h  "
            . "where h.f_datecash between ? and ? and h.f_state=2 "
            . "group by 1 ) h on h.f_working_session=s.f_id "
            . "left join (select a.f_working_session, abs(sum(e.f_amount*e.f_sign)) as f_outamount from e_cash e "
            ."left join a_header a on a.f_id=e.f_header "
            ."where e.f_sign<0 and a.f_date between ? and ? group by 1) ah on ah.f_working_session=s.f_id "
            . "where cast(s.f_close as date) between ? and ? or s.f_close is null "
            . "order by s.f_id desc ",
            "ssssss",
            [$this->params->date1, $this->params->date2, 
            $this->params->date1, $this->params->date2, 
            $this->params->date1, $this->params->date2]
        )->fetch_all();
    }
    function sumColumns()
    {
        return [3, 4, 5, 6, 7, 8];
    }
    function widget()
    {
        return ["icon" => ":/cash.png", "title" => $this->tr("Shifts")];
    }

}

$shift = new Shifts();
if (empty($params->action)) {
    $shift->echoResult();
} else {
    $shift->{$params->action}();
}
