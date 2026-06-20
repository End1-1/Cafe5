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
    define('CASH_OP_DELIVERY_FEE', 10);
    define('CASH_OP_TRANSFER_OUT', 11);
    define('CASH_OP_TRANSFER_IN', 12);
}

/** @deprecated use CASH_OP_SALES_REVENUE */
if (!defined('CASHBOX_IN')) {
    define('CASHBOX_IN', CASH_OP_SALES_REVENUE);
}

return [
    'names' => [
        CASH_OP_SALES_REVENUE => Translator::t('Sales Revenue'),
        CASH_OP_TOTAL_EXPENSES => Translator::t('Total Expenses'),
        CASH_OP_PURCHASING_COSTS => Translator::t('Purchasing Costs'),
        CASH_OP_SALARIES => Translator::t('Salaries and Wages'),
        CASH_OP_DEBT_RECOVERY => Translator::t('Debt Recovery'),
        CASH_OP_DEBT_REPAYMENT => Translator::t('Debt Repayment'),
        CASH_OP_UTILITIES => Translator::t('Utilities'),
        CASH_OP_CASH_SHORTAGE => Translator::t('Cash Shortage'),
        CASH_OP_CASH_OVERAGE => Translator::t('Cash Overage'),
        CASH_OP_DELIVERY_FEE => Translator::t('Delivery Fee'),
        CASH_OP_TRANSFER_OUT => Translator::t('Transfer Out'),
        CASH_OP_TRANSFER_IN => Translator::t('Transfer In'),
    ],
];
