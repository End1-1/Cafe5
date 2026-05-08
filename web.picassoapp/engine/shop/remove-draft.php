<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modified - 2025-03-11 13:17:51
require_once __DIR__ . "/shop.php";

$shop = new ShopOrder();
$shop->removeDraft();