<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-11-23 16:03:13
 * Last Modified: 2026-06-06
 * License: MIT
 * Description: Values for filter of dishes status in menu
 */
require_once __DIR__ . "/filters.php";

$cols = ["X", tr("Code"), tr("Name")];
$sql = <<<EOD
    SELECT '0' AS x, 1 AS f_id, 'Active' AS f_name
    UNION ALL
    SELECT '0', 0, 'Inactive'
    ORDER BY 3
EOD;

echo json_encode(["rows" => getFilterResult($sql), "cols" => $cols], JSON_UNESCAPED_UNICODE);
