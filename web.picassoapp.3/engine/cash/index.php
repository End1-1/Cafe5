<?php
require_once(__DIR__ . "/../app.php");

class Cash extends Report
{

    public function __construct()
    {
        parent::__construct();
    }

    function columns()
    {

    }
    function filter()
    {

    }
    function rows()
    {

    }
    function sumColumns()
    {

    }
    function widget()
    {

    }

    public function report() {

    }

}

$cash = new Cash();
if (empty($params->cash)) {
    $cash->report();
} else {
    $cash->{$params->cash}();
}