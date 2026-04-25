<?php 
# (C) 2024-2025 Kudryashov Vasili
# Created - 23/02/2025
# Last modified - 23/02/2025
require_once __DIR__ . "/worker.php";

class CheckQty extends PClass {
    public function checkStore() {
        $this->result["goodsid"] = $this->params->goodsid;
        $sql = "select coalesce(sum(f_qty*f_type), 0) as remain from a_store where f_goods=?";
        $this->result["remain"] = $this->stmtall($sql, "i", [$this->params->goodsid])->fetch_assoc()["remain"];
        $this->echoResult();
    }
}

$cq = new CheckQty();
$cq->{$params->method}();