<?php
# Guest menu for the web opened from the table booklet QR.
# POST /engine/v2/ararix/menu/get   { "locale": "ru", "key": "<booklet key>" }
# Staff: issue-key (per table), tables (halls/tables list)

require_once __DIR__ . "/../../worker/auth.php";
require_once __DIR__ . "/../../worker/selfboard-menu.php";
require_once __DIR__ . "/guestkey.php";

class Menu extends Auth
{
    public function auth()
    {
        $path = (string)($_SERVER["REQUEST_URI"] ?? "");
        if (preg_match('#/(issue-key|tables)(?:\\?|$)#i', $path)) {
            return parent::auth();
        }
        guest_require_table_key($this);
        return true;
    }

    public function Get($params)
    {
        $keyRow = guest_require_table_key($this);
        $withImage = $this->flag($params, "with_image", true);
        $group = (int)($this->field($params, "group") ?? 0);
        $limitRaw = $this->field($params, "limit");
        $limit = $limitRaw === null ? null : max(0, (int)$limitRaw);
        $offset = max(0, (int)($this->field($params, "offset") ?? 0));
        $payload = selfboard_menu_payload($this, $params, [
            "with_image" => $withImage,
            "group" => $group,
            "limit" => $limit,
            "offset" => $offset,
        ]);
        if (!$withImage) {
            $key = guest_presented_key();
            $this->attachImageUrls($payload["groups"], "group", $key);
            $this->attachImageUrls($payload["dishes"], "dish", $key);
            foreach ($payload["dishes"] as &$dish) {
                if (!empty($dish["f_complectation"]) && is_array($dish["f_complectation"])) {
                    $this->attachImageUrls($dish["f_complectation"], "dish", $key);
                }
            }
            unset($dish);
        }
        $table = $keyRow["table"];
        $this->result["groups"] = $payload["groups"];
        $this->result["parts"] = $payload["parts"];
        $this->result["dishes"] = $payload["dishes"];
        $this->result["locale"] = $payload["locale"];
        $this->result["dish_total"] = $payload["dish_total"];
        $this->result["limit"] = $limit === null ? $payload["dish_total"] : $limit;
        $this->result["offset"] = $offset;
        $this->result["table"] = [
            "f_id" => (int)$table["f_id"],
            "f_name" => (string)$table["f_name"],
        ];
        $this->result["hall"] = [
            "f_id" => (int)($table["f_hall"] ?? 0),
            "f_name" => (string)($table["f_hall_name"] ?? ""),
        ];
        $this->echoResult();
    }

    /**
     * GET /ararix/menu/image?dish=315&key=...
     * GET /ararix/menu/image?group=24&key=...
     */
    public function Image($params)
    {
        guest_require_table_key($this);
        $dish = (int)($this->field($params, "dish") ?? 0);
        $group = (int)($this->field($params, "group") ?? 0);
        if (($dish > 0) === ($group > 0)) {
            guest_fail(404, "NOT_FOUND", "Not found");
        }
        $raw = $dish > 0
            ? $this->dishImageRaw($dish)
            : $this->groupImageRaw($group);
        $bytes = selfboard_menu_image_bytes(selfboard_normalize_menu_image($raw));
        if ($bytes === "") {
            guest_fail(404, "NOT_FOUND", "Not found");
        }
        header("Content-Type: image/png");
        header("Content-Length: " . strlen($bytes));
        header("Cache-Control: public, max-age=86400");
        echo $bytes;
        exit;
    }

    /**
     * Staff: halls + tables for booklet printing / QRNeto mapping.
     * POST /ararix/menu/tables
     */
    public function Tables($params)
    {
        unset($params);
        $halls = $this->select(
            "SELECT f_id, f_name FROM h_halls ORDER BY f_name, f_id"
        )->fetch_all(MYSQLI_ASSOC);
        $tables = $this->select(
            "SELECT t.f_id, t.f_name, t.f_hall,
                    COALESCE(k.f_token, '') AS f_token,
                    CASE WHEN k.f_id IS NULL THEN 0 ELSE 1 END AS f_has_key
             FROM h_tables t
             LEFT JOIN external_api_key k
               ON k.f_partner = ? AND k.f_table_id = t.f_id AND k.f_enabled = 1
             ORDER BY t.f_hall, t.f_name, t.f_id",
            "s",
            [guest_partner()]
        )->fetch_all(MYSQLI_ASSOC);
        $byHall = [];
        foreach ($halls as $h) {
            $byHall[(int)$h["f_id"]] = [
                "f_id" => (int)$h["f_id"],
                "f_name" => (string)$h["f_name"],
                "tables" => [],
            ];
        }
        foreach ($tables as $t) {
            $hallId = (int)$t["f_hall"];
            if (!isset($byHall[$hallId])) {
                $byHall[$hallId] = [
                    "f_id" => $hallId,
                    "f_name" => "",
                    "tables" => [],
                ];
            }
            $byHall[$hallId]["tables"][] = [
                "f_id" => (int)$t["f_id"],
                "f_name" => (string)$t["f_name"],
                "f_has_key" => (int)$t["f_has_key"],
                "f_token" => (string)$t["f_token"],
            ];
        }
        $this->result["halls"] = array_values($byHall);
        $this->echoResult();
    }

    /**
     * Staff: issue or rotate booklet key for one table.
     * { "table_id": 5, "rotate": false }
     */
    public function IssueKey($params)
    {
        $tableId = (int)($params->table_id ?? $params->table ?? 0);
        if ($tableId <= 0) {
            guest_fail(400, "INVALID_TABLE", "table_id is required");
        }
        $table = $this->select(
            "SELECT t.f_id, t.f_name, t.f_hall, h.f_name AS f_hall_name
             FROM h_tables t
             LEFT JOIN h_halls h ON h.f_id = t.f_hall
             WHERE t.f_id = ?
             LIMIT 1",
            "i",
            [$tableId]
        )->fetch_assoc();
        if (!$table) {
            guest_fail(404, "INVALID_TABLE", "Table not found");
        }

        $rotate = !empty($params->rotate);
        $existing = $this->select(
            "SELECT f_id, f_token FROM external_api_key
             WHERE f_partner = ? AND f_table_id = ?
             LIMIT 1",
            "si",
            [guest_partner(), $tableId]
        )->fetch_assoc();
        $current = trim((string)($existing["f_token"] ?? ""));
        if ($existing && !$rotate && $current !== "") {
            $this->result["partner"] = guest_partner();
            $this->result["token"] = $current;
            $this->result["rotated"] = false;
            $this->result["table"] = [
                "f_id" => (int)$table["f_id"],
                "f_name" => (string)$table["f_name"],
            ];
            $this->result["hall"] = [
                "f_id" => (int)($table["f_hall"] ?? 0),
                "f_name" => (string)($table["f_hall_name"] ?? ""),
            ];
            $this->echoResult();
            return;
        }

        $token = bin2hex(random_bytes(16));
        $fields = [
            "f_partner" => guest_partner(),
            "f_table_id" => $tableId,
            "f_token_hash" => hash("sha256", $token),
            "f_token" => $token,
            "f_enabled" => 1,
        ];
        if ($existing) {
            $this->update("external_api_key", $fields, (int)$existing["f_id"]);
        } else {
            $this->insert("external_api_key", $fields);
        }
        $this->result["partner"] = guest_partner();
        $this->result["token"] = $token;
        $this->result["rotated"] = (bool)$existing && $rotate;
        $this->result["table"] = [
            "f_id" => (int)$table["f_id"],
            "f_name" => (string)$table["f_name"],
        ];
        $this->result["hall"] = [
            "f_id" => (int)($table["f_hall"] ?? 0),
            "f_name" => (string)($table["f_hall_name"] ?? ""),
        ];
        $this->echoResult();
    }

    private function dishImageRaw(int $dishId): string
    {
        // Prefer direct goods image; do not require menu EXISTS when image blob exists
        // (complectation / package lines share the same id space).
        $row = $this->select(
            "SELECT gi.f_image AS f_image
             FROM c_goods_images gi
             WHERE gi.f_id = ?
             LIMIT 1",
            "i",
            [$dishId]
        )->fetch_assoc();
        return (string)($row["f_image"] ?? "");
    }

    private function groupImageRaw(int $groupId): string
    {
        $row = $this->select(
            "SELECT si.f_data AS f_image
             FROM c_groups gr
             INNER JOIN s_images si ON si.f_id = gr.f_image
             WHERE gr.f_id = ?
             LIMIT 1",
            "i",
            [$groupId]
        )->fetch_assoc();
        return (string)($row["f_image"] ?? "");
    }

    private function field($params, string $name)
    {
        if (is_object($params) && isset($params->$name) && $params->$name !== "") {
            return $params->$name;
        }
        if (isset($_GET[$name]) && $_GET[$name] !== "") {
            return $_GET[$name];
        }
        return null;
    }

    private function flag($params, string $name, bool $default): bool
    {
        $value = $this->field($params, $name);
        if ($value === null) {
            return $default;
        }
        if (is_bool($value)) {
            return $value;
        }
        return !in_array(strtolower(trim((string)$value)), ["0", "false", "no"], true);
    }

    private function attachImageUrls(array &$rows, string $kind, string $key): void
    {
        foreach ($rows as &$row) {
            unset($row["f_has_image"]);
            $row["f_image"] = "";
            $id = $kind === "group"
                ? (int)($row["f_id"] ?? 0)
                : (int)($row["f_dish"] ?? $row["f_goods"] ?? 0);
            if ($id <= 0) {
                $row["f_image_url"] = "";
                continue;
            }
            $raw = $kind === "group" ? $this->groupImageRaw($id) : $this->dishImageRaw($id);
            $bytes = selfboard_menu_image_bytes(selfboard_normalize_menu_image($raw));
            if ($bytes === "") {
                $row["f_image_url"] = "";
                continue;
            }
            $query = $kind === "group" ? "group" : "dish";
            $row["f_image_url"] = "/engine/v2/ararix/menu/image?" . $query . "=" . $id
                . "&key=" . rawurlencode($key);
        }
        unset($row);
    }
}
