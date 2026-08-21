<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-17 16:06:01
# Last Modified: 2026-03-15 19:50:23
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../worker/dict-cash-operation-type.php";
require_once __DIR__ . "/../worker/dict-payment.php";
require_once __DIR__ . "/../../worker/fmt.php";
require_once __DIR__ . "/../../worker/helper.php";

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
        $this->result["operation"]["f_comment"] = translate_cash_operation_comment((string)($row["f_comment"] ?? ""));
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
        $currencyId = (int)($params->currency_id ?? 1);
        if ($currencyId <= 0) {
            $currencyId = 1;
        }
        $funds = $this->cashboxFundsByCashbox((int)$cash_session["f_cashbox_id"], $currencyId);
        $expectedCash = (float)($funds[PAYMENT_TYPE_CASH] ?? 0);
        $diff = $amountFact - $expectedCash;

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
        $cashbox["f_amount_difference"] = $diff;
        $this->update("cash_session", $cashbox, $cashbox["f_id"]);
        $session = $this->GetCashboxSession($cash_session["f_id"]);
        $session["f_amount_expected_cash"] = money_fmt($expectedCash);
        $this->result["cashbox"] = $session;
        $this->echoResult();
    }

    /**
     * Net balance per payment type for a cashbox (same basis as revenue report total).
     *
     * @return array<int,float> payment_type_id => available amount
     */
    private function cashboxFundsByCashbox(int $cashboxId, int $currencyId): array
    {
        $payment = require __DIR__ . "/../worker/dict-payment.php";
        $funds = [];

        $rows = $this->select(
            "SELECT op.f_payment_type_id,
                    SUM(COALESCE(op.f_debit, 0) - COALESCE(op.f_credit, 0)) AS f_available
             FROM cash_operations op
             WHERE op.f_cashbox_id = ?
               AND COALESCE(op.f_currency_id, 1) = ?
             GROUP BY op.f_payment_type_id",
            "ii",
            [$cashboxId, $currencyId]
        )->fetch_all(MYSQLI_ASSOC);

        foreach ($rows as $r) {
            $pt = (int)$r["f_payment_type_id"];
            if ($pt <= 0 || empty($payment["cashbox"][$pt])) {
                continue;
            }
            $funds[$pt] = (float)$r["f_available"];
        }

        return $funds;
    }

    /**
     * Available funds in the open session of a cashbox, split by (cashbox-relevant) payment type.
     * Cash payment type also includes the session opening amount.
     *
     * @return array<int,float> payment_type_id => available amount (only positive balances)
     */
    private function cashboxFundsBySession(int $sessionId, float $amountOpen): array
    {
        $payment = require __DIR__ . "/../worker/dict-payment.php";
        $funds = [];

        $rows = $this->select(
            "SELECT f_payment_type_id, SUM(COALESCE(f_debit,0) - COALESCE(f_credit,0)) AS f_available
             FROM cash_operations WHERE f_session_id=? GROUP BY f_payment_type_id",
            "i",
            [$sessionId]
        )->fetch_all(MYSQLI_ASSOC);

        foreach ($rows as $r) {
            $pt = (int)$r["f_payment_type_id"];
            if (empty($payment["cashbox"][$pt])) {
                continue;
            }
            $funds[$pt] = (float)$r["f_available"];
        }

        $funds[PAYMENT_TYPE_CASH] = ($funds[PAYMENT_TYPE_CASH] ?? 0) + $amountOpen;

        return $funds;
    }

    public function GetCashboxFunds($params)
    {
        $cashboxId = (int)($params->cashbox_id ?? 0);
        if ($cashboxId <= 0) {
            dieWithCode("cashbox_id not specified");
        }

        $currencyId = (int)($params->currency_id ?? 1);
        if ($currencyId <= 0) {
            $currencyId = 1;
        }

        $funds = $this->cashboxFundsByCashbox($cashboxId, $currencyId);

        $payment = require __DIR__ . "/../worker/dict-payment.php";
        $out = [];
        foreach ($payment["types"] as $pt) {
            if (empty($payment["cashbox"][$pt])) {
                continue;
            }
            $available = round((float)($funds[$pt] ?? 0), 2);
            if ($available <= 0.00001) {
                continue;
            }
            $out[] = [
                "f_payment_type_id" => $pt,
                "f_name" => Translator::t($payment["names"][$pt] ?? (string)$pt),
                "f_available" => $available,
                "f_available_fmt" => number_format($available, 2, ".", ""),
            ];
        }

        $this->result["funds"] = $out;
        $this->result["currency_id"] = $currencyId;
        $this->echoResult();
    }

    public function TransferFunds($params)
    {
        $sourceId = (int)($params->source_cashbox_id ?? 0);
        $destId = (int)($params->dest_cashbox_id ?? 0);
        $items = $params->items ?? [];

        if ($sourceId <= 0) {
            dieWithCode(Translator::t("Source cashbox is required"));
        }
        if ($destId <= 0) {
            dieWithCode(Translator::t("Destination cashbox is required"));
        }
        if ($sourceId === $destId) {
            dieWithCode(Translator::t("Source and destination must differ"));
        }

        $currencyId = (int)($params->currency_id ?? 1);
        if ($currencyId <= 0) {
            $currencyId = 1;
        }
        $available = $this->cashboxFundsByCashbox($sourceId, $currencyId);

        $transfers = [];
        foreach ((array)$items as $it) {
            $pt = (int)($it->f_payment_type_id ?? 0);
            $amount = round((float)($it->amount ?? 0), 2);
            if ($pt <= 0 || $amount <= 0.00001) {
                continue;
            }
            if ($amount - (float)($available[$pt] ?? 0) > 0.00001) {
                dieWithCode(Translator::t("Amount exceeds available funds"));
            }
            $transfers[] = ["pt" => $pt, "amount" => $amount];
        }

        if (empty($transfers)) {
            dieWithCode(Translator::t("Amount must be greater than zero"));
        }

        $this->beginTransaction();

        $now = date("Y-m-d H:i:s");
        $userComment = trim((string)($params->comment ?? ""));
        $outComment = translate_template('Transfer to cashbox %cashbox', ['%cashbox' => (string)$destId]);
        $inComment = translate_template('Transfer from cashbox %cashbox', ['%cashbox' => (string)$sourceId]);
        if ($userComment !== "") {
            $outComment .= ": " . $userComment;
            $inComment .= ": " . $userComment;
        }

        foreach ($transfers as $t) {
            $this->insert("cash_operations", [
                "f_cashbox_id" => $sourceId,
                "f_session_id" => 0,
                "f_order_id" => "",
                "f_user" => $this->userid,
                "f_operation_type" => CASH_OP_TRANSFER_OUT,
                "f_payment_type_id" => $t["pt"],
                "f_datetime" => $now,
                "f_debit" => 0,
                "f_credit" => $t["amount"],
                "f_currency_id" => $currencyId,
                "f_comment" => $outComment,
            ]);

            $this->insert("cash_operations", [
                "f_cashbox_id" => $destId,
                "f_session_id" => 0,
                "f_order_id" => "",
                "f_user" => $this->userid,
                "f_operation_type" => CASH_OP_TRANSFER_IN,
                "f_payment_type_id" => $t["pt"],
                "f_datetime" => $now,
                "f_debit" => $t["amount"],
                "f_credit" => 0,
                "f_currency_id" => $currencyId,
                "f_comment" => $inComment,
            ]);
        }

        $this->commit();
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
        if (!in_array($paymentTypeId, [PAYMENT_TYPE_CASH, PAYMENT_TYPE_CARD, PAYMENT_TYPE_BANK], true)) {
            dieWithCode(Translator::t("Invalid payment type"));
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
