<?php
# (C) 2024-2025 Kudryashov Vasili
# Created ... 2024
# Last modified - 2025-12-19 02:24:49
defined('APP') or die('Die, vampire!');
require_once __DIR__ . "/dbtranslator.php";
require_once __DIR__ . "/permissions.php";



class PClass
{
    protected $db;
    protected $params;
    protected $userid;
    protected $user;
    protected $result = [];
    protected $tr;
    protected $config;

    protected $permissions;
    public function __construct()
    {
        global $db;
        global $params;
        global $userid;
        global $user;
        global $config;
        $this->db = $db;
        $this->params = $params;
        $this->userid = $userid;
        $this->user = $user;
        $this->config = $config;
        $this->tr = new DBTranslator();

        $rows = $this->stmtall("select f_key, f_value from s_user_access where f_group=? and f_value=1", "i", [$user["f_group"]])->fetch_all(MYSQLI_NUM);
        $prm = array_column($rows, 0); 
        $this->permissions = new Permissions($prm);
    }

    public function uuidv4()
    {
        $data = random_bytes(16);
        $data[6] = chr(ord($data[6]) & 0x0f | 0x40);
        $data[8] = chr(ord($data[8]) & 0x3f | 0x80);
        return vsprintf('%s%s-%s-%s-%s-%s%s%s', str_split(bin2hex($data), 4));
    }

    protected function stmtall($sql, $types = null, $params = null)
    {
        global $db;
        try {
            if (!$stmt = $db->prepare($sql)) {
                $this->exitError($db->error);
            }
            if (!empty($types)) {
                $stmt->bind_param($types, ...$params);
            }
            if (!$stmt->execute()) {
                $this->exitError($stmt->error);
            }
        } catch (Exception $e) {
            throw new Exception($e->getMessage() . " " . $sql);
        }
        $result = $stmt->get_result();
        $stmt->close();
        return $result;
    }

    protected function sinsert($table, &$params)
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

    protected function supdate($table, &$params, $id, $field = "f_id")
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

    protected function sinsertupdate($table, &$params, $id, $isnew, $field = "f_id")
    {
        if ($isnew) {
            return $this->sinsert($table, $params);
        } else {
            $this->supdate($table, $params, $id, $field);
            return $id;
        }
    }

    public function handler()
    {

    }


    public function echoResult()
    {
        if (!isset($this->result["status"])) {
            $this->result["status"] = 1;
        }
        echo json_encode($this->result, JSON_UNESCAPED_UNICODE);
    }

    public function exitError($error)
    {
        header("HTTP/1.1 500 Server error", true, 500);
        die($error);
    }

    protected function oheaderCounter(&$prefix, &$hallid, $hall)
    {
        $hallcounter = $this->stmtall("select f_counterhall from h_halls where f_id=?", "i", [$hall])->fetch_assoc()["f_counterhall"];
        $counterPrefix = $this->stmtall("select f_counter + 1 as f_counter, f_prefix from h_halls where f_id=? for update", "i", [$hallcounter])->fetch_assoc();
        $this->stmtall("update h_halls set f_counter=? where f_id=?", "ii", [$counterPrefix["f_counter"], $hallcounter]);
        $prefix = $counterPrefix["f_prefix"];
        $hallid = $counterPrefix["f_counter"];
    }

    protected function tr($text)
    {
        return $this->tr->tr($text);
    }
}