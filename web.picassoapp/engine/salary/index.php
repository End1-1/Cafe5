<?php
require_once(__DIR__ . "/../app.php");

class Salary extends PClass {
    public function __construct() {
        parent::__construct();
    }

    
}

if (empty($params->action)) {
    $salary = new Salary();
    $salary->{$params->action}();
}