<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-16 04:06:14
# Last Modified: 2026-01-16 04:06:19

require_once __DIR__ . "/index.php";

class ShortDebt extends Auth
{
    public function PayOffDebt($params)
    {
        $this->select(
            "update o_header_flags set f_5=0 where f_id=?",
            "s",
            [$params->payoffdebt],
            true
        );

        $this->select("delete from b_clients_debts where f_order=?", "s", [$params->payoffdebt], true);

        $this->echoResult();
    }
}