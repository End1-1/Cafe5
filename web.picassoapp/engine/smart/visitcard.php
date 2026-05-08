<?php
# (C) 2024-2025 Kudryashov Vasili
# Created - 2025-09-17 09:00:03
# Last modified - 2025-12-25 02:48:46
require_once __DIR__ . "/smart.php";

class VisitCard extends PClass
{
    public function get($p)
    {
        $card = $this->stmtall("select * from b_cards_discount where f_code=?", "s", [$p->code])->fetch_assoc();
        if (empty($card)) {
            $this->exitError($this->tr("Invalid code"));
        }

        $customer = $this->stmtall("select  * from c_partners where f_id=?", "i", [$card["f_client"]])->fetch_assoc();
        if (!empty($customer)) {
            $this->result["f_customer"] = $customer["f_taxname"];
        }
        $balance = $this->stmtall("select cast(sum(f_data) as float) as f_balance from b_history where f_card=?", "i", [$card["f_id"]])->fetch_assoc();
        if (!empty($card['f_dateend']) && $card['f_dateend'] < date('Y-m-d')) {
            if ($balance["f_balance"] > 0) {
                $bi["f_id"] = $this->uuidv4();
                $bi["f_card"] = $card["f_id"];
                $bi["f_value"] = 0;
                $bi["f_data"] = -1 * $balance["f_balance"];
                $bi["f_comment"] = "expired";
                $this->sinsert("b_history", $bi);
            }
            $this->exitError($this->tr("Card expired"));
        }
        $this->result["f_balance"] = intval($balance["f_balance"]);
        $this->result["card"] = $card;
        $this->echoResult();
    }
    public function visit($p)
    {
        $card = $this->stmtall("select * from b_cards_discount where f_code=?", "s", [$p->code])->fetch_assoc();
        if (empty($card)) {
            $this->exitError($this->tr("Invalid code"));
        }
        $customer = $this->stmtall("select  * from c_partners where f_id=?", "i", [$card["f_client"]])->fetch_assoc();
        if (!empty($customer)) {
            $this->result["f_customer"] = $customer["f_taxname"];
        }
        $balance = $this->stmtall("select cast(sum(f_data) as float) as f_balance from b_history where f_card=?", "i", [$card["f_id"]])->fetch_assoc();
        $bind["f_data"] = -1 * $card["f_value"];
        $bind["f_id"] = $this->uuidv4();
        $bind["f_card"] = $card["f_id"];
        $this->sinsert("b_history", $bind);
        $this->result["f_remain"] = intval(($balance["f_balance"] - $card["f_value"]) / $card["f_value"]);
        $this->echoResult();
    }
}

$vc = new VisitCard();
if (method_exists($vc, $params->method)) {
    $vc->{$params->method}($params);
} else {
    exitError("Unknown usement");
}
