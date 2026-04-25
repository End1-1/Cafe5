<?php
#(C) 05/02/2025 Kudryashov Vasili
require_once __DIR__ . "/reports.php";

$widget = ["icon" => "cash.png", "title" => "Վաճառք-ինքնառժեք"];
$cols = ["Խումբ", "Ուտեստ", "Գին", "Քանակ", "Գումար", "Միավորի ինքնառժեք", "Ինքնառժեք", "Եկամուտ", "%"];
$hiddencols = [];
$handler = [];
$sumspecial = [
    [
        "column" => 8,
        "formula" => "percent",
        "cols" => [7, 4]
    ]
];
$filter = [
    ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
    ["type" => "date", "title" => tr("Date of end"), "field" => "date2"],
    ["type" => "checkbox", "title" => tr("Do not show without receipe"), "field" => "donotshowwithoutrecipe"]
];
$colsum = ["3" => 0, "4" => 0, "6" => 0, "7" => 0, "8" => 0];

if (empty($params->date1)) {
    $params->date1 = date("Y-m-d");
}

if (empty($params->date2)) {
    $params->date2 = date("Y-m-d");
}

$where = "";
if (!empty($params->donotshowwithoutrecipe)) {
    if ($params->donotshowwithoutrecipe > 0) {
        $where .= " and rp.f_priceperone > 0 ";
    }
}

$rows = stmtall("SELECT "
    . "p2.f_name AS f_group, d.f_name AS f_dish, cast(b.f_price as float), cast(sum(b.f_qty1) as float) AS f_qty , cast(SUM(b.f_total) as float) AS f_total, "
    . "cast(round(coalesce(rp.f_priceperone, 0),1) as float), cast(floor(coalesce(rp.f_priceperone, 0)*sum(b.f_qty1)) as float) AS f_totalselfcost, "
    . "cast(floor(SUM(b.f_total)-coalesce(rp.f_priceperone, 0)*sum(b.f_qty1)) as float) AS f_profit, "
    . "cast(round(100-(coalesce(rp.f_priceperone, 0)*sum(b.f_qty1)/SUM(b.f_total)*100),1) as float) AS f_percent "
    . "FROM o_body b "
    . "LEFT JOIN d_dish d ON d.f_id=b.f_dish "
    . "LEFT JOIN d_part2 p2 ON p2.f_id=d.f_part  "
    . "LEFT JOIN o_header h ON h.f_id=b.f_header "
    . "LEFT JOIN (SELECT rr.f_dish, sum(rr.f_qty*coalesce(avgin.f_avgin, g.f_lastinputprice)) AS f_priceperone "
    . "FROM d_recipes rr "
    . "LEFT JOIN c_goods g ON g.f_id=rr.f_goods "
    . "left JOIN (SELECT s.f_goods, SUM(s.f_total)/SUM(s.f_qty) AS f_avgin "
    . "FROM a_store_draft s "
    . "LEFT JOIN a_header h ON h.f_id=s.f_document "
    . "WHERE h.f_state=1 and h.f_date BETWEEN '$params->date1' AND '$params->date2' "
    . "AND h.f_type=1 "
    . "GROUP BY 1 ) avgin ON avgin.f_goods=rr.f_goods "
    . "GROUP BY 1) rp ON rp.f_dish=b.f_dish "
    . "WHERE h.f_state=2 AND b.f_state=1 AND h.f_datecash BETWEEN '$params->date1' AND '$params->date2' $where "
    . "GROUP BY 1, 2 "
    . "ORDER BY 1,2")->fetch_all(MYSQLI_NUM);

echo json_encode(
    [
        "widget" => $widget,
        "cols" => $cols,
        "rows" => $rows,
        "handler" => $handler,
        "sum" => $colsum,
        "filter" => $filter,
        "hiddencols" => $hiddencols,
        "sumspecial" => $sumspecial
    ],
    JSON_UNESCAPED_UNICODE
);