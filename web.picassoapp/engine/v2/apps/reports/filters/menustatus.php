<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-11-23 16:03:13
 * Last Modified: 2025-11-23 16:03:17
 * License: MIT
 * Description: Values for filter of dishes status in menu
 */
require_once __DIR__ . "/filters.php";

$cols=["X", tr("Code"), tr("Name")];
$sql =<<<EOD
    select '0' as x, f_id, f_name 
    from d_dish_state 
    order by 3
EOD;

echo json_encode(["rows"=>getFilterResult($sql),"cols"=>$cols], JSON_UNESCAPED_UNICODE);