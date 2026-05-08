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

    public function Checkin($params)
    {
        $active = $this->select(
            "select * from s_attendance where f_worker=? and f_out is null order by f_id desc limit 1",
            "i",
            [$this->userid]
        )->fetch_assoc();
        if (!empty($active)) {
            dieWithCode(Translator::t("Already checked in"));
        }

        $v = [];
        $v["f_date"] = date("Y-m-d");
        $v["f_worker"] = (int)$this->userid;
        $v["f_position"] = (int)($this->user["f_group"] ?? 0);
        $v["f_in"] = date("Y-m-d H:i:s");
        $v["f_state"] = 1;
        $v["f_break_minutes"] = 0;
        if (!empty($params->comment)) {
            $v["f_comment"] = (string)$params->comment;
        }
        $attendanceId = $this->insert("s_attendance", $v);
        $this->result["attendance"] = $this->select("select * from s_attendance where f_id=?", "i", [$attendanceId])->fetch_assoc();
        $this->echoResult();
    }

    public function Checkout($params)
    {
        $active = $this->select(
            "select * from s_attendance where f_worker=? and f_out is null order by f_id desc limit 1",
            "i",
            [$this->userid]
        )->fetch_assoc();
        if (empty($active)) {
            dieWithCode(Translator::t("No active checkin"));
        }

        $v = [];
        $v["f_out"] = date("Y-m-d H:i:s");
        $v["f_state"] = 0;
        if (!empty($params->comment)) {
            $v["f_comment"] = (string)$params->comment;
        }
        $this->update("s_attendance", $v, $active["f_id"]);
        $this->result["attendance"] = $this->select("select * from s_attendance where f_id=?", "i", [$active["f_id"]])->fetch_assoc();
        $this->echoResult();
    }
}
