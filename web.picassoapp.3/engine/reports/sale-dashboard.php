<?php
require_once (__DIR__ . "/report.php");

class SaleDashboard extends DB
{
    public function __construct()
    {
        parent::__construct();
    }

    public function get()
    {
        $total = $this->stmtall("select h.f_name as f_hallname, "
            . "coalesce(count(oh.f_id), 0) as f_count, "
            . "coalesce(sum(f_amounttotal), 0) as f_amounttotal, "
            . "coalesce(sum(f_amountbank), 0) as f_amountbank, "
            . "coalesce(sum(f_amountcash), 0) as f_amountcash, "
            . "coalesce(sum(f_amountcard), 0) as f_amountcard, "
            . "coalesce(sum(f_amountidram), 0) as f_amountidram, "
            . "coalesce(sum(f_amountother), 0) as f_amountother "
            . "from o_header oh "
            . "left join h_halls h on h.f_id=oh.f_hall "
            . "where oh.f_state=2 and oh.f_datecash between ? and ? "
            . "group by 1", "ss", [$this->params->date1, $this->params->date2])->fetch_all(MYSQLI_ASSOC);
        $open = $this->stmtall("select h.f_name as f_hallname, "
            . "coalesce(count(oh.f_id), 0) as f_count, "
            . "coalesce(sum(f_amounttotal), 0) as f_amounttotal, "
            . "coalesce(sum(f_amountbank), 0) as f_amountbank, "
            . "coalesce(sum(f_amountcash), 0) as f_amountcash, "
            . "coalesce(sum(f_amountcard), 0) as f_amountcard, "
            . "coalesce(sum(f_amountidram), 0) as f_amountidram, "
            . "coalesce(sum(f_amountother), 0) as f_amountother "
            . "from o_header oh "
            . "left join h_halls h on h.f_id=oh.f_hall "
            . "where oh.f_state=1  "
            . "group by 1")->fetch_all(MYSQLI_ASSOC);
            $grandtotal = $this->stmtall("select  "
            . "coalesce(count(oh.f_id), 0) as f_count, "
            . "coalesce(sum(f_amounttotal), 0) as f_amounttotal, "
            . "coalesce(sum(f_amountbank), 0) as f_amountbank, "
            . "coalesce(sum(f_amountcash), 0) as f_amountcash, "
            . "coalesce(sum(f_amountcard), 0) as f_amountcard, "
            . "coalesce(sum(f_amountidram), 0) as f_amountidram, "
            . "coalesce(sum(f_amountother), 0) as f_amountother "
            . "from o_header oh "
            . "where oh.f_state=2 and oh.f_datecash between ? and ? ", "ss", [$this->params->date1, $this->params->date2])->fetch_assoc();
            $grandtotalopen = $this->stmtall("select  "
            . "coalesce(count(oh.f_id), 0) as f_count, "
            . "coalesce(sum(f_amounttotal), 0) as f_amounttotal, "
            . "coalesce(sum(f_amountbank), 0) as f_amountbank, "
            . "coalesce(sum(f_amountcash), 0) as f_amountcash, "
            . "coalesce(sum(f_amountcard), 0) as f_amountcard, "
            . "coalesce(sum(f_amountidram), 0) as f_amountidram, "
            . "coalesce(sum(f_amountother), 0) as f_amountother "
            . "from o_header oh "
            . "where oh.f_state=1 ")->fetch_assoc();
        $this->result["total"] = $total;
        $this->result["open"] = $open;
        $this->result["grandtotal"] = $grandtotal;
        $this->result["grandtotalopen"] = $grandtotalopen;
        $this->echoResult();
    }
}

$saleDashboard = new SaleDashboard();
$saleDashboard->get();