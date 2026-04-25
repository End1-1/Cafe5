<?php
# (C) 2025 Kudryashov Vasili 
# Created - 2025-06-30 12:38:49
# Last modified - 2025-06-30 12:38:52
defined('APP') or die('Die, vampire!');

class loadinggoods extends PClass
{
    public function get()
    {
        $sql = <<<EOD
        SELECT
        g.f_id,
        g.f_name,
        g.f_scancode,
        COALESCE(og.f_qty, 0) AS f_qty,
        COALESCE(ogl.f_doneqty, 0) AS f_doneqty
        FROM c_goods g
        LEFT JOIN (
        SELECT og.f_goods, SUM(og.f_qty) AS f_qty
        FROM o_goods og
        JOIN o_draft_sale ods ON ods.f_id = og.f_header
        WHERE ods.f_state = 2 AND ods.f_datefor = ? AND og.f_store = ?
        GROUP BY og.f_goods
        ) og ON og.f_goods = g.f_id
        LEFT JOIN (
        SELECT f_goods, SUM(f_qty) AS f_doneqty
        FROM o_goods_loading
        WHERE f_date = ? AND f_store = ? and f_state=1
        GROUP BY f_goods
        ) ogl ON ogl.f_goods = g.f_id
        WHERE COALESCE(og.f_qty, 0) > 0 OR COALESCE(ogl.f_doneqty, 0) > 0
        group by g.f_id;
        EOD;
        $this->result["data"]["goods"] = $this->stmtall(
            $sql,
            "sisi",
            [$this->params->date, $this->params->store, $this->params->date, $this->params->store]
        )->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function barcode()
    {
        $barcode = $this->params->barcode;
        $qr = null;
        if (strlen($barcode) >= 29) {
            $qr = $barcode;
            if (substr($barcode, 0, 3) == "010") {
                $barcode = (string) intval(substr($barcode, 3, 13));
            } else if (substr($barcode, 0, 6) == "000000") {
                $barcode = substr($barcode, 6, 8);
            }
        }
        $goods = $this->stmtall("select * from c_goods where f_scancode=?", "s", [$barcode])->fetch_assoc();
        if (empty($goods)) {
            $this->exitError($this->tr->tr("Wrong barcode {$barcode}"));
        }
        $v = [
            "f_goods" => $goods["f_id"],
            "f_qty" => $this->params->qty,
            "f_date" => $this->params->date,
            "f_time" => date("H:i:s"),
            "f_state" => 1,
            "f_store" => $this->params->store,
            "f_emarks" => $qr
        ];
        $this->sinsert("o_goods_loading", $v);
        $this->result["data"]["filter"] = $goods["f_id"];
        $this->get();
    }

    public function remove()
    {
        $v = ["f_state" => 2];
        $this->supdate(
            "o_goods_loading",
            $v,
            $this->params->id
        );
        $this->details();
    }

    public function details()
    {
        $this->result["data"]["goodsname"] = $this->stmtall("select f_name from c_goods where f_id=?", "i", [$this->params->goods])->fetch_assoc()["f_name"];
        $this->result["data"]["details"] = $this->stmtall(
            "select f_id, f_time, f_qty, f_emarks from o_goods_loading where f_goods=? and f_date=? and f_store=? and f_state=1",
            "isi",
            [$this->params->goods, $this->params->date, $this->params->store]
        )->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
