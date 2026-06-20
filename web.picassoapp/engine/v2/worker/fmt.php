<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-06-20 11:29:15

/**
 * PHP equivalent of MySQL money_fmt(val):
 * TRIM(TRAILING '.00' FROM FORMAT(val, 2))
 */
function money_fmt($val): string
{
    $formatted = number_format((float)$val, 2, '.', ',');

    if (substr($formatted, -3) === '.00') {
        return substr($formatted, 0, -3);
    }

    return $formatted;
}
