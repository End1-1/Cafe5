<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-18 16:58:17
# Last Modified: 2026-02-02 12:11:52

define("PAYMENT_TYPE_CASH", 1);
define("PAYMENT_TYPE_CARD", 2);
define("PAYMENT_TYPE_BANK", 3);
define("PAYMENT_TYPE_IDRAM", 4);
define("PAYMENT_TYPE_COMPLIMENTARY", 5);
define("PAYMENT_OTHER", 6);
define("PAYMENT_TELCELL", 7);
define("PAYMENT_DEBT", 8);
define("PAYMENT_PREPAID", 9);

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
        PAYMENT_PREPAID => "Prepaid"
    ],

    "cashbox" => [
        PAYMENT_TYPE_CASH => true,
        PAYMENT_TYPE_CARD => true,
        PAYMENT_TYPE_BANK => true,
        PAYMENT_TYPE_IDRAM => true,
        PAYMENT_TYPE_COMPLIMENTARY => false,
        PAYMENT_OTHER => false,
        PAYMENT_TELCELL => true,
        PAYMENT_DEBT => false,
        PAYMENT_PREPAID => false
    ]
];
