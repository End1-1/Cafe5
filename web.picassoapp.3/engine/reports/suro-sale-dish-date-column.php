<?php
#15/01/2025 (C) Kudryashov Vasili
require_once __DIR__ . "/reports.php";

$widget = ["icon" => "cash.png", "title" => "Վաճառք ըստ օրերի"];
$cols = ["Խումբ", "Ուտեստ",];
$hiddencols = [];
$handler = [];
$filter = [
    ["type" => "date", "title" => tr("Date of begin"), "field" => "date1"],
    ["type" => "date", "title" => tr("Date of end"), "field" => "date2"],
    ["type"=> "checkbox", "title"=>tr("Quantities"), "field" => "qnt"],
    ["type"=> "checkbox", "title"=>tr("Amounts"), "field" => "amount"],
];
$colsum = [];

if (empty($params->date1)) {
    $params->date1 = date("Y-m-d");
}

if (empty($params->date2)) {
    $params->date2 = date("Y-m-d");
}

if (empty($params->qnt) && empty($params->amount)) {
    $params->qnt = 1;
}
if (empty($params->qnt)) {
    $params->qnt = 0;
}
if (empty($params->amount)) {
    $params->amount = 0;
}
$i = 2;
if ($params->qnt > 0) {
    array_push($cols, "Քանակ");
    array_push($colsum, [$i => 0]);
    $i++;
}
if ($params->amount > 0) {
    array_push($cols, "Գումար");
    array_push($colsum, [$i => 0]);
    $i++;
}

$d1 = new DateTimeImmutable($params->date1);
$d2 = (new DateTimeImmutable($params->date2))->modify("+1 day");

$sql = "";
do {

    if($params->qnt > 0) {
        if (!empty($sql)) {
            $sql .= ",";
        }
        $sql .= "cast(coalesce(sum(case when oh.f_datecash='" . $d1->format("Y-m-d") . "' then coalesce(ob.f_qty1,0) end), 0) as float)";
    }
    if($params->amount > 0) {
        if (!empty($sql)) {
            $sql .= ",";
        }
        $sql .= "cast(coalesce(sum(case when oh.f_datecash='" . $d1->format("Y-m-d") . "' then coalesce(ob.f_total,0) end), 0) as float)";
    }
    array_push($cols, $d1->format("d/m/Y"));
    if ($params->qnt > 0 && $params->amount > 0) {
        array_push($cols, "");
    }
    
    array_push($colsum, [$i => 0]);
    $i++;
    if ($params->qnt > 0 && $params->amount > 0) {
        array_push($colsum, [$i => 0]);
        $i++;
    }
    $d1 = $d1->modify("+1 day");
} while ($d1 < $d2);

$totalfields = "";
if ($params->qnt  > 0) {
    $totalfields .= "cast(sum(ob.f_qty1) as float),";
}
if ($params->amount  > 0) {
    $totalfields .= "cast(sum(ob.f_total) as float),";
}


$sql = "select gr.f_name, d.f_name, " . $totalfields . $sql
    . "from o_body ob "
    . "left join o_header oh on oh.f_id=ob.f_header "
    . "left join d_dish d on d.f_id=ob.f_dish "
    . "left join d_part2 gr on gr.f_id=d.f_part "
    . "where oh.f_state=2 and ob.f_state=1 "
    . "and oh.f_datecash between '$params->date1' and '$params->date2' "
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