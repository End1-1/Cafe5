<?php
require_once(__DIR__ . "/app.php");

if ($params->mode == 1) {
    require_once(__DIR__ . "/hotel/hotel-status.php");
    return;
}

if ($params->mode == 2) {
    require_once(__DIR__ . "/shop/draft-list.php");
    return;
}

if ($params->mode == 3) {
    require_once(__DIR__ . "/reports/sale-dashboard.php");
    return;
}

printResult(0, "MODE REQUIRED");