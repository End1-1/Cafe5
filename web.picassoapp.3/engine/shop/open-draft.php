<?php
#(C) 10/02/2025 Kudryashov Vasili
# Last modified - 2025-10-24 22:05:41
require_once __DIR__ . "/shop.php" ;

$shopOrder = new ShopOrder();
$shopOrder->openDraft();