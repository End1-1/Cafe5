<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-17 16:06:01
# Last Modified: 2026-03-15 19:50:23
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../worker/dict-cash-operation-type.php";
require_once __DIR__ . "/../worker/dict-payment.php";

class Cashbox extends Auth
{
    private function cashboxIdFromRequestConfig($params): int
    {
        if (empty($params->config)) {
            return 0;
        }
        $cfg = $params->config;
        if (is_string($cfg)) {
            $cfg = json_decode($cfg, true);
        }
        if (!is_array($cfg)) {
            return 0;
        }
        $id = (int)($cfg["f_cashbox_id"] ?? $cfg["cashbox_id"] ?? 0);
        return $id > 0 ? $id : 0;
    }

    /**
     * Request cashbox_id, then config JSON, then any open cash_session (single-site fallback).
     */
    public function resolveCashboxId($params): int
    {
        $id = (int)($params->cashbox_id ?? 0);
        if ($id > 0) {
            return $id;
        }
        $id = $this->cashboxIdFromRequestConfig($params);
        if ($id > 0) {
            return $id;
        }
        $row = $this->select(
            "select f_cashbox_id from cash_session where f_state=1 order by f_id desc limit 1"
        )->fetch_assoc();
        return (int)($row["f_cashbox_id"] ?? 0);
    }

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
        money_fmt(c.f_amount_fact) as f_amount_fact,
        money_fmt(c.f_amount_difference) as f_amount_difference,
        c.f_amount_difference as f_amount_difference_raw,
        concat(u1.f_last, ' ', LEFT(u1.f_first, 1), '.') AS f_user_open_name, 
        concat(u2.f_last, ' ', LEFT(u2.f_first, 1), '.') AS f_user_close_name, 
        count(distinct case when o.f_operation_type = ? then o.f_order_id end) as f_orders_count
        FROM cash_session c
        LEFT JOIN s_user u1 ON u1.f_id=c.f_user_open
        LEFT JOIN s_user u2 ON u2.f_id=c.f_user_close
        left join cash_operations o on o.f_session_id=c.f_id
        where c.f_id=?
        limit 1
        EOD;

        return $this->select($sql, "ii", [CASH_OP_SALES_REVENUE, $cashbox_session_id])->fetch_assoc();
    }

    public function GetOpenCashboxSession($cashbox_id)
    {
        $cash_session = $this->GetOpenedCashboxSessionId($cashbox_id);
        if (!$cash_session) {
            return null;
        }

        return $this->GetCashboxSession($cash_session["f_id"]);
    }

    /**
     * Returns open cash_session row (f_id, f_cashbox_id) or creates one with amount_open=0.
     */
    public function ensureOpenSession(int $cashbox_id, int $user_id, float $amount_open = 0): ?array
    {
        if ($cashbox_id <= 0) {
            return null;
        }
        $cash_session = $this->GetOpenedCashboxSessionId($cashbox_id);
        if ($cash_session) {
            return $cash_session;
        }
        $this->insert("cash_session", [
            "f_state" => 1,
            "f_cashbox_id" => $cashbox_id,
            "f_user_open" => $user_id,
            "f_date_open" => date("Y-m-d H:i:s"),
            "f_amount_open" => $amount_open,
            "f_amount_fact" => 0,
            "f_amount_expected" => 0,
            "f_amount_difference" => 0,
        ]);

        return $this->GetOpenedCashboxSessionId($cashbox_id);
    }

    public function CheckStatus($params)
    {
        $cashbox_id = $this->resolveCashboxId($params);
        if ($cashbox_id <= 0) {
            dieWithCode("cashbox_id not specified");
        }
        $this->result["cashbox_id"] = $cashbox_id;
        $cashbox = $this->GetOpenCashboxSession($cashbox_id);
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
        $v["f_operation_type"] = $this->resolveCashOperationType($params);
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

    public function GetCashOperation($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("id not specified");
        }
        $row = $this->select(
            "SELECT f_id, f_cashbox_id, f_session_id, f_order_id, f_operation_type, f_payment_type_id, f_datetime, f_debit, f_credit, f_currency_id, f_comment FROM cash_operations WHERE f_id=?",
            "i",
            [$id]
        )->fetch_assoc();
        if (!$row) {
            die(Translator::t("Not found"));
        }
        $orderId = trim((string)($row["f_order_id"] ?? ""));
        if ($orderId !== "") {
            die(Translator::t("Only direct cash operations can be edited."));
        }
        $this->result["operation"] = $row;
        $this->echoResult();
    }

    public function UpdateCashOperation($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("id not specified");
        }
        $existing = $this->select("SELECT f_id, f_order_id FROM cash_operations WHERE f_id=?", "i", [$id])->fetch_assoc();
        if (!$existing) {
            die(Translator::t("Not found"));
        }
        $orderId = trim((string)($existing["f_order_id"] ?? ""));
        if ($orderId !== "") {
            die(Translator::t("Only direct cash operations can be edited."));
        }

        $v = [];
        $v["f_payment_type_id"] = (int)($params->f_payment_type_id ?? 0);
        $v["f_debit"] = (float)($params->f_debit ?? 0);
        $v["f_credit"] = (float)($params->f_credit ?? 0);
        $v["f_operation_type"] = $this->resolveCashOperationType($params);
        $dt = trim((string)($params->f_datetime ?? ""));
        if ($dt !== "") {
            $ts = strtotime($dt);
            $v["f_datetime"] = $ts ? date("Y-m-d H:i:s", $ts) : date("Y-m-d H:i:s");
        } else {
            $v["f_datetime"] = date("Y-m-d H:i:s");
        }
        $v["f_currency_id"] = (int)($params->f_currency_id ?? 1);
        $v["f_comment"] = trim((string)($params->f_comment ?? ""));

        if ((float)$v["f_debit"] <= 0.009 && (float)$v["f_credit"] <= 0.009) {
            die(Translator::t("Amount must be greater than zero"));
        }
        if ($v["f_comment"] === "") {
            die(Translator::t("Comment is required"));
        }

        $this->update("cash_operations", $v, $id);
        $this->echoResult();
    }

    public function Open($params)
    {
        $cashbox_id = $this->resolveCashboxId($params);
        if ($cashbox_id <= 0) {
            dieWithCode("cashbox_id not specified");
        }
        $this->result["cashbox_id"] = $cashbox_id;
        $this->ensureOpenSession($cashbox_id, (int)$this->userid, (float)($params->amount_open ?? 0));
        $cashbox = $this->GetOpenCashboxSession($cashbox_id);
        $this->result["cashbox_session"] = $cashbox ?? [];
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
        $cashCounted = !empty($params->cash_counted);
        $amountFact = (float)($params->amount_cash ?? ($params->amount_fact ?? 0));
        $expected = (float)($cashbox["f_amount_expected"] ?? 0);
        $diff = $amountFact - $expected;

        if ($cashCounted && abs($diff) > 0.0001) {
            $isSurplus = $diff > 0;
            $this->insert("cash_operations", [
                "f_cashbox_id" => $cash_session["f_cashbox_id"],
                "f_session_id" => $cash_session["f_id"],
                "f_order_id" => "",
                "f_user" => $this->userid,
                "f_operation_type" => $isSurplus ? CASH_OP_CASH_OVERAGE : CASH_OP_CASH_SHORTAGE,
                "f_datetime" => date("Y-m-d H:i:s"),
                "f_payment_type_id" => PAYMENT_TYPE_CASH,
                "f_debit" => $isSurplus ? abs($diff) : 0,
                "f_credit" => $isSurplus ? 0 : abs($diff),
                "f_currency_id" => (int)($params->currency_id ?? 1),
                "f_comment" => ($isSurplus ? Translator::t("Cashbox close surplus") : Translator::t("Cashbox close shortage"))
                    . ": " . number_format(abs($diff), 2, ".", ""),
            ]);
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

    public function RedeemDebt($params)
    {
        require_once __DIR__ . "/../../worker/uuid.php";

        $docType = (int)($params->doc_type ?? 0);
        $partnerId = (int)($params->partner_id ?? 0);
        $currencyId = (int)($params->currency_id ?? 0);
        $amount = (float)($params->amount ?? 0);
        $cashboxId = (int)($params->cashbox_id ?? 0);
        $paymentTypeId = (int)($params->payment_type_id ?? 1);
        $comment = trim((string)($params->comment ?? ""));

        $dateStr = trim((string)($params->date ?? ""));
        if ($dateStr === "") {
            $txnDate = date("Y-m-d");
        } else {
            $ts = strtotime($dateStr);
            if ($ts === false) {
                dieWithCode(Translator::t("Invalid date"));
            }
            $txnDate = date("Y-m-d", $ts);
        }
        $opDateTime = $txnDate . " " . date("H:i:s");

        if ($docType !== 1 && $docType !== 2) {
            dieWithCode(Translator::t("Invalid debt type"));
        }
        if ($partnerId <= 0) {
            dieWithCode(Translator::t("Partner is required"));
        }
        if ($currencyId <= 0) {
            dieWithCode(Translator::t("Currency is required"));
        }
        if ($amount <= 0.00001) {
            dieWithCode(Translator::t("Amount must be greater than zero"));
        }

        $uuid = uuid_v4();
        $debtRow = [
            "f_date" => $txnDate,
            "f_partner" => $partnerId,
            "f_doc_type" => $docType,
            "f_doc_uuid" => $uuid,
            "f_currency_id" => $currencyId,
        ];
        if ($docType === 1) {
            $debtRow["f_credit"] = 0;
            $debtRow["f_debit"] = $amount;
        } else {
            $debtRow["f_credit"] = $amount;
            $debtRow["f_debit"] = 0;
        }

        $this->beginTransaction();
        $this->insert("cash_debts", $debtRow);

        if ($cashboxId > 0) {
            $cashSession = $this->GetOpenedCashboxSessionId($cashboxId);
            if (!$cashSession) {
                $this->rollback();
                dieWithCode(Translator::t("No active session"));
            }
            $opComment = $comment !== ""
                ? $comment
                : ($docType === 1
                    ? Translator::t("Debt redeem supplier")
                    : Translator::t("Debt redeem customer"));

            if ($docType === 1) {
                $this->insert("cash_operations", [
                    "f_cashbox_id" => (int)$cashSession["f_cashbox_id"],
                    "f_session_id" => (int)$cashSession["f_id"],
                    "f_order_id" => "",
                    "f_user" => $this->userid,
                    "f_operation_type" => CASH_OP_DEBT_REPAYMENT,
                    "f_payment_type_id" => $paymentTypeId,
                    "f_datetime" => $opDateTime,
                    "f_debit" => 0,
                    "f_credit" => $amount,
                    "f_currency_id" => $currencyId,
                    "f_comment" => $opComment,
                ]);
                $this->select(
                    "UPDATE cash_session SET f_amount_expected=f_amount_expected-? WHERE f_id=?",
                    "di",
                    [$amount, (int)$cashSession["f_id"]],
                    true
                );
            } else {
                $this->insert("cash_operations", [
                    "f_cashbox_id" => (int)$cashSession["f_cashbox_id"],
                    "f_session_id" => (int)$cashSession["f_id"],
                    "f_order_id" => "",
                    "f_user" => $this->userid,
                    "f_operation_type" => CASH_OP_DEBT_RECOVERY,
                    "f_payment_type_id" => $paymentTypeId,
                    "f_datetime" => $opDateTime,
                    "f_debit" => $amount,
                    "f_credit" => 0,
                    "f_currency_id" => $currencyId,
                    "f_comment" => $opComment,
                ]);
                $this->select(
                    "UPDATE cash_session SET f_amount_expected=f_amount_expected+? WHERE f_id=?",
                    "di",
                    [$amount, (int)$cashSession["f_id"]],
                    true
                );
            }
        }

        $this->commit();
        $this->echoResult();
    }

    /**
     * Manual cash movement: explicit f_operation_type or infer from debit/credit.
     */
    private function resolveCashOperationType(object $params): int
    {
        $opType = (int)($params->f_operation_type ?? 0);
        if ($opType > 0) {
            return $opType;
        }

        $debit = (float)($params->f_debit ?? 0);
        $credit = (float)($params->f_credit ?? 0);
        if ($debit > 0.009) {
            return CASH_OP_DEBT_RECOVERY;
        }
        if ($credit > 0.009) {
            return CASH_OP_TOTAL_EXPENSES;
        }

        return 0;
    }
}
