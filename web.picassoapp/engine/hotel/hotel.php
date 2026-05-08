<?php
require_once(__DIR__ . "/../app.php");

$hdb = new mysqli($dbhost, $dbuser, $dbpass, "metropol");
if ($hdb->errno) {
    exitError($hdb->error);
}

if (!$hdb->set_charset("utf8mb4")) {
	echo $hdb->error;
	exit();
};

function hqueryStr($sql) {
	
	global $hdb;
	if (!$result = $hdb->query($sql)){ 
		exitError($hdb->error);
	}
	return $result;
}

function hstmtall($sql, $types = null, $params = null) {
	global $hdb;
	if (!$stmt = $hdb->prepare($sql)) {
		exitError($hdb->error);
	}
	if (!empty($types)) {
		$stmt->bind_param($types, ...$params);
	}
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$result = $stmt->get_result();
	$stmt->close();
	return $result;
}

function hinsert($table, $params) {
	global $hdb;
	$fields = "";
	$values = "";
	$bindValues = [];
	$bindTypes = "";
	foreach ($params as $k => $v) {
		if (empty($fields)) {
			
		} else {
			$fields .= ",";
			$values .= ",";
		}
		$fields .= "$k";
		$values .= "?";
		array_push($bindValues, $v);
            switch (gettype($v)) {
                case "integer":
                    $bindTypes .= "i";
                    break;
                case "double":
                    $bindTypes .= "d";
                    break;
                default:
                    $bindTypes .= "s";
                    break;
            }
	}
	$sql = "insert into $table ($fields) values ($values)";
	if (!$stmt = $hdb->prepare($sql)) {
		exitError($hdb->error);
	}
	$stmt->bind_param($bindTypes, ...$bindValues);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$id = $stmt->insert_id;
	$stmt->close();
	return $id;
}

function hupdate($table, $params, $id, $field = "f_id") {
	global $hdb;
	$sql = "update $table set ";
	$bindTypes = "";
	$bindValues = [];
	foreach ($params as $k => $v) {
		if (!empty($bindTypes)) {
			$sql .= ",";
		}
		$sql .= "$k=?";
		array_push($bindValues, $v);
		switch (gettype($v)) {
			case "integer":
				$bindTypes .= "i";
				break;
			case "double":
				$bindTypes .= "d";
				break;
			default:
				$bindTypes .= "s";
				break;
		}
	}
	$sql .= " where $field=?";
	$bindTypes .= "s";
	array_push($bindValues, $id);
	if (!$stmt = $hdb->prepare($sql)) {
		exitError($hdb->error);
	}
	
	$stmt->bind_param($bindTypes, ...$bindValues);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$stmt->close();
}

function voucherNum($prefix) {
	global $hdb;
	$hdb->begin_transaction();
	$result = $hdb->query("select f_max, f_zero from airwick.serv_id_counter where f_id='$prefix' for update")->fetch_all(MYSQLI_ASSOC);
	$id = $result[0]["f_max"] + 1;
	$voucher = str_pad($id, $result[0]["f_zero"], "0", STR_PAD_LEFT);
	$hdb->query("update airwick.serv_id_counter set f_max=$id where f_id='$prefix'");
	$hdb->commit();
	return $voucher;
}

class Folio {

	private $params;

	function __construct() {
		global $params;
		$this->params = $params;
	}

	function open() {
		if (empty($this->params->f_id)) {
			exitError(tr("Empty folio number"));
		}
		
		$sql = "select gg.f_firstname, gg.f_lastname, gg.f_tel1 "
			. "from f_reservation_guests rg "
			. "left join f_guests gg on gg.f_id=rg.f_guest "
			. "where rg.f_reservation=?";
		$guests = hstmtall($sql, "s", [$this->params->f_id])->fetch_all(MYSQLI_ASSOC);
		
		$sql = "select * from f_reservation where f_id=?";
		$reservation = hstmtall($sql, "s", [$this->params->f_id])->fetch_all(MYSQLI_ASSOC);
		
		$sql = "select * from f_room where f_id=?";
		$room = hstmtall($sql, "i", [$reservation[0]["f_room"]])->fetch_all(MYSQLI_ASSOC)[0];
		
		$sql = "select f_wdate, f_amountamd, f_finalname, f_sign  "
			. "from m_register m "
			. "where m.f_inv=? and m.f_canceled=0 and m.f_finance=1 "
			. "order by f_wdate ";
		$folio = hstmtall($sql, "s", [$reservation[0]["f_invoice"]])->fetch_all(MYSQLI_ASSOC);
		
		
		
		printResult(1, ["room" => $room, "reservation"=>$reservation[0], "guests" => $guests, "folio"=>$folio]);
	}

}