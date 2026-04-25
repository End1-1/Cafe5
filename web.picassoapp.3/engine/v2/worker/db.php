<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 13:31:11
# Last Modified: 2026-02-06 17:27:55

class Db
{
    protected $remote_host;
    protected $dbconnection;
    protected $result = [];

    public function __construct()
    {
        global $dbhost;
        global $dbname;
        global $dbuser;
        global $dbpass;
        global $remote_host;
        $this->dbconnection = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
        if ($this->dbconnection->connect_error) {
            die("Connection failed: " . $this->dbconnection->connect_error);
        }
        $this->dbconnection->set_charset("utf8mb4");
        $this->result =  ["status" => 1, "locale" => Translator::$locale];
        $this->remote_host = $remote_host;
    }

    public function __destruct()
    {
        $this->dbconnection->close();
    }

    public function beginTransaction()
    {
        return  $this->dbconnection->begin_transaction();
    }

    public function commit()
    {
        return $this->dbconnection->commit();
    }

    public function rollback()
    {
        return $this->dbconnection->rollback();
    }

    public function select($query, $types = "", $params = [], $noreturn = false)
    {
        $stmt = $this->dbconnection->prepare($query);
        if ($stmt === false) {
            dieWithCode("Prepare failed: {$this->dbconnection->error}");
        }

        if (!empty($params)) {
            $stmt->bind_param($types, ...$params);
        }

        if (!$stmt->execute()) {
            dieWithCode("Execute failed:{$stmt->error}");
        }

        $result = $stmt->get_result();
        if (!$noreturn) {
            if ($result === false) {
                dieWithCode("Get result failed: {$stmt->error}");
            }
        }

        return $result;
    }

    public function insert($table, $params)
    {
        $fields = "";
        $values = "";
        $bindValues = [];
        $bindTypes = "";

        foreach ($params as $k => $v) {
            if (!empty($fields)) {
                $fields .= ",";
                $values .= ",";
            }
            $fields .= $k;
            $values .= "?";
            $bindValues[] = $v;

            $bindTypes .= match (gettype($v)) {
                'integer' => 'i',
                'double' => 'd',
                default => 's',
            };
        }

        $sql = "INSERT INTO $table ($fields) VALUES ($values)";
        if (!$stmt = $this->dbconnection->prepare($sql)) {
            dieWithCode($this->dbconnection->error);
        }

        $stmt->bind_param($bindTypes, ...$bindValues);

        if (!$stmt->execute()) {
            dieWithCode($stmt->error);
        }

        $id = $stmt->insert_id;
        $stmt->close();

        return $id;
    }

    public function update($table, $params, $id, $field = "f_id")
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
        $bindTypes .= gettype($id) === "integer" ? "i" : "s";
        array_push($bindValues, $id);
        try {
            $stmt = $this->dbconnection->prepare($sql);
        } catch (mysqli_sql_exception $e) {
            dieWithCode(
                'DB PREPARE ERROR: ' . $e->getMessage() .
                    "<br>" . $sql
            );
        }


        $stmt->bind_param($bindTypes, ...$bindValues);
        if (!$stmt->execute()) {
            dieWithCode($stmt->error);
        }
        $stmt->close();
    }

    public function delete($tableName, $id, $fieldName = "f_id")
    {
        $sql = <<<EOD
        delete  from $tableName where $fieldName =?
        EOD;
        if (!$stmt = $this->dbconnection->prepare($sql)) {
            dieWithCode($this->dbconnection->error);
        }
        $bindTypes = "";
        switch (gettype($id)) {
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
        $stmt->bind_param($bindTypes, $id);
        if (!$stmt->execute()) {
            dieWithCode($stmt->error);
        }
        $stmt->close();
    }

    public function updateJsonField($table, $idValue, $jsonFieldName, $jsonKey, $newData, $idField = "f_id")
    {
        // 1. Обязательно кодируем в JSON строку, если пришел массив/объект
        if (is_array($newData) || is_object($newData)) {
            $jsonString = json_encode($newData, JSON_UNESCAPED_UNICODE);
        } else {
            $jsonString = $newData;
        }

        // 2. Используем чистый JSON_SET. 
        // Чтобы MariaDB не восприняла это как обычную строку, 
        // мы используем JSON_EXTRACT(?, '$') — это заставит базу распарсить строку в JSON-объект.
        $sql = "UPDATE $table 
            SET $jsonFieldName = JSON_SET(
                COALESCE($jsonFieldName, '{}'), 
                '$.$jsonKey', 
                JSON_EXTRACT(?, '$')
            ) 
            WHERE $idField = ?";

        $stmt = $this->dbconnection->prepare($sql);
        if (!$stmt) {
            dieWithCode("Prepare failed: " . $this->dbconnection->error);
        }

        $idType = is_int($idValue) ? "i" : "s";
        $stmt->bind_param("s$idType", $jsonString, $idValue);

        if (!$stmt->execute()) {
            dieWithCode("Execute failed: " . $stmt->error);
        }

        $affected = $stmt->affected_rows;
        $stmt->close();
        return $affected;
    }


    public function callJsonProcedure($procName, $jsonIn, bool $noout = false)
    {
        if ($noout) {
            $sql = "CALL $procName(?)";
            $stmt = $this->dbconnection->prepare($sql);
            if (!$stmt) {
                dieWithCode("Prepare failed: {$this->dbconnection->error}");
            }
        } else {
            $this->dbconnection->query("SET @f_out = NULL");
            $sql = "CALL $procName(?, @f_out)";
            $stmt = $this->dbconnection->prepare($sql);
            if (!$stmt) {
                dieWithCode("Prepare failed: {$this->dbconnection->error}");
            }
        }

        $stmt->bind_param("s", $jsonIn);

        if (!$stmt->execute()) {
            dieWithCode("Execute failed: {$stmt->error}");
        }
        $stmt->close();

        if (!$noout) {
            $result = $this->dbconnection->query("SELECT @f_out as f_out");
            if (!$result) {
                dieWithCode("Select OUT param failed: {$this->dbconnection->error}");
            }
            $row = $result->fetch_assoc();
            return $row['f_out'] ?? null;
        }
        return true;
    }

    public function echoResult()
    {
        header("Content-Type: application/json; charset=utf-8");
        echo json_encode($this->result, JSON_UNESCAPED_UNICODE);
    }

    function ValidateParams(object $input, array $rules): object
    {
        $result = new stdClass();
        $errors = [];

        foreach ($rules as $field => $ruleStr) {

            $rulesArr = explode('|', $ruleStr);
            $value = $input->$field ?? null;

            $required = in_array('required', $rulesArr);

            if ($required && ($value === null || $value === '')) {
                $errors[] = "$field required";
                continue;
            }

            if ($value === null && in_array('nullable', $rulesArr)) {
                $result->$field = null;
                continue;
            }

            foreach ($rulesArr as $rule) {

                if (
                    $rule === 'integer' &&
                    !filter_var($value, FILTER_VALIDATE_INT) &&
                    $value !== '0'
                ) {
                    $errors[] = "$field must be integer";
                }

                if ($rule === 'numeric' && !is_numeric($value)) {
                    $errors[] = "$field must be numeric";
                }

                if ($rule === 'string' && !is_string($value)) {
                    $errors[] = "$field must be string";
                }

                if ($rule === 'boolean') {
                    $bool = filter_var($value, FILTER_VALIDATE_BOOLEAN, FILTER_NULL_ON_FAILURE);
                    if ($bool === null) {
                        $errors[] = "$field must be boolean";
                    }
                    $value = $bool;
                }

                if (str_starts_with($rule, 'max:')) {
                    $max = (int)substr($rule, 4);
                    if (strlen((string)$value) > $max) {
                        $errors[] = "$field max $max";
                    }
                }

                if (str_starts_with($rule, 'min:')) {
                    $min = (int)substr($rule, 4);
                    if ($value < $min) {
                        $errors[] = "$field min $min";
                    }
                }
            }

            $result->$field = $value;
        }

        if (!empty($errors)) {
            dieWithCode("Validation failed: " . implode(', ', $errors));
        }

        return $result;
    }
}
