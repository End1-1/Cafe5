<?php
# © 2025 Kudryashov Vasili
# Created - 28/01/2025 
# Last modified - 2025-04-27 16:47:14
require_once __DIR__ . "/../app.php";
require_once __DIR__ . "/filter-worker.php";

$tr = new DBTranslator();

$modevalues = [
	["key" => $tr->tr("General form for customers"), "value" => 0],
	["key" => $tr->tr("Customer reports by deal"), "value" => 1],
	["key" => $tr->tr("General costumer t-report"), "value" => 2],
	["key" => $tr->tr("Partner general report"), "value" => 3],
	["key" => $tr->tr("Partners deals"), "value" => 4],
	["key" => $tr->tr("General partner t-report"), "value" => 6],
	["key" => $tr->tr("Range history"), "value" => 5]
];
$widget = ["icon" => "cash.png", "title" => "Հաճախորդների պարտքեր"];
$filter = [
	["type" => "combo", "title" => $tr->tr("Report mode"), "field" => "mode", "default" => 0, "values" => $modevalues],
	["type" => "date", "title" => $tr->tr("Date of begin"), "field" => "date1"],
	["type" => "date", "title" => $tr->tr("Date of end"), "field" => "date2"],
	["type" => "keyvalue", "title" => $tr->tr("Currency"), "field" => "currency", "filter" => "currency"],
	["type" => "keyvalue", "title" => $tr->tr("Partner"), "field" => "partner", "filter" => "partners"],
	["type" => "keyvalue", "title" => $tr->tr("Manager"), "field" => "manager", "filter" => "users"],
	["type" => "keyvalue", "title" => $tr->tr("Mark"), "field" => "mark", "filter" => "halls"]
];


validateFilter($params, $filter);

//customer total report
if ($params->mode == 0) {
	$cols = ["Կոդ", "Անվանում", "Պարտք, Դրամ", "Պարտք, USD"];
	$hiddencols = [];
	$colsum = [["2" => 0], ["3" => 0]];
	$handler = ["90dd520c-f072-11ee-b90b-7c10c9bcac82", "90dd520c-f072-11ee-b90b-7c10c9bcac82"];
	$db->begin_transaction();
	$db->query("CREATE TEMPORARY TABLE debts_total (f_partnerid INTEGER, f_partner TEXT, f_amd float, f_usd float);");
	$db->query("INSERT INTO debts_total (f_partnerid, f_partner, f_amd, f_usd) "
		. " SELECT f_id, concat_ws(';', coalesce(f_name, ' '), coalesce(f_taxname, ''), "
		. "coalesce(f_address, ' '), concat('ՀՎՀՀ` ', coalesce(f_taxcode, ' '))), 0, 0 FROM c_partners where f_state=1 ");

	$flag = empty($params->mark) ? "" : " and f_flag in ($params->mark) ";
	$db->query("UPDATE debts_total d INNER JOIN "
		. "(SELECT f_costumer AS f_partner, SUM(f_amount) AS f_amd "
		. "FROM  b_clients_debts WHERE f_source=1 AND f_currency=1 and f_date<='$params->date2' $flag GROUP BY 1) b ON d.f_partnerid=b.f_partner "
		. "SET d.f_amd=b.f_amd WHERE d.f_partnerid=b.f_partner; ");

	$db->query("UPDATE debts_total d INNER JOIN "
		. "(SELECT f_costumer AS f_partner, SUM(f_amount) AS f_usd "
		. "FROM  b_clients_debts WHERE f_source=1 AND f_currency=2 and f_date<='$params->date2' $flag GROUP BY 1) b ON d.f_partnerid=b.f_partner "
		. "SET d.f_usd=b.f_usd WHERE d.f_partnerid=b.f_partner; ");

	if (!empty($params->manager)) {
		$db->query("delete from debts_total where f_partnerid not in (select f_id from c_partners where f_manager in ($params->manager))");
	}

	$db->query("DELETE FROM debts_total WHERE f_amd=0 AND f_usd=0; ");
	if (!$stmt = $db->prepare("select * from debts_total ")) {
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
	$db->commit();
	$db->close();
}

//customer detailed report
if ($params->mode == 1) {
	$handler = ["90dd520c-f072-11ee-b90b-7c10c9bcac82", "90dd520c-f072-11ee-b90b-7c10c9bcac82"];
	$cols = [
		tr("Կոդ"),
		$tr->tr("Date"),
		$tr->tr("Name"),
		$tr->tr("Sale document"),
		$tr->tr("Payment document"),
		$tr->tr("Store document"),
		$tr->tr("Amount"),
		$tr->tr("Debt"),
		$tr->tr("Return"),
		$tr->tr("Payment"),
		$tr->tr("Balance"),
		$tr->tr("Currency"),
		$tr->tr("Transfer"),
		$tr->tr("Cash debt"),
		$tr->tr("Hall"),
		$tr->tr("Comment")
	];
	$hiddencols = [0, 3, 4, 5];
	$colsum = [["6"=>0], ["7"=>0], ["8"=>0], ["9"=>0], ["12"=>0], ["13"=>0]];
	$filter_partner = empty($params->partner) ? "" : "and cd.f_costumer in ($params->partner)";
	$filter_side = empty($params->mark) ? "" : " and cd.f_flag in ($params->mark) ";


	$sql = "SELECT cd.f_id, cd.f_date, "
	. "concat_ws(';', coalesce(p.f_name, ' '), coalesce(f_taxname, ''), "
	. "coalesce(f_address, ' '), concat('ՀՎՀՀ` ', coalesce(f_taxcode, ' '))) as f_taxname, "

	." cd.f_order, cd.f_cash, cd.f_storedoc, cast(cd.f_amount as float), cast(bm.f_amountmin as float), "
		. "cast(bpp.f_amountret as float), cast(bp.f_amountplu as float), cast(bt.f_amounttot as float), c.f_name as f_currency, "
		. "cast(oh.f_amountbank as float), cast(oh.f_amountdebt as float), "
		. "h.f_name as f_hall, coalesce(bp.f_comment,  cd.f_comment) as f_comment "
		. "FROM b_clients_debts cd "
		. "left join (select cd.f_id, f_amount as f_amountmin from b_clients_debts cd "
		. "left join c_partners p on p.f_id=cd.f_costumer "
		. "where cd.f_date between '$params->date1' and '$params->date2'  $filter_partner $filter_side and cd.f_source=1  and cd.f_amount<0 ) "
		. "bm on bm.f_id=cd.f_id "
#return from sale
		. "left join (select cd.f_id, f_amount as f_amountret "
		. "from b_clients_debts cd "
		. "left join c_partners p on p.f_id=cd.f_costumer "
		. "where cd.f_date between '$params->date1' and '$params->date2'  $filter_partner  $filter_side and cd.f_source=1  and cd.f_amount>0  "
		. "and length(coalesce(f_order ,''))>0) bpp on bpp.f_id=cd.f_id "
#payment
		. "left join (select cd.f_id, f_amount as f_amountplu, f_comment from b_clients_debts cd "
		. "left join c_partners p on p.f_id=cd.f_costumer "
		. "where cd.f_date between '$params->date1' and '$params->date2' $filter_partner $filter_side and cd.f_source=1  "
		. "and cd.f_amount>0 and length(coalesce(cd.f_cash, ''))>0) "
		. "bp on bp.f_id=cd.f_id "
#balance
		. "left join (select f_costumer, sum(f_amount) as f_amounttot from b_clients_debts cd "
		. "left join c_partners p on p.f_id=cd.f_costumer where p.f_state=1 and cd.f_source=1 $filter_side group by 1) "
		. "bt on bt.f_costumer=cd.f_costumer "
		
		. "LEFT JOIN c_partners p ON p.f_id=cd.f_costumer "
		. "LEFT JOIN e_currency c ON c.f_id=cd.f_currency "
		. "left join o_header oh on oh.f_id=cd.f_order "
		. "LEFT join h_halls h on h.f_id=cd.f_flag "
		. "where cd.f_date between '$params->date1' and '$params->date2'  "
		. "$filter_partner  and cd.f_source=1 and p.f_state>0 "
		. "and cd.f_date between '$params->date1' and '$params->date2'  $filter_partner $filter_side and cd.f_source=1 "
		. "order by 2 ";
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
}

//customer t-report
if ($params->mode == 2) {
	$handler = ["90dd520c-f072-11ee-b90b-7c10c9bcac82", "90dd520c-f072-11ee-b90b-7c10c9bcac82"];
	$cols = [
		tr("Կոդ"),
		$tr->tr("Name"),
		$tr->tr("Address"),
		$tr->tr("Begining"),
		$tr->tr("Debt"),
		$tr->tr("Payment"),
		$tr->tr("Return goods"),
		$tr->tr("Return"),
		$tr->tr("Difference")
	];
	$hiddencols = [];
	$colsum = [["3"=>0],[ "4"=>0],[ "5"=>0], ["6"=>0], ["7"=>0], ["8"=>0]];
	$filter_manager = empty($params->manager) ? "" : "and f_manager in ($params->manager)";
	$filter_flag = empty($params->mark) ? "" : "and f_flag in ($params->mark) ";
	$db->begin_transaction();
	if (
		!$db->query("CREATE TEMPORARY TABLE debt3 (fpartnerid INTEGER, fpartnername TINYTEXT, faddress TINYTEXT, "
			. "fbefore float, fdebt float, fpayment float, fpartnerback float, frefund float, fdiff float); ")
	) {
		exitError($db->error);
	}
	if (
		!$db->query("INSERT INTO debt3 (fpartnerid, fpartnername, faddress, fdebt, fpayment, fdiff) "
			. "SELECT f_id, f_address, f_taxname, 0, 0, 0 FROM c_partners where f_state=1 $filter_manager ; ")
	) {
		exitError($db->error);
	}
	if (
		!$db->query("update debt3 d inner join (select f_costumer, sum(f_amount) as famount "
			. "from b_clients_debts where f_source=1 and f_date<'$params->date1' $filter_flag group by 1) "
			. "dm on dm.f_costumer=d.fpartnerid set d.fbefore=dm.famount; ")
	) {
		exitError($db->error);
	}
	if (
		!$db->query("UPDATE debt3 d "
			. "INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "fROM b_clients_debts WHERE f_source=1 and f_amount<0 "
			." and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.fdebt=dm.famount; ")
	) {
		exitError($db->error);
	}

	//payment
	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "    fROM b_clients_debts WHERE f_source=1   "
			.	"and f_amount>0 and f_storedoc is null and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.fpayment=dm.famount; ")
	) {
		exitError($db->error);
	}

	//income from partner back
	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "fROM b_clients_debts "
			. "WHERE f_source=1 and f_amount>0 "
			."and f_storedoc in (SELECT distinct(f_document) FROM a_store WHERE f_reason=11) and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "dm ON dm.f_costumer=d.fpartnerid SET d.fpartnerback=dm.famount; ")
	) {
		exitError($db->error);
	}

	//refund
	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "FROM b_clients_debts WHERE f_source=1 and f_amount>0 and f_storedoc is not null "
			. "and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.frefund=dm.famount; ")
	) {

	}

	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "fROM b_clients_debts WHERE f_source=1 and f_id>0 $filter_flag and f_date<='$params->date2' GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.fdiff=dm.famount;	 ")
	) {
		exitError($db->error);
	}

	//clean unused rows
	if (
		!$db->query("delete from debt3 where coalesce(fdebt, 0)=0 and coalesce(fpayment, 0)=0 "
			. "and coalesce(fdiff, 0)=0 and coalesce(frefund,0)=0 and coalesce(fbefore,0)=0;")
	) {
		exitError($db->error);
	}
	$stmt = $db->prepare("SELECT * FROM debt3;");
	$stmt->execute();
	$result = $stmt->get_result();
	$rows = [];
	while ($row = $result->fetch_row()) {
		array_push($rows, $row);
	}
	$stmt->close();
	$db->commit();
	$db->close();
}

//partner debts common
if ($params->mode == 3) {
	$handler = ["90dd520c-f072-11ee-b90b-7c10c9bcac82", "90dd520c-f072-11ee-b90b-7c10c9bcac82"];
	$cols = [tr("Կոդ"), $tr->tr("Name"), $tr->tr("Address"), $tr->tr("Amount, AMD"), $tr->tr("Amount, USD")];
	$hiddencols = [];
	$colsum = [["3"=>0], ["4"=>0]];
	$db->begin_transaction();
	queryStr("CREATE TEMPORARY TABLE debts_total (f_partnerid INTEGER, f_partner TEXT, f_address text, f_amd float, f_usd float); ");
	queryStr("INSERT INTO debts_total (f_partnerid, f_partner, f_address, f_amd, f_usd) "
		. "SELECT f_id, f_taxname, f_address, 0, 0 FROM c_partners; ");
	queryStr("UPDATE debts_total d "
		. "INNER JOIN (SELECT f_costumer AS f_partner, SUM(f_amount)AS f_amd FROM  b_clients_debts WHERE f_source=2 AND f_currency=1 GROUP BY 1) b ON d.f_partnerid=b.f_partner "
		. "SET d.f_amd=b.f_amd WHERE d.f_partnerid=b.f_partner; ");
	queryStr("UPDATE debts_total d "
		. "INNER JOIN (SELECT f_costumer AS f_partner, SUM(f_amount)AS f_usd FROM  b_clients_debts WHERE f_source=2 AND f_currency=2 GROUP BY 1) b ON d.f_partnerid=b.f_partner "
		. "SET d.f_usd=b.f_usd WHERE d.f_partnerid=b.f_partner; ");
	#queryStr("DELETE FROM debts_total WHERE (f_amd > -0.001 and f_amd < 0.001) AND (f_usd > -0.001 and f_usd < 0.001) ");
	queryStr("DELETE FROM debts_total WHERE f_amd=0 and f_usd=0 ");
	if (!$stmt = $db->prepare("SELECT f_partnerid, f_partner, f_address, f_amd, f_usd FROM debts_total;")) {
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
	$db->commit();
	$db->close();
}

//Partners deals
if ($params->mode == 4) {
	$handler = ["90dd520c-f072-11ee-b90b-7c10c9bcac82", "90dd520c-f072-11ee-b90b-7c10c9bcac82"];
	$cols = [
		tr("Կոդ"),
		$tr->tr("Date"),
		$tr->tr("Name"),
		$tr->tr("Sale document"),
		$tr->tr("Payment document"),
		$tr->tr("Store document"),
		$tr->tr("Amount"),
		$tr->tr("Currency"),
		$tr->tr("Comment")
	];
	$hiddencols = [0, 3, 4, 5];
	$colsum = [["6"=>0]];
	$filter_partner = empty($params->partner) ? "" : "and cd.f_costumer in ($params->partner)";
	$filter_manager = empty($params->manager) ? "" : "and p.f_manager in ($params->manager)";
	$filter_flag = empty($params->mark) ? "" : "and cd.f_flag=$params->mark";
	$sql = "SELECT cd.f_id, cd.f_date, p.f_taxname, cd.f_order, cd.f_cash, "
		. "cd.f_storedoc, cast(cd.f_amount as float), c.f_name as f_currency, "
		. "cd.f_comment "
		. "FROM b_clients_debts cd "
		. "LEFT JOIN c_partners p ON p.f_id=cd.f_costumer "
		. "LEFT JOIN e_currency c ON c.f_id=cd.f_currency "
		. "where cd.f_date between '$params->date1' and '$params->date2' $filter_flag $filter_manager $filter_partner ";
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
}

if ($params->mode == 5) {
	require_once __DIR__ . "/debt-start-move-final.php";
	$debts = new DebtStartMoveFinal();
	$debts->echoResult();
	return;
}

//partners t-account
if ($params->mode == 6) {
	$handler = ["90dd520c-f072-11ee-b90b-7c10c9bcac82", "90dd520c-f072-11ee-b90b-7c10c9bcac82"];
	$cols = [
		tr("Կոդ"),
		$tr->tr("Name"),
		$tr->tr("Address"),
		$tr->tr("Begining"),
		$tr->tr("Debt"),
		$tr->tr("Payment"),
		$tr->tr("Return goods"),
		$tr->tr("Return"),
		$tr->tr("Difference")
	];
	$hiddencols = [];
	$colsum = [["3"=>0], ["4"=>0], ["5"=>0], ["6"=>0], ["7"=>0],[ "8"=>0]];
	$filter_manager = empty($params->manager) ? "" : "and f_manager in ($params->manager)";
	$filter_flag = empty($params->mark) ? "" : "and f_flag in ($params->mark) ";
	$db->begin_transaction();
	if (
		!$db->query("CREATE TEMPORARY TABLE debt3 (fpartnerid INTEGER, fpartnername TINYTEXT, faddress TINYTEXT, "
			. "fbefore float, fdebt float, fpayment float, fpartnerback float, frefund float, fdiff float); ")
	) {
		exitError($db->error);
	}
	if (
		!$db->query("INSERT INTO debt3 (fpartnerid, fpartnername, faddress, fdebt, fpayment, fdiff) "
			. "SELECT f_id, f_address, f_taxname, 0, 0, 0 FROM c_partners where 1=1 $filter_manager ; ")
	) {
		exitError($db->error);
	}
	if (
		!$db->query("update debt3 d inner join (select f_costumer, sum(f_amount) as famount "
			. "from b_clients_debts where f_source=2 and f_date<'$params->date1' $filter_flag group by 1) "
			. "dm on dm.f_costumer=d.fpartnerid set d.fbefore=dm.famount; ")
	) {
		exitError($db->error);
	}
	if (
		!$db->query("UPDATE debt3 d "
			. "INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "fROM b_clients_debts WHERE f_source=2 and f_amount<0 "
			." and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.fdebt=dm.famount; ")
	) {
		exitError($db->error);
	}

	//payment
	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "    fROM b_clients_debts WHERE f_source=2   "
			.	"and f_amount>0 and f_storedoc is null and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.fpayment=dm.famount; ")
	) {
		exitError($db->error);
	}

	//income from partner back
	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "fROM b_clients_debts "
			. "WHERE f_source=2 and f_amount>0 "
			."and f_storedoc in (SELECT distinct(f_document) FROM a_store WHERE f_reason=11) and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "dm ON dm.f_costumer=d.fpartnerid SET d.fpartnerback=dm.famount; ")
	) {
		exitError($db->error);
	}

	//refund
	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "FROM b_clients_debts WHERE f_source=2 and f_amount>0 and f_storedoc is not null "
			. "and f_date between '$params->date1' and '$params->date2' $filter_flag GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.frefund=dm.famount; ")
	) {

	}

	if (
		!$db->query("UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
			. "fROM b_clients_debts WHERE f_source=2 and f_id>0 $filter_flag and f_date<='$params->date2' GROUP BY 1) "
			. "    dm ON dm.f_costumer=d.fpartnerid SET d.fdiff=dm.famount;	 ")
	) {
		exitError($db->error);
	}

	//clean unused rows
	if (
		!$db->query("delete from debt3 where coalesce(fdebt, 0)=0 and coalesce(fpayment, 0)=0 "
			. "and coalesce(fdiff, 0)=0 and coalesce(frefund,0)=0 and coalesce(fbefore,0)=0;")
	) {
		exitError($db->error);
	}
	$stmt = $db->prepare("SELECT * FROM debt3;");
	$stmt->execute();
	$result = $stmt->get_result();
	$rows = [];
	while ($row = $result->fetch_row()) {
		array_push($rows, $row);
	}
	$stmt->close();
	$db->commit();
	$db->close();
}

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
