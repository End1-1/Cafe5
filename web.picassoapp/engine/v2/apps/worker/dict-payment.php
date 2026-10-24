<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-18 16:58:17
# Last Modified: 2026-02-02 12:11:52

if (!defined("PAYMENT_TYPE_CASH")) {
    define("PAYMENT_TYPE_CASH", 1);
    define("PAYMENT_TYPE_CARD", 2);
    define("PAYMENT_TYPE_BANK", 3);
    define("PAYMENT_TYPE_IDRAM", 4);
    define("PAYMENT_TYPE_COMPLIMENTARY", 5);
    define("PAYMENT_OTHER", 6);
    define("PAYMENT_TELCELL", 7);
    define("PAYMENT_DEBT", 8);
    define("PAYMENT_PREPAID", 9);
    define("PAYMENT_TYPE_BANK2", 10);
    define("PAYMENT_TYPE_BANK3", 11);
}

return [
    "types" => [
        PAYMENT_TYPE_CASH,
        PAYMENT_TYPE_CARD,
        PAYMENT_TYPE_BANK,
        PAYMENT_TYPE_IDRAM,
        PAYMENT_TYPE_COMPLIMENTARY,
        PAYMENT_OTHER,
        PAYMENT_TELCELL,
        PAYMENT_DEBT,
        PAYMENT_PREPAID
    ],

    "fields" => [
        PAYMENT_TYPE_CASH => "f_amount_cash",
        PAYMENT_TYPE_CARD => "f_amount_card",
        PAYMENT_TYPE_BANK => "f_amount_bank",
        PAYMENT_TYPE_IDRAM => "f_amount_idram",
        PAYMENT_TYPE_COMPLIMENTARY => "f_amount_complimentary",
        PAYMENT_OTHER => "f_amount_other",
        PAYMENT_TELCELL => "f_amount_telcell",
        PAYMENT_DEBT => "f_amount_debt",
        PAYMENT_PREPAID => "f_amount_prepaid"
    ],

    "names" => [
        PAYMENT_TYPE_CASH => "Cash",
        PAYMENT_TYPE_CARD => "Card",
        PAYMENT_TYPE_BANK => "Bank",
        PAYMENT_TYPE_IDRAM => "Idram",
        PAYMENT_TYPE_COMPLIMENTARY => "Complimentary",
        PAYMENT_OTHER => "Other",
        PAYMENT_TELCELL => "Telcell",
        PAYMENT_DEBT => "Debt",
        PAYMENT_PREPAID => "Prepaid",
        PAYMENT_TYPE_BANK2 => "Bank 2",
        PAYMENT_TYPE_BANK3 => "Bank 3",
    ],

    // Payment types listed in the payment types editor (display name editable, code fixed).
    // Bank 2 / Bank 3 are for purchases, debts and cash operations; sales keep "types" above.
    "editable" => [
        PAYMENT_TYPE_CASH,
        PAYMENT_TYPE_CARD,
        PAYMENT_TYPE_BANK,
        PAYMENT_TYPE_BANK2,
        PAYMENT_TYPE_BANK3,
        PAYMENT_TYPE_IDRAM,
        PAYMENT_TELCELL,
        PAYMENT_OTHER,
        PAYMENT_TYPE_COMPLIMENTARY,
        PAYMENT_DEBT,
        PAYMENT_PREPAID,
    ],

    "cashbox" => [
        PAYMENT_TYPE_CASH => true,
        PAYMENT_TYPE_CARD => true,
        // Bank/debt also go to cash_debts for AR; still record on the shift for reports.
        PAYMENT_TYPE_BANK => true,
        PAYMENT_TYPE_IDRAM => true,
        PAYMENT_TYPE_COMPLIMENTARY => true,
        PAYMENT_OTHER => true,
        PAYMENT_TELCELL => true,
        PAYMENT_DEBT => true,
        PAYMENT_PREPAID => true,
        PAYMENT_TYPE_BANK2 => true,
        PAYMENT_TYPE_BANK3 => true,
    ],

    // Transferable / drawable funds between cashboxes (real money only).
    "funds" => [
        PAYMENT_TYPE_CASH => true,
        PAYMENT_TYPE_CARD => true,
        PAYMENT_TYPE_IDRAM => true,
        PAYMENT_TELCELL => true,
    ],

    // Physical drawer float: only cash. Card/Idram/Telcell are shift turnover.
    "affects_float" => [
        PAYMENT_TYPE_CASH => true,
        PAYMENT_TYPE_CARD => false,
        PAYMENT_TYPE_BANK => false,
        PAYMENT_TYPE_IDRAM => false,
        PAYMENT_TYPE_COMPLIMENTARY => false,
        PAYMENT_OTHER => false,
        PAYMENT_TELCELL => false,
        PAYMENT_DEBT => false,
        PAYMENT_PREPAID => false,
        PAYMENT_TYPE_BANK2 => false,
        PAYMENT_TYPE_BANK3 => false,
    ],
];
