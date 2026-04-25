<?php
require_once(__DIR__ . '/../app.php');

class Shift extends PClass
{
    public function __construct()
    {
        parent::__construct();
    }

    public function getShifts()
    {
        if (empty($this->params->shift)) {
            $this->result["data"] = $this->stmtall("select max(f_id) as f_id, " 
            ."date_format(f_open, '%d/%m/%Y %H:%i') as f_open "
            ."from s_working_sessions")->fetch_assoc();
            $this->params->shift = $this->result["data"]["f_id"];
        } else {
            $check = $this->result['data'] = $this->stmtall("select max(f_id) as f_id, min(f_id) as f_minid, "
            ."date_format(f_open, '%d/%m/%Y %H:%i') as f_open from s_working_sessions")->fetch_assoc();
            if ($check["f_id"] < $this->params->shift) {
                $this->params->shift = $check["f_id"];
            }
            if ($check["f_minid"] > $this->params->shift) {
                $this->params->shift =  $check["f_minid"];
            }
            $this->result['data'] = $this->stmtall(
                "select f_id, concat(date_format(f_open, '%d/%m/%Y %H:%i') , '-', coalesce(date_format(f_close, '%d/%m/%Y %H:%i'), '?')) as f_open from s_working_sessions where f_id=?",
                "i",
                [$this->params->shift]
            )->fetch_assoc();
        }
        $sql = <<<SQL
            SELECT h.f_id, CONCAT(h.f_prefix, h.f_hallid)AS f_hallid, coalesce( c.f_govnumber, '') as f_govnumber, 
            f_amounttotal, f_amountcash, f_amountcard, coalesce(f_amountidram, 0.0) as f_amountidram, 
            cast(coalesce(JSON_VALUE(t.f_out,'$.rseq'), 0) as unsigned) AS f_fiscal,
            coalesce(h.f_timeclose, '00:00:00') as f_timeclose
            FROM o_header h
            LEFT JOIN b_car c ON c.f_id=h.f_id
            LEFT JOIN o_tax_log t ON t.f_order=h.f_id AND t.f_state=1
            WHERE h.f_working_session=? and h.f_state=2
            order by h.f_hallid desc 
        SQL;
        $this->result["orders"] = $this->stmtall($sql, "i",  [$this->params->shift])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function changePayment() {
        $this->db->begin_transaction();
        $v["f_amountcash"] = $this->params->f_amountcash;
        $v["f_amountcard"] = $this->params->f_amountcard;
        $v["f_amountidram"] = $this->params->f_amountidram;
        $this->supdate("o_header", $v, $this->params->f_id);
        $this->stmtall("update o_tax_log set f_state=0 where f_order=?","s", [$this->params->f_id]);
        $cashdoc = $this->stmtall("select * from a_header_cash where f_oheader=?", "s", [$this->params->f_id])->fetch_assoc();
        if(empty($cashdoc)) {
            $this->exitError("No cash document for this order exists");
        }
        $newcashid = 1;
        if ($this->params->f_amountcash > 0) {
            $newcashid = 1;
        } else if  ($this->params->f_amountcard > 0) {
            $newcashid = 2;
        } else  if ($this->params->f_amountidram >0) {
            $newcashid = 3;
        }
        $v["f_cashin"] = $newcashid;
        $this->supdate("a_header_cash", $v, $cashdoc["f_id"]);
        $v["f_cash"] = $newcashid;
        $this->supdate("e_cash", $v, $cashdoc["f_id"], "f_header");
        $this->db->commit();
        $this->getShifts();
    }
}

if (!empty($params->action)) {
    $s = new Shift();
    $s->{$params->action}();
}