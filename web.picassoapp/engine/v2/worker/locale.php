<?php
# © 2026 , Kudryashov Vasili
# Menu/catalog localization helpers (c_*_tr / ararix_*_tr).

/**
 * Resolve request locale for menu data (not UI Translator).
 * Falls back to hy when missing/unknown.
 */
function resolve_menu_locale($params, string $default = 'hy'): string
{
    $raw = '';
    if (is_object($params) && isset($params->locale)) {
        $raw = (string)$params->locale;
    } elseif (is_array($params) && isset($params['locale'])) {
        $raw = (string)$params['locale'];
    }
    $locale = strtolower(trim($raw));
    if ($locale === '' || $locale === 'am' || $locale === 'hy-am') {
        $locale = 'hy';
    }
    if ($locale === '' || !preg_match('/^[a-z]{2}(-[a-z0-9]+)?$/', $locale)) {
        return $default;
    }
    // Prefer base language code (en-US → en).
    if (str_contains($locale, '-')) {
        $locale = explode('-', $locale, 2)[0];
    }
    return $locale;
}

/**
 * Load c_goods_tr (+ base f_name) for a list of goods ids.
 * @return array<int, array{name:string, description:string}>
 */
function load_goods_localized_map($db, array $ids, string $locale): array
{
    $ids = array_values(array_unique(array_filter(array_map('intval', $ids))));
    if (empty($ids)) {
        return [];
    }
    $placeholders = implode(',', array_fill(0, count($ids), '?'));
    $sql = "SELECT g.f_id,
                   COALESCE(NULLIF(TRIM(tr.f_name), ''), g.f_name) AS f_name,
                   COALESCE(NULLIF(TRIM(tr.f_description), ''), g.f_description) AS f_description
            FROM c_goods g
            LEFT JOIN c_goods_tr tr ON tr.f_goods_id = g.f_id AND tr.f_lang = ?
            WHERE g.f_id IN ($placeholders)";
    $bindTypes = 's' . str_repeat('i', count($ids));
    $bind = array_merge([$locale], $ids);

    if (is_object($db) && method_exists($db, 'select')) {
        $rows = $db->select($sql, $bindTypes, $bind)->fetch_all(MYSQLI_ASSOC);
    } else {
        $rows = stmtall($sql, $bindTypes, $bind)->fetch_all(MYSQLI_ASSOC);
    }

    $map = [];
    foreach ($rows as $row) {
        $map[(int)$row['f_id']] = [
            'name' => (string)($row['f_name'] ?? ''),
            'description' => (string)($row['f_description'] ?? ''),
        ];
    }
    return $map;
}

/** @return array<string, array{f_name?:string, f_description?:string}> keyed by lang */
function load_entity_translations($db, string $table, string $idField, int $id): array
{
    if ($id <= 0) {
        return [];
    }
    $withDescription = in_array($table, ['c_goods_tr', 'ararix_menu_tr'], true);
    $sql = $withDescription
        ? "SELECT f_lang, f_name, f_description FROM {$table} WHERE {$idField} = ?"
        : "SELECT f_lang, f_name FROM {$table} WHERE {$idField} = ?";
    if (is_object($db) && method_exists($db, 'select')) {
        $rows = $db->select($sql, 'i', [$id])->fetch_all(MYSQLI_ASSOC);
    } else {
        $rows = stmtall($sql, 'i', [$id])->fetch_all(MYSQLI_ASSOC);
    }
    $out = [];
    foreach ($rows as $row) {
        $lang = strtolower((string)$row['f_lang']);
        $item = ['f_name' => (string)($row['f_name'] ?? '')];
        if ($withDescription) {
            $item['f_description'] = (string)($row['f_description'] ?? '');
        }
        $out[$lang] = $item;
    }
    return $out;
}

/**
 * Upsert/delete translation rows for non-hy languages.
 * $translations: list of {f_lang, f_name, f_description?}
 * Empty f_name deletes the row for that lang.
 */
function save_entity_translations($db, string $table, string $idField, int $id, array $translations, bool $hasDescription = false): void
{
    if ($id <= 0) {
        return;
    }
    foreach ($translations as $tr) {
        if (is_object($tr)) {
            $tr = (array)$tr;
        }
        if (!is_array($tr)) {
            continue;
        }
        $lang = strtolower(trim((string)($tr['f_lang'] ?? '')));
        if ($lang === '' || $lang === 'hy') {
            continue; // hy lives on base table
        }
        $name = trim((string)($tr['f_name'] ?? ''));
        if ($name === '') {
            if (is_object($db) && method_exists($db, 'select')) {
                $db->select("DELETE FROM {$table} WHERE {$idField} = ? AND f_lang = ?", 'is', [$id, $lang], true);
            } else {
                stmtall("DELETE FROM {$table} WHERE {$idField} = ? AND f_lang = ?", 'is', [$id, $lang]);
            }
            continue;
        }
        $description = $hasDescription ? trim((string)($tr['f_description'] ?? '')) : null;
        if ($hasDescription) {
            $sql = "INSERT INTO {$table} ({$idField}, f_lang, f_name, f_description)
                    VALUES (?, ?, ?, ?)
                    ON DUPLICATE KEY UPDATE f_name = VALUES(f_name), f_description = VALUES(f_description)";
            $types = 'isss';
            $params = [$id, $lang, $name, $description];
        } else {
            $sql = "INSERT INTO {$table} ({$idField}, f_lang, f_name)
                    VALUES (?, ?, ?)
                    ON DUPLICATE KEY UPDATE f_name = VALUES(f_name)";
            $types = 'iss';
            $params = [$id, $lang, $name];
        }
        if (is_object($db) && method_exists($db, 'select')) {
            $db->select($sql, $types, $params, true);
        } else {
            stmtall($sql, $types, $params);
        }
    }
}

/**
 * Replace f_name on modificator/related JSON items using c_goods_tr map.
 */
function localize_embedded_goods_names(array &$items, array $nameMap, string $idKey = 'f_id', string $nameKey = 'f_name'): void
{
    foreach ($items as &$item) {
        if (!is_array($item)) {
            continue;
        }
        $gid = (int)($item[$idKey] ?? $item['id'] ?? $item['f_goods'] ?? 0);
        if ($gid > 0 && isset($nameMap[$gid])) {
            $item[$nameKey] = $nameMap[$gid]['name'];
            if (isset($item['name'])) {
                $item['name'] = $nameMap[$gid]['name'];
            }
        }
    }
    unset($item);
}

/**
 * When POS goods translations change, mirror into ararix_menu_tr for linked rows.
 */
function sync_ararix_menu_tr_from_goods($db, int $goodsId, array $translations): void
{
    if ($goodsId <= 0) {
        return;
    }
    $sql = "SELECT f_id FROM ararix_menu WHERE f_source_goods_id = ?";
    if (is_object($db) && method_exists($db, 'select')) {
        $rows = $db->select($sql, 'i', [$goodsId])->fetch_all(MYSQLI_ASSOC);
    } else {
        $rows = stmtall($sql, 'i', [$goodsId])->fetch_all(MYSQLI_ASSOC);
    }
    foreach ($rows as $row) {
        save_entity_translations($db, 'ararix_menu_tr', 'f_menu_id', (int)$row['f_id'], $translations, true);
    }
}
