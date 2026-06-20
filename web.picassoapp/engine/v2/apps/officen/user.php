<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-06-20 00:32:00

require_once __DIR__ . "/index.php";

class User extends Auth
{
    private function userId($params): int
    {
        $userId = (int)($params->user_id ?? $params->f_user ?? 0);
        if ($userId <= 0) {
            dieWithCode("User is required");
        }
        return $userId;
    }

    public function Open($params)
    {
        $userId = $this->userId($params);

        $sql = <<<SQL
        SELECT
            u.f_id,
            CONCAT(u.f_last, ' ', u.f_first) AS f_name,
            fp.f_size,
            fp.f_updated
        FROM s_user u
        LEFT JOIN s_user_fingerprint fp ON fp.f_user = u.f_id
        WHERE u.f_id = ?
        SQL;

        $row = $this->select($sql, "i", [$userId])->fetch_assoc();
        if (empty($row)) {
            dieWithCode("User not found");
        }

        $this->result["user_id"] = (int)$row["f_id"];
        $this->result["f_name"] = (string)$row["f_name"];
        $this->result["has_fingerprint"] = !empty($row["f_size"]);
        $this->result["f_size"] = (int)($row["f_size"] ?? 0);
        $this->result["f_updated"] = (string)($row["f_updated"] ?? "");
        $this->echoResult();
    }

    public function LoadFingerprint($params)
    {
        $userId = $this->userId($params);

        $row = $this->select(
            "SELECT f_template, f_size, f_updated FROM s_user_fingerprint WHERE f_user = ?",
            "i",
            [$userId]
        )->fetch_assoc();

        if (empty($row)) {
            $this->result["has_fingerprint"] = false;
            $this->echoResult();
            return;
        }

        $this->result["has_fingerprint"] = true;
        $this->result["f_size"] = (int)$row["f_size"];
        $this->result["f_updated"] = (string)$row["f_updated"];
        $this->result["template"] = $this->templatePayload((string)$row["f_template"], (int)$row["f_size"]);
        $this->echoResult();
    }

    private function templatePayload(string $stored, int $size): string
    {
        if ($stored === "") {
            return "";
        }

        // New format: ASCII base64 stored in f_template.
        $decoded = base64_decode($stored, true);
        if ($decoded !== false && $size > 0 && strlen($decoded) === $size) {
            return $stored;
        }

        // Legacy format: raw binary blob.
        if ($size > 0 && strlen($stored) === $size) {
            return base64_encode($stored);
        }

        return base64_encode($stored);
    }

    public function SaveFingerprint($params)
    {
        $userId = $this->userId($params);
        $templateB64 = (string)($params->template ?? "");
        if ($templateB64 === "") {
            dieWithCode("Fingerprint template is required");
        }

        $template = base64_decode($templateB64, true);
        if ($template === false || $template === "") {
            dieWithCode("Invalid fingerprint template");
        }

        $size = (int)($params->size ?? strlen($template));
        if ($size <= 0 || $size > strlen($template)) {
            $size = strlen($template);
        }

        $exists = $this->select("SELECT f_user FROM s_user_fingerprint WHERE f_user = ?", "i", [$userId])->fetch_assoc();
        if (empty($exists)) {
            $this->insert("s_user_fingerprint", [
                "f_user" => $userId,
                "f_template" => $templateB64,
                "f_size" => $size,
            ]);
        } else {
            $this->update("s_user_fingerprint", [
                "f_template" => $templateB64,
                "f_size" => $size,
            ], $userId, "f_user");
        }

        $this->result["saved"] = true;
        $this->result["f_size"] = $size;
        $this->echoResult();
    }

    public function RemoveFingerprint($params)
    {
        $userId = $this->userId($params);
        $this->delete("s_user_fingerprint", $userId, "f_user");
        $this->result["removed"] = true;
        $this->echoResult();
    }

    public function ListFingerprints($params)
    {
        unset($params);
        $rows = $this->select(
            <<<SQL
            SELECT
                fp.f_user,
                fp.f_template,
                fp.f_size,
                CONCAT(u.f_last, ' ', u.f_first) AS f_name
            FROM s_user_fingerprint fp
            INNER JOIN s_user u ON u.f_id = fp.f_user
            SQL
        )->fetch_all(MYSQLI_ASSOC);

        $items = [];
        foreach ($rows as $row) {
            $size = (int)$row["f_size"];
            $items[] = [
                "user_id" => (int)$row["f_user"],
                "f_name" => (string)$row["f_name"],
                "f_size" => $size,
                "template" => $this->templatePayload((string)$row["f_template"], $size),
            ];
        }

        $this->result["items"] = $items;
        $this->echoResult();
    }

    public function FingerprintLogin($params)
    {
        require_once __DIR__ . "/../../worker/uuid.php";

        $userId = $this->userId($params);

        $registered = $this->select(
            "SELECT f_user FROM s_user_fingerprint WHERE f_user = ?",
            "i",
            [$userId]
        )->fetch_assoc();
        if (empty($registered)) {
            dieWithCode("Fingerprint not registered for this user");
        }

        $row = $this->select("SELECT * FROM s_user WHERE f_id = ?", "i", [$userId])->fetch_assoc();
        if (empty($row)) {
            dieWithCode("User not found");
        }

        $sessionKey = uuid_v4();
        $this->insert("s_login_session", [
            "f_session" => $sessionKey,
            "f_user" => $userId,
            "f_iplogin" => json_encode([$_SERVER['HTTP_X_FORWARDED_FOR'] ?? "0.0.0.0", $_SERVER['REMOTE_ADDR']]),
            "f_datestart" => date("Y-m-d"),
            "f_timestart" => date("H:i:s"),
        ]);

        $permissions = $this->select(
            "SELECT f_key FROM s_user_access WHERE f_group = ? AND f_value = 1",
            "i",
            [$row["f_group"]]
        )->fetch_all(MYSQLI_NUM);
        $permissions = array_column($permissions, 0);

        $settings = $this->select(
            "SELECT f_key, f_value FROM s_settings_values WHERE f_settings = ?",
            "i",
            [$row["f_config"]]
        )->fetch_all(MYSQLI_ASSOC);

        $rowConfig = $this->select(
            "SELECT f_config FROM sys_json_config WHERE f_id = ?",
            "i",
            [$row["f_config"]]
        )->fetch_assoc();
        if (empty($rowConfig)) {
            $rowConfig = ["f_config" => ["dashboard" => "none"]];
        } else {
            $rowConfig = ["f_config" => (array) json_decode($rowConfig["f_config"] ?? "{}")];
        }

        $cashsession = $this->select(
            "SELECT f_id FROM s_working_sessions WHERE f_close IS NULL"
        )->fetch_assoc();
        if (empty($cashsession)) {
            $cashsession = ["f_id" => 0];
        }

        $menuversion = $this->select(
            "SELECT f_version FROM s_app WHERE f_app = 'menu'"
        )->fetch_row()[0] ?? null;
        if ($menuversion !== null) {
            $rowConfig["f_config"] = array_merge(
                (array) $rowConfig["f_config"],
                ["menuversion" => $menuversion]
            );
        }

        $cardPattern = $this->select("SELECT * FROM b_card_types")->fetch_all(MYSQLI_ASSOC);
        $map = [
            1 => "code_pattern_discount_card",
            4 => "code_pattern_accumulate_card",
            10 => "code_pattern_present_card",
        ];
        foreach ($cardPattern as $cp) {
            $id = (int)$cp["f_id"];
            if (isset($map[$id])) {
                $rowConfig["f_config"][$map[$id]] = $cp["f_pattern"];
            }
        }

        $active = $this->select(
            <<<SQL
            SELECT f_id
            FROM s_attendance
            WHERE f_worker = ?
              AND f_out IS NULL
              AND f_in > NOW() - INTERVAL 16 HOUR
            LIMIT 1
            SQL,
            "i",
            [$row["f_id"]]
        )->fetch_assoc();

        unset($row["f_password"], $row["f_altpassword"]);

        $this->result["active"] = !empty($active);
        $this->result["sessionkey"] = $sessionKey;
        $this->result["user"] = $row;
        $this->result["permissions"] = $permissions;
        $this->result["config"] = $rowConfig;
        $this->result["settings"] = $settings;
        $this->result["cashsession"] = $cashsession;
        $this->echoResult();
    }
}
