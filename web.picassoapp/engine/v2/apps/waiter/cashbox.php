<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-17 16:06:01
# Last Modified: 2026-03-15 19:50:23
require_once __DIR__ . "/index.php";

class Cashbox extends Auth
{
    public function GetOpenedCashboxSessionId($cashbox_id)
    {
        $cashbox_session = $this->select("select f_id, f_cashbox_id from cash_session where f_state=1 and f_cashbox_id=?", "i", [$cashbox_id])->fetch_assoc();
        return $cashbox_session;
    }

    public function GetRawCashboxSession($cashbox_session_id)
    {
        return $this->select("select * from cash_session where f_id=?", "i", [$cashbox_session_id])->fetch_assoc();
    }

    public function GetCashboxSession($cashbox_session_id)
    {
        $sql = <<<EOD
        SELECT c.f_id, datetime_fmt(c.f_date_open, 0) as f_date_open, datetime_fmt(c.f_date_close, 0) as f_date_close, 
        money_fmt(c.f_amount_expected) as f_amount_expected,
        concat(u1.f_last, ' ', LEFT(u1.f_first, 1), '.') AS f_user_open_name, 
        concat(u2.f_last, ' ', LEFT(u2.f_first, 1), '.') AS f_user_close_name, 
        count(distinct case when o.f_operation_type = 1 then o.f_order_id end) as f_orders_count
        FROM cash_session c
        LEFT JOIN s_user u1 ON u1.f_id=c.f_user_open
        LEFT JOIN s_user u2 ON u2.f_id=c.f_user_open
        left join cash_operations o on o.f_session_id=c.f_id
        where c.f_id=?
        limit 1
        EOD;

        return $this->select($sql, "i", [$cashbox_session_id])->fetch_assoc();
    }

    public function GetOpenCashboxSession($cashbox_id)
    {
        $cash_session = $this->GetOpenedCashboxSessionId($cashbox_id);
        if (!$cash_session) {
            return null;
        }

        return $this->GetCashboxSession($cash_session["f_id"]);
    }

    public function CheckStatus($params)
    {
        if (empty($params->cashbox_id)) {
            dieWithCode("cashbox_id not specified");
        }
        $cashbox = $this->GetOpenCashboxSession($params->cashbox_id);
        $this->result["cashbox_session_id"] = $cashbox ? $cashbox["f_id"]  : 0;
        $this->result["cashbox_session"] = $cashbox ?? [];
        $this->echoResult();
    }

    public function MoveMoney($params)
    {
        $cash_session = $this->GetOpenCashboxSession($params->cashbox_id);
        if (!$cash_session) {
            die(Translator::t("No active session"));
        }
        $v["f_cashbox_id"] = $params->cashbox_id;
        $v["f_session_id"] = $cash_session["f_id"];
        $v["f_order_id"] = $params->f_order_id;
        $v["f_user"] = $this->userid;
        $v["f_operation_type"] = (int)($params->f_operation_type ?? 0);
        $dt = trim((string)($params->f_datetime ?? ""));
        if ($dt !== "") {
            $ts = strtotime($dt);
            $v["f_datetime"] = $ts ? date("Y-m-d H:i:s", $ts) : date("Y-m-d H:i:s");
        } else {
            $v["f_datetime"] = date("Y-m-d H:i:s");
        }
        $v["f_payment_type_id"] = $params->f_payment_type_id;
        $v["f_debit"] = $params->f_debit;
        $v["f_credit"] = $params->f_credit;
        $v["f_currency_id"] = $params->f_currency_id;
        $v["f_comment"]  = $params->f_comment;
        $this->insert("cash_operations", $v);
        $this->echoResult();
    }


    public function Open($params)
    {
        $cashbox = $this->GetOpenCashboxSession($params->cashbox_id);
        if (!$cashbox) {
            $this->insert("cash_session", [
                "f_state" => 1,
                "f_cashbox_id" => $params->cashbox_id,
                "f_user_open" => $this->userid,
                "f_date_open" => date("Y-m-d H:i:s"),
                "f_amount_open" => $params->amount_open,
                "f_amount_fact" => 0,
                "f_amount_expected" => 0,
                "f_amount_difference" => 0
            ]);

            $cashbox = $this->GetOpenCashboxSession($params->cashbox_id);
        }
        $this->result["cashbox_session"] = $cashbox;
        $this->echoResult();
    }

    public function Close($params)
    {
        $cash_session = $this->GetOpenedCashboxSessionId($params->cashbox_id);
        if (!$cash_session) {
            die(Translator::t("No active session"));
        }
        $cashbox = $this->GetRawCashboxSession($cash_session["f_id"]);
        if (!$cashbox) {
            dieWithCode("Do you want to hack close function of cashbox?");
        }
        $amountFact = (float)($params->amount_cash ?? ($params->amount_fact ?? 0));
        $expected = (float)($cashbox["f_amount_expected"] ?? 0);
        if ($amountFact > 0.0001) {
            $diff = $amountFact - $expected;
            if (abs($diff) > 0.0001) {
                $isSurplus = $diff > 0;
                $this->insert("cash_operations", [
                    "f_cashbox_id" => $cash_session["f_cashbox_id"],
                    "f_session_id" => $cash_session["f_id"],
                    "f_user" => $this->userid,
                    "f_datetime" => date("Y-m-d H:i:s"),
                    "f_payment_type_id" => 1,
                    "f_debit" => $isSurplus ? abs($diff) : 0,
                    "f_credit" => $isSurplus ? 0 : abs($diff),
                    "f_currency_id" => 1,
                    "f_comment" => ($isSurplus ? Translator::t("Cashbox close surplus") : Translator::t("Cashbox close shortage"))
                        . ": " . number_format(abs($diff), 2, ".", "")
                ]);
            }
        }
        $cashbox["f_state"] = 2;
        $cashbox["f_date_close"] = date("Y-m-d H:i:s");
        $cashbox["f_user_close"] = $this->userid;
        $cashbox["f_amount_fact"] = $amountFact;
        $cashbox["f_amount_expected"] = $cashbox["f_amount_expected"];
        $cashbox["f_amount_difference"] = $amountFact - $expected;
        $this->update("cash_session", $cashbox, $cashbox["f_id"]);
        $this->result["cashbox"] = $this->GetCashboxSession($cash_session["f_id"]);
        $this->echoResult();
    }
}
