<?php
require_once(__DIR__ . "/smart.php");

class GetServiceValues extends DB
{
    public function __construct()
    {
        parent::__construct();
    }

    public function getData()
    {
        $data = $this->stmtall("select f_value, f_comment from o_service_values ")->fetch_all(MYSQLI_ASSOC);
        return $data;
    }

    public function get()
    {
        $this->result["service"] = $this->getData();
        $this->echoResult();
    }
}

if (!defined("noout")) {
    $getServiceValues = new GetServiceValues();
    $getServiceValues->get();
}
