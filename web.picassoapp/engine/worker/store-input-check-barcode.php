<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modified - 2025-04-01 11:11:47
require_once __DIR__ . "/../app.php";

if ($params->mode == 1) {
	if (empty($params->barcode)) {
		exitError(tr("Barcode empty"));
	}
	$sql = <<<EOD
	select ad.f_id, spa.f_id as f_acc, ah.f_date, g.f_name, g.f_scancode, ad.f_qty, if (gpr.f_price1disc>0,gpr.f_price1disc, gpr.f_price1) as f_price1 
	from a_store_draft ad 
	left join a_header_shop2partneraccept spa on spa.f_id=ad.f_id 
	inner join c_goods g on g.f_id=ad.f_goods 
	inner join a_header ah on ah.f_id=ad.f_document 
	inner join a_header_shop2partner sp on sp.f_id=ah.f_id 
	left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1 
	where ah.f_state=2 and sp.f_store=? and sp.f_accept=0 and g.f_scancode=? 
	order by g.f_scancode
	EOD;
	if (!$stmt = $db->prepare($sql)) {
		exitError($db->error);
	}
	$stmt->bind_param("is", $params->store, $params->barcode);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$result = $stmt->get_result();
	if ($result->num_rows == 0) {
		exitError(tr('No result for barcode') . " $params->barcode");
	}
	$rows = [];
	while ($row = $result->fetch_assoc()) {
		array_push($rows, $row);
	}
	$stmt->close();
}

if ($params->mode == 2) {
	$sql = <<<EOD
	select ad.f_id, spa.f_id as f_acc, ah.f_date, g.f_name, g.f_scancode, 
	ad.f_qty, if (gpr.f_price1disc>0,gpr.f_price1disc, gpr.f_price1) as f_price1 
	from a_store_draft ad 
	left join a_header_shop2partneraccept spa on spa.f_id=ad.f_id 
	inner join c_goods g on g.f_id=ad.f_goods 
	inner join a_header ah on ah.f_id=ad.f_document
	left join a_header_store hs on hs.f_id=ah.f_id 
	left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1 
	where hs.f_storein=? and ah.f_state=2 and ah.f_type=1 
	order by g.f_scancode
	EOD;
	if (!$stmt = $db->prepare($sql)) {
		exitError($db->error);
	}
	$stmt->bind_param("i", $params->store);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$result = $stmt->get_result();
	if ($result->num_rows == 0) {
		exitError(tr("Nothig"));
	}
	$rows = [];
	while ($row = $result->fetch_assoc()) {
		array_push($rows, $row);
	}
	$stmt->close();
}

$acc = null;
if ($params->mode == 3){ 
	if (!$stmt = $db->prepare("select f_id from a_header_shop2partneraccept where f_id=?")) {
		exitError($db->error);
	}
	$stmt->bind_param("s", $params->id);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$result = $stmt->get_result();
	$stmt->close();
	if ($result->num_rows == 0) {
		if (!$stmt = $db->prepare("insert into a_header_shop2partneraccept (f_id, f_datetime) values (?, current_timestamp())")) {
			exitError($db->error);
		}
		$stmt->bind_param("s", $params->id);
		if (!$stmt->execute()) {
			exitError($stmt->error);
		}
		$acc = $params->id;
	} else {
		if (!$stmt = $db->prepare("delete from a_header_shop2partneraccept where f_id=?")) {
			exitError($db->error);
		}
		$stmt->bind_param("s", $params->id);
		if (!$stmt->execute()) {
			exitError($stmt->error);
		}
	}
	$stmt->close();
	$rows = [];
}

echo json_encode(["status"=>1, "data"=>["result"=>$rows, "acc"=>$acc]]);
