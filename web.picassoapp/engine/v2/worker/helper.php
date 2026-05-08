<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-04-25 00:26:28

function money_fmt_php($val)
{
    // FORMAT(val, 2) в MySQL по умолчанию использует запятую как разделитель тысяч
    $formatted = number_format((float)$val, 2, '.', ',');

    // Аналог TRIM(TRAILING '.00' FROM ...)
    return str_replace('.00', '', $formatted);
}
