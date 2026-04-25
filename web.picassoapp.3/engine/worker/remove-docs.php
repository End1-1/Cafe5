<?php
require_once (__DIR__ . "/worker.php");

class RemoveStoreDocument extends DB
{

    public function __construct()
    {
        parent::__construct();
    }

    public function removeOheader($id, $tr = true) {
        if ($tr) {
            $this->db->begin_transaction();
        }

        $store = [];
        $storeHeader = [];

        $sh = $this->stmtall("select distinct(d.f_document), d.f_store "
            . "from a_store_draft d "
            . "left join o_goods g on g.f_storerec=d.f_id "
            . "where g.f_header=?", "s", [$id])->fetch_row();
        if (!empty($sh)) {
            array_push($storeHeader, $sh[0]);
            array_push($store, $sh[1]);
        }
        $sh = $this->stmtall("select f_id, f_storein, f_storeout "
            . "from a_header_store  "
            . "where f_saleuuid=?", "s", [$id])->fetch_row();
        if (!empty($sh)) {
            array_push($storeHeader, $sh[0]);
            array_push($store, $sh[1]);
            array_push($store, $sh[2]);
        }

        foreach ($storeHeader as $sh) {
            $this->stmtall("delete from a_store where f_document=?", "s", [$sh]);
            $this->stmtall("delete from a_store_draft where f_document=?", "s", [$sh]);
            $this->stmtall("delete from a_header_store where f_id=?", "s", [$sh]);
            $this->stmtall("delete from a_header where f_id=?", "s", [$sh]);
        }
        foreach ($store as $s) {
            if ($s == 0) {
                continue;
            }
            $this->stmtall("update c_storages set f_have_changes=1 where f_id=?", "i", [$s]);
        }

        $cash = $this->stmtall("select f_id from a_header_cash where f_oheader=?", "s", [$id])->fetch_all();
        foreach ($cash as $c) {
            $c = $c[0];
            $this->stmtall("delete from e_cash where f_header=?", "s", [$c]);
            $this->stmtall("delete from a_header_cash where f_id=?", "s", [$c]);
            $this->stmtall("delete from a_header where f_id=?", "s", [$c]);
        }

        $this->stmtall("delete from b_clients_debts where f_order=?", "s", [$id]);
        $trsale = $this->stmtall("select * from b_gift_card_history where f_trsale=?", "s", [$id])->fetch_assoc();
        if (!empty($trsale)) {
            if ($trsale["f_amount"] > 0) {
                $this->stmtall("update b_gift_card set f_datesaled=null where f_id=?", "s", [$trsale["f_card"]]);
            }
        }
        $this->stmtall("delete from b_gift_card_history where f_trsale=?", "s", [$id]);
        $this->stmtall("delete from b_clients_debts where f_order=?", "s", [$id]);

        $this->stmtall("delete from o_goods where f_header=?", "s", [$id]);
        $this->stmtall("delete from o_header where f_id=?", "s", [$id]);
        $this->stmtall("delete from o_header_flags where f_id=?", "s", [$id]);
        $this->stmtall("delete from o_header_options where f_id=?", "s", [$id]);


        if ($tr) {
            $this->db->commit();
        }
    }

    public function removeDoc($id)
    {

        $cashdoc = $this->stmtall("select f_baseonsale, f_cashuuid from a_header_store where f_id=?", "s", [$id])->fetch_assoc();
        $this->stmtall("delete from a_store where f_document=?", "s", [$id]);
        $stores = $this->stmtall("select f_storein, f_storeout from a_header_store where f_id=?", "s", [$id])->fetch_assoc();
        $this->stmtall(
            "update c_storages set f_have_changes=1 where f_id=? or f_id=?",
            "ii",
            [$stores["f_storein"], $stores["f_storeout"]]
        );
        $this->stmtall("delete from a_store_draft where f_document=?", "s", [$id]);
        $this->stmtall("delete from b_clients_debts where f_storedoc=?", "s", [$id]);
        $this->stmtall("delete from a_header where f_id=?", "s", [$id]);
        $this->stmtall("delete from a_header_store where f_id=?", "s", [$id]);
        if (!empty($cashdoc)) {
            $cashdoc=$cashdoc["f_cashuuid"];
            $this->stmtall("delete from e_cash where f_header=?", "s", [$cashdoc]);
            $this->stmtall("delete from a_header_cash where f_id=?", "s", [$cashdoc]);
            $this->stmtall("delete from a_header where f_id=?", "s", [$cashdoc]);
        }
        $this->stmtall("delete from a_store_dish_waste where f_document=?", "s", [$id]);
    }
}