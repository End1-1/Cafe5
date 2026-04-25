<?php
# 26/01/2025 (C) Kudryashov Vasili
require_once __DIR__ . "/../app.php";

class ShopMovement extends PClass
{

    public function setup()
    {
        $this->result["user"] = $this->userid;
        $cnf = $this->stmtall("select f_config from sys_json_config where f_id=?", "i", [$this->user["f_config"]])->fetch_assoc();
        $this->result["config"] = json_decode($cnf["f_config"]);
        $outstorelist = implode(',', $this->result["config"]->availableoutstore);
        if (!empty($outstorelist)) {
        $sql = "select f_id, f_name from c_storages where f_id in ({$outstorelist})";
        $this->result["availableoutstore"] = $this->stmtall($sql)->fetch_all(MYSQLI_ASSOC);
        }
        $this->result["userconfig"] = $this->user;
        $this->echoResult();
    }

    public function search() {
        if (empty($this->params->barcode)) {
            $this->echoResult();
        }
        $goods = $this->stmtall("select * from c_goods where f_scancode=?", "s", [$this->params->barcode])->fetch_assoc();
        if (empty($goods)) {
            $this->exitError($this->tr("Unknown barcode"));
        }
        $price = $this->stmtall("select if(f_price1disc>0, f_price1disc, f_price1) as f_price1, if(f_price2disc>0, f_price2disc, f_price2) as f_price2 "
            . "from c_goods_prices where f_goods=?", "i", [$goods["f_id"]])->fetch_assoc();
        $store = $this->stmtall("select coalesce(sum(f_qty*f_type), 0) as asum from a_store where f_goods=? and f_store=?", "ii", 
        [$goods["f_id"], $this->params->store])->fetch_assoc();
        $this->result["goods"] = $goods;
        $this->result["price"] = $price;
        $this->result["qty"] = $store["asum"];
        $this->echoResult();
    }
}

if (!empty($params->action)) {
    $sm = new ShopMovement();
    $sm->{$params->action}();

}