<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-09 13:32:12
# Last Modified: 2026-02-09 13:32:16

require_once __DIR__ . "/index.php";

class Attendance extends Auth
{
    public function IsCheckin($workerId)
    {
        $active = $this->select(
            "SELECT f_id 
         FROM s_attendance 
         WHERE f_worker = ? 
           AND f_out IS NULL 
           AND f_in > NOW() - INTERVAL 16 HOUR 
         LIMIT 1",
            "i",
            [$workerId]
        )->fetch_assoc();

        return !empty($active);
    }

    private function findOpenAttendance(int $workerId): ?array
    {
        $active = $this->select(
            "SELECT * FROM s_attendance WHERE f_worker = ? AND f_out IS NULL ORDER BY f_id DESC LIMIT 1",
            "i",
            [$workerId]
        )->fetch_assoc();

        return $active ?: null;
    }

    private function closeOpenAttendance(array $active, ?string $comment = null): void
    {
        $v = [
            "f_out" => date("Y-m-d H:i:s"),
            "f_state" => 0,
        ];
        if ($comment !== null && $comment !== "") {
            $v["f_comment"] = $comment;
        }
        $this->update("s_attendance", $v, $active["f_id"]);
    }

    public function Open($params)
    {
        $this->result["user_id"] = (int)$this->userid;
        $this->result["active"] = $this->IsCheckin($this->userid);
        $this->echoResult();
    }

    public function Checkin($params)
    {
        if ($this->IsCheckin($this->userid)) {
            dieWithCode(Translator::t("Already checked in"));
        }

        $comment = !empty($params->comment) ? (string)$params->comment : null;
        $stale = $this->findOpenAttendance($this->userid);
        if ($stale !== null) {
            $this->closeOpenAttendance($stale, $comment);
        }

        $v = [];
        $v["f_date"] = date("Y-m-d");
        $v["f_worker"] = (int)$this->userid;
        $v["f_position"] = (int)($this->user["f_group"] ?? 0);
        $v["f_in"] = date("Y-m-d H:i:s");
        $v["f_state"] = 1;
        $v["f_break_minutes"] = 0;
        if ($comment !== null) {
            $v["f_comment"] = $comment;
        }
        $attendanceId = $this->insert("s_attendance", $v);
        $this->result["attendance"] = $this->select("select * from s_attendance where f_id=?", "i", [$attendanceId])->fetch_assoc();
        $this->result["active"] = true;
        $this->echoResult();
    }

    public function Checkout($params)
    {
        if (!$this->IsCheckin($this->userid)) {
            dieWithCode(Translator::t("No active checkin"));
        }

        $active = $this->findOpenAttendance($this->userid);
        if ($active === null) {
            dieWithCode(Translator::t("No active checkin"));
        }

        $comment = !empty($params->comment) ? (string)$params->comment : null;
        $this->closeOpenAttendance($active, $comment);
        $this->result["attendance"] = $this->select("select * from s_attendance where f_id=?", "i", [$active["f_id"]])->fetch_assoc();
        $this->result["active"] = false;
        $this->echoResult();
    }

    public function VerifyPin($params)
    {
        $pin = (string)($params->pin ?? "");
        if ($pin === "") {
            dieWithCode("PIN is required");
        }

        $row = $this->select(
            "SELECT f_id, f_first, f_last FROM s_user WHERE f_altpassword = MD5(?)",
            "s",
            [$pin]
        )->fetch_assoc();
        if (empty($row)) {
            dieWithCode(Translator::t("Access denied"), 401);
        }

        $userId = (int)$row["f_id"];
        $this->result["user_id"] = $userId;
        $this->result["f_name"] = trim($row["f_last"] . " " . $row["f_first"]);
        $this->result["active"] = $this->IsCheckin($userId);
        $this->echoResult();
    }

    public function VerifyLogin($params)
    {
        $login = trim((string)($params->login ?? ""));
        $password = (string)($params->password ?? "");
        if ($login === "" || $password === "") {
            dieWithCode(Translator::t("Access denied"), 401);
        }

        $row = $this->select(
            "SELECT f_id, f_first, f_last FROM s_user WHERE f_login = ? AND f_password = MD5(?)",
            "ss",
            [$login, $password]
        )->fetch_assoc();
        if (empty($row)) {
            dieWithCode(Translator::t("Access denied"), 401);
        }

        $userId = (int)$row["f_id"];
        $this->result["user_id"] = $userId;
        $this->result["f_name"] = trim($row["f_last"] . " " . $row["f_first"]);
        $this->result["active"] = $this->IsCheckin($userId);
        $this->echoResult();
    }

    public function CheckedIn($params)
    {
        $rows = $this->select(
            "SELECT u.f_id, u.f_group, concat(u.f_last, ' ', u.f_first) as f_name,
                    to_base64(p.f_data) as f_photo
             FROM s_attendance a
             INNER JOIN s_user u ON u.f_id = a.f_worker
             LEFT JOIN s_user_photo p ON p.f_id = u.f_id
             WHERE a.f_out IS NULL AND a.f_state = 1
               AND a.f_in > NOW() - INTERVAL 16 HOUR
             ORDER BY u.f_last, u.f_first"
        )->fetch_all(MYSQLI_ASSOC);

        $this->result["users"] = $rows ?: [];
        $this->echoResult();
    }

    public function OpenUser($params)
    {
        $userId = (int)($params->user_id ?? 0);
        if ($userId <= 0) {
            dieWithCode("User is required");
        }

        $row = $this->select(
            "SELECT f_id, f_first, f_last FROM s_user WHERE f_id = ?",
            "i",
            [$userId]
        )->fetch_assoc();
        if (empty($row)) {
            dieWithCode("User not found");
        }

        $this->result["user_id"] = $userId;
        $this->result["f_name"] = trim($row["f_last"] . " " . $row["f_first"]);
        $this->result["active"] = $this->IsCheckin($userId);
        $this->echoResult();
    }

    public function ToggleForUser($params)
    {
        $userId = (int)($params->user_id ?? 0);
        if ($userId <= 0) {
            dieWithCode("User is required");
        }

        $user = $this->select("SELECT f_id, f_group FROM s_user WHERE f_id = ?", "i", [$userId])->fetch_assoc();
        if (empty($user)) {
            dieWithCode("User not found");
        }

        $comment = !empty($params->comment) ? (string)$params->comment : null;

        if ($this->IsCheckin($userId)) {
            $active = $this->findOpenAttendance($userId);
            if ($active === null) {
                dieWithCode(Translator::t("No active checkin"));
            }
            $this->closeOpenAttendance($active, $comment);
            $this->result["action"] = "checkout";
            $this->result["attendance"] = $this->select(
                "SELECT * FROM s_attendance WHERE f_id = ?",
                "i",
                [$active["f_id"]]
            )->fetch_assoc();
        } else {
            $stale = $this->findOpenAttendance($userId);
            if ($stale !== null) {
                $this->closeOpenAttendance($stale, $comment);
            }

            $v = [
                "f_date" => date("Y-m-d"),
                "f_worker" => $userId,
                "f_position" => (int)($user["f_group"] ?? 0),
                "f_in" => date("Y-m-d H:i:s"),
                "f_state" => 1,
                "f_break_minutes" => 0,
            ];
            if ($comment !== null) {
                $v["f_comment"] = $comment;
            }
            $attendanceId = $this->insert("s_attendance", $v);
            $this->result["action"] = "checkin";
            $this->result["attendance"] = $this->select(
                "SELECT * FROM s_attendance WHERE f_id = ?",
                "i",
                [$attendanceId]
            )->fetch_assoc();
        }

        $this->result["user_id"] = $userId;
        $this->result["active"] = $this->IsCheckin($userId);
        $this->echoResult();
    }
}
