<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-09 13:32:12
# Last Modified: 2026-02-09 13:32:16

require_once __DIR__ . "/index.php";

class GiftCard  extends Auth
{
    private function partnerById($id)
    {
        $validators = require __DIR__ . "/dict-validators.php";
        $sql = $validators["select"]["c_partners"]["sql"];
        $sql = str_replace("1=1", "p.f_id=?", $sql);
        return $this->select($sql, "ssi", [Translator::$locale, Translator::$locale, $id])->fetch_assoc();
    }

    public function Get($params)
    {
        $params->code = str_replace([";", "?"], "", $params->code);
        $sql = <<<EOD
        SELECT b.f_id, b.f_costumer, date_fmt(b.f_valid_until) as f_valid_until,
        DATEDIFF(b.f_valid_until, CURRENT_DATE) AS f_days_left,
        money_fmt(coalesce(ops.f_sum, 0)) as f_sum
        FROM b_gift_card b
        LEFT JOIN (
            select f_card_id, SUM(f_amount) as f_sum
            FROM b_gift_card_ops
            GROUP BY f_card_id
        ) ops ON ops.f_card_id = b.f_id
        WHERE b.f_code = ?
        EOD;

        $card = $this->select($sql, "s", [$params->code])->fetch_assoc();
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

        $this->result["partner"] = $this->partnerById($card["f_costumer"]);
        $this->result["card"] = $card;
        $this->echoResult();
    }

    public function GetAccumulate($params)
    {
        $params->code = str_replace([";", "?"], "", $params->code);
        $sql = <<<EOD
        SELECT b.f_id, b.f_client, date_fmt(b.f_dateend) as f_valid_until,
        DATEDIFF(b.f_dateend, CURRENT_DATE) AS f_days_left,
        money_fmt(coalesce(ops.f_sum, 0)) as f_sum, b.f_value
        FROM b_accumulate_cards b
        LEFT JOIN (
            select f_card_id, SUM(f_amount) as f_sum
            FROM b_accumulate_ops
            GROUP BY f_card_id
        ) ops ON ops.f_card_id = b.f_id
        WHERE b.f_code = ?
        EOD;

        $card = $this->select($sql, "s", [$params->code])->fetch_assoc();
        if (!$card) {
            dieWithCode(Translator::t("Invalid card code"));
        }

        $this->result["partner"] = $this->partnerById($card["f_client"]);
        $this->result["card"] = $card;
        $this->echoResult();
    }

    public function GetGiftOfPartnerId($params)
    {
        $check = $this->select("select f_code from b_accumulate_cards where f_client=?", "i", [$params->id])->fetch_assoc();
        if ($check) {
            $this->result["code"] = $check["f_code"];
        }
        $this->echoResult();
    }
}
