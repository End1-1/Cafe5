<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-06 02:43:02
# Last Modified: 2026-03-29 02:48:00

$apps = [
    "waiter" => "2.4.8",
    "officen" => "2.8.39",
    "shop" => "1.7.33",
    "webapi" => "1.0.1",
    "picasso.store" => "1.1.5",
    "picasso.waiter" => "1.0.4",
    "smart" => "1.9.12",
    "carwash" => "1.0.2"
];

$appName = $_SERVER['HTTP_X_APPLICATION_NAME'] ?? null;
$appVersion = $_SERVER['HTTP_X_APPLICATION_VERSION'] ?? null;

if (!$appName || !$appVersion) {
    die("PLEASE SEND APPLICATION NAME AND VERSION");
}

if (!isset($apps[$appName])) {
    die("UNKNOWN APPLICATION");
}

$clientParts = explode('.', $appVersion);
$clientBase = implode('.', array_slice($clientParts, 0, 3));

$serverVersion = $apps[$appName];

if ($clientBase !== $serverVersion) {
    http_response_code(426);

    echo json_encode([
        "error" => "Application update required",
        "old_version" => $clientBase,
        "new_version" => $serverVersion
    ]);
    exit;
}
