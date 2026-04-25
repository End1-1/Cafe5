<?php
require_once (__DIR__ . "/shop.php");

$qty = stmtall("select distinct(g.f_name) as f_name, g.f_scancode, cast(coalesce(sum(s.f_qty*s.f_type), 0) as float) as f_qty, "
    . "p.f_taxname "
    . "from a_store s "
    . "left join c_goods g on g.f_id=s.f_goods "
    . "left join o_goods og on og.f_goods=g.f_id "
    . "left join c_partners p on p.f_id=g.f_supplier "
    . "where s.f_store=og.f_store and s.f_goods=og.f_goods "
    . "and og.f_header=?", "s", [$params->header])->fetch_all(MYSQLI_ASSOC);

printResult(1, ["qty" => $qty]);