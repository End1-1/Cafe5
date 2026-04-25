<?php
require_once(__DIR__ . "/../app.php");
require_once(__DIR__ . "/../cash/cashsession.php");

$cs = new CashSession();
$csnum = $cs->getSession();
if ($csnum["f_id"] == 0) {
    $cs->exitError(tr("No active session"));
}

$cs->closeSession();

printResult(1, ["closed"=>1]);
