<?php 
require_once(__DIR__ . "/production.php");

$result = stmtall("SELECT t.f_id AS `NN`, date_format(t.f_datecreate, '%d/%m/%Y') AS `Օր`, m.f_name AS `Արտադրանք`, "
."t.f_qty AS `Քանակ`, t.f_ready AS `Վիճակ`, t.f_out as `Ելք`, s.f_name as `Արտադրամաս` "
. "FROM mf_tasks t "
. "LEFT JOIN mf_actions_group m ON m.f_id=t.f_product "
. "left join mf_stage s on s.f_id=t.f_workshop "
. "WHERE t.f_state=1")->fetch_all(MYSQLI_ASSOC);

printResult(1, $result);