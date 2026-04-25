<?php
require_once(__DIR__ . "/carwash.php");


stmtall("update o_header_progress set f_state=? where f_id=?", "is", [$params->newstatus, $params->f_id]);
switch ($params->newstatus) {
    case 1:
        stmtall("update o_header set f_state=5 where f_id=?", "s", [$params->f_id]);
        stmtall("update o_header_progress set f_startwash=null, f_startdry=null, f_parking=null where f_id=?", "s", [$params->f_id]);
        break;
    case 2:
        stmtall("update o_header set f_state=1 where f_id=?", "s", [$params->f_id]);
        stmtall("update o_header_progress set f_startwash=current_timestamp(), f_startdry=null, f_parking=null where f_id=?", "s", [$params->f_id]);
        break;
    case 3:
        stmtall("update o_header set f_state=1 where f_id=?", "s", [$params->f_id]);
        stmtall("update o_header_progress set f_startwash=if(f_startwash is null, current_timestamp(), f_startwash), f_startdry=current_timestamp(), f_parking=null where f_id=?", "s", [$params->f_id]);
        break;
    case 4:
        stmtall("update o_header set f_state=1 where f_id=?", "s", [$params->f_id]);
        stmtall("update o_header_progress set f_startwash=if(f_startwash is null, current_timestamp(), f_startwash), "
            . "f_startdry=if(f_startdry is null, current_timestamp(), f_startdry), "
            . "f_parking=current_timestamp() where f_id=?", "s", [$params->f_id]);
        break;
    case 5:
        break;  
    default:
    break;
}

printResult(1, "");