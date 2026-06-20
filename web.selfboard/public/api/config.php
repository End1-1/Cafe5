<?php
require __DIR__ . '/_lib.php';

$cfg = sb_config();
$host = sb_normalize_host((string)$cfg['host']);

sb_send_json([
    'appVersion' => $cfg['appVersion'],
    'defaultLocale' => $cfg['locale'] ?? 'en',
    'locales' => ['ru', 'en', 'hy'],
    'host' => $host,
    'username' => (string)($cfg['username'] ?? ''),
    'https' => ($cfg['https'] ?? true) !== false,
    'configured' => (bool)($host !== '' && !empty($cfg['username']) && !empty($cfg['password'])),
]);
