<?php
require_once __DIR__ . "/production.php";


$sql = <<<EOD
select t.f_datecreate , t.f_timecreate , t.f_qty, 
g.f_name as f_productname, t.f_ready, t.f_workshop, t.f_stage, t.f_ready, t.f_out, t.f_notes 
from mf_tasks t 
left join mf_actions_group g on g.f_id=t.f_product 
where t.f_id={$params->id} 
EOD;
$task = stmtall($sql)->fetch_assoc();
if (empty($task)) {
    exitError("Invalid task id: {$params->id}");
}

$sql = <<<EOD
SELECT p.f_process, s.f_name as `Փուլ`, a.f_name as `Գործողություն`, p.f_durationsec as `Տևող․`, 
p.f_price as `Գին`, 
t.f_qty as `Պահանջ`, coalesce(d.f_qty, 0) AS `Կատ․`,  coalesce(d.f_qty, 0) / t.f_qty * 100 as '%' 
FROM mf_process p 
INNER JOIN mf_actions a ON a.f_id=p.f_process 
left join mf_actions_state s on s.f_id=a.f_state 
INNER JOIN mf_tasks t ON t.f_product=p.f_product 
LEFT JOIN (SELECT f_process, SUM(f_qty) AS f_qty FROM mf_daily_process WHERE f_taskid={$params->id}  GROUP BY f_process) d ON d.f_process=p.f_process 
WHERE t.f_id={$params->id} 
ORDER BY p.f_rowid
EOD;

$taskdescription = stmtall($sql)->fetch_all(MYSQLI_ASSOC);

printResult(1, ["task" => $task, "description" => $taskdescription]);