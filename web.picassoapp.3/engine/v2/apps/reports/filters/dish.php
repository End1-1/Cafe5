<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-04-07 01:02:50
 * Last Modified: 2025-04-07 01:02:55
 * License: MIT
 * Description: Values for filter of dishes names
 */
require_once __DIR__ . "/filters.php";

$cols=["X", tr("Code"), tr("Name")];
$sql =<<<EOD
    select '0' as x, f_id, f_name 
    from d_dish 
    order by 3
EOD;

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);