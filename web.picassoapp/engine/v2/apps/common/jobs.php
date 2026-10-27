<?php
# © 2026 , Kudryashov Vasili
# Status of background jobs started by long endpoints (see worker/async-job.php).

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/async-job.php";

class Jobs extends Auth
{
    public function status($params)
    {
        $id = (string)($params->id ?? "");
        if ($id === "") {
            dieWithCode(Translator::t("Job id is required"));
        }
        $row = $this->select(
            "select f_status, f_result, f_error from async_job where f_id=? and f_user=?",
            "si",
            [$id, $this->currentUserId()]
        )->fetch_assoc();
        if (!$row) {
            dieWithCode(Translator::t("Job not found"), 404);
        }
        $status = (int)$row["f_status"];
        $this->result["job_status"] = $status;
        $this->result["done"] = $status >= ASYNC_JOB_DONE;
        if ($status === ASYNC_JOB_DONE) {
            $this->result["result"] = json_decode($row["f_result"] ?? "{}", true) ?: new stdClass();
        } elseif ($status === ASYNC_JOB_ERROR) {
            $this->result["error"] = (string)($row["f_error"] ?? "");
        }
        $this->echoResult();
    }
}
