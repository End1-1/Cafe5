<?php
/**
 * Fix closed orders from the last N cash sessions where tendered cash
 * (e.g. 10000 note) was stored as f_amount_cash instead of net sale (4000)
 * with f_amount_change = 6000.
 *
 * Logic matches Waiter closeOrderInternal / cash_ops_order_payment_debits:
 * change is taken from cash first, then other funds; payment fields become net;
 * original cash tender is kept in f_amount_cash_tendered.
 *
 * Usage (browser or CLI):
 *   dry-run (default):  .../engine/db/fix_order_change_overpay.php
 *   apply:              .../engine/db/fix_order_change_overpay.php?apply=1
 *   sessions:           ...?sessions=2   (default 2)
 *
 * CLI:
 *   php fix_order_change_overpay.php
 *   php fix_order_change_overpay.php --apply
 *   php fix_order_change_overpay.php --apply --sessions=2
 */
define('APP', true);

require_once __DIR__ . '/../config.php';
require_once __DIR__ . '/../functions/db.php';
require_once __DIR__ . '/../v2/apps/worker/cash-ops-helper.php';

$isCli = (PHP_SAPI === 'cli');
$apply = false;
$sessions = 2;

if ($isCli) {
    foreach ($argv as $arg) {
        if ($arg === '--apply') {
            $apply = true;
        }
        if (preg_match('/^--sessions=(\d+)$/', $arg, $m)) {
            $sessions = max(1, (int)$m[1]);
        }
    }
} else {
    header('Content-Type: text/plain; charset=utf-8');
    $apply = isset($_GET['apply']) && (string)$_GET['apply'] === '1';
    if (isset($_GET['sessions'])) {
        $sessions = max(1, (int)$_GET['sessions']);
    }
}

$payment = require __DIR__ . '/../v2/apps/worker/dict-payment.php';
$cashField = $payment['fields'][PAYMENT_TYPE_CASH];

$sessionIds = [];
$rs = stmtall(
    "SELECT f_id FROM cash_session ORDER BY f_id DESC LIMIT ?",
    'i',
    [$sessions]
);
while ($row = $rs->fetch_assoc()) {
    $sessionIds[] = (int)$row['f_id'];
}

if (!$sessionIds) {
    echo "No cash sessions found.\n";
    exit(0);
}

$inList = implode(',', $sessionIds);
echo "Sessions: {$inList}\n";
echo $apply ? "MODE: APPLY\n" : "MODE: DRY-RUN (add ?apply=1 or --apply to write)\n";
echo str_repeat('-', 60) . "\n";

$orders = stmtall(
    "SELECT f_id, f_prefix, f_amounttotal, f_cash_session_id, f_data
     FROM o_header
     WHERE f_state = 2
       AND f_cash_session_id IN ($inList)
     ORDER BY f_cash_session_id DESC, f_id"
)->fetch_all(MYSQLI_ASSOC);

$checked = 0;
$fixed = 0;
$skipped = 0;

foreach ($orders as $order) {
    $checked++;
    $odata = json_decode($order['f_data'] ?? '{}', true);
    if (!is_array($odata)) {
        $odata = [];
    }

    $totalDue = (float)$order['f_amounttotal'];
    $tenderedSum = 0.0;
    foreach ($payment['types'] as $pt) {
        $pn = $payment['fields'][$pt] ?? null;
        if ($pn === null) {
            continue;
        }
        $tenderedSum += (float)($odata[$pn] ?? 0);
    }

    $change = (float)($odata['f_amount_change'] ?? 0);
    if ($change < 0.00001 && $tenderedSum > $totalDue + 0.01) {
        $change = $tenderedSum - $totalDue;
        $odata['f_amount_change'] = $change;
    }

    if ($change < 0.01) {
        $skipped++;
        continue;
    }

    // Snapshot tendered before netting
    $before = [];
    foreach ($payment['types'] as $pt) {
        $pn = $payment['fields'][$pt];
        $before[$pn] = round((float)($odata[$pn] ?? 0), 2);
    }

    $odataForDebit = $odata;
    $odataForDebit['f_amount_change'] = $change;
    $debits = cash_ops_order_payment_debits($odataForDebit);

    $cashTendered = $before[$cashField] ?? 0.0;
    $needsFix = false;
    $after = [];
    $paidNet = 0.0;
    foreach ($payment['types'] as $pt) {
        $pn = $payment['fields'][$pt];
        $net = round((float)($debits[(int)$pt] ?? 0), 2);
        if ($net < 0) {
            $net = 0.0;
        }
        $after[$pn] = $net;
        $paidNet += $net;
        if (abs(($before[$pn] ?? 0) - $net) > 0.009) {
            $needsFix = true;
        }
    }

    if (!$needsFix) {
        $skipped++;
        continue;
    }

    $newData = $odata;
    $newData['f_amount_change'] = round($change, 2);
    if ($cashTendered > 0.01) {
        $newData['f_amount_cash_tendered'] = round($cashTendered, 2);
    }
    foreach ($after as $pn => $net) {
        $newData[$pn] = $net;
    }
    $newData['f_amount_paid'] = round($paidNet, 2);

    $prefix = $order['f_prefix'] ?? '';
    $sid = (int)$order['f_cash_session_id'];
    echo sprintf(
        "order %s (%s) session=%d due=%.2f change=%.2f cash %.2f → %.2f paid→%.2f\n",
        $order['f_id'],
        $prefix,
        $sid,
        $totalDue,
        $change,
        $before[$cashField] ?? 0,
        $after[$cashField] ?? 0,
        $paidNet
    );

    if ($apply) {
        $json = json_encode($newData, JSON_UNESCAPED_UNICODE);
        if ($json === false) {
            echo "  ERROR: json_encode failed\n";
            continue;
        }
        stmtall(
            "UPDATE o_header SET f_data = ? WHERE f_id = ?",
            'ss',
            [$json, $order['f_id']]
        );
        // cash_operations were already posted as net on close — do not rewrite them.
    }
    $fixed++;
}

echo str_repeat('-', 60) . "\n";
echo "Checked: {$checked}, to fix / fixed: {$fixed}, skipped (no change / already net): {$skipped}\n";
if (!$apply && $fixed > 0) {
    echo "Dry-run only. Re-run with ?apply=1 or --apply to write.\n";
}
