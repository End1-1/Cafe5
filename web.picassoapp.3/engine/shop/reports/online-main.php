<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modified - 2025-03-24 09:10:11
require_once __DIR__ . "/index.php";

class OnlineMainReport extends PClass
{
    public function report()
    {
        $sql = <<<EOD
        select f_id, f_name from h_halls where f_onlinereport=1 order by f_id
        EOD;
        $this->result["halllist"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->result["hall"] = empty($this->params->hall) ? "0" : $this->params->hall;
        $hallcond = "";
        if (!empty($this->params->hall)) {
            $hallcond = " and oh.f_hall=" . $this->params->hall;
        }
        $sql = <<<EOD
        select oh.f_datecash, h.f_name as f_hallname, gg.f_name as f_groupname,
        sum(og.f_qty * og.f_sign) as f_qty, cast(sum(og.f_total*og.f_sign) as float) as f_total 
        from o_goods og 
        inner join o_header oh on oh.f_id=og.f_header 
        left join o_tax ot on ot.f_id=oh.f_id 
        inner join c_goods g on g.f_id=og.f_goods 
        left join c_groups gg on gg.f_id=g.f_group 
        left join s_user u on u.f_id=oh.f_staff 
        left join h_halls h on h.f_id=oh.f_hall 
        where oh.f_datecash between '{$this->params->date1}' and '{$this->params->date2}' 
        and oh.f_state=2 and h.f_onlinereport=1
        $hallcond
        group by gg.f_name 
        order by gg.f_class, 4 desc
        EOD;
        $this->result["items"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);

        $sql = <<<EOD
        select h.f_name, sum(f_amounttotal) as f_amounttotal, sum(f_amountcash) as f_amountcash, 
        sum(f_amountdiscount) as f_disc, sum(f_amountbank) as f_amountbank,
        sum(f_amountcard+f_amountidram+f_amounttelcell) as f_amountcard, coalesce(f.f_fiscal, 0) as f_fiscal,
        sum(f_amountprepaid) as f_amountprepaid
        from o_header oh
        left join h_halls h on oh.f_hall=h.f_id
        left join (select f_hall, sum(f_amounttotal) as f_fiscal 
            from o_header oh
            left join h_halls h on oh.f_hall=h.f_id
            inner join o_tax ot on ot.f_id=oh.f_id 
            where oh.f_datecash between '{$this->params->date1}' and '{$this->params->date2}' 
            and oh.f_state=2 and h.f_onlinereport=1 and ot.f_receiptnumber>0
            group by 1) f on f.f_hall=h.f_id
        where oh.f_datecash between '{$this->params->date1}' and '{$this->params->date2}' 
        and oh.f_state=2 and h.f_onlinereport=1
        group by 1
        order by h.f_id
        EOD;
        $this->result["totals"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);

        ///CASHBOX
        $this->db->begin_transaction();
        $this->stmtall("CREATE TEMPORARY TABLE cashreport (f_hallname TEXT, f_handznum FLOAT, f_kopek FLOAT, f_spent float, f_remainkopek float)");
        $sql = <<<EOD
        INSERT INTO cashreport (f_hallname, f_handznum, f_kopek, f_spent, f_remainkopek)
        SELECT f_name, 0, 0, 0, 0 FROM h_halls WHERE f_onlinereport=1;
        EOD;
        $this->stmtall($sql);

        //canxiki handznmum
        $sql = <<<EOD
        UPDATE cashreport crt
        inner JOIN (
        SELECT hh.f_name, abs( SUM(e.f_amount*e.f_sign)) AS f_amount
        FROM h_halls hh
        LEFT JOIN sys_json_config sn ON sn.f_id=hh.f_settings
        inner JOIN e_cash e ON e.f_cash=JSON_VALUE(sn.f_config, '$.cashbox_id')
        LEFT JOIN a_header ah ON ah.f_id=e.f_header
        WHERE hh.f_onlinereport=1 AND ah.f_date between '{$this->params->date1}' and '{$this->params->date2}'
        and coalesce(ah.f_comment, '') not LIKE 'Կանխիկի հ%' and ah.f_state=1
        GROUP BY 1) ee ON ee.f_name=crt.f_hallname
        SET crt.f_handznum=ee.f_amount;
        EOD;
        $this->stmtall($sql);

        //cash spent
        $sql = <<<EOD
        UPDATE cashreport crt
        inner JOIN (
        SELECT hh.f_name, sum(e.f_amount) as f_amount
        FROM h_halls hh
        LEFT JOIN sys_json_config sn ON sn.f_id=hh.f_settings
        inner JOIN e_cash e ON e.f_cash=JSON_VALUE(sn.f_config, '$.cashbox_id')
        LEFT JOIN a_header ah ON ah.f_id=e.f_header
        WHERE ah.f_date between '{$this->params->date1}' and '{$this->params->date2}'
        and ah.f_state=1
        AND coalesce(ah.f_comment, '') NOT LIKE 'Կանխիկի հանձնում%' 
        AND coalesce(ah.f_comment, '') NOT LIKE 'Պահեստի մուտք%' 
        AND coalesce(ah.f_comment, '') NOT LIKE 'Կոպեկ%' 
        AND coalesce(ah.f_comment, '') NOT LIKE '%վերադարձ%'
        and hh.f_onlinereport=1  and e.f_sign<0  GROUP BY 1) ee ON ee.f_name=crt.f_hallname
        SET crt.f_spent=ee.f_amount;
        EOD;
        $this->stmtall($sql);

        //kopek total
        $sql = <<<EOD
        UPDATE cashreport crt
        inner JOIN (
        SELECT hh.f_name, SUM(e.f_amount*e.f_sign) AS f_amount
        FROM h_halls hh
        LEFT JOIN sys_json_config sn ON sn.f_id=hh.f_settings
        inner JOIN e_cash e ON e.f_cash=JSON_VALUE(sn.f_config, '$.coincash_id')
        LEFT JOIN a_header ah ON ah.f_id=e.f_header
        WHERE hh.f_onlinereport=1 GROUP BY 1) ee ON ee.f_name=crt.f_hallname
        SET crt.f_remainkopek=ee.f_amount;
        EOD;
        $this->stmtall($sql);

        //coin report
        $sql = <<<EOD
        UPDATE cashreport crt
        inner JOIN (
        SELECT hh.f_name, SUM(e.f_amount) AS f_amount
        FROM h_halls hh
        LEFT JOIN sys_json_config sn ON sn.f_id=hh.f_settings
        inner JOIN e_cash e ON e.f_cash=JSON_VALUE(sn.f_config, '$.coincash_id')
        LEFT JOIN a_header ah ON ah.f_id=e.f_header
        WHERE hh.f_onlinereport=1 AND ah.f_date between '{$this->params->date1}' and '{$this->params->date2}'
        GROUP BY 1) ee ON ee.f_name=crt.f_hallname
        SET crt.f_kopek=ee.f_amount;
        EOD;
        $this->stmtall($sql);

        $this->result["cashbox"] = $this->stmtall("SELECT * FROM cashreport")->fetch_all(MYSQLI_ASSOC);
        
        $this->echoResult();
    }
}

if (!empty($params->report)) {
    $omr = new OnlineMainReport();
    $omr->{$params->report}();
}