<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modified - 2025-05-13 13:47:51
require_once __DIR__ . "/smart.php";

if (empty($params->header)) {

    //COUNTER OF ORDER
    $hallcounter = stmtall("select f_counterhall from h_halls where f_id=?", "i", [$config->hall])->fetch_assoc()["f_counterhall"];
    $counterPrefix = stmtall("select f_counter + 1 as f_counter, f_prefix from h_halls where f_id=? for update", "i", [$hallcounter])->fetch_assoc();
    stmtall("update h_halls set f_counter=? where f_id=?", "ii", [$counterPrefix["f_counter"], $hallcounter]);

    $params->header = uuidv4();
    $bind["f_id"] = $params->header;
    $bind["f_hallid"] = $counterPrefix["f_counter"];
    $bind["f_prefix"] = $counterPrefix["f_prefix"];
    $bind["f_state"] = 1;
    $bind["f_hall"] = $config->hall;
    $bind["f_table"] = $params->table;
    $bind["f_datecash"] = date("Y-m-d");

    $bind["f_dateopen"] = date("Y-m-d");
    $bind["f_timeopen"] = date("H:i:s");

    $bind["f_staff"] = $userid;
    $bind["f_cashier"] = $userid;
    $bind["f_comment"] = "";
    $bind["f_amounttotal"] = 0;
    $bind["f_amountcash"] = 0;
    $bind["f_amountcard"] = 0;
    $bind["f_amountother"] = 0;
    $bind["f_amountidram"] = 0;
    $bind["f_amountdiscount"] = 0;
    $bind["f_discountfactor"] = 0;
    $bind["f_partner"] = $params->customer;
    $bind["f_taxpayertin"] = "";
    $bind["f_working_session"] = $params->sessionid;
    sinsert("o_header", $bind);

    $bind["f_id"] = $params->header;
    $bind["f_card"] = $params->bhistory->card;
    $bind["f_type"] = $params->bhistory->type;
    $bind["f_value"] = $params->bhistory->value;
    $bind["f_data"] = $params->bhistory->data;
    sinsertupdate("b_history", $bind, $params->header, true);

    $bind["f_id"] = $params->header;
    $bind["f_1"] = $params->flags->f1;
    $bind["f_2"] = 0;
    $bind["f_3"] = $params->flags->f3;
    $bind["f_4"] = $params->flags->f4;
    $bind["f_5"] = $params->flags->f5;
    sinsertupdate("o_header_flags", $bind, $params->header, true);
}

$params->obodyid = uuidv4();
$emark = empty($params->f_emarks) ? "null" : "'$params->f_emarks'";
if (!empty($params->f_emarks)) {
    $check = stmtall("select * from o_body where f_emarks=?", "s", [$params->f_emarks])->fetch_assoc();
    if (!empty($check)) {
        exitError(tr("Emarks duplicate"));
    }
}
if ($params->dishid > 0 && $params->menuid == 0) {
    $item = stmtall("select * from d_menu where f_dish=? and f_menu=?", "ii", [$params->dishid, $params->menucode])->fetch_assoc();
    if (empty($item)) {
        exitError(tr("Dish not found"));
    }
    $params->menuid = $item["f_id"];
}
stmtall("insert into o_body (f_id, f_header, f_state, f_dish, f_qty1, f_qty2, f_price, f_total, f_service, f_discount, "
    . "f_store, f_print1, f_print2, f_comment, f_remind, f_adgcode, f_removereason, f_timeorder, f_package, f_row, f_emarks,"
    . "f_appenduser, f_appendtime ) "
    . "select ?, ?, 1, m.f_dish, 1, 0, m.f_price, m.f_price, 0, 0, "
    . "m.f_store, m.f_print1, m.f_print2, '', 0, if(length(md.f_adgt)>0, md.f_adgt, p2.f_adgcode), 0, 0, 0, $params->row, $emark, "
    . "$userid, current_timestamp() "
    . "from d_menu m "
    . "left join d_dish md on md.f_id=m.f_dish "
    . "left join d_part2 p2 on p2.f_id=md.f_part "
    . "where m.f_id=$params->menuid", "ss", [$params->obodyid, $params->header]);

$sql = <<<EOD
    select m.f_id as menuid, md.f_id, 1 as f_qty1, 0 as f_qty2, m.f_price, m.f_price as f_total, 0 as f_service, 0 as f_discount, 
    m.f_store, m.f_print1, m.f_print2, '' as f_comment, 0, if(length(md.f_adgt)>0, md.f_adgt, p2.f_adgcode) as f_adgcode, 0, 0, 0, 
    $params->row, '$params->f_emarks' as f_emarks, md.f_extra,
    md.f_name, p2.f_qr, md.f_specialdiscount 
    from d_menu m 
    left join d_dish md on md.f_id=m.f_dish 
    left join d_part2 p2 on p2.f_id=md.f_part 
    where m.f_id=$params->menuid
    EOD;
$dish = stmtall($sql)->fetch_assoc();
$dish["obodyid"] = $params->obodyid;

printResult(1, array_merge(["header" => $params->header], $dish));

