<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-10-06 23:59:29
# Last Modified: 2026-02-10 12:22:42
require_once __DIR__ . "/index.php";

class DiscountSystem extends Auth
{

    public function GetInfo($params)
    {
        $card = $this->select("select * from b_cards_discount where f_code=?", "s", [$params->code])->fetch_assoc();
        if (empty($card)) {
            dieWithCode(Translator::t("Card with the specified code not found"));
        }
        if (!empty($params->need)) {
            if ($card["f_mode"] !== $params->need) {
                dieWithCode(Translator::t("Card type differs from the requested type"));
            }
        }
        $partner = $this->select("select * from c_partners where f_id=?", "i", [$card["f_client"]])->fetch_assoc();
        $this->result["card"] = $card;
        $this->result["partner"] = $partner;
        $this->echoResult();
    }
}
