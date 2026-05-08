<?php
defined('APP') or die('Die, vampire!');
function queryStr($sql)
{

	global $db;
	if (!$result = $db->query($sql)) {
		exitError($db->error);
	}
	return $result;
}

function stmtall($sql, $types = null, $params = null)
{
	global $db;
	try {
		if (!$stmt = $db->prepare($sql)) {
			exitError($db->error);
		}
		if (!empty($types)) {
			$stmt->bind_param($types, ...$params);
		}
		if (!$stmt->execute()) {
			exitError($stmt->error);
		}
	} catch (Exception $e) {
		throw new Exception($e->getMessage() . " " . $sql);
	}
	$result = $stmt->get_result();
	$stmt->close();
	return $result;
}

function sinsert($table, &$params)
{
	global $db;
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
	if (!$stmt = $db->prepare($sql)) {
		exitError($db->error);
	}
	$stmt->bind_param($bindTypes, ...$bindValues);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$id = $stmt->insert_id;
	$stmt->close();
	$params = [];
	return $id;
}

function sinsertnoerror($table, &$params)
{
	global $db;
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
	if (!$stmt = $db->prepare($sql)) {
		//exitError($db->error);
	}
	$stmt->bind_param($bindTypes, ...$bindValues);
	if (!$stmt->execute()) {
		//exitError($stmt->error);
	}
	$id = $stmt->insert_id;
	$stmt->close();
	$params = [];
	return $id;
}

function supdate($table, &$params, $id, $field = "f_id")
{
	global $db;
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
	if (!$stmt = $db->prepare($sql)) {
		exitError($db->error);
	}

	$stmt->bind_param($bindTypes, ...$bindValues);
	if (!$stmt->execute()) {
		exitError($stmt->error);
	}
	$stmt->close();
	$params = [];
}

function sinsertupdate($table, &$params, $id, $isnew, $field = "f_id")
{
	if ($isnew) {
		return sinsert($table, $params);
	} else {
		supdate($table, $params, $id, $field);
		return $id;
	}
}

class DB
{
	protected $db;

	protected $result = [];

	protected $params;

	protected $user;

	public function __construct()
	{
		global $dbhost;
		global $dbuser;
		global $dbpass;
		global $dbname;

		global $params;
		$this->params = $params;
		global $user;
		$this->user = $user;

		$this->db = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
		if ($this->db->connect_errno) {
			echo $this->db->connect_error;
			exit();
		}
		if (!$this->db->set_charset("utf8mb4")) {
			echo $this->db->error;
			exit();
		}
	}
	public function beginTransaction()
	{
		$this->db->begin_transaction();
	}

	public function commit()
	{
		$this->db->commit();
	}

	public function rollback()
	{
		$this->db->rollback();
	}
	public function stmtall($sql, $types = null, $params = null)
	{
		try {
			if (!$stmt = $this->db->prepare($sql)) {
				exitError($this->db->error . " n1");
			}
			if (!empty($types)) {
				$stmt->bind_param($types, ...$params);
			}
			if (!$stmt->execute()) {
				exitError($stmt->error. " n2");
			}
			$result = $stmt->get_result();
			$stmt->close();
		} catch (Exception $e) {
			throw new Exception($e->getMessage() . " HERE " . $sql);
		}
		return $result;
	}

	function sinsert($table, &$params)
	{
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
		if (!$stmt = $this->db->prepare($sql)) {
			exitError($this->db->error . " n3");
		}
		$stmt->bind_param($bindTypes, ...$bindValues);
		if (!$stmt->execute()) {
			exitError($stmt->error);
		}
		$id = $stmt->insert_id;
		$stmt->close();
		$params = [];
		return $id;
	}

	function supdate($table, &$params, $id, $field = "f_id")
	{
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
		if (!$stmt = $this->db->prepare($sql)) {
			exitError($this->db->error);
		}

		$stmt->bind_param($bindTypes, ...$bindValues);
		if (!$stmt->execute()) {
			exitError($stmt->error);
		}
		$stmt->close();
		$params = [];
	}

	public function sdelete($tableName, $id, $fieldName = "f_id") {
			
	}

	public function echoResult()
	{
		$this->result["status"] = 1;
		echo json_encode($this->result, JSON_UNESCAPED_UNICODE);
	}

	public function exitError($error)
	{
		header("HTTP/1.1 500 Server error", true, 500);
		die($error);
	}

	public function uuidv4()
	{
		$data = random_bytes(16);

		$data[6] = chr(ord($data[6]) & 0x0f | 0x40); // set version to 0100
		$data[8] = chr(ord($data[8]) & 0x3f | 0x80); // set bits 6-7 to 10

		return vsprintf('%s%s-%s-%s-%s-%s%s%s', str_split(bin2hex($data), 4));
	}

	public function docNum($doctype, $storeid, $value, $withupdate)
	{
		$counterField = "";
		switch ($doctype) {
			case 1:
				$counterField = "f_inputcounter";
				break;
			case 3:
				$counterField = "f_movecounter";
				break;
			case 2:
				$counterField = "f_outcounter";
				break;
			case 6:
				$counterField = "f_complectcounter";
				break;
		}
		$r = $this->stmtall("select $counterField + 1 as $counterField from c_storages where f_id=? for update", "i", [$storeid])->fetch_assoc();
		if (empty($r)) {
			return "UNKNOWN STORE";
		}
		if ($value > 0) {
			$r[$counterField] = $value;
		}
		if ($withupdate) {
			$this->stmtall("update c_storages set $counterField  =? where f_id=? and $counterField < ?", "iii", [$r[$counterField], $storeid, $r[$counterField]]);
		}
		switch ($doctype) {
			case 1:
				return str_pad($r[$counterField], 5, "0", STR_PAD_LEFT);
			case 3:
				return str_pad($r[$counterField], 5, "0", STR_PAD_LEFT);
			case 2:
				return str_pad($r[$counterField], 5, "0", STR_PAD_LEFT);
			case 6:
				return str_pad($r[$counterField], 5, "0", STR_PAD_LEFT);
		}
		return "ERROR";
	}

}