<?php

require_once(__DIR__ . "/../app.php");

class CashSession extends DB
{
    public $cashsession;
    public function __construct()
    {
        parent::__construct();
    }

    public function getSession()
    {
        $this->cashsession = $this->stmtall("select f_id from s_working_sessions where f_close is null")->fetch_assoc();
        if (empty($this->cashsession)) {
            $this->cashsession = ["f_id" => 0];
        }
        $this->result["cashsession"] = $this->cashsession;
        return $this->cashsession;
    }

    public function openSession() {
        $this->getSession();
        if ($this->cashsession["f_id"]  > 0) {
            return $this->cashsession;
        }
        $v["f_open"] = date("Y-m-d H:i:s");
        $v["f_host"] = $this->params->hostinfo;
        $v["f_user"] = $this->user["f_id"];
        $this->sinsert("s_working_sessions", $v);
        $this->getSession(); 
    }

    public function closeSession() {
        $v["f_close"] = date("Y-m-d H:i:s");
        $this->supdate("s_working_sessions", $v, $this->cashsession["f_id"]);
    }
}