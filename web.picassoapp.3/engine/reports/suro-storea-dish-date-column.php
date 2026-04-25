<?php
#27/01/2025 (C) Kudryashov Vasili
#Expluded from usage
require_once __DIR__ . "/reports.php";

$widget = ["icon" => "cash.png", "title" => "Վաճառք ըստ օրերի"];
$cols = ["Խումբ", "Ուտեստ", "Գործընկեր", "Քանակ",];
$hiddencols = [];
$handler = [];
$filter = [
    ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
    ["type" => "date", "title" => tr("Date of end"), "field" => "date2"]
];
$colsum = [];

if (empty($params->date1)) {
    $params->date1 = date("Y-m-d");
}

if (empty($params->date2)) {
    $params->date2 = date("Y-m-d");
}

$d1 = new DateTimeImmutable($params->date1);
$d2 = (new DateTimeImmutable($params->date2))->modify("+1 day");

$sql = "";
$i = 3;
do {
    if (!empty($sql)) {
        $sql .= ",";
    }
    $sql .= "coalesce(sum(case when h.f_date='" . $d1->format("Y-m-d") . "' then coalesce(ad.f_total,0) end), 0) ";
    array_push($cols, $d1->format("d/m/Y"));
    array_push($colsum, [$i => 0]);
    $i++;
    $d1 = $d1->modify("+1 day");
} while ($d1 < $d2);

$sql = "select gr.f_name, d.f_name, p.f_taxname, sum(ad.f_total), " . $sql
    . "from a_store ad "
    . "left join a_header h on h.f_id=ad.f_document "
    . "left join c_goods d on d.f_id=ad.f_goods "
    . "left join c_groups gr on gr.f_id=d.f_group "
    . "left join c_partners p on p.f_id=h.f_partner "
    . "where h.f_state=1  "
    . "and h.f_type=1 and h.f_date between '$params->date1' and '$params->date2' "
    . "group by 1,2 ";

$rows = stmtall($sql)->fetch_all(MYSQLI_NUM);


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