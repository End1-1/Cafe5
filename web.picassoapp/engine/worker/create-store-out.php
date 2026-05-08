<?php
require_once(__DIR__ . "/worker.php");
require_once(__DIR__ . "/remove-docs.php");
class CreateStoreOut extends DB
{
    public function construct()
    {
        parent::__construct();
    }

    public function makeoutput($id)
    {
        $log["f_timestamp"] = date("Y-m-d H:i:s");
        $this->beginTransaction();
        $oheader = $this->stmtall("select * from o_header where f_id=?", "s", [$id])->fetch_assoc();
        //Remove old docs
        $olddocs = $this->stmtall("select distinct(ad.f_document) from o_goods g "
            . "left join a_store_draft ad on ad.f_id=g.f_storerec "
            . "where g.f_header=?", "s", [$id])->fetch_all();
        $rd = new RemoveStoreDocument();
        foreach ($olddocs as $o) {
            if (!empty($o[0])) {
                $rd->removeDoc($o[0]);
            }
        }

        $this->stmtall("delete from o_goods where f_header=?", "s", [$id]);
        //NONE COMPONNENTS EXIT (W/O COMPLECTATION)
        $ogoods1 = $this->stmtall("select b.f_id, r.f_goods, r.f_qty*b.f_qty1 as f_totalqty, r.f_qty, "
            . "g.f_lastinputprice, b.f_store "
            . "from o_body b "
            . "inner join d_recipes r on r.f_dish=b.f_dish "
            . "inner join c_goods g on g.f_id=r.f_goods and f_component_exit=0 "
            . "where b.f_header=? and (b.f_state=1 or b.f_state=3) ", "s", [$id])->fetch_all(MYSQLI_ASSOC);
        //COMPONENT EXIT WITH COMPLECTATON
        $ogoods2 = $this->stmtall("select b.f_id, gc.f_goods, ((gc.f_qty/g.f_complectout) * r.f_qty)*b.f_qty1 as f_totalqty, gc.f_qty, "
            . "g.f_lastinputprice, b.f_store "
            . "from o_body b "
            . "inner join d_recipes r on r.f_dish=b.f_dish "
            . "inner join c_goods_complectation gc on gc.f_base=r.f_goods "
            . "inner join c_goods g on g.f_id=r.f_goods and f_component_exit=1 "
            . "where b.f_header=? and (b.f_state=1 or b.f_state=3) ", "s", [$id])->fetch_all(MYSQLI_ASSOC);
        //DISHES SET
        $sets = $this->stmtall("select b.f_id, r.f_goods, r.f_qty*b.f_qty1*ds.f_qty as f_totalqty, r.f_qty, "
            . "g.f_lastinputprice, b.f_store "
            . "from o_body b "
            . "inner join d_dish_set ds on ds.f_dish=b.f_dish "
            . "inner join d_recipes r on r.f_dish=ds.f_part "
            . "inner join c_goods g on g.f_id=r.f_goods and f_component_exit=0 "
            . "where b.f_header=? and (b.f_state=1 or b.f_state=3) ", "s", [$id])->fetch_all(MYSQLI_ASSOC);
        $this->commit();
        $ogoods = array_merge($ogoods1, $ogoods2, $sets);
        $row = 0;


        //$goods = $this->stmtall("select * from o_goods where f_header=? and f_sign=1", "s", [$id])->fetch_all(MYSQLI_ASSOC);
        $stores = [];
        //EXTRACT DISTINCT STORE FROM GOODS
        foreach ($ogoods as $g) {
            if (!in_array($g["f_store"], $stores)) {
                $stores[] = $g["f_store"];
            }
        }

        //write new store document 
        foreach ($stores as $store) {

            $jdoc = [];
            $jheader;
            $jheader["f_id"] = $this->uuidv4();
            $jheader["f_userid"] = $this->docNum(2, $store, 0, true);
            $jheader["f_state"] = 2;
            $jheader["f_type"] = 2;
            $jheader["f_date"] = $oheader["f_datecash"];
            $jheader["f_operator"] = $this->user["f_id"];
            $jheader["f_partner"] = 0;
            $jheader["f_amount"] = 0;
            $jheader["f_currency"] = 1;
            $jheader["f_storein"] = 0;
            $jheader["f_storeout"] = $store;
            $jheader["f_reason"] = 4;
            $jheader["f_comment"] = "Output of " . $oheader["f_prefix"] . $oheader["f_hallid"];
            $jheader["f_paid"] = 0;
            $jdoc["header"] = $jheader;
            $jbody = [];
            $jbody["f_accepted"] = $this->user["f_id"];
            $jbody["f_passed"] = $this->user["f_id"];
            $jbody["f_invoice"] = "";
            $jbody["f_invoicedate"] = $oheader["f_datecash"];
            $jbody["f_storein"] = 0;
            $jbody["f_storeout"] = $store;
            $jbody["f_complectationcode"] = 0;
            $jbody["f_complectationqty"] = 0;
            $jbody["f_cashdoc"] = "";
            $jbody["f_basedonsale"] = 1;
            $jdoc["body"] = $jbody;
            $jadd = [];
            $jdoc["add"] = $jadd;
            $jcomplect = [];
            $jcomplect["f_goods"] = 0;
            $jcomplect["f_qty"] = 0;
            $jdoc["complect"] = $jcomplect;
            $jgoods = [];
            $rownum = 0;
            foreach ($ogoods as $g) {

                if ($g["f_store"] != $store) {
                    continue;
                }

                $storerec = $this->uuidv4();
                $b["f_id"] = $storerec;
                $b["f_header"] = $id;
                $b["f_body"] = $g["f_id"];
                $b["f_goods"] = $g["f_goods"];
                $b["f_qty"] = $g["f_totalqty"];
                $b["f_store"] = $g["f_store"];
                $b["f_price"] = $g["f_lastinputprice"];
                $b["f_total"] = $g["f_totalqty"] * $g["f_lastinputprice"];
                $b["f_row"] = $rownum;
                $b["f_sign"] = 1;
                $b["f_storerec"] = $storerec;
                $b["f_tax"] = 0;
                $b["f_taxdept"] = 0;
                $this->sinsert("o_goods", $b);

                $g["f_id"] = $storerec;
                $g["f_storerec"] = $storerec;
                $g["f_qty"] = $g["f_totalqty"];
                $g["f_price"] = $g["f_lastinputprice"];
                $g["f_comment"] = $storerec;
                $g["f_row"] = $rownum++;
                array_push($jgoods, $g);
            }
            $jdoc["goods"] = $jgoods;
            $jpartner = [];
            $jpartner["partner"] = 0;
            $jpartner["paid"] = 0;
            $jpartner["cash"] = 0;
            $jdoc["partner"] = $jpartner;
            $jdoc["session"] = $this->uuidv4();
            $jdocstr = json_encode($jdoc);
            $this->stmtall("call sf_create_store_document('$jdocstr')");
        }
        $this->commit();

        $log["f_done"] = date("Y-m-d H:i:s");
        $log["f_document"] = $id;
        $log["f_request"] = "makeoutput";
        $this->sinsert("a_result", $log);
    }


    private function process()
    {
        $ids = $this->stmtall(
            "select f_id from o_header where f_datecash between ? and ? and f_state=2",
            "ss",
            [$this->params->date1, $this->params->date2]
        )->fetch_all();

        foreach ($ids as $id) {

            $this->makeoutput($id[0]);

        }
    }

    public function handle()
    {
        $this->makeoutput($this->params->id);
        $this->result["id"] = $this->params->id;
        $this->echoResult();
    }
}

if (!empty($params->action)) {
    $c = new CreateStoreOut();
    $c->{$params->action}();
}