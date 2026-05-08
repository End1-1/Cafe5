<?php
require_once __DIR__ . "/production.php";


    $v["f_state"] = 1;
    $v["f_datecreate"] = date("Y-m-d");
    $v["f_timecreate"] = date("H:i:s");
    $v["f_user"] = 1;
    $v["f_product"] = $params->productid;
    $v["f_qty"] = $params->qty;
    $v["f_ready"] = 0;
    $v["f_workshop"] = $params->workshopid;
    $v["f_stage"] = $params->stage;
    $taskid = sinsert("mf_tasks", $v);
    if ($taskid == 0) {
        exitError("Could not create task");
    }
    printResult(1, ["id"=>$taskid]);