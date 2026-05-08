<?php
require_once(__DIR__ . "/production.php");

$result1 = stmtall("SELECT t.f_id, w.f_name as wfname, a.f_name as afname, "
."date_format(t.f_datecreate, '%d/%m/%Y')as f_date, t.f_qty, t.f_ready, t.f_out, s.f_name as sfnameelina, "
."coalesce(tt.f_ttqty*t.f_qty, 0) AS f_qtyneeded, coalesce(su.f_taskqty, 0), "
."format(coalesce(su.f_taskqty, 0) / coalesce(tt.f_ttqty*t.f_qty, 0) *100, 0) AS f_cmpt "
."FROM mf_tasks t "
."LEFT join mf_actions_group a ON a.f_id=t.f_product "
."LEFT JOIN mf_actions_state s ON s.f_id=t.f_stage "
."LEFT JOIN mf_stage w ON w.f_id=t.f_workshop "
."LEFT JOIN (SELECT f_product, count(f_id) AS f_ttqty FROM mf_process GROUP BY 1) tt ON tt.f_product=t.f_product "
."left join (select f_taskid, sum(f_qty) as f_taskqty from mf_daily_process group by 1) su on su.f_taskid=t.f_id "
."where t.f_workshop=?", "i", [$params->workshop])->fetch_all(MYSQLI_ASSOC);

$result2 = stmtall("SELECT ts.f_task, date_format(ts.f_date, '%d/%m/%Y')as f_date, s.f_name, "
"FORMAT(coalesce(dp.f_cmpt, 0) / (coalesce(sn.f_needed, 1)*t.f_qty) * 100, 0) AS f_percent,"
"coalesce(sn.f_needed, 0), coalesce(dp.f_cmpt, 0) "
"FROM mf_task_stage ts "
"LEFT JOIN mf_tasks t ON t.f_id=ts.f_task "
"LEFT JOIN mf_actions_state s ON s.f_id=ts.f_stage "
"LEFT JOIN (SELECT tta.f_state, ttmp.f_product, COUNT(ttmp.f_id) AS f_needed "
"    FROM mf_process ttmp LEFT JOIN mf_actions tta ON tta.f_id=ttmp.f_process GROUP BY 1, 2) sn on t.f_product=sn.f_product AND sn.f_state=ts.f_stage "
"LEFT JOIN (SELECT tta.f_state, ttdp.f_taskid, SUM(ttdp.f_qty) AS f_cmpt "
"    FROM mf_daily_process ttdp LEFT JOIN mf_actions tta ON tta.f_id=ttdp.f_process GROUP BY 1,2) dp ON dp.f_state=ts.f_stage AND dp.f_taskid=t.f_id "
"WHERE t.f_workshop=? "
"ORDER BY 1, 2", "i", [$params->workshop])->fetch_all(MYSQLI_ASSOC);

printResult(1, ["r1" => $result1, "r2"=>$result2]);