<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modifed - 2025-03-22 13:40:14
require_once __DIR__ . "/../app.php";

class Goods extends PClass
{
    private $id;
    public function __construct()
    {
        parent::__construct();
        $this->id = empty($this->params->id) ? 0 : $this->params->id;
    }

    public function open()
    {
        $image = stmtall("select f_image from c_goods_images where f_id=?", "i", [$this->id])->fetch_assoc();
        $this->result["image"] = $image["f_image"] ?? "";

        $goods = stmtall("select * from c_goods where f_id=?", "i", [$this->id])->fetch_assoc();
        $typename = stmtall("select f_value from l_dictionary where f_dict='c_goods_type' and f_lang='hy' and f_dict_id=?", "i", [$goods["f_type"]])->fetch_assoc();
        $goods["f_type_name"] = $typename["f_value"];
        $this->result["goods"] = $goods;

        $complect = stmtall("select c.f_id, c.f_goods, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_goodsname, "
            . "c.f_qty, u.f_name as f_unitname, "
            . "g.f_lastinputprice, c.f_qty*g.f_lastinputprice as f_total "
            . "from c_goods_complectation c "
            . "left join c_goods g on g.f_id=c.f_goods "
            . "left join c_units u on u.f_id=g.f_unit "
            . "where c.f_base=?", "i", [$this->id])->fetch_all(MYSQLI_ASSOC);
        $this->result["complect"] = $complect;

        $goodsPrices = stmtall("select * from c_goods_prices where f_goods=?", "i", [$this->id])->fetch_all(MYSQLI_ASSOC);
        $this->result["goods_prices"] = $goodsPrices;

        $astable = stmtall("select f_asdbid, f_ascode from as_convert where f_table='c_goods' and f_tableid=?", "i", [$this->id])->fetch_all(MYSQLI_ASSOC);
        $this->result["astable"] = $astable;

        $bc = $this->stmtall("select f_id from c_goods_multiscancode where f_goods=?", "i", [$this->id])->fetch_all(MYSQLI_NUM);
        $barcodes = [];
        foreach ($bc as $b) {
            array_push($barcodes, $b[0]);
        }
        $this->result["barcodes"] = $barcodes;

        $sql = <<<EOD
        SELECT m.f_id, m.f_name as f_menu_name, mm.f_price, mm.f_store, mm.f_print1, mm.f_print2, mm.f_state, mm.f_recent
        FROM c_menu_names m
        LEFT JOIN c_menu mm ON mm.f_menu=m.f_id and  mm.f_dish=?
        EOD;
        $menu = stmtall($sql, "i", [$this->id])->fetch_all(MYSQLI_ASSOC);
        $this->result["menu"] = $menu;
        $storages = stmtall("select f_id, f_name from c_storages where f_state=1")->fetch_all(MYSQLI_ASSOC);
        $this->result["storages"] = $storages;
        $printers = stmtall("select f_name from d_printers")->fetch_all(MYSQLI_ASSOC);
        $this->result["printers"] = $printers;

        $this->echoResult();
    }

    public function save()
    {
        $this->db->begin_transaction();
        if (empty($this->params->goods->f_id)) {
            $isnew = true;
        } else {
            $checknew = $this->stmtall("select f_id from c_goods where f_id=?", "i", [$this->params->goods->f_id])->fetch_assoc();
            $isnew = empty($checknew);
        }

        $duplicateScancode = $this->stmtall(
            "select f_id from c_goods where f_scancode=? and f_id<>?",
            "si",
            [$this->params->goods->f_scancode, $this->params->goods->f_id]
        )->fetch_row();
        if (!empty($duplicateScancode)) {
            exitError(tr("Duplicate barcode"));
        }

        $v = get_object_vars($this->params->goods);
        if (isset($v["f_data"]) && is_object($v["f_data"])) {
            $v["f_data"] = json_encode($v["f_data"], JSON_UNESCAPED_UNICODE);
        }
        $this->params->goods->f_id = $this->sinsertupdate("c_goods", $v, $this->params->goods->f_id, $isnew);
        if ($isnew) {
            if ($this->params->samestore) {
                $this->stmtall("update c_goods set f_storeid=? where f_id=?", "ii", [$this->params->goods->f_id, $this->params->goods->f_id]);
            }
        }

        $v["f_id"] = $this->params->goods->f_id;
        $this->sinsertupdate("c_goods_option", $v, $this->params->goods->f_id, $isnew);
        $imageSize = empty($this->params->image) ? 0 : strlen($this->params->image);
        $v["f_id"] = $this->params->goods->f_id;
        $v["f_image"] = $this->params->image;
        if ($imageSize == 0) {
            $v["f_bigimagesize"] = 0;
            $v["f_bigimage"] = null;
        }
        if (!empty($this->params->bigimage)) {
            $v["f_bigimage"] = $this->params->bigimage;
            $v["f_bigimagesize"] = strlen($this->params->bigimage);
        }
        $v["f_size"] = $imageSize;
        $this->sinsertupdate("c_goods_images", $v, $this->params->goods->f_id, $isnew);

        $this->stmtall("delete from c_goods_complectation where f_base=?", "i", [$this->params->goods->f_id]);
        foreach ($this->params->c_goods_complectation as $c) {
            $v = get_object_vars($c);
            $v["f_base"] = $this->params->goods->f_id;
            $this->sinsert("c_goods_complectation", $v);
        }

        $this->stmtall("delete from c_goods_prices where f_goods=?", "i", [$this->params->goods->f_id]);
        foreach ($this->params->c_goods_prices as $c) {
            $v = get_object_vars($c);
            $v["f_goods"] = $this->params->goods->f_id;
            $this->sinsert("c_goods_prices", $v);
        }

        if ($this->params->scangenerated) {
            $this->stmtall("update c_goods_scancode_counter set f_counter=? where f_counter<?", "ii", [$this->params->scancounter, $this->params->scancounter]);
        }

        $this->stmtall("delete from as_convert where f_table=? and f_tableid=?", "si", [$this->params->astable, $this->params->astableid]);
        foreach ($this->params->asconver as $c) {
            $v = get_object_vars($c);
            $v["f_tableid"] = $this->params->goods->f_id;
            $this->sinsert("as_convert", $v);
        }

        $this->result["AAAAAAAAAAAA"] = $this->params->goods->f_id;
        $this->stmtall("delete from c_goods_multiscancode where f_goods=?", "i", [$this->params->goods->f_id]);
        foreach ($this->params->f_barcodes as $b) {
            $this->stmtall("delete from c_goods_multiscancode where f_id=?", "s", [$b]);
            $v["f_id"] = $b;
            $v["f_goods"] = $this->params->goods->f_id;
            $this->sinsert("c_goods_multiscancode", $v);
        }

        $this->stmtall("delete from c_menu where f_dish=?", "i", [$this->params->goods->f_id]);
        foreach ($this->params->menu as $m) {

            $m->f_dish = $this->params->goods->f_id;
            $this->sinsert("c_menu", $m);
        }

        $this->result["f_id"] = $this->params->goods->f_id;
        $this->result["isnew"] = $isnew;
        $this->db->commit();

        $notify = require_once __DIR__ . "/../v2/worker/ws-notify.php";
        $notify->notify("goods", $this->params->goods->f_id, $isnew);
        $this->echoResult();
    }
}
