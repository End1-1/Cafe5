<?php
#(C) 10/02/2025 Kudryashov Vasili
# Last modified - 2025-10-24 22:05:41
require_once __DIR__ . "/../app.php";


class ShopOrder extends PClass
{
    private $id;
    public function __construct($id = "")
    {
        parent::__construct();
        $this->id = $id ?: ($this->params->id ?? null);

    }

    public function openDraft()
    {
        $ds = $this->stmtall("select * from o_draft_sale where f_id=?", "s", [$this->id])->fetch_assoc();
        if (empty($ds)) {
            $this->exitError(tr("Draft not exists"));
        }
        $sql =<<<EOD
        select d.f_id, d.f_id as f_bodyid, d.f_header, 
        d.f_goods, g.f_name as f_goodsname, u.f_name as f_unitname, 
        cast(d.f_qty as float) as f_qty, cast(d.f_price as float) as f_price1, 
        cast(d.f_price as float) as f_price2, 0 as f_storeqty,
        g.f_candiscount
        from o_draft_sale_body d 
        left join c_goods g on g.f_id=d.f_goods 
        left join c_units u on u.f_id=g.f_unit 
        where d.f_header=? and d.f_state=1 
        EOD;
        $goods = $this->stmtall($sql, "s", [$this->id])->fetch_all(MYSQLI_ASSOC);
        $this->result["ds"] = $ds;
        $this->result["goods"] = $goods;
        $result["ds"] = $ds;
        $result["goods"] = $goods;
        $this->result["data"] = $result;
        $this->echoResult();
    }
    public function makeDraft()
    {
        $this->db->begin_transaction();
        $newid = $this->uuidv4();
        $this->stmtall("insert into o_draft_sale (f_id, f_state, f_saletype, f_date, f_time, "
            . " f_staff, f_amount, f_comment, f_payment, f_partner, f_debt, f_discount, f_cashier) "
            . "select ?, 1, f_saletype, current_date(), current_time(), f_staff,f_amounttotal, f_comment, 1, f_partner, 0, "
            . "0, f_cashier from o_header where f_id=?", "ss", [$newid, $this->id]);

        $this->stmtall("insert into o_draft_sale_body (f_id, f_header, f_state, f_store, f_goods, f_qty, f_price, f_dateappend, f_timeappend) "
            . "select uuid(), ?, 1, f_store, f_goods, f_qty, f_price, current_date(), current_time() "
            . "from o_goods where f_header=?", "ss", [$newid, $this->id]);

        $this->remove();
        $this->db->commit();
        $this->result["id"] = $newid;
        $this->echoResult();
    }
    public function remove($tr = false, $noecho = true)
    {
        if ($tr) {
            $this->db->begin_transaction();
        }

        $store = [];
        $storeHeader = [];

        $sh = $this->stmtall("select distinct(d.f_document), d.f_store "
            . "from a_store_draft d "
            . "left join o_goods g on g.f_storerec=d.f_id "
            . "where g.f_header=?", "s", [$this->id])->fetch_row();
        if (!empty($sh)) {
            array_push($storeHeader, $sh[0]);
            array_push($store, $sh[1]);
        }
        $sh = $this->stmtall("select f_id, f_storein, f_storeout "
            . "from a_header_store  "
            . "where f_saleuuid=?", "s", [$this->id])->fetch_row();
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

        $cash = $this->stmtall("select f_id from a_header_cash where f_oheader=?", "s", [$this->id])->fetch_all();
        foreach ($cash as $c) {
            $c = $c[0];
            $this->stmtall("delete from e_cash where f_header=?", "s", [$c]);
            $this->stmtall("delete from a_header_cash where f_id=?", "s", [$c]);
            $this->stmtall("delete from a_header where f_id=?", "s", [$c]);
        }

        $this->stmtall("delete from b_clients_debts where f_order=?", "s", [$this->id]);
        $trsale = $this->stmtall("select * from b_gift_card_history where f_trsale=?", "s", [$this->id])->fetch_assoc();
        if (!empty($trsale)) {
            if ($trsale["f_amount"] > 0) {
                $this->stmtall("update b_gift_card set f_datesaled=null where f_id=?", "s", [$trsale["f_card"]]);
            }
        }
        $this->stmtall("delete from b_gift_card_history where f_trsale=?", "s", [$this->id]);
        $this->stmtall("delete from b_clients_debts where f_order=?", "s", [$this->id]);

        $this->stmtall("delete from o_goods where f_header=?", "s", [$this->id]);
        $this->stmtall("delete from o_header where f_id=?", "s", [$this->id]);
        $this->stmtall("delete from o_header_flags where f_id=?", "s", [$this->id]);
        $this->stmtall("delete from o_header_options where f_id=?", "s", [$this->id]);
        $this->stmtall("delete from o_qr where f_header=?", "s", [$this->id]);

        $this->stmtall("delete from o_draft_sale_body where f_header=?", "s", [$this->id]);
        $this->stmtall("delete from o_draft_sale where f_id=?", "s", [$this->id]);

        if ($tr) {
            $this->db->commit();
        }
        if ($noecho == false) {
            $this->result["data"] = true;
            $this->echoResult();
        }
    }

    public function createDraft()
    {

        $this->id = $this->uuidv4();
        $this->stmtall("insert into o_draft_sale (f_id, f_state, f_saletype, f_date, f_time, "
            . "f_staff, f_amount, f_comment, f_payment, f_partner, f_debt, f_discount, f_cashier) "
            . "select ?, 1, 1, current_date(), current_time(), ?, 0, '', 1, 0, 0, "
            . "0, ? ", "sii", [$this->id, $this->userid, $this->userid]);


    }

    public function addGoodsToDraft()
    {
        $goods = $this->stmtall("select g.f_id, g.f_name  as f_goodsname, u.f_name as f_unitname, cast(gp.f_price1 as float) as f_price1 "
            . "from c_goods g "
            . "left join c_goods_prices gp on gp.f_goods=g.f_id and gp.f_currency=1 "
            . "left join c_units u on u.f_id=g.f_unit "
            . "where g.f_scancode=?", "s", [$this->params->barcode])->fetch_assoc();
        if (empty($goods)) {
            $this->exitError(tr("Invalid barcode"));
        }

        if (empty($this->params->id)) {
            $this->createDraft();
        } else {
            $draft = $this->stmtall("select * from o_draft_sale where f_id=?", "s", [$this->params->id])->fetch_assoc();
            if (empty($draft)) {
                $this->createDraft();
            }
        }

        $bodyid = $this->uuidv4();
        $this->stmtall(
            "insert into o_draft_sale_body (f_id, f_header, f_state, f_store, f_goods, f_qty, f_price, f_dateappend, f_timeappend) "
            . "select ?, ?, 1, ?, ?, 0, ?, current_date(), current_time() ",
            "ssiid",
            [$bodyid, $this->id, $this->params->store, $goods["f_id"], $goods["f_price1"]]
        );
        $this->stmtall("update o_draft_sale set f_state=1 where f_id=?", "s", [$this->params->id]);


        $store = $this->stmtall(
            "select coalesce(cast(sum(f_qty*f_type) as float), 0) as f_storeqty from a_store where f_store=? and f_goods=?",
            "ii",
            [$this->params->store, $goods["f_id"]]
        )->fetch_assoc();

        $result["f_draftid"] = $this->id;
        $result["f_goodsname"] = $goods["f_goodsname"];
        $result["f_bodyid"] = $bodyid;
        $result["f_price1"] = $goods["f_price1"];
        $result["f_unitname"] = $goods["f_unitname"];
        $result["f_storeqty"] = $store["f_storeqty"];
        $this->result["data"] = $result;
        $this->echoResult();
    }

    public function updateGoodsInDraft()
    {
        $this->stmtall("update o_draft_sale_body set f_qty=? where f_id=?", "ds", [$this->params->f_qty, $this->params->bodyid]);
        $this->stmtall(
            "update o_draft_sale set f_amount=(select sum(f_price*f_qty) from o_draft_sale_body where f_header=? and f_state=1) where f_id=?",
            "ss",
            [$this->id, $this->id]
        );
        $result["f_bodyid"] = $this->params->bodyid;
        $result["f_qty"] = $this->params->f_qty;
        $this->result["data"] = $result;
        $this->echoResult();
    }

    public function removeGoodsFromDraft()
    {
        $row = $this->stmtall("select f_header from o_draft_sale_body where f_id=?", "s", [$this->params->bodyid])->fetch_assoc();

        $this->stmtall(
            "update o_draft_sale_body set f_state=3, f_dateremoved=current_date(), f_timeremoved=current_time() where f_id=?",
            "s",
            [$this->params->bodyid]
        );
        $isempty = $this->stmtall("select coalesce(count(f_id), 0) as f_qty, sum(f_price*f_qty) as f_amount from o_draft_sale_body where f_header=? and f_state=1", "s", [$row["f_header"]])->fetch_assoc();
        $this->stmtall(
            "update o_draft_sale set f_amount=?, f_state=? where f_id=?",
            "dis",
            [$isempty["f_amount"], $isempty["f_qty"] > 0 ? 1 : 0, $row["f_header"]]
        );

        $result["bodyid"] = $this->params->bodyid;
        $this->result["data"] = $result;
        $this->echoResult();
    }

    public function removeDraft()
    {
        $this->stmtall("update o_draft_sale set f_state=3 where f_id=?", "s", [$this->id]);
        $this->stmtall("update o_draft_sale_body set f_state=3 where f_id=?", "s", [$this->id]);
        $this->echoResult();
    }
}