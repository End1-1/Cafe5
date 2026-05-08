<?php
#(C) 01/02/2025 Kudryashov Vasili
require_once __DIR__ . "/smart.php";

if ($params->mode == 1 || $params->mode == 3) {

    $header = stmtall("select o.f_id ,o.f_prefix, o.f_hallid, t.f_firmname, t.f_address, t.f_dept, t.f_hvhh, t.f_devnum, "
        . "t.f_serial, t.f_fiscal, t.f_receiptnumber, t.f_time as f_taxtime, concat(left(u.f_first, 1), '. ', u.f_last) as f_staff, "
        . "o.f_amountcash, o.f_amountcard, o.f_amountidram, o.f_amountother, o.f_amounttotal, o.f_print, o.f_comment, ht.f_name as f_tablename, "
        . "p.f_contact, p.f_phone, p.f_address, o.f_taxpayertin, o.f_partner "
        . "from o_header o "
        . "left join o_tax t on t.f_id=o.f_id "
        . "left join s_user u on u.f_id=o.f_staff "
        . "left join h_tables ht on ht.f_id=o.f_table "
        . "left join c_partners p on p.f_id=o.f_partner "
        . "where o.f_id=?", "s", [$params->header])->fetch_assoc();
    $flags = stmtall("select * from o_header_flags where f_id=?", "s", [$params->header])->fetch_assoc();
    $dishes1 = [];
    $dishes2 = [];
    $get = false;

    $check = stmtall("select f_id from o_body b where f_header=? and f_state=1 and f_qty2<f_qty1", "s", [$params->header])->fetch_all();
    if (!empty($check)) {
        $get = true;
    }

    $flag1modified = false;
    $checkflag = stmtall("select f_2 from o_header_flags where f_id=?", "s", [$params->header])->fetch_assoc();
    if (($checkflag["f_2"] ?? 0) == 1) {
        $get = true;
        $flag1modified = true;
    }

    if ($get) {
        $dishes1 = stmtall("select d.f_name, cast(b.f_qty1 as float) as  f_qty1, cast(b.f_qty2 as float) as f_qty2, "
            . "b.f_comment, b.f_print1, b.f_print2, b.f_state "
            . "from o_body b "
            . "left join d_dish d on d.f_id=b.f_dish "
            . "where b.f_header=? and b.f_state in (1,2) "
            . "and length(b.f_print1)>0  "
            . "order by b.f_row ", "s", [$params->header])->fetch_all(MYSQLI_ASSOC);
        $dishes2 = stmtall("select d.f_name, cast(b.f_qty1 as float) as f_qty1, cast(b.f_qty2 as float) as f_qty2, "
            . "b.f_comment, b.f_print1, b.f_print2, b.f_state "
            . "from o_body b "
            . "left join d_dish d on d.f_id=b.f_dish "
            . "where b.f_header=? and b.f_state in (1,2) "
            . "and length(b.f_print2)>0  "
            . "order by b.f_row ", "s", [$params->header])->fetch_all(MYSQLI_ASSOC);
    }
    $modified = false || $flag1modified;
    foreach ($dishes1 as $d) {
        if ($d["f_qty2"] > 0) {
            $modified = true;
            break;
        }
    }
    foreach ($dishes2 as $d) {
        if ($d["f_qty2"] > 0) {
            $modified = true;
            break;
        }
    }
    $header["modified"] = $modified;
    printResult(1, ["dishes1" => $dishes1, "dishes2" => $dishes2, "header" => $header, "flags" => $flags]);
    return;
}

if ($params->mode == 2) {
    stmtall("update o_body set f_printuser=$userid, f_printtime=current_timestamp(), f_qty2=f_qty1 where f_header=? and f_state=1", "s", [$params->header]);
    stmtall("update o_header_flags set f_2=0 where f_id=?", "s", [$params->header]);
    printResult(1, ["header" => $params->header]);
    return;
}

exitError("Hackers printing bbq");