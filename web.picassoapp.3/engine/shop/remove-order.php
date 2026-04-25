<?php
#(C) 10/02/2025 Kudryashov Vasili
require_once __DIR__ . "/shop.php";

$shopOrder = new ShopOrder();
$shopOrder->remove(true, false);