<?php
# © 2026 , Kudryashov Vasili
# Shift + Tender helpers for cash_session / cash_operations (not e_cash).

require_once __DIR__ . "/dict-payment.php";
require_once __DIR__ . "/dict-cash-operation-type.php";
require_once __DIR__ . "/../../worker/fmt.php";

/**
 * Display name of a payment type: custom cash_payment_types.f_name, else the translated default.
 *
 * @param object $db Auth-like with select()
 */
function payment_type_name(object $db, int $paymentTypeId): string
{
    static $custom = null;
    if ($custom === null) {
        $custom = [];
        try {
            $rows = $db->select(
                "SELECT f_id, f_name FROM cash_payment_types WHERE COALESCE(f_name, '') <> ''"
            )->fetch_all(MYSQLI_ASSOC);
            foreach ($rows as $r) {
                $custom[(int)$r["f_id"]] = trim((string)$r["f_name"]);
            }
        } catch (Throwable $e) {
            $custom = [];
        }
    }
    if (($custom[$paymentTypeId] ?? '') !== '') {
        return $custom[$paymentTypeId];
    }
    $payment = require __DIR__ . "/dict-payment.php";
    return Translator::t($payment["names"][$paymentTypeId] ?? ("PT" . $paymentTypeId));
}

/**
 * Payment types that create cash_operations rows (turnover on the shift).
 */
function cash_ops_records_payment(int $paymentTypeId): bool
{
    $payment = require __DIR__ . "/dict-payment.php";
    return !empty($payment["cashbox"][$paymentTypeId]);
}

/**
 * Payment types that count as transferable cashbox funds (real money).
 */
function cash_ops_counts_as_funds(int $paymentTypeId): bool
{
    $payment = require __DIR__ . "/dict-payment.php";
    if (isset($payment["funds"][$paymentTypeId])) {
        return (bool)$payment["funds"][$paymentTypeId];
    }
    // Legacy fallback: cashbox + float/card-like before funds map existed.
    return !empty($payment["cashbox"][$paymentTypeId]) && cash_ops_affects_float($paymentTypeId);
}

/**
 * Only cash tender moves the physical drawer float (f_amount_expected).
 */
function cash_ops_affects_float(int $paymentTypeId): bool
{
    $payment = require __DIR__ . "/dict-payment.php";
    if (isset($payment["affects_float"][$paymentTypeId])) {
        return (bool)$payment["affects_float"][$paymentTypeId];
    }
    return $paymentTypeId === PAYMENT_TYPE_CASH;
}

/**
 * Insert one cash_operations row. Sets f_affects_float from payment type.
 *
 * @param object $db Auth-like with insert()
 * @param array<string,mixed> $row
 */
function cash_ops_insert(object $db, array $row): void
{
    $pt = (int)($row["f_payment_type_id"] ?? 0);
    if (!array_key_exists("f_affects_float", $row)) {
        $row["f_affects_float"] = cash_ops_affects_float($pt) ? 1 : 0;
    }
    if (!isset($row["f_currency_id"]) || (int)$row["f_currency_id"] <= 0) {
        $currencyId = 1;
        $cashboxId = (int)($row["f_cashbox_id"] ?? 0);
        if ($cashboxId > 0 && method_exists($db, "select")) {
            $cb = $db->select(
                "SELECT f_currency_id FROM cash_box WHERE f_id=?",
                "i",
                [$cashboxId]
            )->fetch_assoc();
            $currencyId = (int)($cb["f_currency_id"] ?? 1);
            if ($currencyId <= 0) {
                $currencyId = 1;
            }
        }
        $row["f_currency_id"] = $currencyId;
    }
    if (!isset($row["f_debit"])) {
        $row["f_debit"] = 0;
    }
    if (!isset($row["f_credit"])) {
        $row["f_credit"] = 0;
    }
    $db->insert("cash_operations", $row);
}

/**
 * Adjust cash_session.f_amount_expected by cash-float delta only.
 * Positive delta = more expected cash in drawer.
 *
 * @param object $db Auth-like with select()
 */
function cash_ops_adjust_session_float(object $db, int $sessionId, float $delta): void
{
    if ($sessionId <= 0 || abs($delta) < 0.00001) {
        return;
    }
    $db->select(
        "UPDATE cash_session SET f_amount_expected = f_amount_expected + ? WHERE f_id = ?",
        "di",
        [$delta, $sessionId],
        true
    );
}

/**
 * Write sale revenue lines for each tender; bump session float only for cash.
 *
 * @param object $db
 * @param array<int,float> $debitsByPaymentType payment_type_id => debit amount
 * @return float cash float delta applied to session
 */
function cash_ops_post_sale_payments(
    object $db,
    int $cashboxId,
    int $sessionId,
    string $orderId,
    int $userId,
    array $debitsByPaymentType,
    string $comment = "",
    int $currencyId = 1,
    int $operationType = CASH_OP_SALES_REVENUE
): float {
    $floatDelta = 0.0;
    foreach ($debitsByPaymentType as $pt => $debit) {
        $pt = (int)$pt;
        $debit = (float)$debit;
        if ($debit < 0.00001 || !cash_ops_records_payment($pt)) {
            continue;
        }
        $affects = cash_ops_affects_float($pt);
        cash_ops_insert($db, [
            "f_cashbox_id" => $cashboxId,
            "f_session_id" => $sessionId,
            "f_order_id" => $orderId,
            "f_user" => $userId,
            "f_operation_type" => $operationType,
            "f_payment_type_id" => $pt,
            "f_datetime" => date("Y-m-d H:i:s"),
            "f_debit" => $debit,
            "f_credit" => 0,
            "f_currency_id" => $currencyId,
            "f_comment" => $comment,
            "f_affects_float" => $affects ? 1 : 0,
        ]);
        if ($affects) {
            $floatDelta += $debit;
        }
    }
    cash_ops_adjust_session_float($db, $sessionId, $floatDelta);
    return $floatDelta;
}

/**
 * Reverse float impact of an order's ops that affect the drawer, then delete those ops.
 * Call before DELETE FROM cash_operations WHERE f_order_id=...
 *
 * @param object $db
 */
function cash_ops_reverse_order_float(object $db, string $orderId): void
{
    $sessionRows = $db->select(
        "SELECT f_session_id,
                COALESCE(SUM(CASE WHEN f_affects_float = 1 THEN f_debit ELSE 0 END), 0) AS debit_sum,
                COALESCE(SUM(CASE WHEN f_affects_float = 1 THEN f_credit ELSE 0 END), 0) AS credit_sum
         FROM cash_operations
         WHERE f_order_id = ? AND f_session_id > 0
         GROUP BY f_session_id",
        "s",
        [$orderId]
    )->fetch_all(MYSQLI_ASSOC);

    foreach ($sessionRows as $sr) {
        $sid = (int)($sr["f_session_id"] ?? 0);
        if ($sid <= 0) {
            continue;
        }
        // Debits raised expected; credits lowered it — reverse both.
        $delta = (float)$sr["credit_sum"] - (float)$sr["debit_sum"];
        cash_ops_adjust_session_float($db, $sid, $delta);
    }
}

/**
 * Tender totals for a session (all recorded payment types).
 *
 * @param object $db
 * @return list<array{f_payment_type_id:int,f_name:string,f_debit:float,f_credit:float,f_net:float,f_affects_float:int}>
 */
function cash_ops_session_tender_breakdown(object $db, int $sessionId, float|int|string|null $amountOpen = 0.0): array
{
    $amountOpen = (float)($amountOpen ?? 0);
    $payment = require __DIR__ . "/dict-payment.php";
    $rows = $db->select(
        "SELECT f_payment_type_id,
                COALESCE(SUM(f_debit), 0) AS f_debit,
                COALESCE(SUM(f_credit), 0) AS f_credit,
                MAX(f_affects_float) AS f_affects_float
         FROM cash_operations
         WHERE f_session_id = ?
         GROUP BY f_payment_type_id
         ORDER BY f_payment_type_id",
        "i",
        [$sessionId]
    )->fetch_all(MYSQLI_ASSOC);

    $byPt = [];
    foreach ($rows as $r) {
        $pt = (int)$r["f_payment_type_id"];
        if ($pt <= 0 || !cash_ops_records_payment($pt)) {
            continue;
        }
        $debit = (float)$r["f_debit"];
        $credit = (float)$r["f_credit"];
        $byPt[$pt] = [
            "f_payment_type_id" => $pt,
            "f_name" => payment_type_name($db, $pt),
            "f_debit" => $debit,
            "f_credit" => $credit,
            "f_net" => $debit - $credit,
            "f_affects_float" => cash_ops_affects_float($pt) ? 1 : 0,
        ];
    }

    // Opening float counts as cash already in the drawer.
    if ($amountOpen > 0.00001 || isset($byPt[PAYMENT_TYPE_CASH])) {
        if (!isset($byPt[PAYMENT_TYPE_CASH])) {
            $byPt[PAYMENT_TYPE_CASH] = [
                "f_payment_type_id" => PAYMENT_TYPE_CASH,
                "f_name" => payment_type_name($db, PAYMENT_TYPE_CASH),
                "f_debit" => 0.0,
                "f_credit" => 0.0,
                "f_net" => 0.0,
                "f_affects_float" => 1,
            ];
        }
        $byPt[PAYMENT_TYPE_CASH]["f_net"] += $amountOpen;
        $byPt[PAYMENT_TYPE_CASH]["f_opening"] = $amountOpen;
    }

    ksort($byPt);
    return array_values($byPt);
}

/**
 * Net tender amounts for one order (payment fields minus change; change from cash first, then funds).
 *
 * @param array<string,mixed> $odata order f_data
 * @return array<int,float> payment_type_id => net amount
 */
function cash_ops_order_payment_debits(array $odata): array
{
    $payment = require __DIR__ . "/dict-payment.php";
    $debits = [];
    foreach ($payment["types"] as $pt) {
        $pn = $payment["fields"][$pt] ?? null;
        if ($pn === null) {
            continue;
        }
        $debits[(int)$pt] = (float)($odata[$pn] ?? 0);
    }

    $change = (float)($odata["f_amount_change"] ?? 0);
    if ($change <= 0.00001) {
        return $debits;
    }

    $cashPt = PAYMENT_TYPE_CASH;
    $cashTendered = $debits[$cashPt] ?? 0.0;
    $deduct = min($change, $cashTendered);
    $debits[$cashPt] = $cashTendered - $deduct;
    $change -= $deduct;

    if ($change <= 0.00001) {
        return $debits;
    }

    foreach ($payment["types"] as $pt) {
        $pt = (int)$pt;
        if (empty($payment["funds"][$pt]) || $pt === PAYMENT_TYPE_CASH) {
            continue;
        }
        $deduct = min($change, $debits[$pt] ?? 0.0);
        $debits[$pt] = ($debits[$pt] ?? 0.0) - $deduct;
        $change -= $deduct;
        if ($change <= 0.00001) {
            break;
        }
    }

    return $debits;
}

/**
 * Sales for a session by payment type — from closed orders of this shift.
 * Independent of cashbox/funds flags: whatever was punched on the order is reported.
 *
 * @return list<array{f_payment_type_id:int,f_name:string,f_debit:float,f_credit:float,f_net:float,f_net_fmt:string}>
 */
function cash_ops_session_sales_by_payment(object $db, int $sessionId): array
{
    $payment = require __DIR__ . "/dict-payment.php";
    $rows = $db->select(
        "SELECT f_data FROM o_header WHERE f_cash_session_id = ? AND f_state = 2",
        "i",
        [$sessionId]
    )->fetch_all(MYSQLI_ASSOC);

    $byPt = [];
    foreach ($rows as $row) {
        $odata = json_decode($row["f_data"] ?? "{}", true);
        if (!is_array($odata)) {
            $odata = [];
        }
        foreach (cash_ops_order_payment_debits($odata) as $pt => $amount) {
            $pt = (int)$pt;
            $amount = (float)$amount;
            if ($pt <= 0 || $amount < 0.00001) {
                continue;
            }
            if (!isset($byPt[$pt])) {
                $byPt[$pt] = [
                    "f_payment_type_id" => $pt,
                    "f_name" => payment_type_name($db, $pt),
                    "f_debit" => 0.0,
                    "f_credit" => 0.0,
                    "f_net" => 0.0,
                ];
            }
            $byPt[$pt]["f_debit"] += $amount;
            $byPt[$pt]["f_net"] += $amount;
        }
    }

    ksort($byPt);
    $out = [];
    foreach ($byPt as $row) {
        $row["f_net_fmt"] = money_fmt($row["f_net"]);
        $out[] = $row;
    }
    return $out;
}

/**
 * Total sales revenue for a session (sum of all payment types punched on closed orders).
 */
function cash_ops_session_sales_revenue(object $db, int $sessionId): float
{
    $sum = 0.0;
    foreach (cash_ops_session_sales_by_payment($db, $sessionId) as $row) {
        $sum += (float)($row["f_net"] ?? 0);
    }
    return $sum;
}

define("CASH_DISCREPANCY_SHORTAGE", 1);
define("CASH_DISCREPANCY_OVERAGE", 2);
define("CASH_DISCREPANCY_STAGE_OPEN", 1);
define("CASH_DISCREPANCY_STAGE_CLOSE", 2);

/**
 * Log a shortage/overage found on shift open or close into cash_discrepancy (to be resolved later).
 * The cashbox itself is corrected by the CASH_OP_CASH_SHORTAGE / CASH_OP_CASH_OVERAGE operation.
 * Skipped silently while the table does not exist yet (DB update not applied).
 *
 * @param object $db Auth-like with select()/insert()
 * @param float $diff counted - expected (negative = shortage)
 */
function cash_discrepancy_log(
    object $db,
    int $stage,
    float $diff,
    int $cashboxId,
    int $sessionId,
    int $shiftUserId,
    int $countedUserId,
    int $currencyId,
    string $datetime
): void {
    static $tableExists = null;
    if ($tableExists === null) {
        $tableExists = $db->select("SHOW TABLES LIKE 'cash_discrepancy'")->num_rows > 0;
    }
    if (!$tableExists || abs($diff) < 0.009) {
        return;
    }
    $db->insert("cash_discrepancy", [
        "f_date" => $datetime,
        "f_type" => $diff < 0 ? CASH_DISCREPANCY_SHORTAGE : CASH_DISCREPANCY_OVERAGE,
        "f_stage" => $stage,
        "f_amount" => round(abs($diff), 2),
        "f_currency_id" => $currencyId > 0 ? $currencyId : 1,
        "f_cashbox_id" => $cashboxId,
        "f_session_id" => $sessionId,
        "f_user" => $shiftUserId,
        "f_user_counted" => $countedUserId,
        "f_resolved" => 0,
    ]);
}

/**
 * Expected cash in drawer = opening balance + operations that move the drawer.
 * Close shortage/overage rows have f_affects_float = 0, so they stay out.
 * An opening count difference has f_affects_float = 1 and is included.
 */
function cash_ops_session_expected_cash(object $db, int $sessionId, float|int|string|null $amountOpen = 0): float
{
    $amountOpen = (float)($amountOpen ?? 0);
    $row = $db->select(
        "SELECT COALESCE(SUM(f_debit), 0) - COALESCE(SUM(f_credit), 0) AS f_net
         FROM cash_operations
         WHERE f_session_id = ? AND f_affects_float = 1",
        "i",
        [$sessionId]
    )->fetch_assoc();
    return $amountOpen + (float)($row["f_net"] ?? 0);
}
