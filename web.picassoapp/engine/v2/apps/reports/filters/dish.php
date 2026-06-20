<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-04-07 01:02:50
 * Last Modified: 2026-06-06
 * License: MIT
 * Description: Values for filter of dishes names
 */
require_once __DIR__ . "/filters.php";

$cols = ["X", tr("Code"), tr("Name")];
$sql = <<<EOD
    SELECT '0' AS x, g.f_id, g.f_name
    FROM c_goods g
    WHERE g.f_type IN (1, 2, 5)
      AND g.f_id IN (SELECT f_dish FROM c_menu)
    ORDER BY 3
EOD;

echo json_encode(["rows" => getFilterResult($sql), "cols" => $cols], JSON_UNESCAPED_UNICODE);
