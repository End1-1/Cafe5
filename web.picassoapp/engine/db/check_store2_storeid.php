<?php
/**
 * Check store2 SP versions against SQL files (f_storeid mapping marker).
 * Usage: php engine/db/check_store2_storeid.php
 */
define('APP', true);
require __DIR__ . '/../cnf.php';

$mysqli = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
if ($mysqli->connect_errno) {
    fwrite(STDERR, $mysqli->connect_error . "\n");
    exit(1);
}

$expected = [
    'sf_store2_input' => ['f_storeid', 'LEFT JOIN c_goods'],
    'sf_store2_output' => ['f_storeid', 'LEFT JOIN c_goods'],
    'sf_store2_move' => ['f_storeid', 'curr_user_item_id', 'LEFT JOIN c_goods'],
    'sf_store2_move_delete' => [], // reverse only — no f_storeid needed
    'sf_store2_complect' => ['f_storeid', 'complect_stock_id', 'LEFT JOIN c_goods'],
    'sf_store2_complect_delete' => [],
];

echo "DB: {$dbhost}/{$dbname}\n\n";
$allOk = true;

foreach ($expected as $name => $markers) {
    $st = $mysqli->prepare(
        'SELECT LENGTH(ROUTINE_DEFINITION) AS len, ROUTINE_DEFINITION
         FROM information_schema.ROUTINES
         WHERE ROUTINE_SCHEMA = DATABASE() AND ROUTINE_NAME = ?'
    );
    $st->bind_param('s', $name);
    $st->execute();
    $res = $st->get_result();
    $row = $res->fetch_assoc();
    if (!$row || $row['len'] === null) {
        echo "MISSING  $name\n";
        $allOk = false;
        continue;
    }
    $body = $row['ROUTINE_DEFINITION'];
    $ok = true;
    $miss = [];
    foreach ($markers as $m) {
        if (stripos($body, $m) === false) {
            $ok = false;
            $miss[] = $m;
        }
    }
    if ($ok) {
        echo "OK       $name  (len={$row['len']})\n";
    } else {
        echo "OLD      $name  (len={$row['len']}) missing: " . implode(', ', $miss) . "\n";
        $allOk = false;
    }
}

echo "\n";
if ($allOk) {
    echo "All checked functions look like the f_storeid version.\n";
    exit(0);
}
echo "Some functions are OLD or missing — re-apply the corresponding engine/db/sp/*.sql\n";
exit(1);
