<?php
# (C) 2024-2025  Kudryashov Vasili
# Created 18/12/24 23:59 
# 14/02/25 09:30 - append mysql data types 8,9

define("APP", 1);
require_once __DIR__ . "/config.php";

$sk = '5cfafe13-a886-11ee-ac3e-1078d2d2b808';
$ct = $_SERVER["CONTENT_TYPE"] ?? $_SERVER["HTTP_CONTENT_TYPE"] ?? "";
if (!str_contains($ct, "application/json")) {
    http_response_code(415);
    info_out(["error" => "Wrong Content-Type", "ct" => $ct]);
    exit;
}
$requestStr = file_get_contents("php://input");
$str = json_decode($requestStr, true);

if ($str["sk"] != $sk) {
        echo "Go away, ugly vimpire!";
        exit();
}
$time_start = microtime(true);

/**
 * Id of the INSERT the client just sent. Returned as a string so old Smart can read it.
 */
function info_new_insert_id(mysqli $db): string
{
    $newId = (int)$db->insert_id;
    if ($newId <= 0) {
        $lid = $db->query("SELECT LAST_INSERT_ID()");
        if ($lid) {
            $row = $lid->fetch_row();
            $newId = (int)($row[0] ?? 0);
        }
    }
    return (string)$newId;
}

/** Marker so a raw response shows this file is the one on the server. */
function info_out(array $payload): void
{
    $payload["rev"] = "idstr1";
    echo json_encode($payload, JSON_UNESCAPED_UNICODE);
}

if ($str["call"] == "sql") {
        try {
                $result = $db->query($str["sql"]);
                $time_query = microtime(true);
                if (!$result) {
                        echo $db->error;
                        exit();
                }
        } catch (Throwable $e) {
                echo $e->getMessage();
                exit();
        }
        if (
                strcasecmp(substr($str["sql"], 0, 4), "call") == 0
                || strcasecmp(substr($str["sql"], 0, 6), "delete") == 0
                || strcasecmp(substr($str["sql"], 0, 6), "update") == 0
                || strcasecmp(substr($str["sql"], 0, 6), "insert") == 0
        ) {
                $r = "";
                if (strcasecmp(substr($str["sql"], 0, 6), "insert") == 0) {
                        $r = info_new_insert_id($db);
                        $time_query = microtime(true);
                        info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                        return;
                } else if (strcasecmp(substr($str["sql"], 0, 4), "call") == 0) {
                        if (gettype($result) == "boolean") {
                                $time_query = microtime(true);
                                info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                                return;
                        } else {
                                $fields = $result->fetch_fields();
                                if (count($fields) == 0) {
                                        $time_query = microtime(true);
                                        info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                                        return;
                                }
                        }
                } else {
                        $time_query = microtime(true);
                        info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                        return;
                }
        }
        $fields = $result->fetch_fields();
        $i = 0;
        $colname = [];
        $namecol = [];
        $types = [];
        foreach ($fields as $f) {
                array_push($colname, ["name" => strtolower($f->name), "value" => $i]);
                array_push($namecol, ["name" => "$i", "value" => strtolower($f->name)]);
                array_push($types, $f->type);
                $i++;
        }
        $rows = [];
        while ($row = $result->fetch_row()) {
                $r = [];
                for ($i = 0; $i < count($types); $i++) {
                        switch ($types[$i]) {
                                case 3:
                                case 8:
                                case 9:
                                        array_push($r, intval($row[$i]));
                                        break;
                                case 4:
                                case 5:
                                case 246:
                                        array_push($r, floatval($row[$i]));
                                        break;
                                default:
                                        array_push($r, $row[$i]);
                                        break;
                        }
                }
                array_push($rows, $r);
        }
        $time_end = microtime(true);
        $data = [
                "columns" => array(
                        "column_index_name" => $colname,
                        "column_name_index" => $namecol
                ),
                "data" => $rows,
                "types" => $types,
                "execution_time" => $time_end - $time_start,
                "query_time" => $time_query - $time_start
        ];
        $out = ["status" => 1, "data" => $data];
        info_out($out);
        return;
}

if ($str["call"] == "sqllist") {
        $time_query = microtime(true);
        $sqltimes = [];
        $sqllist = explode(";;;", $str["sql"]);



        $sqlnum = 1;
        $db->begin_transaction();
        foreach ($sqllist as $sql) {
                try {

                        if ($sql == end($sqllist)) {
                                try {
                                        $result = $db->query($sql);
                                        $time_query = microtime(true);
                                        if (!$result) {
                                                echo $db->error;
                                                exit();
                                        }
                                } catch (Throwable $e) {
                                        $db->rollback();
                                        echo $e->getMessage();
                                        exit();
                                }
                                if (
                                        strcasecmp(substr($sql, 0, 4), "call") == 0
                                        || strcasecmp(substr($sql, 0, 6), "delete") == 0
                                        || strcasecmp(substr($sql, 0, 6), "update") == 0
                                        || strcasecmp(substr($sql, 0, 6), "insert") == 0
                                ) {
                                        $r = "";
                                        if (strcasecmp(substr($sql, 0, 6), "insert") == 0) {
                                                $r = info_new_insert_id($db);
                                                $db->commit();
                                                $time_query = microtime(true);
                                                info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                                                return;
                                        } else if (strcasecmp(substr($sql, 0, 4), "call") == 0) {
                                                if (gettype($result) == "boolean") {
                                                        $db->commit();
                                                        $time_query = microtime(true);
                                                        info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                                                        return;
                                                } else {
                                                        $fields = $result->fetch_fields();
                                                        if (count($fields) == 0) {
                                                                $db->commit();
                                                                $time_query = microtime(true);
                                                                info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                                                                return;
                                                        }
                                                }
                                        } else {
                                                $db->commit();
                                                $time_query = microtime(true);
                                                info_out(array("status" => 1, "data" => $r, "query_time" => $time_query - $time_start));
                                                return;
                                        }
                                }
                                $fields = $result->fetch_fields();
                                $i = 0;
                                $colname = [];
                                $namecol = [];
                                $types = [];
                                foreach ($fields as $f) {
                                        array_push($colname, ["name" => strtolower($f->name), "value" => $i]);
                                        array_push($namecol, ["name" => "$i", "value" => strtolower($f->name)]);
                                        array_push($types, $f->type);
                                        $i++;
                                }
                                $rows = [];
                                while ($row = $result->fetch_row()) {
                                        $r = [];
                                        for ($i = 0; $i < count($types); $i++) {
                                                switch ($types[$i]) {
                                                        case 3:
                                                        case 8:
                                                        case 9:
                                                                array_push($r, intval($row[$i]));
                                                                break;
                                                        case 4:
                                                        case 5:
                                                        case 246:
                                                                array_push($r, floatval($row[$i]));
                                                                break;
                                                        default:
                                                                array_push($r, $row[$i]);
                                                                break;
                                                }
                                        }
                                        array_push($rows, $r);
                                }
                                $db->commit();
                                $time_end = microtime(true);
                                $data = [
                                        "columns" => array(
                                                "column_index_name" => $colname,
                                                "column_name_index" => $namecol
                                        ),
                                        "data" => $rows,
                                        "types" => $types,
                                        "execution_time" => $time_end - $time_start,
                                        "query_time" => $time_query - $time_start
                                ];
                                $out = ["status" => 1, "data" => $data];
                                info_out($out);
                                return;
                        } else {
                                $thistime = microtime(true);
                                $db->query($sql);
                                if (!empty($str["debug"])) {
                                        $sqltimes[strval($sqlnum)] = ["sql" => $sql, "time" => microtime(true) - $thistime];
                                }
                                $sqlnum++;
                        }

                } catch (Throwable $e) {
                        $db->rollback();
                        echo $e->getMessage();
                        exit();
                }
        }
        $db->commit();
        $out = ["status" => 1, "data" => $sqltimes];
        info_out($out);
        return;
}

echo "Are you crazy?";