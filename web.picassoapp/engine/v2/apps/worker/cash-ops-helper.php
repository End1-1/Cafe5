<?php
# © 2026 , Kudryashov Vasili
# Shift + Tender helpers for cash_session / cash_operations (not e_cash).

require_once __DIR__ . "/dict-payment.php";
require_once __DIR__ . "/dict-cash-operation-type.php";

/**
 * Payment types that create cash_operations rows (turnover on the shift).
 */
function cash_ops_records_payment(int $paymentTypeId): bool
{
    $payment = require __DIR__ . "/dict-payment.php";
    return !empty($payment["cashbox"][$paymentTypeId]);
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
    if (!isset($row["f_currency_id"])) {
        $row["f_currency_id"] = 1;
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
function cash_ops_session_tender_breakdown(object $db, int $sessionId, float $amountOpen = 0.0): array
{
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
            "f_name" => Translator::t($payment["names"][$pt] ?? ("PT" . $pt)),
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
                "f_name" => Translator::t($payment["names"][PAYMENT_TYPE_CASH] ?? "Cash"),
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
 * Expected cash in drawer for session = open + float ops net.
 */
function cash_ops_session_expected_cash(object $db, int $sessionId, float $amountOpen): float
{
    $row = $db->select(
        "SELECT COALESCE(SUM(f_debit), 0) - COALESCE(SUM(f_credit), 0) AS f_net
         FROM cash_operations
         WHERE f_session_id = ? AND f_affects_float = 1",
        "i",
        [$sessionId]
    )->fetch_assoc();
    return $amountOpen + (float)($row["f_net"] ?? 0);
}
