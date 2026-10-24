<?php
# Menu payload used by the kiosk (waiter/menu/get) and the partner API (ararix/menu/get).

require_once __DIR__ . "/locale.php";

function selfboard_normalize_menu_image($payload): string
{
    if ($payload === null || $payload === '') {
        return '';
    }

    $payload = (string)$payload;
    $trim = preg_replace('/\s+/', '', $payload);

    if (stripos($trim, 'data:image') === 0) {
        $commaPos = strpos($trim, ',');
        if ($commaPos !== false) {
            $trim = substr($trim, $commaPos + 1);
        }
    }

    if (strncmp($trim, 'iVBORw0KGgo', 11) === 0) {
        return $trim;
    }

    if (strlen($payload) >= 8 && strncmp($payload, "\x89PNG\r\n\x1a\n", 8) === 0) {
        return base64_encode($payload);
    }

    return $trim;
}

function selfboard_menu_image_bytes(string $normalized): string
{
    if ($normalized === '') {
        return '';
    }
    if (strncmp($normalized, "\x89PNG\r\n\x1a\n", 8) === 0) {
        return $normalized;
    }
    $bin = base64_decode($normalized, true);
    if ($bin === false || strncmp($bin, "\x89PNG\r\n\x1a\n", 8) !== 0) {
        return '';
    }
    return $bin;
}

function selfboard_load_package_complectation($db, int $baseId, string $locale, bool $withImage = true): array
{
    $imageSql = $withImage
        ? "COALESCE(NULLIF(TRIM(gi.f_image), ''), '') AS f_image"
        : "CASE WHEN gi.f_id IS NOT NULL AND CHAR_LENGTH(gi.f_image) > 20 THEN 1 ELSE 0 END AS f_has_image";
    $sql = <<<SQL
    SELECT
        c.f_goods,
        c.f_price,
        c.f_qty,
        COALESCE(NULLIF(TRIM(tr.f_name), ''), g.f_name) AS f_goods_name,
        g.f_data,
        $imageSql
    FROM c_goods_complectation c
    INNER JOIN c_goods g ON g.f_id = c.f_goods
    LEFT JOIN c_goods_tr tr ON tr.f_goods_id = g.f_id AND tr.f_lang = ?
    LEFT JOIN c_goods_images gi ON gi.f_id = g.f_id
    WHERE c.f_base = ?
    ORDER BY c.f_price, f_goods_name
    SQL;

    $rows = $db->select($sql, "si", [$locale, $baseId])->fetch_all(MYSQLI_ASSOC);
    foreach ($rows as &$row) {
        if ($withImage) {
            $row['f_image'] = selfboard_normalize_menu_image($row['f_image'] ?? '');
        } else {
            $row['f_has_image'] = (int)($row['f_has_image'] ?? 0);
            $row['f_image'] = '';
        }
        $row['f_data'] = selfboard_encode_goods_data(selfboard_decode_goods_data($row['f_data'] ?? null));
    }
    unset($row);
    return $rows;
}

/**
 * c_goods.f_data is a JSON object. Empty, null and a JSON array ([]) are the same
 * as {}: the row has no modifiers, badges or attributes.
 * A PHP list must not be json_encoded as-is — [] is an array, {} is an object.
 */
function selfboard_decode_goods_data($raw): array
{
    if (is_string($raw)) {
        $trim = trim($raw);
        if ($trim === '' || $trim === '[]' || $trim === 'null') {
            return [];
        }
        $decoded = json_decode($trim, true);
    } elseif (is_array($raw)) {
        $decoded = $raw;
    } else {
        return [];
    }

    if (!is_array($decoded) || $decoded === [] || array_keys($decoded) === range(0, count($decoded) - 1)) {
        return [];
    }

    return $decoded;
}

function selfboard_encode_goods_data(array $data): string
{
    if ($data === []) {
        return '{}';
    }

    $json = json_encode($data, JSON_UNESCAPED_UNICODE);
    return $json === false ? '{}' : $json;
}

function selfboard_collect_embedded_goods_ids(array $data): array
{
    $ids = [];
    foreach (['f_modificators', 'f_related_drink', 'f_related_other'] as $key) {
        $list = $data[$key] ?? null;
        if (!is_array($list)) {
            continue;
        }
        foreach ($list as $item) {
            if (!is_array($item)) {
                continue;
            }
            $gid = (int)($item['f_id'] ?? $item['id'] ?? $item['f_goods'] ?? 0);
            if ($gid > 0) {
                $ids[] = $gid;
            }
        }
    }
    return $ids;
}

function selfboard_localize_dish_data($db, array &$dish, string $locale, array &$nameCache): void
{
    $data = selfboard_decode_goods_data($dish['f_data'] ?? null);

    $ids = selfboard_collect_embedded_goods_ids($data);
    $missing = [];
    foreach ($ids as $gid) {
        if (!isset($nameCache[$gid])) {
            $missing[] = $gid;
        }
    }
    if (!empty($missing)) {
        $nameCache += load_goods_localized_map($db, $missing, $locale);
    }

    foreach (['f_modificators', 'f_related_drink', 'f_related_other'] as $key) {
        if (!empty($data[$key]) && is_array($data[$key])) {
            localize_embedded_goods_names($data[$key], $nameCache);
        }
    }
    $dish['f_data'] = selfboard_encode_goods_data($data);
}

/**
 * Same groups / parts / dishes the SelfBoard kiosk loads.
 * @return array{groups:array, parts:array, dishes:array, locale:string}
 */
/**
 * @param array{with_image?:bool,group?:int,limit?:int|null,offset?:int} $view
 * limit null = every dish. limit 0 = no dishes (groups only). limit > 0 = a page.
 */
function selfboard_menu_payload($db, $params, array $view = []): array
{
    $locale = resolve_menu_locale($params, 'hy');
    $withImage = array_key_exists('with_image', $view) ? (bool)$view['with_image'] : true;
    $groupId = (int)($view['group'] ?? 0);
    $limit = array_key_exists('limit', $view) ? $view['limit'] : null;
    $offset = max(0, (int)($view['offset'] ?? 0));
    $groupImageSql = $withImage
        ? "COALESCE(si.f_data, '') AS f_image"
        : "CASE WHEN gr.f_image IS NOT NULL AND gr.f_image <> '' THEN 1 ELSE 0 END AS f_has_image";
    $groupImageJoin = $withImage ? "LEFT JOIN s_images si ON si.f_id = gr.f_image" : "";

    $sql = <<<EOD
    SELECT
    gr.f_id,
    gr.f_parent,
    COALESCE(NULLIF(TRIM(gtr.f_name), ''), gr.f_name) AS f_name,
    gr.f_taxdept as f_fiscal_department,
    gr.f_order as f_queue,
    COALESCE(g.f_qty, 0) AS f_qty,
    CAST(gr.f_class AS INT) AS f_class,
    gr.f_color,
    $groupImageSql
    FROM c_groups gr
    LEFT JOIN c_groups_tr gtr ON gtr.f_group_id = gr.f_id AND gtr.f_lang = ?
    $groupImageJoin
    LEFT JOIN (
    SELECT f_group, COUNT(*) AS f_qty
    FROM c_goods
    WHERE f_type IN (1, 2, 5)
    GROUP BY f_group
    ) g ON g.f_group = gr.f_id
    LEFT JOIN (
    SELECT f_parent, COUNT(*) AS f_children
    FROM c_groups
    WHERE f_parent IS NOT NULL
    GROUP BY f_parent
    ) gp ON gp.f_parent = gr.f_id
    WHERE
    (COALESCE(g.f_qty, 0) > 0
    OR COALESCE(gp.f_children, 0) > 0)
    and gr.f_id in (
        select f_group from c_goods
        where f_type in (1, 2, 5)
          and f_id in (select f_dish from c_menu where f_state=1)
    )
    order by  coalesce(gr.f_order, 9999), f_name
    EOD;
    $groups = $db->select($sql, "s", [$locale])->fetch_all(MYSQLI_ASSOC);
    foreach ($groups as &$group) {
        if ($withImage) {
            $group['f_image'] = selfboard_normalize_menu_image($group['f_image'] ?? '');
        } else {
            $group['f_has_image'] = (int)($group['f_has_image'] ?? 0);
            $group['f_image'] = '';
        }
    }
    unset($group);

    $parts = [];
    if ($db->select("SHOW TABLES LIKE 'd_part1'")->num_rows > 0) {
        $parts = $db->select("SELECT f_id, f_name FROM d_part1 ORDER BY f_id")->fetch_all(MYSQLI_ASSOC);
        foreach ($parts as &$part) {
            $part['f_id'] = (int)$part['f_id'];
        }
        unset($part);
    }

    $dishImageSql = $withImage
        ? "COALESCE(NULLIF(TRIM(gi.f_image), ''), '') AS f_image"
        : "CASE WHEN gi.f_id IS NOT NULL AND CHAR_LENGTH(gi.f_image) > 20 THEN 1 ELSE 0 END AS f_has_image";
    $groupFilter = $groupId > 0 ? " AND g.f_group = ? " : "";
    $limitSql = "";
    $types = "sss";
    $binds = [$locale, $locale, $locale];
    if ($groupId > 0) {
        $types .= "i";
        $binds[] = $groupId;
    }
    if ($limit !== null && (int)$limit > 0) {
        $limitSql = " LIMIT ? OFFSET ? ";
        $types .= "ii";
        $binds[] = (int)$limit;
        $binds[] = $offset;
    }

    $countTypes = "";
    $countBinds = [];
    if ($groupId > 0) {
        $countTypes = "i";
        $countBinds[] = $groupId;
    }
    $countRow = $db->select(
        "SELECT COUNT(*) AS c
         FROM c_menu_names m
         INNER JOIN c_menu mm ON mm.f_menu = m.f_id
         INNER JOIN c_goods g ON g.f_id = mm.f_dish
         WHERE m.f_enabled = 1 AND mm.f_state = 1 AND g.f_type IN (1, 2, 5)
         $groupFilter",
        $countTypes,
        $countBinds
    )->fetch_assoc();
    $dishTotal = (int)($countRow["c"] ?? 0);

    $dishes = [];
    if ($limit === null || (int)$limit > 0) {
        $sql = <<< EOD
    SELECT m.f_id AS f_menu,
    COALESCE(NULLIF(TRIM(mntr.f_name), ''), m.f_name) AS f_name,
    mm.f_price,
    g.f_type,
    gr.f_taxdept as f_fiscal_department,
    COALESCE(NULLIF(TRIM(gtr.f_name), ''), gr.f_name) as f_group_name,
    mm.f_dish, g.f_group,
    COALESCE(NULLIF(TRIM(gdtr.f_name), ''), g.f_name) AS f_goods_name,
    g.f_color,gr.f_taxdept,
    mm.f_store, mm.f_print1, mm.f_print2, mm.f_state,
    if (length(coalesce(g.f_adg, ''))>0, g.f_adg, gr.f_adgcode) AS f_adgt, gr.f_taxdept,
    mm.f_recent, g.f_emark_required, cast(coalesce(sl.f_qty, -999) AS INT) as f_stoplist,
    g.f_data,
    CONCAT_WS(',', NULLIF(g.f_scancode, ''),
        (SELECT GROUP_CONCAT(ms.f_id SEPARATOR ',') FROM c_goods_multiscancode ms WHERE ms.f_goods = g.f_id)) AS f_scancode,
    COALESCE(NULLIF(TRIM(gdtr.f_description), ''), g.f_description) AS f_description,
    $dishImageSql
    FROM c_menu_names m
    LEFT JOIN c_menu_names_tr mntr ON mntr.f_menu_id = m.f_id AND mntr.f_lang = ?
    LEFT JOIN c_menu mm ON mm.f_menu=m.f_id
    LEFT JOIN c_goods g ON g.f_id=mm.f_dish
    LEFT JOIN c_goods_tr gdtr ON gdtr.f_goods_id = g.f_id AND gdtr.f_lang = ?
    LEFT JOIN c_groups gr ON gr.f_id=g.f_group
    LEFT JOIN c_groups_tr gtr ON gtr.f_group_id = gr.f_id AND gtr.f_lang = ?
    LEFT JOIN c_goods_images gi ON gi.f_id = g.f_id
    left join c_stoplist sl on sl.f_dish=mm.f_dish
    WHERE m.f_enabled=1 AND mm.f_state=1 AND g.f_type IN (1, 2, 5)
    $groupFilter
    order by coalesce(g.f_queue,9999)
    $limitSql
    EOD;
        $dishes = $db->select($sql, $types, $binds)->fetch_all(MYSQLI_ASSOC);
        $nameCache = [];
        foreach ($dishes as &$dish) {
            if ($withImage) {
                $dish['f_image'] = selfboard_normalize_menu_image($dish['f_image'] ?? '');
            } else {
                $dish['f_has_image'] = (int)($dish['f_has_image'] ?? 0);
                $dish['f_image'] = '';
            }
            selfboard_localize_dish_data($db, $dish, $locale, $nameCache);
            if ((int)($dish['f_type'] ?? 0) === 5) {
                $dish['f_complectation'] = selfboard_load_package_complectation($db, (int)$dish['f_dish'], $locale, $withImage);
            }
        }
        unset($dish);
    }

    return [
        "groups" => $groups,
        "parts" => $parts,
        "dishes" => $dishes,
        "locale" => $locale,
        "dish_total" => $dishTotal,
        "with_image" => $withImage,
    ];
}
