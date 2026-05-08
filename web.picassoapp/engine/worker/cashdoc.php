<?php
# (C) 2025 Kudryashov Vasili
# Created - 2025-03-11 13:18:27
# Last modified - 2025-03-11 13:18:36
defined('APP') or die('Die, vampire!');
require_once __DIR__ . "/worker.php";

class CashDoc extends PClass {
    
}

if (!empty($params->method)) {
    $c = new CashDoc();
    $c->{$params->method}();
}


/*

 C5CashDoc *doc = new C5CashDoc(__c5config.dbParams());
    doc->setRelation(true);
    doc->setCashOutput(__c5config.cashId());
    doc->setDate(QDate::currentDate());
    doc->addRow(tr("Change output"), cash);
    doc->save(true);
    db[":f_id"] = doc->uuid();
    db.exec("update a_header set f_sessionid=uuid() where f_Id=:f_id");
    C5CashDoc *d2 = new C5CashDoc(__c5config.dbParams());
    d2->setRelation(true);
    d2->setCashInput(__c5config.cashId());
    d2->setDate(QDate::currentDate());
    d2->addRow(tr("Change input"), cash);
    d2->save(true);


    */