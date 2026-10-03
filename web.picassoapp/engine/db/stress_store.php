<?php
/**
 * Concurrency probe for warehouse postings (CLI only).
 *
 * Runs many parallel processes posting inputs, write-offs, movements, re-postings and
 * deletions against the same goods and storages, then checks that no document was lost
 * and that stock still matches its movements.
 *
 *   php stress_store.php                      serialized path (runStoreOperation)
 *   php stress_store.php --raw                old path: plain transaction, no store lock
 *   php stress_store.php --workers=16 --iterations=20
 *   php stress_store.php --keep               do not delete test data afterwards
 */

if (PHP_SAPI !== 'cli') {
    die('CLI only');
}

require_once __DIR__ . '/../cnf.php';
require_once __DIR__ . '/../v2/worker/die-with-code.php';
require_once __DIR__ . '/../v2/worker/translator.php';
require_once __DIR__ . '/../v2/worker/db.php';

new Translator('en');

const STORE_A = 9001;
const STORE_B = 9002;
const ITEM_FIRST = 900001;
const ITEM_LAST = 900006;
const DOC_USER = 'STRESS';

$opts = [];
foreach (array_slice($argv, 1) as $arg) {
    if (preg_match('/^--([a-z_]+)(?:=(.*))?$/', $arg, $m)) {
        $opts[$m[1]] = $m[2] ?? '1';
    }
}
$raw = !empty($opts['raw']);

if (isset($opts['worker'])) {
    runWorker((int)$opts['worker'], (int)($opts['iterations'] ?? 10), $raw);
    exit(0);
}

runMaster(
    (int)($opts['workers'] ?? 10),
    (int)($opts['iterations'] ?? 12),
    $raw,
    !empty($opts['keep'])
);

// ---------------------------------------------------------------- master

function runMaster(int $workers, int $iterations, bool $raw, bool $keep): void
{
    $db = new Db();
    echo "mode: " . ($raw ? 'RAW (no store lock, repeatable read)' : 'SERIALIZED (get_lock + read committed + retry)') . "\n";
    echo "workers: $workers, iterations each: $iterations\n";

    cleanupTestData($db);
    setupTestData($db);

    $started = microtime(true);
    $procs = [];
    for ($i = 1; $i <= $workers; $i++) {
        $cmd = escapeshellarg(PHP_BINARY) . ' ' . escapeshellarg(__FILE__)
            . " --worker={$i} --iterations={$iterations}" . ($raw ? ' --raw' : '');
        $pipes = [];
        $proc = proc_open($cmd, [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
        if (!is_resource($proc)) {
            die("cannot start worker {$i}\n");
        }
        $procs[$i] = ['proc' => $proc, 'pipes' => $pipes];
    }

    $totals = ['ops' => 0, 'ok' => 0, 'business' => 0, 'lock_errors' => 0, 'retries' => 0, 'other_errors' => 0];
    $byOp = [];
    $slowest = 0.0;
    $problems = [];
    foreach ($procs as $i => $p) {
        $out = stream_get_contents($p['pipes'][1]);
        $err = stream_get_contents($p['pipes'][2]);
        fclose($p['pipes'][1]);
        fclose($p['pipes'][2]);
        proc_close($p['proc']);

        $report = json_decode(trim($out), true);
        if (!is_array($report)) {
            $problems[] = "worker {$i} produced no report: " . trim($out . ' ' . $err);
            continue;
        }
        foreach ($totals as $k => $_) {
            $totals[$k] += (int)($report[$k] ?? 0);
        }
        foreach ($report['by_op'] ?? [] as $op => $counts) {
            foreach ($counts as $k => $v) {
                $byOp[$op][$k] = $k === 'max_ms'
                    ? max($byOp[$op][$k] ?? 0, $v)
                    : ($byOp[$op][$k] ?? 0) + $v;
            }
        }
        $slowest = max($slowest, (float)($report['slowest_ms'] ?? 0));
        foreach ($report['errors'] ?? [] as $e) {
            $problems[] = "worker {$i}: {$e}";
        }
    }
    $elapsed = microtime(true) - $started;

    echo "\n--- operations ---\n";
    ksort($byOp);
    foreach ($byOp as $op => $counts) {
        printf(
            "  %-8s ok=%-4d business=%-4d lock_errors=%-3d retries=%-3d avg=%.0fms max=%.0fms\n",
            $op,
            $counts['ok'] ?? 0,
            $counts['business'] ?? 0,
            $counts['lock_errors'] ?? 0,
            $counts['retries'] ?? 0,
            ($counts['ok'] ?? 0) > 0 ? ($counts['ms'] ?? 0) / max(1, $counts['ok']) : 0,
            $counts['max_ms'] ?? 0
        );
    }

    echo "\n--- totals ---\n";
    printf("  wall time      : %.1fs (%.1f ops/s)\n", $elapsed, $totals['ops'] / max(0.001, $elapsed));
    printf("  operations     : %d\n", $totals['ops']);
    printf("  succeeded      : %d\n", $totals['ok']);
    printf("  business errors: %d (insufficient stock / version conflict — expected)\n", $totals['business']);
    printf("  LOCK ERRORS    : %d (deadlock 1213 / lock wait timeout 1205)\n", $totals['lock_errors']);
    printf("  lock retries   : %d\n", $totals['retries']);
    printf("  other errors   : %d\n", $totals['other_errors']);
    printf("  slowest op     : %.0fms\n", $slowest);

    echo "\n--- integrity ---\n";
    $issues = checkIntegrity($db);
    if (empty($issues)) {
        echo "  stock matches movements, no orphan documents\n";
    } else {
        foreach ($issues as $issue) {
            echo "  FAIL: {$issue}\n";
        }
    }
    foreach ($problems as $p) {
        echo "  worker problem: {$p}\n";
    }

    $verdict = $totals['lock_errors'] === 0 && empty($issues) && $totals['other_errors'] === 0;
    echo "\nRESULT: " . ($verdict ? 'PASS' : 'FAIL') . "\n";

    if (!$keep) {
        cleanupTestData($db);
        echo "test data removed\n";
    }
}

function setupTestData(Db $db): void
{
    $db->select("insert ignore into c_storages (f_id, f_name, f_state) values (?, 'STRESS A', 1)", "i", [STORE_A], true);
    $db->select("insert ignore into c_storages (f_id, f_name, f_state) values (?, 'STRESS B', 1)", "i", [STORE_B], true);
    for ($id = ITEM_FIRST; $id <= ITEM_LAST; $id++) {
        $db->select(
            "insert ignore into c_goods (f_id, f_type, f_name, f_storeid, f_lastinputprice, f_enabled, f_unit)
             values (?, 1, concat('STRESS item ', ?), ?, 100, 1, 1)",
            "iii",
            [$id, $id, $id],
            true
        );
    }
    // Opening stock so that write-offs have something to consume.
    $doc = newDoc(1, ['doc_store_in' => STORE_A]);
    $doc['items'] = [];
    for ($id = ITEM_FIRST; $id <= ITEM_LAST; $id++) {
        $doc['items'][] = ['id' => uuid(), 'item_id' => $id, 'qty' => 500, 'price' => 100, 'row' => $id - ITEM_FIRST, 'comment' => 'opening'];
    }
    $doc['doc_sum'] = docSum($doc['items']);
    $doc['paid_amount'] = $doc['doc_sum'];
    $res = $db->runStoreOperation([STORE_A], fn() => $db->callStoreFunction('sf_store2_input', json_encode($doc)));
    if ((int)($res['status'] ?? 1) !== 0) {
        die("opening stock failed: " . json_encode($res) . "\n");
    }
    echo "test data ready: storages " . STORE_A . "/" . STORE_B . ", items " . ITEM_FIRST . ".." . ITEM_LAST . "\n";
}

function cleanupTestData(Db $db): void
{
    $db->select("delete from store_moves where f_item_id between ? and ?", "ii", [ITEM_FIRST, ITEM_LAST], true);
    $db->select("delete from store_stock where f_item_id between ? and ?", "ii", [ITEM_FIRST, ITEM_LAST], true);
    $db->select("delete from store_user where f_doc in (select f_id from store_document where f_user_id=?)", "s", [DOC_USER], true);
    $db->select("delete from cash_operations where f_order_id in (select f_id from store_document where f_user_id=?)", "s", [DOC_USER], true);
    $db->select("delete from cash_debts where f_doc_uuid in (select f_id from store_document where f_user_id=?)", "s", [DOC_USER], true);
    $db->select("delete from b_clients_debts where f_storedoc in (select f_id from store_document where f_user_id=?)", "s", [DOC_USER], true);
    $db->select("delete from store_document where f_user_id=?", "s", [DOC_USER], true);
    $db->select("delete from c_goods where f_id between ? and ?", "ii", [ITEM_FIRST, ITEM_LAST], true);
    $db->select("delete from c_storages where f_id in (?, ?)", "ii", [STORE_A, STORE_B], true);
}

function checkIntegrity(Db $db): array
{
    $issues = [];

    $rows = $db->select(
        "select ss.f_id, ss.f_qty_left,
                coalesce(sum(sm.f_qty_in), 0) - coalesce(sum(sm.f_qty_out), 0) as calc
         from store_stock ss
         left join store_moves sm on sm.f_batch_id = ss.f_id
         where ss.f_item_id between ? and ?
         group by ss.f_id, ss.f_qty_left
         having abs(ss.f_qty_left - calc) > 0.0001",
        "ii",
        [ITEM_FIRST, ITEM_LAST]
    )->fetch_all(MYSQLI_ASSOC);
    foreach ($rows as $r) {
        $issues[] = "batch {$r['f_id']}: qty_left={$r['f_qty_left']} but movements give {$r['calc']}"
            . " | " . batchHistory($db, $r['f_id']);
    }

    $rows = $db->select(
        "select count(*) c from store_document where f_user_id=? and (f_status = -1 or f_doc_type is null)",
        "s",
        [DOC_USER]
    )->fetch_assoc();
    if ((int)$rows['c'] > 0) {
        $issues[] = "{$rows['c']} header stub(s) left behind (f_status=-1 or f_doc_type is null)";
    }

    $rows = $db->select(
        "select count(*) c from store_moves sm
         left join store_document sd on sd.f_id = sm.f_doc
         where sm.f_item_id between ? and ? and sd.f_id is null",
        "ii",
        [ITEM_FIRST, ITEM_LAST]
    )->fetch_assoc();
    if ((int)$rows['c'] > 0) {
        $issues[] = "{$rows['c']} movement(s) point to a deleted document";
    }

    return $issues;
}

/** Short human readable history of a batch: owning document and every movement on it. */
function batchHistory(Db $db, string $batchId): string
{
    $own = $db->select(
        "select ss.f_doc, ss.f_doc_row_id, ss.f_qty_in, sd.f_doc_type, sd.f_status
         from store_stock ss left join store_document sd on sd.f_id = ss.f_doc
         where ss.f_id = ?",
        "s",
        [$batchId]
    )->fetch_assoc();
    $parts = ['owner=' . ($own['f_doc'] === null ? 'none' : 'type' . $own['f_doc_type'] . '/st' . $own['f_status'])
        . ' self_row=' . (($own['f_doc_row_id'] ?? null) === $batchId ? 'yes' : 'no')];

    $moves = $db->select(
        "select sm.f_doc, sm.f_doc_row_id, sm.f_qty_in, sm.f_qty_out, sd.f_doc_type, sd.f_status
         from store_moves sm left join store_document sd on sd.f_id = sm.f_doc
         where sm.f_batch_id = ?",
        "s",
        [$batchId]
    )->fetch_all(MYSQLI_ASSOC);
    foreach ($moves as $m) {
        $parts[] = 'move(type' . ($m['f_doc_type'] ?? '?') . '/st' . ($m['f_status'] ?? '?')
            . ' in=' . rtrim(rtrim($m['f_qty_in'], '0'), '.')
            . ' out=' . rtrim(rtrim($m['f_qty_out'], '0'), '.')
            . ' healing=' . ($m['f_doc_row_id'] === $batchId ? 'no' : 'yes') . ')';
    }
    return implode(' ', $parts);
}

// ---------------------------------------------------------------- worker

function runWorker(int $index, int $iterations, bool $raw): void
{
    mt_srand($index * 7919 + (int)(microtime(true) * 1000) % 1000);
    $db = new Db();
    $report = ['ops' => 0, 'ok' => 0, 'business' => 0, 'lock_errors' => 0, 'retries' => 0,
        'other_errors' => 0, 'by_op' => [], 'errors' => [], 'slowest_ms' => 0.0];
    $myInputs = [];

    for ($i = 0; $i < $iterations; $i++) {
        $roll = mt_rand(1, 100);
        if ($roll <= 30) {
            $op = 'input';
        } elseif ($roll <= 60) {
            $op = 'output';
        } elseif ($roll <= 75) {
            $op = 'sale';
        } elseif ($roll <= 88) {
            $op = 'move';
        } elseif ($roll <= 95 && !empty($myInputs)) {
            $op = 'repost';
        } elseif (!empty($myInputs)) {
            $op = 'remove';
        } else {
            $op = 'input';
        }

        $t0 = microtime(true);
        $retriesBefore = $db->storeRetryCount();
        $outcome = runOperation($db, $op, $raw, $myInputs);
        $ms = (microtime(true) - $t0) * 1000;

        $report['by_op'][$op]['retries'] = ($report['by_op'][$op]['retries'] ?? 0)
            + ($db->storeRetryCount() - $retriesBefore);
        $report['ops']++;
        $report['by_op'][$op]['ms'] = ($report['by_op'][$op]['ms'] ?? 0) + $ms;
        $report['by_op'][$op]['max_ms'] = max($report['by_op'][$op]['max_ms'] ?? 0, $ms);
        $report['slowest_ms'] = max($report['slowest_ms'], $ms);
        $report[$outcome['kind']]++;
        $report['by_op'][$op][$outcome['kind']] = ($report['by_op'][$op][$outcome['kind']] ?? 0) + 1;
        if (!empty($outcome['message']) && count($report['errors']) < 5) {
            $report['errors'][] = "{$op}: {$outcome['message']}";
        }
        usleep(mt_rand(0, 25) * 1000);
    }

    $report['retries'] = $db->storeRetryCount();
    echo json_encode($report) . "\n";
}

/** @return array{kind:string, message?:string} kind: ok|business|lock_errors|other_errors */
function runOperation(Db $db, string $op, bool $raw, array &$myInputs): array
{
    switch ($op) {
        case 'input':
            $doc = newDoc(1, ['doc_store_in' => STORE_A]);
            $doc['items'] = randomItems(mt_rand(1, 3), 5, 20, 90, 110);
            $doc['doc_sum'] = docSum($doc['items']);
            $doc['paid_amount'] = $doc['doc_sum'];
            $out = post($db, 'sf_store2_input', $doc, [STORE_A], $raw);
            if ($out['kind'] === 'ok') {
                $myInputs[] = ['doc' => $doc, 'version' => (int)($out['result']['version'] ?? 1)];
            }
            return $out;

        case 'output':
        case 'sale':
            $doc = newDoc(2, ['doc_store_out' => STORE_A]);
            $doc['items'] = randomItems(mt_rand(1, 3), 1, 6, 0, 0);
            if ($op === 'sale') {
                foreach ($doc['items'] as &$it) {
                    $it['comment'] = 'sale_row';
                }
                unset($it);
            }
            return post($db, 'sf_store2_output', $doc, [STORE_A], $raw);

        case 'move':
            $doc = newDoc(3, ['doc_store_out' => STORE_A, 'doc_store_in' => STORE_B]);
            $doc['items'] = randomItems(mt_rand(1, 2), 1, 4, 0, 0);
            return post($db, 'sf_store2_move', $doc, [STORE_A, STORE_B], $raw);

        case 'repost':
            $entry = &$myInputs[array_rand($myInputs)];
            $doc = $entry['doc'];
            $doc['doc_version'] = $entry['version'];
            foreach ($doc['items'] as &$it) {
                $it['qty'] = mt_rand(5, 25);
            }
            unset($it);
            $doc['doc_sum'] = docSum($doc['items']);
            $doc['paid_amount'] = $doc['doc_sum'];
            $out = post($db, 'sf_store2_input', $doc, [STORE_A], $raw);
            if ($out['kind'] === 'ok') {
                $entry['doc'] = $doc;
                $entry['version'] = (int)($out['result']['version'] ?? $entry['version'] + 1);
            }
            return $out;

        case 'remove':
            $key = array_rand($myInputs);
            $docId = $myInputs[$key]['doc']['doc_uuid'];
            unset($myInputs[$key]);
            $myInputs = array_values($myInputs);
            return postId($db, 'sf_store2_input_delete', $docId, [STORE_A], $raw);
    }
    return ['kind' => 'other_errors', 'message' => "unknown op {$op}"];
}

function post(Db $db, string $fn, array $doc, array $stores, bool $raw): array
{
    return execute($db, $fn, json_encode($doc, JSON_UNESCAPED_UNICODE), $stores, $raw);
}

function postId(Db $db, string $fn, string $docId, array $stores, bool $raw): array
{
    return execute($db, $fn, $docId, $stores, $raw);
}

function execute(Db $db, string $fn, string $payload, array $stores, bool $raw): array
{
    try {
        if ($raw) {
            $db->beginTransaction();
            $result = $db->callStoreFunction($fn, $payload);
            if ((int)($result['status'] ?? 1) !== 0) {
                $db->rollback();
                return ['kind' => 'business', 'result' => $result];
            }
            $db->commit();
        } else {
            $result = $db->runStoreOperation($stores, function () use ($db, $fn, $payload) {
                $res = $db->callStoreFunction($fn, $payload);
                if ((int)($res['status'] ?? 1) !== 0) {
                    throw new BusinessRejection(json_encode($res));
                }
                return $res;
            });
        }
        return ['kind' => 'ok', 'result' => $result];
    } catch (BusinessRejection $e) {
        return ['kind' => 'business'];
    } catch (StoreLockConflict $e) {
        return ['kind' => 'lock_errors', 'message' => 'lock conflict: ' . $e->getMessage()];
    } catch (mysqli_sql_exception $e) {
        $code = (int)$e->getCode();
        if ($code === 1205 || $code === 1213) {
            return ['kind' => 'lock_errors', 'message' => "sql {$code}: " . $e->getMessage()];
        }
        return ['kind' => 'other_errors', 'message' => "sql {$code}: " . $e->getMessage()];
    } catch (Throwable $e) {
        return ['kind' => 'other_errors', 'message' => get_class($e) . ': ' . $e->getMessage()];
    }
}

class BusinessRejection extends RuntimeException {}

// ---------------------------------------------------------------- helpers

function newDoc(int $type, array $extra): array
{
    return array_merge([
        'doc_uuid' => uuid(),
        'doc_date' => date('Y-m-d H:i:s'),
        'doc_status' => 1,
        'doc_type' => $type,
        'doc_user_id' => DOC_USER,
        'doc_create_user' => 1,
        'doc_version' => 0,
        'doc_partner' => 0,
        'doc_sum' => 0,
        'doc_store_in' => 0,
        'doc_store_out' => 0,
        // Paid in cash, so the posting also writes cash_operations — one of the tables that
        // used to be locked whole by an unindexed DELETE.
        'cashbox_id' => 1,
        'payment_type_id' => 1,
        'currency_id' => 1,
        'paid_amount' => 0,
        'doc_data' => ['comment' => 'stress'],
        'items' => [],
    ], $extra);
}

function randomItems(int $count, int $qtyMin, int $qtyMax, int $priceMin, int $priceMax): array
{
    $ids = range(ITEM_FIRST, ITEM_LAST);
    shuffle($ids);
    $items = [];
    for ($i = 0; $i < $count; $i++) {
        $items[] = [
            'id' => uuid(),
            'item_id' => $ids[$i],
            'qty' => mt_rand($qtyMin, $qtyMax),
            'price' => $priceMax > 0 ? mt_rand($priceMin, $priceMax) : 0,
            'expire_date' => null,
            'comment' => 'stress',
            'row' => $i,
        ];
    }
    return $items;
}

function docSum(array $items): float
{
    $sum = 0.0;
    foreach ($items as $it) {
        $sum += (float)$it['qty'] * (float)$it['price'];
    }
    return $sum;
}

function uuid(): string
{
    $d = random_bytes(16);
    $d[6] = chr(ord($d[6]) & 0x0f | 0x40);
    $d[8] = chr(ord($d[8]) & 0x3f | 0x80);
    return vsprintf('%s%s-%s-%s-%s-%s%s%s', str_split(bin2hex($d), 4));
}
