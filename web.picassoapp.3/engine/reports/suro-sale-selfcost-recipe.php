<?php
#(C) 05/02/2025 Kudryashov Vasili
require_once __DIR__ . "/reports.php";

$widget = ["icon" => "cash.png", "title" => "Վաճառք-ինքնառժեք"];
$cols = ["Խումբ", "Ուտեստ", "Քանակ", "Բաղադրիչ", "Գին", "Քանակ", "Ինքնառժեք"];
$hiddencols = [];
$handler = [];
$filter = [
    ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
    ["type" => "date", "title" => tr("Date of end"), "field" => "date2"]
];
$colsum = ["2"=>0,  "5"=>0 , "6" => 0];

if (empty($params->date1)) {
    $params->date1 = date("Y-m-d");
}

if (empty($params->date2)) {
    $params->date2 = date("Y-m-d");
}

$rows = stmtall("SELECT "
    . "p2.f_name AS f_group, d.f_name AS f_dish, cast(sum(b.f_qty1) as float) , g.f_name, "
    . "cast(coalesce(avgin.f_avgin, g.f_lastinputprice) as float) AS f_price, "
    . "cast(sum(b.f_qty1) * r.f_qty as float) AS f_recipeqty, "
    . "cast(sum(b.f_qty1) * r.f_qty * coalesce(avgin.f_avgin, g.f_lastinputprice) as float) AS f_totalselfcost "
    . "FROM o_body b "
    . "LEFT JOIN d_dish d ON d.f_id=b.f_dish "
    . "LEFT JOIN d_part2 p2 ON p2.f_id=d.f_part "
    . "LEFT JOIN o_header h ON h.f_id=b.f_header "
    . "LEFT JOIN d_recipes r ON r.f_dish=b.f_dish "
    . "LEFT JOIN c_goods g ON g.f_id=r.f_goods "
    . "LEFT JOIN (SELECT s.f_goods, SUM(s.f_total)/SUM(s.f_qty) AS f_avgin "
    . "FROM a_store_draft s "
    . "LEFT JOIN a_header h ON h.f_id=s.f_document "
    . "WHERE h.f_state=1 and h.f_date BETWEEN '$params->date1' AND '$params->date2' "
    . "AND h.f_type=1 "
    . "GROUP BY 1) avgin ON avgin.f_goods=r.f_goods "
    . "WHERE h.f_state=2 AND b.f_state=1 AND h.f_datecash BETWEEN '$params->date1' AND '$params->date2' "
    . "GROUP BY 1, 2, 4 "
    . "ORDER BY 1,2")->fetch_all(MYSQLI_NUM);

echo json_encode(
    [
        "widget" => $widget,
        "cols" => $cols,
        "rows" => $rows,
        "handler" => $handler,
        "sum" => $colsum,
        "filter" => $filter,
        "hiddencols" => $hiddencols
    ],
    JSON_UNESCAPED_UNICODE
);