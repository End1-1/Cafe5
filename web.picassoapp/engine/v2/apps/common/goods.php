<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Goods extends Auth
{
    public function Rename($params)
    {
        $id = (int)($params->f_id ?? 0);
        $name = trim((string)($params->f_name ?? ""));
        if ($id <= 0) {
            dieWithCode("f_id is required");
        }
        if ($name === "") {
            dieWithCode("f_name is required");
        }
        $row = $this->select("select f_id from c_goods where f_id=?", "i", [$id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Goods not found");
        }
        $this->update("c_goods", ["f_name" => $name], $id);
        $notify = require_once __DIR__ . "/../../worker/ws-notify.php";
        $notify->notify("goods", $id, false);
        $this->result["f_id"] = $id;
        $this->echoResult();
    }
}
