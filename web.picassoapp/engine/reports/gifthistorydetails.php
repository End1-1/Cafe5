<?php 
require_once(__DIR__ . "/reports.php");


$tr = new DBTranslator();

$modevalues = [
	
];
$widget = ["icon" => "cash.png", "title" => "Նվեր քարտերի բացվացք"];
$filter = [
	
];



	$cols = ["Կոդ", "Ամսաթիվ", "Պատվեր", "Գումար"];
	$hiddencols = [];
	$colsum = [3];
	$handler = [];


    $code = $params->card;
    
    $sql = <<<EOD
    select bc.f_code, oh.f_datecash, concat(oh.f_prefix, oh.f_hallid), bh.f_amount 
    from b_gift_card_history bh 
    left join o_header oh on oh.f_id=bh.f_trsale
    left join b_gift_card bc on bc.f_id=bh.f_card
    where bc.f_code='$code'
    EOD;
	
	
	if (!$stmt = $db->prepare($sql)) {
		exitError($db->error);
	}

	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$result = $stmt->get_result();

	$rows = [];
	while ($row = $result->fetch_row()) {
		array_push($rows, $row);
	}
	$stmt->close();
	$db->close();

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
