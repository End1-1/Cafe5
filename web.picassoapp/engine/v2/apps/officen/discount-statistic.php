<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-10-20 02:45:28
# Last Modified: 2025-11-09 16:21:44

require_once __DIR__ . "/index.php";
class DiscountStatistic extends Auth
{
    public function get($params) {}

    public function Transaction($params) {
        $sql = <<<EOD
        SELECT bh.f_date, bh.f_data as f_amount, 
        bc.f_id as f_card, bc.f_client AS f_partner,  bc.f_code
        FROM b_history bh
        LEFT JOIN b_cards_discount bc ON bc.f_id=bh.f_card
        WHERE bh.f_id=?
        EOD;
        $data = $this->select($sql, "s", [$params->f_id])->fetch_assoc();
        if (empty($data)) {
            dieWithCode(tr("Invalid transaction id"));
        }
        $this->result = array_merge($this->result, $data);

        $this->echoResult();
    }

    public function DeleteTransaction($params) {
        $this->beginTransaction();
        $this->delete("b_history", $params->f_id);
        $this->delete("b_gift_card_history", $params->f_id, "f_trsale");
        $this->commit();
        $this->echoResult();
    }

    public function GetCards($params)
    {
        $sql = <<<EOD
        SELECT  d.f_id, d.f_code, SUM(f_amount) as f_amount
        FROM b_cards_discount d
        LEFT JOIN b_gift_card_history h ON h.f_card=d.f_id
        where d.f_client={$params->partner}
        GROUP BY 1
        EOD;
        $this->result["data"] = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function getBalance($params)
    {
        $sql = <<<EOD
        select sum(f_amount) as f_balance
        from b_gift_card_history bh
        inner join b_cards_discount bc on bc.f_id=bh.f_card
        where bc.f_client={$params->partner}
        EOD;
        $this->result["data"] = $this->select($sql)->fetch_assoc();
        $this->echoResult();
    }

    public function redeem($params)
    {
        $v = [
            "f_card" => $params->f_card,
            "f_trsale" => $params->f_id,
            "f_amount" => $params->f_amount * -1,
            "f_type" => 9
        ];
        $this->insert("b_gift_card_history", $v);

        $v = [
            "f_id" => $params->f_id,
            "f_type" => $params->f_type,
            "f_card" => $params->f_card,
            "f_value" => -1,
            "f_data" => $params->f_amount * -1,
            "f_date" => date("Y-m-d")
        ];
        $this->insert("b_history", $v);

        $this->echoResult();
    }
}
