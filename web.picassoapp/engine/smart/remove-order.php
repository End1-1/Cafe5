<?php
require_once(__DIR__ . "/smart.php");

class RemoveOrder extends DB{ 
    public function __construct() {
        parent::__construct();
    }

    public function post() {
        $this->stmtall("update o_header set f_state=3 where f_id=?", "s", [$this->params->id]);
        $this->stmtall("update o_body set f_state=2 where f_state=1 and f_header=?", "s", [$this->params->id]);
        $this->echoResult();
    }
}

$removeOrder = new RemoveOrder();
$removeOrder->post();