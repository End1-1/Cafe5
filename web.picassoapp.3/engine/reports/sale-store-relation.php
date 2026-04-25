<?php
require_once __DIR__ . "/reports.php";

class SaleStoreRelation extends Report {
    protected function columns() {
        return [
            $this->tr("Store doc"),
            $this->tr("Sale doc"),
            $this->tr("Store qty"),
            $this->tr("Sale qty")
        ];
    }

    protected function filter() {
        return [];
    }

    protected function rows()
     {
        $sql = <<<EOD
        select ah.f_id, oh.f_id ,
        asd.f_qty, og.f_qty
        from o_goods og
        left join o_header oh on oh.f_id=og.f_header
        left join a_store_draft asd on asd.f_id=og.f_storerec 
        left join a_header ah on ah.f_id=asd.f_document
        EOD;
        return $this->db->stmtall($sql)->fetch_all();
    }

    protected function sumColumns() {
        return [];
    }

    protected function widget() {
        return [];
    }

}

$s = new SaleStoreRelation();
$s->echoResult();