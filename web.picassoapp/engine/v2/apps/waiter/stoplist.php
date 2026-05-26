<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-30 12:04:46
# Last Modified: 2025-12-30 12:04:49
require_once __DIR__ . "/index.php";

class StopList extends Auth
{
    public function set($params)
    {
        if ($params->qty < 0) {
            $this->delete("c_stoplist", $params->dish, "f_dish");
        } else {
            $sl = $this->select("select * from c_stoplist where f_dish=?", "i", [$params->dish])->fetch_assoc();
            if (empty($sl)) {
                $v["f_dish"] = $params->dish;
                $v["f_qty"] = $params->qty;
                $this->insert("c_stoplist", $v);
            } else {
                $v["f_qty"] = $params->qty;
                $this->update("c_stoplist", $v, $params->dish, "f_dish");
            }
        }
        $this->echoResult();
    }

    public function RemoveStopList($params)
    {
        $this->result["stoplist"] = $this->select("select f_dish from c_stoplist")->fetch_all();
        $this->select("delete from c_stoplist", "", [], true);
        $this->echoResult();
    }

    public function Get($params)
    {
        $stoplist = $this->stmtall("select f_dish, f_qty from d_stoplist")->fetch_all(MYSQLI_ASSOC);
        $this->result["list"] = $stoplist;
        $this->echoResult();
    }
}
