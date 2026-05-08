<?php
require_once(__DIR__ . "/cashsession.php");

$cs = new CashSession();
$cs->openSession();
$cs->echoResult();