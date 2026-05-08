<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-11-24 12:43:37
 * Last Modified: 2025-11-24 12:43:40
 * License: MIT
 * Description: Values for filter of goods names
 */
require_once __DIR__ . "/filters.php";

$cols=["X", tr("Code"), tr("Name"), tr("Barcode")];
$sql =<<<EOD
    select '0' as x, f_id, f_name , f_scancode
    from c_goods 
    order by 3
EOD;

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);