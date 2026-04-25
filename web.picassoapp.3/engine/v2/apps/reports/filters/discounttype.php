<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-10-22 11:15:55
 * Last Modified: 2025-10-22 11:15:59
 * License: MIT
 * Description: Values for filter of dishes names
 */
require_once __DIR__ . "/filters.php";

$cols=["X", tr("Code"), tr("Name")];
$sql =<<<EOD
    select '0' as x, f_id, f_name 
    from b_card_types 
    order by 3
EOD;

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);