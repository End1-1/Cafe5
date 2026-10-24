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

    /**
     * Request currency, then cash_box.f_currency_id, then 1.
     */
    public function resolveCurrencyId($params, int $cashboxId = 0): int
    {
        $id = (int)($params->currency_id ?? $params->f_currency_id ?? 0);
        if ($id > 0) {
            return $id;
        }
        if ($cashboxId <= 0) {
            $cashboxId = (int)($params->cashbox_id ?? $params->source_cashbox_id ?? 0);
        }
        if ($cashboxId > 0) {
            $row = $this->select(
                "SELECT f_currency_id FROM cash_box WHERE f_id=?",
                "i",
                [$cashboxId]
            )->fetch_assoc();
            $id = (int)($row["f_currency_id"] ?? 0);
            if ($id > 0) {
                return $id;
            }
        }
        return 1;
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
        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        $sql = <<<EOD
        SELECT c.f_id, c.f_cashbox_id, COALESCE(cb.f_name, '') AS f_cashbox_name,
        COALESCE(cb.f_currency_id, 1) AS f_currency_id, c.f_state, c.f_amount_open,
        datetime_fmt(c.f_date_open, 0) as f_date_open, datetime_fmt(c.f_date_close, 0) as f_date_close, 
        money_fmt(c.f_amount_expected) as f_amount_expected,
        money_fmt(c.f_amount_fact) as f_amount_fact,
        money_fmt(c.f_amount_difference) as f_amount_difference,
        c.f_amount_difference as f_amount_difference_raw,
        concat(u1.f_last, ' ', LEFT(u1.f_first, 1), '.') AS f_user_open_name, 
        concat(u2.f_last, ' ', LEFT(u2.f_first, 1), '.') AS f_user_close_name, 
        count(distinct case when o.f_operation_type = ? then o.f_order_id end) as f_orders_count
        FROM cash_session c
        LEFT JOIN cash_box cb ON cb.f_id = c.f_cashbox_id
        LEFT JOIN s_user u1 ON u1.f_id=c.f_user_open
        LEFT JOIN s_user u2 ON u2.f_id=c.f_user_close
        left join cash_operations o on o.f_session_id=c.f_id
        where c.f_id=?
        limit 1
        EOD;

        $session = $this->select($sql, "ii", [CASH_OP_SALES_REVENUE, $cashbox_session_id])->fetch_assoc();
        if (!$session) {
            return null;
        }
        $raw = $this->GetRawCashboxSession((int)$cashbox_session_id);
        $amountOpen = $this->resolveSessionAmountOpen($raw ?: []);
        $expectedCash = cash_ops_session_expected_cash($this, (int)$cashbox_session_id, $amountOpen);
        $session["f_amount_expected_cash"] = money_fmt($expectedCash);
        $session["f_amount_expected_cash_raw"] = $expectedCash;
        $session["f_amount_open_raw"] = $amountOpen;
        $revenue = cash_ops_session_sales_revenue($this, (int)$cashbox_session_id);
        $session["f_amount_revenue"] = money_fmt($revenue);
        $session["f_amount_revenue_raw"] = $revenue;
        $session["tender_breakdown"] = cash_ops_session_tender_breakdown($this, (int)$cashbox_session_id, $amountOpen);
        $session["sales_by_payment"] = cash_ops_session_sales_by_payment($this, (int)$cashbox_session_id);
        return $session;
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
     * Cash left in drawer after the previous closed session for this cashbox.
     * Used as opening float for the next shift when client sends amount_open=0.
     */
    private function previousClosingCash(int $cashboxId, ?int $beforeSessionId = null): float
    {
        if ($cashboxId <= 0) {
            return 0.0;
        }
        $sql = "SELECT f_id, f_amount_fact, f_amount_expected
                FROM cash_session
                WHERE f_cashbox_id = ? AND f_state = 2";
        $types = "i";
        $args = [$cashboxId];
        if ($beforeSessionId !== null && $beforeSessionId > 0) {
            $sql .= " AND f_id < ?";
            $types .= "i";
            $args[] = $beforeSessionId;
        }
        $sql .= " ORDER BY f_id DESC LIMIT 1";
        $prev = $this->select($sql, $types, $args)->fetch_assoc();
        if (!$prev) {
            return 0.0;
        }
        $fact = (float)($prev["f_amount_fact"] ?? 0);
        $expected = (float)($prev["f_amount_expected"] ?? 0);
        if (abs($fact) >= 0.0001) {
            return $fact;
        }
        // Counted empty drawer inserts shortage/overage; uncounted legacy close left fact=0.
        $reconcile = $this->select(
            "SELECT 1 AS x FROM cash_operations
             WHERE f_session_id = ? AND f_operation_type IN (?, ?) LIMIT 1",
            "iii",
            [(int)$prev["f_id"], CASH_OP_CASH_SHORTAGE, CASH_OP_CASH_OVERAGE]
        )->fetch_assoc();
        if ($reconcile) {
            return 0.0;
        }
        return $expected;
    }

    /**
     * Opening float for a session: explicit amount_open, else carry previous close cash.
     * For sessions already opened with 0, still recover previous leftover at recount/close.
     */
    private function resolveSessionAmountOpen(array $rawSession): float
    {
        $amountOpen = (float)($rawSession["f_amount_open"] ?? 0);
        if (abs($amountOpen) >= 0.0001 || !empty($rawSession["f_open_counted"])) {
            return $amountOpen;
        }
        return $this->previousClosingCash(
            (int)($rawSession["f_cashbox_id"] ?? 0),
            (int)($rawSession["f_id"] ?? 0)
        );
    }

    /**
     * Returns open cash_session row (f_id, f_cashbox_id) or creates one.
     * amount_open=0 → carry cash left from previous closed session.
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
        if (abs($amount_open) < 0.0001) {
            $amount_open = $this->previousClosingCash($cashbox_id);
        }
        $this->insert("cash_session", [
            "f_state" => 1,
            "f_cashbox_id" => $cashbox_id,
            "f_user_open" => $user_id,
            "f_date_open" => date("Y-m-d H:i:s"),
            "f_amount_open" => $amount_open,
            "f_amount_fact" => 0,
            "f_amount_expected" => $amount_open,
            "f_amount_difference" => 0,
        ]);

        return $this->GetOpenedCashboxSessionId($cashbox_id);
    }

    /**
     * One open session per calendar day for a cashbox.
     * No session → open; open but f_date_open before today → close (no cash count) and open new.
     *
     * @return array{f_id:int|string,f_cashbox_id:int}|null
     */
    public function ensureDailyOpenSession(int $cashbox_id, int $user_id, float $amount_open = 0): ?array
    {
        if ($cashbox_id <= 0) {
            return null;
        }
        $open = $this->GetOpenedCashboxSessionId($cashbox_id);
        if ($open) {
            $raw = $this->GetRawCashboxSession((int)$open["f_id"]);
            $openedAt = $raw["f_date_open"] ?? null;
            $openDay = $openedAt ? date("Y-m-d", strtotime((string)$openedAt)) : null;
            $today = date("Y-m-d");
            if ($openDay !== null && $openDay < $today) {
                $this->closeSessionInternal((object)[
                    "cashbox_id" => $cashbox_id,
                    "cash_counted" => false,
                    "amount_fact" => 0,
                    "currency_id" => $this->resolveCurrencyId((object)[], $cashbox_id),
                ], $user_id);
                $open = null;
            }
        }
        if (!$open) {
            return $this->ensureOpenSession($cashbox_id, $user_id, $amount_open);
        }
        return $open;
    }

    /**
     * Close open session without HTTP echo. Same rules as Close().
     */
    public function closeSessionInternal(object $params, ?int $user_id = null): array
    {
        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        $userId = $user_id ?? (int)$this->userid;
        $cashboxId = (int)($params->cashbox_id ?? 0);
        $cash_session = $this->GetOpenedCashboxSessionId($cashboxId);
        if (!$cash_session) {
            dieWithCode(Translator::t("No active session"));
        }
        $cashbox = $this->GetRawCashboxSession($cash_session["f_id"]);
        if (!$cashbox) {
            dieWithCode("Do you want to hack close function of cashbox?");
        }
        $cashCounted = !empty($params->cash_counted);
        $amountFact = (float)($params->amount_cash ?? ($params->amount_fact ?? 0));
        $currencyId = $this->resolveCurrencyId($params, $cashboxId);
        // Always resolve opening float (explicit f_amount_open, else previous close cash).
        $amountOpen = (float)$this->resolveSessionAmountOpen(is_array($cashbox) ? $cashbox : []);
        // Persist recovered opening float on sessions opened with 0 before this fix.
        if (abs((float)($cashbox["f_amount_open"] ?? 0)) < 0.0001 && abs($amountOpen) >= 0.0001) {
            $cashbox["f_amount_open"] = $amountOpen;
        }
        $expectedCash = cash_ops_session_expected_cash($this, (int)$cash_session["f_id"], $amountOpen);
        $diff = $amountFact - $expectedCash;

        if ($cashCounted && abs($diff) > 0.0001) {
            $isSurplus = $diff > 0;
            cash_ops_insert($this, [
                "f_cashbox_id" => $cash_session["f_cashbox_id"],
                "f_session_id" => $cash_session["f_id"],
                "f_order_id" => "",
                "f_user" => $userId,
                "f_operation_type" => $isSurplus ? CASH_OP_CASH_OVERAGE : CASH_OP_CASH_SHORTAGE,
                "f_datetime" => date("Y-m-d H:i:s"),
                "f_payment_type_id" => PAYMENT_TYPE_CASH,
                "f_affects_float" => 0,
                "f_debit" => $isSurplus ? abs($diff) : 0,
                "f_credit" => $isSurplus ? 0 : abs($diff),
                "f_currency_id" => $currencyId,
                "f_comment" => ($isSurplus ? Translator::t("Cashbox close surplus") : Translator::t("Cashbox close shortage"))
                    . ": " . number_format(abs($diff), 2, ".", ""),
            ]);
            $shiftUserId = (int)($cashbox["f_user_open"] ?? 0);
            cash_discrepancy_log(
                $this,
                CASH_DISCREPANCY_STAGE_CLOSE,
                $diff,
                (int)$cash_session["f_cashbox_id"],
                (int)$cash_session["f_id"],
                $shiftUserId > 0 ? $shiftUserId : $userId,
                $userId,
                $currencyId,
                date("Y-m-d H:i:s")
            );
        }
        // No cash count → cash stays in drawer; next shift opens with this amount.
        if (!$cashCounted) {
            $amountFact = $expectedCash;
            $diff = 0.0;
        }
        $cashbox["f_state"] = 2;
        $cashbox["f_date_close"] = date("Y-m-d H:i:s");
        $cashbox["f_user_close"] = $userId;
        $cashbox["f_amount_fact"] = $amountFact;
        $cashbox["f_amount_expected"] = $expectedCash;
        $cashbox["f_amount_difference"] = $diff;
        $cashbox["f_cash_counted"] = $cashCounted ? 1 : 0;
        $this->update("cash_session", $cashbox, $cashbox["f_id"]);
        $session = $this->GetCashboxSession($cash_session["f_id"]);
        return $session;
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
        require_once __DIR__ . "/../worker/cash-ops-helper.php";
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
        $v["f_currency_id"] = $this->resolveCurrencyId($params, (int)$params->cashbox_id);
        $v["f_comment"]  = $params->f_comment;
        cash_ops_insert($this, $v);
        $pt = (int)$v["f_payment_type_id"];
        if (cash_ops_affects_float($pt)) {
            $delta = (float)$v["f_debit"] - (float)$v["f_credit"];
            cash_ops_adjust_session_float($this, (int)$cash_session["f_id"], $delta);
        }
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
        $cashboxId = (int)($params->f_cashbox_id ?? $params->cashbox_id ?? 0);
        if ($cashboxId > 0) {
            $v["f_cashbox_id"] = $cashboxId;
        }
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
        $v["f_currency_id"] = $this->resolveCurrencyId($params);
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
        if (!empty($params->cash_counted)) {
            $this->result["open_reconcile"] = $this->openCountedSession(
                $cashbox_id,
                (int)$this->userid,
                round((float)($params->amount_open ?? 0), 2),
                $params
            );
        } else {
            $this->ensureOpenSession($cashbox_id, (int)$this->userid, (float)($params->amount_open ?? 0));
        }
        $cashbox = $this->GetOpenCashboxSession($cashbox_id);
        $this->result["cashbox_session"] = $cashbox ?? [];
        $this->echoResult();
    }

    /**
     * Cash ops without a session (collection after close, office transfers) since the last close.
     */
    private function cashMovedOutsideSession(int $cashboxId): float
    {
        $last = $this->select(
            "SELECT MAX(f_date_close) AS f_date_close FROM cash_session WHERE f_cashbox_id = ? AND f_state = 2",
            "i",
            [$cashboxId]
        )->fetch_assoc();
        $since = (string)($last["f_date_close"] ?? "");
        if ($since === "") {
            return 0.0;
        }
        $row = $this->select(
            "SELECT COALESCE(SUM(f_debit), 0) - COALESCE(SUM(f_credit), 0) AS f_net
             FROM cash_operations
             WHERE f_cashbox_id = ? AND COALESCE(f_session_id, 0) = 0
               AND f_payment_type_id = ? AND f_datetime >= ?",
            "iis",
            [$cashboxId, PAYMENT_TYPE_CASH, $since]
        )->fetch_assoc();
        return (float)($row["f_net"] ?? 0);
    }

    /**
     * Open with a physical cash count.
     * The amount already in the drawer stays the opening balance.
     * The typed amount is compared with it; only the difference is recorded.
     *
     * @return array<string,mixed>
     */
    private function openCountedSession(int $cashboxId, int $userId, float $counted, object $params): array
    {
        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        if ($this->GetOpenedCashboxSessionId($cashboxId)) {
            dieWithCode(Translator::t("Cashbox session is already open"));
        }

        // Cash already in the drawer. The amount typed at open is a count of that cash,
        // not extra money added on top of it.
        $carry = round($this->previousClosingCash($cashboxId) + $this->cashMovedOutsideSession($cashboxId), 2);
        $diff = round($counted - $carry, 2);
        $now = date("Y-m-d H:i:s");

        $this->beginTransaction();
        $this->insert("cash_session", [
            "f_state" => 1,
            "f_cashbox_id" => $cashboxId,
            "f_user_open" => $userId,
            "f_date_open" => $now,
            "f_amount_open" => $carry,
            "f_open_counted" => 1,
            "f_amount_fact" => 0,
            "f_amount_expected" => $counted,
            "f_amount_difference" => 0,
        ]);
        $session = $this->GetOpenedCashboxSessionId($cashboxId);
        $sessionId = (int)($session["f_id"] ?? 0);

        if ($sessionId > 0 && abs($diff) > 0.009) {
            $isSurplus = $diff > 0;
            cash_ops_insert($this, [
                "f_cashbox_id" => $cashboxId,
                "f_session_id" => $sessionId,
                "f_order_id" => "",
                "f_user" => $userId,
                "f_operation_type" => $isSurplus ? CASH_OP_CASH_OVERAGE : CASH_OP_CASH_SHORTAGE,
                "f_datetime" => $now,
                "f_payment_type_id" => PAYMENT_TYPE_CASH,
                "f_affects_float" => 1,
                "f_debit" => $isSurplus ? abs($diff) : 0,
                "f_credit" => $isSurplus ? 0 : abs($diff),
                "f_currency_id" => $this->resolveCurrencyId($params, $cashboxId),
                "f_comment" => ($isSurplus ? Translator::t("Cashbox open surplus") : Translator::t("Cashbox open shortage"))
                    . ": " . number_format(abs($diff), 2, ".", ""),
            ]);
            // The drawer was left by the previous shift: it is responsible for the difference.
            $prevSession = $this->select(
                "SELECT f_user_open, f_user_close FROM cash_session
                 WHERE f_cashbox_id = ? AND f_state = 2 AND f_id < ?
                 ORDER BY f_id DESC LIMIT 1",
                "ii",
                [$cashboxId, $sessionId]
            )->fetch_assoc();
            $shiftUserId = (int)($prevSession["f_user_close"] ?? 0);
            if ($shiftUserId <= 0) {
                $shiftUserId = (int)($prevSession["f_user_open"] ?? 0);
            }
            cash_discrepancy_log(
                $this,
                CASH_DISCREPANCY_STAGE_OPEN,
                $diff,
                $cashboxId,
                $sessionId,
                $shiftUserId > 0 ? $shiftUserId : $userId,
                $userId,
                $this->resolveCurrencyId($params, $cashboxId),
                $now
            );
        }
        $this->commit();

        return [
            "f_amount_expected_cash" => money_fmt($carry),
            "f_amount_fact" => money_fmt($counted),
            "f_amount_difference" => money_fmt(abs($diff)),
            "f_amount_difference_raw" => $diff,
        ];
    }

    public function Close($params)
    {
        $session = $this->closeSessionInternal($params);
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
        require_once __DIR__ . "/../worker/cash-ops-helper.php";
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
            if ($pt <= 0 || !cash_ops_counts_as_funds($pt)) {
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
        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        $funds = [];

        $rows = $this->select(
            "SELECT f_payment_type_id, SUM(COALESCE(f_debit,0) - COALESCE(f_credit,0)) AS f_available
             FROM cash_operations WHERE f_session_id=? GROUP BY f_payment_type_id",
            "i",
            [$sessionId]
        )->fetch_all(MYSQLI_ASSOC);

        foreach ($rows as $r) {
            $pt = (int)$r["f_payment_type_id"];
            if ($pt <= 0 || !cash_ops_counts_as_funds($pt)) {
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

        $currencyId = $this->resolveCurrencyId($params, $cashboxId);

        $funds = $this->cashboxFundsByCashbox($cashboxId, $currencyId);

        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        $payment = require __DIR__ . "/../worker/dict-payment.php";
        $out = [];
        foreach ($payment["types"] as $pt) {
            if (!cash_ops_counts_as_funds((int)$pt)) {
                continue;
            }
            $available = round((float)($funds[$pt] ?? 0), 2);
            if ($available <= 0.00001) {
                continue;
            }
            $out[] = [
                "f_payment_type_id" => $pt,
                "f_name" => payment_type_name($this, (int)$pt),
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

        $currencyId = $this->resolveCurrencyId($params, $sourceId);
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

        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        foreach ($transfers as $t) {
            cash_ops_insert($this, [
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

            cash_ops_insert($this, [
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

    /** Restaurant config (workstation type 5): f_collection_cashbox_id, 0 when unset. */
    private function collectionCashboxId(): int
    {
        $row = $this->select(
            "SELECT f_config FROM workstations WHERE f_type = 5 ORDER BY f_id ASC LIMIT 1"
        )->fetch_assoc();
        $cfg = json_decode((string)($row["f_config"] ?? "{}"), true);
        return is_array($cfg) ? max(0, (int)($cfg["f_collection_cashbox_id"] ?? 0)) : 0;
    }

    /**
     * Cash collection (encashment): cash leaves the source cashbox.
     * With f_collection_cashbox_id set, the same amount arrives there as a transfer;
     * otherwise only the outgoing operation is written.
     */
    public function CashCollection($params)
    {
        require_once __DIR__ . "/../worker/cash-ops-helper.php";

        $sourceId = $this->resolveCashboxId($params);
        if ($sourceId <= 0) {
            dieWithCode(Translator::t("Source cashbox is required"));
        }
        $amount = round((float)($params->amount ?? 0), 2);
        if ($amount <= 0.009) {
            dieWithCode(Translator::t("Amount must be greater than zero"));
        }

        $destId = $this->collectionCashboxId();
        if ($destId === $sourceId) {
            dieWithCode(Translator::t("Source and destination must differ"));
        }
        if ($destId > 0) {
            $exists = $this->select("SELECT f_id FROM cash_box WHERE f_id=?", "i", [$destId])->fetch_assoc();
            if (!$exists) {
                dieWithCode(translate_template('Cash collection cashbox %cashbox not found', ['%cashbox' => (string)$destId]));
            }
        }

        $currencyId = $this->resolveCurrencyId($params, $sourceId);
        $openSession = $this->GetOpenedCashboxSessionId($sourceId);
        $sessionId = (int)($openSession["f_id"] ?? 0);

        $userComment = trim((string)($params->comment ?? ""));
        $outComment = $destId > 0
            ? translate_template('Cash collection to cashbox %cashbox', ['%cashbox' => (string)$destId])
            : Translator::t('Cash collection');
        $inComment = translate_template('Cash collection from cashbox %cashbox', ['%cashbox' => (string)$sourceId]);
        if ($userComment !== "") {
            $outComment .= ": " . $userComment;
            $inComment .= ": " . $userComment;
        }

        $now = date("Y-m-d H:i:s");
        $this->beginTransaction();

        cash_ops_insert($this, [
            "f_cashbox_id" => $sourceId,
            "f_session_id" => $sessionId,
            "f_order_id" => "",
            "f_user" => $this->userid,
            "f_operation_type" => CASH_OP_CASH_COLLECTION,
            "f_payment_type_id" => PAYMENT_TYPE_CASH,
            "f_datetime" => $now,
            "f_debit" => 0,
            "f_credit" => $amount,
            "f_currency_id" => $currencyId,
            "f_comment" => $outComment,
        ]);
        if ($sessionId > 0) {
            cash_ops_adjust_session_float($this, $sessionId, -$amount);
        }

        if ($destId > 0) {
            cash_ops_insert($this, [
                "f_cashbox_id" => $destId,
                "f_session_id" => 0,
                "f_order_id" => "",
                "f_user" => $this->userid,
                "f_operation_type" => CASH_OP_TRANSFER_IN,
                "f_payment_type_id" => PAYMENT_TYPE_CASH,
                "f_datetime" => $now,
                "f_debit" => $amount,
                "f_credit" => 0,
                "f_currency_id" => $currencyId,
                "f_comment" => $inComment,
            ]);
        }

        $this->commit();

        $this->result["dest_cashbox_id"] = $destId;
        $this->result["amount"] = $amount;
        $this->echoResult();
    }

    /**
     * Open balance for one partner: suppliers (doc_type=1) credit-debit; customers (2) debit-credit.
     */
    public function PartnerDebtBalance($params)
    {
        $partnerId = (int)($params->partner_id ?? 0);
        $docType = (int)($params->doc_type ?? 0);
        $currencyId = (int)($params->currency_id ?? 0);
        if ($partnerId <= 0) {
            dieWithCode(Translator::t("Partner is required"));
        }
        if ($docType !== 1 && $docType !== 2) {
            dieWithCode(Translator::t("Invalid debt type"));
        }
        if ($currencyId <= 0) {
            $currencyId = 1;
        }

        $balExpr = $docType === 2
            ? "COALESCE(SUM(d.f_debit), 0) - COALESCE(SUM(d.f_credit), 0)"
            : "COALESCE(SUM(d.f_credit), 0) - COALESCE(SUM(d.f_debit), 0)";
        $row = $this->select(
            "SELECT {$balExpr} AS bal
             FROM cash_debts d
             WHERE d.f_partner=? AND d.f_doc_type=? AND d.f_currency_id=?",
            "iii",
            [$partnerId, $docType, $currencyId]
        )->fetch_assoc();

        $this->result["balance"] = (float)($row["bal"] ?? 0);
        $this->result["partner_id"] = $partnerId;
        $this->result["doc_type"] = $docType;
        $this->result["currency_id"] = $currencyId;
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

        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        $allowedPts = [
            PAYMENT_TYPE_CASH,
            PAYMENT_TYPE_CARD,
            PAYMENT_TYPE_BANK,
            PAYMENT_TYPE_BANK2,
            PAYMENT_TYPE_BANK3,
            PAYMENT_TYPE_IDRAM,
            PAYMENT_TELCELL,
        ];
        if (!in_array($paymentTypeId, $allowedPts, true)) {
            dieWithCode(Translator::t("Invalid payment type"));
        }

        // Funds tenders (cash/card/idram/telcell) move through an open cashbox session.
        // Bank transfer only updates the debt ledger — no till required.
        $needsCashbox = cash_ops_counts_as_funds($paymentTypeId);
        if ($needsCashbox) {
            if ($cashboxId <= 0) {
                dieWithCode(Translator::t("Cashbox not selected"));
            }
        } else {
            $cashboxId = 0;
        }

        $uuid = uuid_v4();
        $opComment = $comment !== ""
            ? $comment
            : ($docType === 1
                ? Translator::t("Debt redeem supplier")
                : Translator::t("Debt redeem customer"));

        $debtRow = [
            "f_date" => $txnDate,
            "f_partner" => $partnerId,
            "f_doc_type" => $docType,
            "f_doc_uuid" => $uuid,
            "f_currency_id" => $currencyId,
            "f_comment" => $opComment,
        ];
        if ($docType === 1) {
            $debtRow["f_credit"] = 0;
            $debtRow["f_debit"] = $amount;
        } else {
            $debtRow["f_credit"] = $amount;
            $debtRow["f_debit"] = 0;
        }

        $this->beginTransaction();
        $debtId = (int)$this->insert("cash_debts", $debtRow);

        if ($needsCashbox) {
            $cashSession = $this->GetOpenedCashboxSessionId($cashboxId);
            if (!$cashSession) {
                $this->rollback();
                dieWithCode(Translator::t("No active session"));
            }

            if ($docType === 1) {
                cash_ops_insert($this, [
                    "f_cashbox_id" => (int)$cashSession["f_cashbox_id"],
                    "f_session_id" => (int)$cashSession["f_id"],
                    "f_order_id" => $uuid,
                    "f_user" => $this->userid,
                    "f_operation_type" => CASH_OP_DEBT_REPAYMENT,
                    "f_payment_type_id" => $paymentTypeId,
                    "f_datetime" => $opDateTime,
                    "f_debit" => 0,
                    "f_credit" => $amount,
                    "f_currency_id" => $currencyId,
                    "f_comment" => $opComment,
                ]);
                if (cash_ops_affects_float($paymentTypeId)) {
                    cash_ops_adjust_session_float($this, (int)$cashSession["f_id"], -$amount);
                }
            } else {
                cash_ops_insert($this, [
                    "f_cashbox_id" => (int)$cashSession["f_cashbox_id"],
                    "f_session_id" => (int)$cashSession["f_id"],
                    "f_order_id" => $uuid,
                    "f_user" => $this->userid,
                    "f_operation_type" => CASH_OP_DEBT_RECOVERY,
                    "f_payment_type_id" => $paymentTypeId,
                    "f_datetime" => $opDateTime,
                    "f_debit" => $amount,
                    "f_credit" => 0,
                    "f_currency_id" => $currencyId,
                    "f_comment" => $opComment,
                ]);
                if (cash_ops_affects_float($paymentTypeId)) {
                    cash_ops_adjust_session_float($this, (int)$cashSession["f_id"], $amount);
                }
            }
        }

        $this->commit();
        $this->result["debt_id"] = $debtId;
        $this->result["doc_uuid"] = $uuid;
        $this->echoResult();
    }

    /**
     * Load redeem (manual debt payment) for edit. Only rows not linked to store/order docs.
     */
    public function GetRedeemDebt($params)
    {
        $debtId = (int)($params->debt_id ?? 0);
        if ($debtId <= 0) {
            dieWithCode(Translator::t("id not specified"));
        }
        $debt = $this->loadDebtRow($debtId);
        if (!$debt) {
            die(Translator::t("Not found"));
        }
        $uuid = (string)$debt["f_doc_uuid"];
        if ($this->isLinkedDebtDocument($uuid)) {
            die(Translator::t("Only debt payments can be edited"));
        }

        $partner = $this->select(
            "SELECT f_id, TRIM(CONCAT(COALESCE(f_taxname,''), ' ', COALESCE(f_name,''))) AS f_name
             FROM c_partners WHERE f_id=?",
            "i",
            [(int)$debt["f_partner"]]
        )->fetch_assoc();

        $op = $this->findRedeemCashOperation($debt);
        // Heal legacy rows that were saved with empty f_order_id.
        if ($op && trim((string)($op["f_order_id"] ?? "")) === "") {
            $this->select(
                "UPDATE cash_operations SET f_order_id=? WHERE f_id=?",
                "si",
                [$uuid, (int)$op["f_id"]],
                true
            );
            $op["f_order_id"] = $uuid;
        }

        $cashboxName = "";
        if ($op && (int)$op["f_cashbox_id"] > 0) {
            $cb = $this->select("SELECT f_name FROM cash_box WHERE f_id=?", "i", [(int)$op["f_cashbox_id"]])->fetch_assoc();
            $cashboxName = (string)($cb["f_name"] ?? "");
        }

        $docType = (int)$debt["f_doc_type"];
        $amount = $docType === 1 ? (float)$debt["f_debit"] : (float)$debt["f_credit"];
        $comment = (string)($debt["f_comment"] ?? "");
        if ($comment === "" && $op) {
            $comment = (string)($op["f_comment"] ?? "");
        }
        $this->result["redeem"] = [
            "debt_id" => (int)$debt["f_id"],
            "doc_uuid" => $uuid,
            "doc_type" => $docType,
            "partner_id" => (int)$debt["f_partner"],
            "partner_name" => trim((string)($partner["f_name"] ?? "")),
            "currency_id" => (int)$debt["f_currency_id"],
            "date" => (string)$debt["f_date"],
            "amount" => $amount,
            "comment" => $comment,
            "cash_op_id" => $op ? (int)$op["f_id"] : 0,
            "cashbox_id" => $op ? (int)$op["f_cashbox_id"] : 0,
            "cashbox_name" => $cashboxName,
            "payment_type_id" => $op ? (int)$op["f_payment_type_id"] : PAYMENT_TYPE_BANK,
        ];
        $this->echoResult();
    }

    public function UpdateRedeemDebt($params)
    {
        $debtId = (int)($params->debt_id ?? 0);
        if ($debtId <= 0) {
            dieWithCode(Translator::t("id not specified"));
        }

        $amount = (float)($params->amount ?? 0);
        $paymentTypeId = (int)($params->payment_type_id ?? PAYMENT_TYPE_CASH);
        $comment = trim((string)($params->comment ?? ""));

        $dateStr = trim((string)($params->date ?? ""));
        if ($dateStr === "") {
            dieWithCode(Translator::t("Invalid date"));
        }
        $ts = strtotime($dateStr);
        if ($ts === false) {
            dieWithCode(Translator::t("Invalid date"));
        }
        $txnDate = date("Y-m-d", $ts);
        $opDateTime = $txnDate . " " . date("H:i:s");

        if ($amount <= 0.00001) {
            dieWithCode(Translator::t("Amount must be greater than zero"));
        }

        require_once __DIR__ . "/../worker/cash-ops-helper.php";
        $allowedPts = [
            PAYMENT_TYPE_CASH,
            PAYMENT_TYPE_CARD,
            PAYMENT_TYPE_BANK,
            PAYMENT_TYPE_BANK2,
            PAYMENT_TYPE_BANK3,
            PAYMENT_TYPE_IDRAM,
            PAYMENT_TELCELL,
        ];
        if (!in_array($paymentTypeId, $allowedPts, true)) {
            dieWithCode(Translator::t("Invalid payment type"));
        }

        $debt = $this->loadDebtRow($debtId);
        if (!$debt) {
            die(Translator::t("Not found"));
        }
        $uuid = (string)$debt["f_doc_uuid"];
        if ($this->isLinkedDebtDocument($uuid)) {
            die(Translator::t("Only debt payments can be edited"));
        }

        $docType = (int)$debt["f_doc_type"];
        $oldAmount = $docType === 1 ? (float)$debt["f_debit"] : (float)$debt["f_credit"];
        $opComment = $comment !== ""
            ? $comment
            : ($docType === 1
                ? Translator::t("Debt redeem supplier")
                : Translator::t("Debt redeem customer"));

        $debtUpdate = [
            "f_date" => $txnDate,
            "f_comment" => $opComment,
        ];
        if ($docType === 1) {
            $debtUpdate["f_credit"] = 0;
            $debtUpdate["f_debit"] = $amount;
        } else {
            $debtUpdate["f_credit"] = $amount;
            $debtUpdate["f_debit"] = 0;
        }

        $this->beginTransaction();
        $this->updateDebtRow($debtId, $debtUpdate);

        $op = $this->findRedeemCashOperation($debt);
        if ($op) {
            $opUpdate = [
                "f_order_id" => $uuid,
                "f_payment_type_id" => $paymentTypeId,
                "f_datetime" => $opDateTime,
                "f_comment" => $opComment,
            ];
            if ($docType === 1) {
                $opUpdate["f_debit"] = 0;
                $opUpdate["f_credit"] = $amount;
            } else {
                $opUpdate["f_debit"] = $amount;
                $opUpdate["f_credit"] = 0;
            }
            $this->update("cash_operations", $opUpdate, (int)$op["f_id"]);

            $delta = $amount - $oldAmount;
            require_once __DIR__ . "/../worker/cash-ops-helper.php";
            if (abs($delta) > 0.00001 && (int)$op["f_session_id"] > 0 && cash_ops_affects_float($paymentTypeId)) {
                if ($docType === 1) {
                    cash_ops_adjust_session_float($this, (int)$op["f_session_id"], -$delta);
                } else {
                    cash_ops_adjust_session_float($this, (int)$op["f_session_id"], $delta);
                }
            }
        }

        $this->commit();
        $this->echoResult();
    }

    public function DeleteRedeemDebt($params)
    {
        $debtId = (int)($params->debt_id ?? 0);
        if ($debtId <= 0) {
            dieWithCode(Translator::t("id not specified"));
        }

        $debt = $this->loadDebtRow($debtId);
        if (!$debt) {
            die(Translator::t("Not found"));
        }
        $uuid = (string)$debt["f_doc_uuid"];
        if ($this->isLinkedDebtDocument($uuid)) {
            die(Translator::t("Only debt payments can be deleted"));
        }

        $docType = (int)$debt["f_doc_type"];
        $amount = $docType === 1 ? (float)$debt["f_debit"] : (float)$debt["f_credit"];

        $this->beginTransaction();
        $op = $this->findRedeemCashOperation($debt);
        if ($op) {
            require_once __DIR__ . "/../worker/cash-ops-helper.php";
            $opPt = (int)($op["f_payment_type_id"] ?? PAYMENT_TYPE_CASH);
            if ((int)$op["f_session_id"] > 0 && $amount > 0.00001 && cash_ops_affects_float($opPt)) {
                if ($docType === 1) {
                    cash_ops_adjust_session_float($this, (int)$op["f_session_id"], $amount);
                } else {
                    cash_ops_adjust_session_float($this, (int)$op["f_session_id"], -$amount);
                }
            }
            $this->select("DELETE FROM cash_operations WHERE f_id=?", "i", [(int)$op["f_id"]], true);
            $this->select("DELETE FROM cash_operations WHERE f_order_id=?", "s", [$uuid], true);
        }
        $this->select("DELETE FROM cash_debts WHERE f_id=?", "i", [$debtId], true);
        $this->commit();
        $this->echoResult();
    }

    private function loadDebtRow(int $debtId): ?array
    {
        $row = $this->select(
            "SELECT f_id, f_date, f_partner, f_doc_type, f_doc_uuid, f_credit, f_debit, f_currency_id,
                    COALESCE(f_comment, '') AS f_comment
             FROM cash_debts WHERE f_id=?",
            "i",
            [$debtId]
        )->fetch_assoc();
        return $row ?: null;
    }

    private function updateDebtRow(int $debtId, array $fields): void
    {
        $this->update("cash_debts", $fields, $debtId);
    }

    /**
     * @param array<string,mixed> $debt
     * @return array<string,mixed>|null
     */
    private function findRedeemCashOperation(array $debt): ?array
    {
        $uuid = (string)($debt["f_doc_uuid"] ?? "");
        if ($uuid !== "") {
            $op = $this->select(
                "SELECT f_id, f_cashbox_id, f_session_id, f_order_id, f_payment_type_id, f_datetime, f_debit, f_credit, f_comment
                 FROM cash_operations WHERE f_order_id=? LIMIT 1",
                "s",
                [$uuid]
            )->fetch_assoc();
            if ($op) {
                return $op;
            }
        }

        $docType = (int)($debt["f_doc_type"] ?? 0);
        $amount = $docType === 1 ? (float)$debt["f_debit"] : (float)$debt["f_credit"];
        $date = (string)($debt["f_date"] ?? "");
        if ($amount <= 0.00001 || $date === "") {
            return null;
        }
        $opType = $docType === 1 ? CASH_OP_DEBT_REPAYMENT : CASH_OP_DEBT_RECOVERY;
        if ($docType === 1) {
            $op = $this->select(
                "SELECT f_id, f_cashbox_id, f_session_id, f_order_id, f_payment_type_id, f_datetime, f_debit, f_credit, f_comment
                 FROM cash_operations
                 WHERE (f_order_id IS NULL OR f_order_id='')
                   AND f_operation_type=?
                   AND DATE(f_datetime)=?
                   AND ABS(f_credit-?) < 0.009
                 ORDER BY f_id DESC
                 LIMIT 1",
                "isd",
                [$opType, $date, $amount]
            )->fetch_assoc();
        } else {
            $op = $this->select(
                "SELECT f_id, f_cashbox_id, f_session_id, f_order_id, f_payment_type_id, f_datetime, f_debit, f_credit, f_comment
                 FROM cash_operations
                 WHERE (f_order_id IS NULL OR f_order_id='')
                   AND f_operation_type=?
                   AND DATE(f_datetime)=?
                   AND ABS(f_debit-?) < 0.009
                 ORDER BY f_id DESC
                 LIMIT 1",
                "isd",
                [$opType, $date, $amount]
            )->fetch_assoc();
        }
        return $op ?: null;
    }

    private function isLinkedDebtDocument(string $uuid): bool
    {
        if ($uuid === "") {
            return true;
        }
        $store = $this->select("SELECT f_id FROM store_document WHERE f_id=? LIMIT 1", "s", [$uuid])->fetch_assoc();
        if ($store) {
            return true;
        }
        $order = $this->select("SELECT f_id FROM o_header WHERE f_id=? LIMIT 1", "s", [$uuid])->fetch_assoc();
        return (bool)$order;
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
