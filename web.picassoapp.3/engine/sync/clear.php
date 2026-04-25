<?php
require_once (__DIR__ . "/sync.php");

if (!defined("CLEANER")) {
    exitError("Are you right cleaner?");
}

$ids = queryStr("SELECT h.f_id  FROM $params->db.o_header h "
    . " LEFT JOIN $params->db.o_tax t ON t.f_id=h.f_id WHERE  COALESCE(f_receiptnumber, 0)=0")->fetch_all(MYSQLI_ASSOC);
foreach ($ids as $rec) {
    $db->begin_transaction();
    $id = $rec["f_id"];
    $storerec = stmtall("select f_storerec from $params->db.o_goods where f_header=?", "s", [$id])->fetch_all(MYSQLI_NUM);

    $storeheaders = [];
    foreach ($storerec as $st) {
        $storeheaders = array_merge($storeheaders, stmtall("select f_document from $params->db.a_store_draft where f_id=?", "s", [$st[0]])->fetch_all(MYSQLI_NUM));
    }
    foreach ($storeheaders as $st) {
        stmtall("delete from $params->db.a_store where f_document=?", "s", [$st[0]]);
        stmtall("delete from $params->db.a_store_draft where f_document=?", "s", [$st[0]]);
        stmtall("delete from $params->db.a_header_store where f_id=?", "s", [$st[0]]);
        stmtall("delete from $params->db.a_header where f_id=?", "s", [$st[0]]);
    }

    $cashheaders = stmtall("select f_id from $params->db.a_header_cash where f_oheader=?", "s", [$id])->fetch_all(MYSQLI_NUM);
    foreach ($cashheaders as $st) {
        stmtall("delete from $params->db.e_cash where f_header=?", "s", [$st[0]]);
        stmtall("delete from $params->db.a_header_cash where f_id=?", "s", [$st[0]]);
        stmtall("delete from $params->db.a_header where f_id=?", "s", [$st[0]]);
    }

    stmtall("delete from $params->db.b_clients_debts where f_order=?", "s", [$id]);
    $cardsale = stmtall("select * from $params->db.b_gift_card_history where f_trsale=?", "s", [$id])->fetch_all(MYSQLI_ASSOC);
    foreach ($cardsale as $cs) {
        if ($cs["f_amount"] > 0) {
            stmtall("update $params->db.b_gift_card set f_datesaled=null where f_id=?", "i", [$cs["f_card"]]);
        }
    }
    stmtall("delete from $params->db.b_gift_card_history where f_trsale=?", "s", [$id]);
    stmtall("delete from $params->db.o_body where f_header=?", "s", [$id]);
    stmtall("delete from $params->db.b_clients_debts where f_order=?", "s", [$id]);
    stmtall("delete from $params->db.o_goods where f_header=?", "s", [$id]);
    stmtall("delete from $params->db.o_header where f_id=?", "s", [$id]);
    stmtall("delete from $params->db.o_header_flags where f_id=?", "s", [$id]);
    stmtall("delete from $params->db.o_header_options where f_id=?", "s", [$id]);
    $db->commit();
}

queryStr("delete from $params->db.b_clients_debts");
queryStr("update $params->db.a_header set f_partner=null");
queryStr("update $params->db.o_header set f_partner=null");

printResult(1, "OK");