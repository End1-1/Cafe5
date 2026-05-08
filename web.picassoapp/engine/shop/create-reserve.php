<?php
# (C) 25/01/2025 Kudryashov Vasili
# Last modified - 2025-04-02 12:09:23
require_once __DIR__ . "/shop.php";


class CreateReserve extends PClass
{
    private function getUnreadMessages()
    {
        $this->result["messages"] = $this->stmtall(
            "select f_id, f_body from sys_chat where f_userto=? and f_state=0",
            "i",
            [$this->params->userfrom]
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($this->result["messages"] as $v) {
            $this->stmtall("update sys_chat set f_state=1 where f_id=?", "i", [$v["f_id"]]);
        }
    }
    private function writeMessage()
    {
        $this->db->begin_transaction();
        if (empty($this->params->f_id)) {
            $this->params->f_id = 0;
        }
        if ($this->params->f_id == 0) {
            $v["f_state"] = 0;
            $v["f_created"] = date("Y-m-d H:i:s");
            $v["f_userfrom"] = $this->params->userfrom;
            $v["f_userto"] = $this->params->userto;
            $v["f_body"] = json_encode($this->params, JSON_UNESCAPED_UNICODE);
            $this->sinsert("sys_chat", $v);
        }

        $v["f_date"] = date("Y-m-d");
        $v["f_time"] = date("H:i:s");
        $v["f_state"] = 1;
        $v["f_source"] = $this->params->userfrom;
        $v["f_enddate"] = $this->params->f_enddate;
        $v["f_store"] = $this->params->userto;
        $v["f_goods"] = $this->params->goods;
        $v["f_qty"] = $this->params->f_qty;
        $v["f_message"] = $this->params->f_message;
        $v["f_fiscal"] = $this->params->f_fiscal;
        $v["f_prepaid"] = $this->params->f_prepaid;
        $v["f_prepaidcard"] = $this->params->f_prepaidcard;
        if ($this->params->f_id == 0) {
            $this->params->f_id = $this->sinsert("a_store_reserve", $v);
            $this->stmtall(
                "update a_store_sale set f_qty=f_qty+? where f_goods=? and f_store=?",
                "dii",
                [$this->params->qty, $this->params->goods, $this->params->userto]
            );
        } else {
            $this->supdate("a_store_reserve", $v, $this->params->f_id);
        }

        $this->db->commit();
        $this->result["f_id"] = $this->params->f_id;
        $this->result["f_fiscal"] = $this->params->f_fiscal;
    }

    public function handler()
    {
        switch ($this->params->action) {
            case -1:
                $this->getUnreadMessages();
                break;
            default:
                $this->writeMessage();
                break;
        }

        $this->echoResult();
    }
}

$cr = new CreateReserve();
$cr->handler();