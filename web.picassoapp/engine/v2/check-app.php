<?php
# Ãƒâ€šÃ‚Â© 2026 , Kudryashov Vasili
# Created: 2026-02-06 02:43:02
# Last Modified: 2026-03-29 02:48:00

$version_check = true;

$apps = [
    "waiter" => "2.5.14",
    "officen" => "2.14.57",
    "shop" => "1.8.46",
    "webapi" => "1.1.2",
    "picasso.store" => "1.1.5",
    "picasso.waiter" => "1.0.4",
    "smart" => "1.9.12",
    "carwash" => "1.0.2",
    "selfboard" => "0.1.0",
    "ararix" => "1.0.0",
    "cookingprogress" => "1.0.0",
];

$appName = $_SERVER['HTTP_X_APPLICATION_NAME'] ?? null;
$appVersion = $_SERVER['HTTP_X_APPLICATION_VERSION'] ?? null;

if (!$appName || !$appVersion) {
    die("PLEASE SEND APPLICATION NAME AND VERSION");
}

if (!isset($apps[$appName])) {
    die("UNKNOWN APPLICATION");
}

if ($version_check) {
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
}
