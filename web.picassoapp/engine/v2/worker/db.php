<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-05-25 13:31:11
# Last Modified: 2026-02-06 17:27:55

/** Raised when a store operation hit a deadlock (1213) or lock wait timeout (1205). */
class StoreLockConflict extends RuntimeException {}

class Db
{
    protected $remote_host;
    protected $dbconnection;
    protected $result = [];
    private $storeLocksHeld = [];
    private $storeSessionReady = false;
    private $transactionOpen = false;
    private $storeRetries = 0;

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
        $this->transactionOpen = true;
        return  $this->dbconnection->begin_transaction();
    }

    public function commit()
    {
        $this->transactionOpen = false;
        return $this->dbconnection->commit();
    }

    public function rollback()
    {
        $this->transactionOpen = false;
        return $this->dbconnection->rollback();
    }

    /**
     * Warehouse postings run one at a time per storage: every caller takes the same named
     * locks in the same order, so documents queue up instead of deadlocking each other.
     *
     * @param int[] $storeIds storages touched by the document (in/out)
     * @param callable $op runs inside the transaction; must not commit
     */
    public function runStoreOperation(array $storeIds, callable $op, int $attempts = 3)
    {
        $this->prepareStoreSession();
        $names = $this->storeLockNames($storeIds);
        $delayMs = 150;

        // Called inside a bigger transaction (e.g. a shop return): only serialize, because
        // starting a nested transaction here would silently commit the caller's work.
        if ($this->transactionOpen) {
            $this->acquireStoreLocks($names);
            try {
                return $op();
            } finally {
                $this->releaseStoreLocks();
            }
        }

        for ($attempt = 1; ; $attempt++) {
            $this->acquireStoreLocks($names);
            $this->beginTransaction();
            try {
                $res = $op();
                $this->commit();
                $this->releaseStoreLocks();
                return $res;
            } catch (Throwable $e) {
                $this->rollback();
                $this->releaseStoreLocks();
                $retryable = $e instanceof StoreLockConflict
                    || ($e instanceof mysqli_sql_exception && in_array((int)$e->getCode(), [1205, 1213], true));
                if (!$retryable || $attempt >= $attempts) {
                    throw $e;
                }
                $this->storeRetries++;
                error_log("store operation retry {$attempt}/{$attempts}: " . $e->getMessage());
                usleep($delayMs * 1000);
                $delayMs *= 2;
            }
        }
    }

    /** How many times a store operation had to be replayed after a lock conflict. */
    public function storeRetryCount(): int
    {
        return $this->storeRetries;
    }

    /**
     * Calls a store2 function and decodes its JSON result. Lock conflicts are converted to
     * StoreLockConflict so runStoreOperation can replay the whole document.
     */
    public function callStoreFunction(string $fn, string $jsonParams): array
    {
        try {
            $row = $this->select("select {$fn}(?) as result", "s", [$jsonParams])->fetch_assoc();
        } catch (mysqli_sql_exception $e) {
            if (in_array((int)$e->getCode(), [1205, 1213], true)) {
                throw new StoreLockConflict($e->getMessage(), (int)$e->getCode(), $e);
            }
            throw $e;
        }
        if (!$row || !isset($row["result"])) {
            return ["status" => 1, "msg" => "Database function returned nothing"];
        }
        $result = json_decode($row["result"], true);
        if (!is_array($result)) {
            return ["status" => 1, "msg" => "Invalid function result"];
        }
        return $result;
    }

    /**
     * READ COMMITTED drops the gap/next-key locks that row-by-row FIFO postings would
     * otherwise take on whole tables; the longer timeout lets a queued document wait.
     */
    private function prepareStoreSession(): void
    {
        if ($this->storeSessionReady) {
            return;
        }
        $this->storeSessionReady = true;
        $this->dbconnection->query("set session innodb_lock_wait_timeout=120");
        $row = $this->dbconnection
            ->query("select @@global.log_bin as log_bin, @@session.binlog_format as fmt")
            ->fetch_assoc();
        $statementBinlog = (int)($row["log_bin"] ?? 0) === 1
            && strtoupper((string)($row["fmt"] ?? "")) === "STATEMENT";
        if (!$statementBinlog) {
            $this->dbconnection->query("set session transaction isolation level read committed");
        }
    }

    private function storeLockNames(array $storeIds): array
    {
        global $dbname;
        $ids = [];
        foreach ($storeIds as $id) {
            $id = (int)$id;
            if ($id > 0) {
                $ids[$id] = $id;
            }
        }
        if (empty($ids)) {
            $ids[0] = 0;
        }
        sort($ids, SORT_NUMERIC);
        return array_map(fn($id) => "store_post:{$dbname}:{$id}", $ids);
    }

    private function acquireStoreLocks(array $names, int $timeout = 120): void
    {
        foreach ($names as $name) {
            $escaped = $this->dbconnection->real_escape_string($name);
            $row = $this->dbconnection->query("select get_lock('{$escaped}', {$timeout}) as l")->fetch_assoc();
            if ((int)($row["l"] ?? 0) !== 1) {
                $this->releaseStoreLocks();
                dieWithCode(Translator::t("Warehouse is busy, please repeat the operation"));
            }
            $this->storeLocksHeld[] = $name;
        }
    }

    private function releaseStoreLocks(): void
    {
        while ($name = array_pop($this->storeLocksHeld)) {
            $escaped = $this->dbconnection->real_escape_string($name);
            $this->dbconnection->query("select release_lock('{$escaped}')");
        }
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

        try {
            if (!$stmt->execute()) {
                dieWithCode($stmt->error);
            }
        } catch (mysqli_sql_exception $e) {
            dieWithCode($e->getMessage());
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
            // For scalar values (e.g. string date) JSON_EXTRACT expects valid JSON.
            // Encode it to JSON literal: "2026-08-18 12:14:00", 123, true, null...
            $jsonString = json_encode($newData, JSON_UNESCAPED_UNICODE);
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

                if ($rule === 'integer') {
                    $intVal = filter_var($value, FILTER_VALIDATE_INT, FILTER_NULL_ON_FAILURE);
                    if ($intVal === null) {
                        $errors[] = "$field must be integer";
                    } else {
                        $value = (int)$intVal;
                    }
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
                    if (is_string($value) || in_array('string', $rulesArr, true)) {
                        if (mb_strlen((string)$value, 'UTF-8') > $max) {
                            $errors[] = "$field max $max";
                        }
                    } elseif (is_numeric($value) && (float)$value > $max) {
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
