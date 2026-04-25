<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-01 11:26:55
# Last Modified: 2025-12-01 11:26:57

require_once __DIR__ . "/index.php";

class FiscalData extends Auth {
    public function get($params) {
        $sql =<<<EOD
        select * from o_tax_log where f_order=? order by f_date, f_time
        EOD;
        $this->result["data"] = $this->select($sql, "s", [$params->id])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}