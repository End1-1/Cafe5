<?php
# Key printed in the table booklet QR (one key per table).
# Gates guest menu/order. Not the QRNeto tracking key.

require_once __DIR__ . "/guest-error.php";

function guest_partner(): string
{
    return "guest";
}

function guest_presented_key(): string
{
    global $bearerToken, $jsonParams;
    $bearer = trim((string)($bearerToken ?? ""));
    if ($bearer !== "") {
        return $bearer;
    }
    $query = trim((string)($_GET["key"] ?? $_POST["key"] ?? ""));
    if ($query !== "") {
        return $query;
    }
    if (isset($jsonParams) && is_object($jsonParams) && isset($jsonParams->key)) {
        return trim((string)$jsonParams->key);
    }
    $raw = file_get_contents("php://input");
    if (!is_string($raw) || trim($raw) === "") {
        return "";
    }
    $json = json_decode($raw);
    if (!is_object($json) || !isset($json->key)) {
        return "";
    }
    return trim((string)$json->key);
}

/**
 * @return array{f_id:int,f_table_id:int,f_enabled:int,f_token:?string}|null
 */
function guest_key_row($db): ?array
{
    $key = guest_presented_key();
    if ($key === "") {
        return null;
    }
    $row = $db->select(
        "SELECT f_id, f_table_id, f_enabled, f_token
         FROM external_api_key
         WHERE f_enabled = 1 AND f_partner = ? AND f_token_hash = ?
         LIMIT 1",
        "ss",
        [guest_partner(), hash("sha256", $key)]
    )->fetch_assoc();
    return $row ?: null;
}

/** Require a booklet key bound to a real table. */
function guest_require_table_key($db): array
{
    $key = guest_presented_key();
    if ($key === "") {
        guest_fail(401, "UNAUTHORIZED", "Unauthorized");
    }
    $row = guest_key_row($db);
    if (!$row) {
        guest_fail(403, "FORBIDDEN", "Forbidden");
    }
    $tableId = (int)($row["f_table_id"] ?? 0);
    if ($tableId <= 0) {
        guest_fail(403, "INVALID_TABLE", "Booklet key is not bound to a table");
    }
    $table = $db->select(
        "SELECT t.f_id, t.f_name, t.f_hall, h.f_name AS f_hall_name
         FROM h_tables t
         LEFT JOIN h_halls h ON h.f_id = t.f_hall
         WHERE t.f_id = ?
         LIMIT 1",
        "i",
        [$tableId]
    )->fetch_assoc();
    if (!$table) {
        guest_fail(403, "INVALID_TABLE", "Table not found");
    }
    $row["table"] = $table;
    return $row;
}
