<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Payments extends ArarixAuth
{
    public function List($params)
    {
        $rows = $this->select(
            "SELECT f_id AS id, f_brand AS brand, f_last4 AS last4, f_holder AS holder FROM ararix_payment_cards WHERE f_client_id = ? ORDER BY f_id DESC",
            "i",
            [$this->clientId]
        )->fetch_all(MYSQLI_ASSOC);

        $this->result["cards"] = $rows;
        $this->echoResult();
    }
}
