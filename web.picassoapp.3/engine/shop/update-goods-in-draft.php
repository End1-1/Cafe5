<?php
require_once(__DIR__ . "/shop.php");

$shop = new ShopOrder();
$shop->updateGoodsInDraft();