<?php
# © 2026 , Kudryashov Vasili
# cash_operations_types.f_id — see DB seed in run_update.php

if (!defined('CASH_OP_SALES_REVENUE')) {
    define('CASH_OP_SALES_REVENUE', 1);
    define('CASH_OP_TOTAL_EXPENSES', 2);
    define('CASH_OP_PURCHASING_COSTS', 3);
    define('CASH_OP_SALARIES', 4);
    define('CASH_OP_DEBT_RECOVERY', 5);
    define('CASH_OP_DEBT_REPAYMENT', 6);
    define('CASH_OP_UTILITIES', 7);
    define('CASH_OP_CASH_SHORTAGE', 8);
    define('CASH_OP_CASH_OVERAGE', 9);
}

/** @deprecated use CASH_OP_SALES_REVENUE */
if (!defined('CASHBOX_IN')) {
    define('CASHBOX_IN', CASH_OP_SALES_REVENUE);
}

return [
    'names' => [
        CASH_OP_SALES_REVENUE => 'Sales Revenue',
        CASH_OP_TOTAL_EXPENSES => 'Total Expenses',
        CASH_OP_PURCHASING_COSTS => 'Purchasing Costs',
        CASH_OP_SALARIES => 'Salaries and Wages',
        CASH_OP_DEBT_RECOVERY => 'Debt Recovery',
        CASH_OP_DEBT_REPAYMENT => 'Debt Repayment',
        CASH_OP_UTILITIES => 'Utilities',
        CASH_OP_CASH_SHORTAGE => 'Cash Shortage',
        CASH_OP_CASH_OVERAGE => 'Cash Overage',
    ],
];
