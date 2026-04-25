<?php
require_once (__DIR__ . "/production.php");

if (empty($params->mode)) {
    $params->mode = 0;
}

if ($params->mode == 1) {
    $main = stmtall("select * from mf_actions_group where f_id=?", "i", [$params->f_id])->fetch_assoc();
    $process = stmtall("select p.f_id, p.f_process, ac.f_name as f_processname, p.f_durationsec, p.f_goalprice, p.f_price, p.f_process "
        . "from mf_process p "
        . "left join mf_actions ac on ac.f_id=p.f_process "
        . "where p.f_product=? "
        . "order by p.f_rowid ", "i", [$params->f_id])->fetch_all(MYSQLI_ASSOC);
    printResult(1, ["main"=>$main, "process"=>$process]);
    return;
}

if ($params->mode == 2) {
    $db->begin_transaction();

    $isnew = empty($params->main->f_id);
    $v["f_name"] = $params->main->f_name;
    $v["f_data"] = json_encode($params->body);
    $params->main->f_id = sinsertupdate("mf_actions_group", $v, $params->main->f_id, $isnew);

    stmtall("delete from mf_process where f_product=?", "i", [$params->main->f_id]);
    foreach ($params->process as $p) {
        $p = get_object_vars($p);
        $p["f_product"] = $params->main->f_id;
        sinsert("mf_process", $p);
    }
    $db->commit();
    printResult(1, "ok");
    return;
}

exitError("HACKER, GO AWAY!");

