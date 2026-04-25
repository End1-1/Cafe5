<?php
require_once(__DIR__ . "/carwash.php");

if (empty($params->session) || $params->session == 0){ 
    exitError("No session number");
} else {
    $totalin = stmtall("select cn.f_name, cast(sum(e.f_amount) as float) as f_amount from e_cash e "
    ."left join a_header_cash ac on ac.f_id=e.f_header "
    ."left join a_header h on h.f_id=e.f_header "
    ."left join e_cash_names cn on cn.f_id=e.f_cash "
    ."where h.f_state=1 and h.f_working_session=? and e.f_sign=1 "
    ."group by 1", "i", [$params->session])->fetch_all(MYSQLI_ASSOC);

    $totalout = stmtall("select h.f_id, e.f_remarks, cast(e.f_amount as float) as f_amount from e_cash e "
    ."left join a_header_cash ac on ac.f_id=e.f_header "
    ."left join a_header h on h.f_id=e.f_header "
    ."left join e_cash_names cn on cn.f_id=e.f_cash "
    ."where h.f_state=1 and h.f_working_session=? and e.f_sign=-1 "
    ."order by h.f_createdate, h.f_createtime ", "i", [$params->session])->fetch_all(MYSQLI_ASSOC);

    $total = stmtall("select cn.f_name, cast(sum(e.f_amount*e.f_sign) as float) as f_amount from e_cash e "
    ."left join a_header_cash ac on ac.f_id=e.f_header "
    ."left join a_header h on h.f_id=e.f_header "
    ."left join e_cash_names cn on cn.f_id=e.f_cash "
    ."where h.f_state=1 and h.f_working_session=? "
    ."group by 1", "i", [$params->session])->fetch_all(MYSQLI_ASSOC);


    printResult(1, ["totalin" => $totalin, "totalout"=>$totalout, "total"=>$total]);
    return;
}

printResult(1, "ok");