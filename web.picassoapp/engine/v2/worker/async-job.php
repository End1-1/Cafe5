<?php
# © 2026 , Kudryashov Vasili
# Long operations behind Cloudflare (524 after ~125 s): the request answers at once with a
# job id, the same PHP process keeps working, and the client polls common/jobs/status.

require_once __DIR__ . "/uuid.php";

const ASYNC_JOB_QUEUED = 0;
const ASYNC_JOB_RUNNING = 1;
const ASYNC_JOB_DONE = 2;
const ASYNC_JOB_ERROR = 3;

function async_job_create(Db $db, string $type, int $userId, $params): string
{
    $id = uuid_v4();
    $db->insert("async_job", [
        "f_id" => $id,
        "f_type" => $type,
        "f_user" => $userId,
        "f_status" => ASYNC_JOB_QUEUED,
        "f_params" => json_encode($params, JSON_UNESCAPED_UNICODE),
    ]);
    return $id;
}

/** Sends the JSON body and closes the HTTP response; the script keeps running afterwards. */
function async_job_respond_and_continue(array $payload): void
{
    ignore_user_abort(true);
    set_time_limit(0);
    $body = json_encode($payload, JSON_UNESCAPED_UNICODE);
    while (ob_get_level() > 0) {
        ob_end_clean();
    }
    header("Content-Type: application/json; charset=utf-8");
    header("Content-Length: " . strlen($body));
    header("Connection: close");
    echo $body;
    if (function_exists("fastcgi_finish_request")) {
        fastcgi_finish_request();
    } else {
        flush();
    }
}

/**
 * Runs $work and stores its array result or error message in async_job.
 * dieWithCode()/fatal errors inside $work end the script; the shutdown hook records them.
 */
function async_job_run(Db $db, string $jobId, callable $work): void
{
    $finished = false;
    register_shutdown_function(function () use ($db, $jobId, &$finished) {
        if ($finished) {
            return;
        }
        $err = error_get_last();
        $msg = $err !== null ? (string)$err["message"] : "Job was interrupted";
        try {
            $db->rollback();
            $db->select(
                "update async_job set f_status=?, f_error=?, f_finished=now() where f_id=? and f_status<?",
                "issi",
                [ASYNC_JOB_ERROR, $msg, $jobId, ASYNC_JOB_DONE],
                true
            );
        } catch (Throwable $e) {
            error_log("async_job {$jobId}: " . $e->getMessage());
        }
    });

    $db->select(
        "update async_job set f_status=?, f_started=now() where f_id=?",
        "is",
        [ASYNC_JOB_RUNNING, $jobId],
        true
    );
    try {
        $result = $work();
        $db->select(
            "update async_job set f_status=?, f_result=?, f_finished=now() where f_id=?",
            "iss",
            [ASYNC_JOB_DONE, json_encode($result ?? [], JSON_UNESCAPED_UNICODE), $jobId],
            true
        );
    } catch (Throwable $e) {
        $db->select(
            "update async_job set f_status=?, f_error=?, f_finished=now() where f_id=?",
            "iss",
            [ASYNC_JOB_ERROR, $e->getMessage(), $jobId],
            true
        );
    }
    $finished = true;
}
