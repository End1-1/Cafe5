<?php
require_once(__DIR__ . "/shop.php");

$drafts = stmtall("select d.f_id, d.f_date, d.f_amount, concat_ws(' ', u.f_last, u.f_first) as f_staff "
."from o_draft_sale d "
."left join s_user u on u.f_id=d.f_staff "
."where d.f_state=1 "
."order by d.f_date, d.f_time ")->fetch_all(MYSQLI_ASSOC);

printResult(1, $drafts);