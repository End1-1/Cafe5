<?php
require_once(__DIR__ . "/../app.php");

if (empty($params->barcode)) {
	exitError("Barcode cannot be empty");
}
#add multi scancode
$stmt = $db->prepare("select g.f_id, gr.f_name as f_groupname, g.f_name as f_goodsname, "
	. "g.f_scancode, "
	. "if(gpr.f_price1disc>0, gpr.f_price1disc, gpr.f_price1) as f_price1, "
	. "if(gpr.f_price2disc>0, gpr.f_price2disc, gpr.f_price2) as f_price2 "
	. "from c_goods g "
	. "left join c_groups gr on gr.f_id=g.f_group "
	. "left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=1 "
	. "where g.f_scancode=?");
$stmt->bind_param("s", $params->barcode);
$stmt->execute();
$result = $stmt->get_result();
if ($result->num_rows == 0) {
	exitError(tr("Barcode not exists"));
}
$goods = $result->fetch_assoc();
$stmt->close();
//$goodsid = $row["f_id"];

$stmt = $db->prepare("select st.f_name as f_storename, s.f_store,  "
	. "cast(coalesce(r.f_reserve, 0.0) as float) as f_reserve, cast(sum(s.f_qty*s.f_type) as float) as f_qty  "
	. "from a_store s "
	. "left join c_storages st on st.f_id=s.f_store "
	. "left join (select f_store, cast(sum(f_qty) as float) as f_reserve "
	. "from a_store_reserve where f_state=1 and f_goods=? group by 1) r on r.f_store=s.f_store "
	. "where s.f_goods=? "
	. "group by 1,2 ");
if (!$stmt) {
	exitError($db->error);
}
$stmt->bind_param("ii", $goods["f_id"], $goods["f_id"]);
if (!$stmt->execute()) {
	exitError($stmt->error);
}
$result = $stmt->get_result();
$rows = [];
while ($row = $result->fetch_assoc()) {
	array_push($rows, $row);
}
$stmt->close();
$db->close();

echo json_encode(["status" => 1, "data" => ["result" => $rows, "goods" => $goods]], JSON_UNESCAPED_UNICODE);