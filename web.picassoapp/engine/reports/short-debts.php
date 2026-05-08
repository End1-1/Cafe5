<?php
# (C) 2024-2024 Kudryashov Vasili
# Last modified - 21/02/2025
require_once __DIR__ . "/reports.php";

$widget = ["icon" => "cash.png", "title" => "Կարճ պարտքեր"];
$cols = ["UUID", "Գործընկերոջ կոդ", "Կտրոն", "Ամսաթիվ", "Ժամ", "Նշում", "Գումար", ];
$hiddencols = [0, 1];
$handler = ["ec26fd1c-2391-11ef-a99a-7c10c9bcac82", "ec26fd1c-2391-11ef-a99a-7c10c9bcac82"];
$filter = [
];
$colsum = [["6" => 0]];

$rows = stmtall("SELECT oh.f_id, oh.f_partner, concat(oh.f_prefix, oh.f_hallid), oh.f_datecash, oh.f_timeclose, oh.f_comment, cast(oh.f_amounttotal as float) "
    ."from o_header oh "
    ."left join o_header_flags f on f.f_id=oh.f_id "
    ."where f.f_5=1")->fetch_all(MYSQLI_NUM);

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