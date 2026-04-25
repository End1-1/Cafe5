<?php
# © 2025 , Kudryashov Vasili
# Last Modified: 2025-10-17 00:30:42
require_once __DIR__ . "/goods.php";

class QuickChangeCurrency extends DB
{
    private $id;
    private $currency;
    public function __construct($id, $currency)
    {
        parent::__construct();
        $this->id = $id;
        $this->currency = $currency;
    }

    function change()
    {
        $this->stmtall("update a_header set f_currency=$this->currency where f_id='$this->id'");
        $this->stmtall("update b_clients_debts set f_currency=$this->currency where f_storedoc='$this->id'");

        $docs = $this->stmtall("SELECT  distinct(dr2.f_document) "
            . "from a_store dr "
            . "left join a_store dr2 on dr2.f_base=dr.f_id  "
            . "where dr.f_document=?", "s", [$this->id])->fetch_all();

        foreach ($docs as $d) {
            $this->stmtall("update a_header set f_currency=? where f_id=?", "is", [$this->currency, $d[0]]);
        }

        $this->echoResult();
    }
}

$quickChangeCurrency = new QuickChangeCurrency($params->id, $params->currency);
$quickChangeCurrency->change();