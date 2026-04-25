<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-06-25 13:12:11
# Last Modified: 2025-06-25 13:12:14

require_once __DIR__ . "/index.php";
class SaleDoc extends Auth
{
    public function getEmarks($params) {
        $emarks = $this->select("select f_qr from o_qr where f_header=?", "s", [$params->f_header])->fetch_all(MYSQLI_ASSOC);
        $this->result["emarks"] = array_column($emarks, "f_qr");
        $this->echoResult();
    }
}