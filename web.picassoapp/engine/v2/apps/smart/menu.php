<?php
# © 2025 , Kudryashov Vasili
require_once __DIR__ . "/index.php";

class Menu extends Auth
{
    public function GetComments($params)
    {
        // DB ≤198: d_dish_comment (c_goods_comment appears only from ~214)
        $this->result["data"] = $this->select("select f_name from d_dish_comment")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetRemoveReason($params)
    {
        $this->result["data"] = $this->select("select f_name from o_dish_remove_reason")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
