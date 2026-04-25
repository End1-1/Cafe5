<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-09 13:32:12
# Last Modified: 2026-02-09 13:32:16

require_once __DIR__ . "/index.php";

class GiftCard  extends Auth
{
    public function Get($params)
    {
        $params->code = str_replace([";", "?"], "", $params->code);
        $sql = <<<EOD
        SELECT b.f_id, b.f_costumer, date_fmt(b.f_valid_until) as f_valid_until,
        DATEDIFF(b.f_valid_until, CURRENT_DATE) AS f_days_left,
        money_fmt(bh.f_sum) as f_sum
        FROM b_gift_card b 
        LEFT JOIN (
            select bh.f_card, SUM(bh.f_amount) as f_sum
            FROM  b_gift_card_history bh 
            LEFT JOIN b_gift_card b ON b.f_id=bh.f_card
            WHERE b.f_code=?
        ) bh ON bh.f_card=b.f_id 
        WHERE f_code=?
        EOD;

        $card = $this->select($sql, "ss", [$params->code, $params->code])->fetch_assoc();
        if (!$card) {
            dieWithCode(Translator::t("Invalid gift card code"));
        }
        if ($card["f_days_left"] < 0) {
            dieWithCode(
                Translator::t("Card expired") . "<br>" .
                    abs($card["f_days_left"]) . " " .
                    Translator::t("days")
            );
        }


        $validators = require_once __DIR__ . "/dict-validators.php";
        $sql = $validators["select"]["c_partners"]["sql"];
        $sql = str_replace("1=1", "p.f_id=?", $sql);

        $partner = $this->select($sql, "ssi", [Translator::$locale, Translator::$locale, $card["f_costumer"]])->fetch_assoc();
        $this->result["partner"] = $partner;
        $this->result["card"] = $card;
        $this->echoResult();
    }

    public function GetAccumulate($params)
    {
        $params->code = str_replace([";", "?"], "", $params->code);
        $cardinfo = $this->select("select * from b_cards_discount where f_code=?", "s", [$params->code])->fetch_assoc();
        if (!$cardinfo) {
            dieWithCode(Translator::t("Invalid card code"));
        }

        $sql = <<<EOD
        SELECT b.f_id, b.f_client, date_fmt(b.f_dateend) as f_valid_until,
        money_fmt(bh.f_sum) as f_sum, b.f_value
        FROM b_cards_discount b 
        LEFT JOIN (
            select bh.f_card, SUM(bh.f_amount) as f_sum
            FROM  b_gift_card_history bh 
            LEFT JOIN b_gift_card b ON b.f_id=bh.f_card
            WHERE bh.f_card=?
        ) bh ON bh.f_card=b.f_id 
        WHERE b.f_code=?
        EOD;

        $card = $this->select($sql, "is", [$cardinfo["f_id"], $params->code])->fetch_assoc();
        if (!$card) {
            dieWithCode(Translator::t("Invalid gift card code"));
        }
        $validators = require_once __DIR__ . "/dict-validators.php";
        $sql = $validators["select"]["c_partners"]["sql"];
        $sql = str_replace("1=1", "p.f_id=?", $sql);
        $partner = $this->select($sql, "ssi", [Translator::$locale, Translator::$locale, $card["f_client"]])->fetch_assoc();
        $this->result["partner"] = $partner;
        $this->result["card"] = $card;
        $this->echoResult();
    }

    public function GetGiftOfPartnerId($params)
    {
        $check = $this->select("select f_code from b_cards_discount where f_client=? and f_mode=4", "i", [$params->id])->fetch_assoc();
        if ($check) {
            $this->result["code"] = $check["f_code"];
        }
        $this->echoResult();
    }
}
