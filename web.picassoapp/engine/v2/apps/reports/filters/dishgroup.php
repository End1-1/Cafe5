<?php
/**
 * © 2025 Kudryashov Vasili
 * Author: Kudryashov Vasili
 * Created: 2025-04-07 01:02:50
 * Last Modified: 2026-06-06
 * License: MIT
 * Description: Values for filter of dishes groups names
 */
require_once __DIR__ . "/filters.php";

$cols = ["X", tr("Code"), tr("Name")];
$sql = <<<EOD
    SELECT '0' AS x, gr.f_id, gr.f_name
    FROM c_groups gr
    WHERE gr.f_id IN (
        SELECT g.f_group
        FROM c_goods g
        INNER JOIN c_menu mm ON mm.f_dish = g.f_id
        WHERE g.f_type IN (1, 2, 5)
    )
    ORDER BY 3
EOD;

echo json_encode(["rows" => getFilterResult($sql), "cols" => $cols], JSON_UNESCAPED_UNICODE);
