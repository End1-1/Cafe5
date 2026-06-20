<?php
/**
 * © 2026 Kudryashov Vasili
 * Description: Values for filter of menu names (c_menu_names)
 */
require_once __DIR__ . "/filters.php";

$cols = ["X", tr("Code"), tr("Name")];
$sql = <<<EOD
    SELECT '0' AS x, f_id, f_name
    FROM c_menu_names
    ORDER BY 3
EOD;

echo json_encode(["rows" => getFilterResult($sql), "cols" => $cols], JSON_UNESCAPED_UNICODE);
