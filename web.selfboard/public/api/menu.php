<?php
require __DIR__ . '/_lib.php';

try {
    $cfg = sb_config();
    $locale = isset($_GET['locale']) ? (string)$_GET['locale'] : ($cfg['locale'] ?? 'en');
    $menu = sb_fetch_menu($cfg, $locale);
    sb_send_json([
        'status' => 1,
        'groups' => $menu['groups'] ?? [],
        'dishes' => $menu['dishes'] ?? [],
    ]);
} catch (Throwable $e) {
    error_log('selfboard menu.php: ' . $e->getMessage());
    sb_send_json(['status' => 0, 'message' => $e->getMessage()]);
}
