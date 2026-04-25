<?php
#(C) 2025 Kudryashov Vasili
# Last modified - 2025-11-12 23:55:03
require_once __DIR__ . "/smart.php";

$session = stmtall("select s.f_id, s.f_user, concat_ws(' ', u.f_last, u.f_first) as f_fullname "
    . "from s_working_sessions s "
    . "left join s_user u on u.f_id=s.f_user "
    . "where f_host=? and f_close is null", "s", [$params->hostinfo])->fetch_all(MYSQLI_ASSOC);
if (empty($session)) {
    $welcome = "ok";
    $bind["f_open"] = date("Y-m-d H:i:s");
    $bind["f_user"] = $userid;
    $bind["f_host"] = $params->hostinfo;
    $sessionid = sinsert("s_working_sessions", $bind);
    $welcome = ["ok" => true, "sessionid" => $sessionid, "staff" => $user["f_last"] . " " . $user["f_first"]];
} else {
    $session = $session[0];
    if ($session["f_user"] == $userid) {
        $welcome = ["ok" => true, "sessionid" => $session["f_id"], "staff" => $user["f_last"] . " " . $user["f_first"]];
    } else {
        $welcome = ["ok" => false, "who" => $session["f_fullname"]];
    }
}

$printAliases = stmtall("select * from d_print_aliases")->fetch_all(MYSQLI_ASSOC);

$part1 = stmtall("select * from d_part1")->fetch_all(MYSQLI_ASSOC);

$part2 = stmtall("select f_id, f_name, f_color "
    . "from d_part2 "
    . "where f_id in (select f_part from d_dish where f_id in (Select f_dish from d_menu where f_menu=? and f_state=1)) "
    . "order by f_queue", "i", [$config->menu])->fetch_all(MYSQLI_ASSOC);

$sql = <<<EOD
    SELECT m.f_menu, m.f_id as menuid, d.f_id, d.f_part, d.f_name,  m.f_print1, m.f_store, m.f_price, p2.f_adgcode, d.f_color, 
    d.f_netweight, d.f_cost, m.f_recent, d.f_barcode, p2.f_name as f_groupname, d.f_adgt, d.f_specialdiscount, 
    m.f_print2, p2.f_qr, d.f_extra,si.f_data as f_image
    FROM d_menu m 
    left join d_dish d on d.f_id=m.f_dish 
    left join d_part2 p2 on p2.f_id=d.f_part 
    left join s_images si on si.f_id=d.f_image
    where m.f_menu in (select f_id from d_menu_names where f_enabled=1)  and m.f_state=1 
    order by d.f_queue, d.f_name 
    EOD;
$menu = stmtall($sql)->fetch_all(MYSQLI_ASSOC);

$sql = <<<EOD
select dp.f_menu, p.f_package, p.f_dish, d.f_name, p.f_price, dg.f_adgcode, 
    p.f_store, p.f_printer, p.f_qty 
    from d_package_list p 
    inner join d_dish d on d.f_id=p.f_dish 
    inner join d_package dp on dp.f_id=p.f_package 
    inner join d_part2 dg on dg.f_id=d.f_part 
    where dp.f_menu in (select f_id from d_menu_names where f_enabled=1) and dp.f_enabled=1
EOD;
$package = stmtall($sql)->fetch_all(MYSQLI_ASSOC);

$packageNames = stmtall("select f_id, f_name, f_price from d_package where f_menu in (select f_id from d_menu_names where f_enabled=1)  and f_enabled=1")->fetch_all(MYSQLI_ASSOC);

$hallids = $config->addhall;
$sql = <<<EOD
SELECT t.f_id, t.f_name,
h.f_settings, t.f_special_config,
if (t.f_special_config IS NULL, j1.f_config, j2.f_config) AS f_config
from h_tables t
LEFT JOIN h_halls h on h.f_id=t.f_hall
LEFT JOIN sys_json_config j1 ON j1.f_id=h.f_settings
LEFT JOIN sys_json_config j2 ON j2.f_id=t.f_special_config
where f_hall in ($hallids) 
order by FIELD(f_hall, $hallids), f_id 
EOD;
$tables = stmtall($sql)->fetch_all(MYSQLI_ASSOC);

$opentables = stmtall("select f_id, f_table from o_header where f_state=1 and f_hall in ($hallids) order by FIELD(f_hall, $hallids)")->fetch_all(MYSQLI_ASSOC);

printResult(1, [
    "welcome" => $welcome,
    "part2" => $part2,
    "printaliases" => $printAliases,
    "menu" => $menu,
    "package" => $package,
    "packagenames" => $packageNames,
    "tables" => $tables,
    "opentables" => $opentables
]);