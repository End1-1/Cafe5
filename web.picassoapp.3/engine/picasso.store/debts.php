<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-03-27 00:54:32
# Last modified - 2025-03-27 00:54:34
defined('APP') or die('Die, vampire!');

class debts extends PClass {

    public function getAll () {
        $where = " where 1=1 ";
        if (!empty($this->params->searchString)) {
            $where .= " and (lower(p.f_name) like '%{$this->params->searchString}%' ";
            $where .= " or lower(p.f_address) like '%{$this->params->searchString}%' ";
            $where .= " or lower(p.f_taxname) like '%{$this->params->searchString}%' ";
            $where .= " or lower(p.f_taxcode) like '%{$this->params->searchString}%' )";
        }
        $sql = <<<EOD
        select d.f_costumer, p.f_taxname, p.f_taxcode, p.f_address, sum(d.f_amount) as f_amount
        from b_clients_debts d
        left join c_partners p on p.f_id=d.f_costumer
        $where
        group by 1
        EOD;
        $this->result["data"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function getPartnerDebts() {
        $where = " and d.f_date < '{$this->params->date1}'";
        $sql = <<<EOD
        select '{$this->tr('Start')}' as f_purpose, 
        '' as f_order, '' as f_cash, '' as f_storedoc, 
        cast(coalesce(sum(d.f_amount), 0) as float)  as f_amount
        from b_clients_debts d
        where d.f_costumer=? $where
        EOD;
        $start = $this->stmtall($sql, "i", [$this->params->partner])->fetch_all(MYSQLI_ASSOC);
        $where = " and d.f_date between'{$this->params->date1}' and '{$this->params->date2}'";
        $sql = <<<EOD
        select DATE_FORMAT(f_date, '%d/%m/%Y') AS  f_purpose, 
         f_order,  f_cash, f_storedoc, 
        d.f_amount 
        from b_clients_debts d
        where d.f_costumer=? $where
        EOD;
        $current = $this->stmtall($sql,"i", [$this->params->partner])->fetch_all(MYSQLI_ASSOC);
        $this->result["data"] = array_merge($start,$current);
        $this->echoResult();
    }

}