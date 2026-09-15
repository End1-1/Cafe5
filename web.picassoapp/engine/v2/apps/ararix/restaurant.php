<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Restaurant extends ArarixAuth
{
    public function Get($params)
    {
        $id = (int)($params->restaurant_id ?? $params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("restaurant_id is required", 400);
        }

        $restaurant = $this->loadRestaurantRow($id);
        if (!$restaurant) {
            dieWithCode("Restaurant not found", 404);
        }

        $this->result["restaurant"] = $this->formatRestaurant($restaurant);
        $this->echoResult();
    }

    public function Menu($params)
    {
        $id = (int)($params->restaurant_id ?? $params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("restaurant_id is required", 400);
        }

        $restaurant = $this->loadRestaurantRow($id);
        if (!$restaurant) {
            dieWithCode("Restaurant not found", 404);
        }

        $this->result["restaurant"] = $this->formatRestaurant($restaurant);
        $this->result["groups"] = $this->loadArarixMenuGroups($id);
        $this->result["dishes"] = $this->loadArarixMenuDishes($id);
        $this->echoResult();
    }

    private function hasArarixMenu(int $restaurantId): bool
    {
        $row = $this->select(
            'SELECT 1 AS x FROM ararix_menu WHERE f_restaurant_id = ? AND f_state = 1 LIMIT 1',
            'i',
            [$restaurantId]
        )->fetch_assoc();
        return !empty($row);
    }

    private function loadArarixMenuGroups(int $restaurantId): array
    {
        $rows = $this->select(
            "SELECT g.f_id, g.f_name, COALESCE(g.f_image_url, '') AS f_image
             FROM ararix_restaurant_groups rg
             INNER JOIN ararix_goods_groups g ON g.f_id = rg.f_group_id
             WHERE rg.f_restaurant_id = ?
               AND EXISTS (
                   SELECT 1 FROM ararix_menu m
                   WHERE m.f_restaurant_id = rg.f_restaurant_id
                     AND m.f_group_id = rg.f_group_id
                     AND m.f_state = 1
               )
             ORDER BY rg.f_sort, g.f_sort, g.f_name",
            'i',
            [$restaurantId]
        )->fetch_all(MYSQLI_ASSOC);

        $out = [];
        foreach ($rows as $group) {
            $image = trim((string)($group['f_image'] ?? ''));
            $out[] = [
                'id' => (int)$group['f_id'],
                'name' => $group['f_name'],
                'image' => $image !== '' ? $this->absoluteMediaUrl($image) : null,
            ];
        }
        return $out;
    }

    private function loadArarixMenuDishes(int $restaurantId): array
    {
        $rows = $this->select(
            "SELECT m.f_id, m.f_group_id, m.f_name, m.f_description, m.f_price,
                    m.f_image_url, m.f_sort, m.f_data, g.f_name AS f_group_name
             FROM ararix_menu m
             INNER JOIN ararix_goods_groups g ON g.f_id = m.f_group_id
             WHERE m.f_restaurant_id = ? AND m.f_state = 1
             ORDER BY m.f_sort, m.f_name, m.f_id",
            'i',
            [$restaurantId]
        )->fetch_all(MYSQLI_ASSOC);

        $out = [];
        foreach ($rows as $dish) {
            $data = $this->parseGoodsData($dish['f_data'] ?? null);
            $image = trim((string)($dish['f_image_url'] ?? ''));
            $out[] = [
                'id' => (int)$dish['f_id'],
                'menu_id' => (int)$dish['f_id'],
                'group_id' => (int)$dish['f_group_id'],
                'group_name' => $dish['f_group_name'] ?? '',
                'type' => 2,
                'name' => $dish['f_name'] ?? '',
                'description' => trim((string)($dish['f_description'] ?? '')),
                'price' => (float)($dish['f_price'] ?? 0),
                'image' => $image !== '' ? $this->absoluteMediaUrl($image) : null,
                'prep_time' => trim((string)($data['f_prep_time'] ?? $data['f_cooking_time'] ?? '')),
                'popular' => false,
                'attr_type' => '',
                'attr_size' => '',
                'attr_measurement' => '',
                'bju' => $this->bjuFromData($data),
                'dietary' => $this->dietaryFromData($data),
                'modificators' => [],
                'package_components' => [],
                'related_drinks' => [],
                'related_other' => [],
                'needs_attribute_picker' => false,
                'needs_config' => false,
                'store' => 0,
                'print1' => '',
                'print2' => '',
            ];
        }
        return $out;
    }

    private function loadRestaurantRow(int $id): ?array
    {
        $row = $this->select(
            "SELECT f_id, f_name, f_score, f_image_url, f_logo_url, f_category, f_eta_min, f_db,
                    IF(f_location IS NULL, NULL, ST_Y(f_location)) AS lat,
                    IF(f_location IS NULL, NULL, ST_X(f_location)) AS lng
             FROM ararix_restaurants
             WHERE f_id = ?
             LIMIT 1",
            "i",
            [$id]
        )->fetch_assoc();

        return $row ?: null;
    }

    private function formatRestaurant(array $r): array
    {
        $addr = $this->clientAddressStub();
        $lat = isset($r["lat"]) ? (float)$r["lat"] : null;
        $lng = isset($r["lng"]) ? (float)$r["lng"] : null;
        $distance = null;
        if ($lat !== null && $lng !== null) {
            $distance = (int)round($this->haversineMeters(
                (float)$addr["lat"],
                (float)$addr["lng"],
                $lat,
                $lng
            ));
        }

        return [
            "id" => (int)$r["f_id"],
            "name" => $r["f_name"],
            "score" => (int)$r["f_score"],
            "image_url" => $this->absoluteMediaUrl($r["f_image_url"] ?? null),
            "logo_url" => $this->absoluteMediaUrl($r["f_logo_url"] ?? null),
            "category" => $r["f_category"],
            "eta_min" => (int)($r["f_eta_min"] ?? 55),
            "distance_m" => $distance,
            "lat" => $lat,
            "lng" => $lng,
            "menu_configured" => $this->hasArarixMenu((int)$r["f_id"]),
        ];
    }

    /**
     * SelfBoard expects base64 PNG text (s_images.f_data from goods group editor).
     */
    private function normalizeMenuImagePayload($payload): string
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

    private function imageDataUri(string $payload): ?string
    {
        $payload = trim($payload);
        if ($payload === '') {
            return null;
        }
        if (preg_match('#^data:image#i', $payload)) {
            return $payload;
        }
        $mime = 'image/jpeg';
        if (strncmp($payload, 'iVBORw0KGgo', 11) === 0) {
            $mime = 'image/png';
        } elseif (strncmp($payload, '/9j/', 4) === 0) {
            $mime = 'image/jpeg';
        } elseif (strncmp($payload, 'R0lGOD', 6) === 0) {
            $mime = 'image/gif';
        } elseif (strncmp($payload, 'UklGR', 5) === 0) {
            $mime = 'image/webp';
        }
        return "data:{$mime};base64,{$payload}";
    }

    private function loadPackageComplectation(int $baseId): array
    {
        $sql = <<<SQL
        SELECT
            c.f_goods,
            c.f_price,
            c.f_qty,
            g.f_name AS f_goods_name,
            g.f_data,
            COALESCE(NULLIF(TRIM(gi.f_image), ''), '') AS f_image
        FROM c_goods_complectation c
        INNER JOIN c_goods g ON g.f_id = c.f_goods
        LEFT JOIN c_goods_images gi ON gi.f_id = g.f_id
        WHERE c.f_base = ?
        ORDER BY c.f_price, g.f_name
        SQL;

        $rows = $this->select($sql, "i", [$baseId])->fetch_all(MYSQLI_ASSOC);
        $out = [];
        foreach ($rows as $row) {
            $data = $this->parseGoodsData($row["f_data"] ?? null);
            $dyn = is_array($data["f_dynamic_attributes"] ?? null) ? $data["f_dynamic_attributes"] : [];
            $out[] = [
                "goods_id" => (int)$row["f_goods"],
                "name" => $row["f_goods_name"],
                "price" => (float)$row["f_price"],
                "qty" => (float)($row["f_qty"] ?? 1),
                "image" => $this->imageDataUri($this->normalizeMenuImagePayload($row["f_image"] ?? "")),
                "attr_type" => $this->dynStr($dyn["Type"] ?? null),
                "attr_size" => $this->dynStr($dyn["Size"] ?? null),
                "attr_measurement" => $this->dynMeasurement($dyn),
                "attr_type_price" => $this->dynPrice($dyn["Type"] ?? null),
                "attr_size_price" => $this->dynPrice($dyn["Size"] ?? null),
                "bju" => $this->bjuFromData($data),
                "dietary" => $this->dietaryFromData($data),
            ];
        }
        return $out;
    }

    private function loadMenuGroups(): array
    {
        $sql = <<<EOD
        SELECT
        gr.f_id,
        gr.f_name,
        COALESCE(si.f_data, '') AS f_image
        FROM c_groups gr
        LEFT JOIN s_images si ON si.f_id = gr.f_image
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
        order by  coalesce(gr.f_order, 9999), gr.f_name
        EOD;

        $groups = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $out = [];
        foreach ($groups as $group) {
            $out[] = [
                "id" => (int)$group["f_id"],
                "name" => $group["f_name"],
                "image" => $this->imageDataUri($this->normalizeMenuImagePayload($group["f_image"] ?? "")),
            ];
        }
        return $out;
    }

    private function loadMenuDishes(): array
    {
        $sql = <<< EOD
        SELECT m.f_id AS f_menu, m.f_name, mm.f_price,
        g.f_type,
        gr.f_name as f_group_name,
        mm.f_dish, g.f_group, g.f_name AS f_goods_name,
        mm.f_store, mm.f_print1, mm.f_print2, mm.f_state,
        mm.f_recent, g.f_data, g.f_description,
        COALESCE(NULLIF(TRIM(gi.f_image), ''), '') AS f_image
        FROM c_menu_names m
        LEFT JOIN c_menu mm ON mm.f_menu=m.f_id
        LEFT JOIN c_goods g ON g.f_id=mm.f_dish
        LEFT JOIN c_groups gr ON gr.f_id=g.f_group
        LEFT JOIN c_goods_images gi ON gi.f_id = g.f_id
        WHERE m.f_enabled=1 AND mm.f_state=1 AND g.f_type IN (1, 2, 5)
        order by coalesce(g.f_queue,9999)
        EOD;

        $dishes = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $out = [];
        foreach ($dishes as $dish) {
            $data = $this->parseGoodsData($dish["f_data"] ?? null);
            $dyn = is_array($data["f_dynamic_attributes"] ?? null) ? $data["f_dynamic_attributes"] : [];
            $modificators = $this->parseModificators($data);
            $type = (int)($dish["f_type"] ?? 1);
            $components = [];
            $relatedDrinks = [];
            $relatedOther = [];
            if ($type === 5) {
                $components = $this->loadPackageComplectation((int)$dish["f_dish"]);
                $relatedDrinks = $this->parseRelatedItems($data["f_related_drink"] ?? null);
                $relatedOther = $this->parseRelatedItems($data["f_related_other"] ?? null);
            }
            $attrType = $this->dynStr($dyn["Type"] ?? null);
            $attrSize = $this->dynStr($dyn["Size"] ?? null);
            $needsAttrPicker = $this->packageNeedsAttributePicker($components);
            $needsConfig = $type === 5
                || !empty($modificators)
                || !empty($relatedDrinks)
                || !empty($relatedOther);

            $out[] = [
                "id" => (int)$dish["f_dish"],
                "menu_id" => (int)$dish["f_menu"],
                "group_id" => (int)$dish["f_group"],
                "group_name" => $dish["f_group_name"] ?? "",
                "type" => $type,
                "name" => $dish["f_goods_name"] ?? $dish["f_name"] ?? "",
                "description" => trim((string)($dish["f_description"] ?? "")),
                "price" => (float)($dish["f_price"] ?? 0),
                "image" => $this->imageDataUri($this->normalizeMenuImagePayload($dish["f_image"] ?? "")),
                "prep_time" => trim((string)($data["f_prep_time"] ?? $data["f_cooking_time"] ?? "")),
                "popular" => !empty($dish["f_recent"]),
                "attr_type" => $attrType,
                "attr_size" => $attrSize,
                "attr_measurement" => $this->dynMeasurement($dyn),
                "bju" => $this->bjuFromData($data),
                "dietary" => $this->dietaryFromData($data),
                "modificators" => $modificators,
                "package_components" => $components,
                "related_drinks" => $relatedDrinks,
                "related_other" => $relatedOther,
                "needs_attribute_picker" => $needsAttrPicker,
                "needs_config" => $needsConfig,
                "store" => (int)($dish["f_store"] ?? 0),
                "print1" => $dish["f_print1"] ?? "",
                "print2" => $dish["f_print2"] ?? "",
            ];
        }
        return $out;
    }

    private function parseGoodsData($vData): array
    {
        if (is_array($vData)) {
            return $vData;
        }
        $raw = trim((string)$vData);
        if ($raw === "") {
            return [];
        }
        $parsed = json_decode($raw, true);
        return is_array($parsed) ? $parsed : [];
    }

    private function toBool(mixed $v): bool
    {
        return $v === true || $v === 1 || $v === 1.0 || $v === "1" || $v === "true";
    }

    private function dynStr(mixed $v): string
    {
        if (is_array($v)) {
            return trim((string)($v["value"] ?? ""));
        }
        return trim((string)($v ?? ""));
    }

    private function dynPrice(mixed $v): float
    {
        if (is_array($v)) {
            return (float)($v["price"] ?? 0);
        }
        return 0.0;
    }

    private function dynMeasurement(array $dyn): string
    {
        $m = trim((string)($dyn["Measurement"] ?? ""));
        if ($m !== "") {
            return $m;
        }
        $size = $dyn["Size"] ?? null;
        if (is_array($size)) {
            return trim((string)($size["measurement"] ?? ""));
        }
        return "";
    }

    private function packageNeedsAttributePicker(array $components): bool
    {
        if (empty($components)) {
            return false;
        }
        $types = [];
        $sizes = [];
        foreach ($components as $c) {
            $t = trim((string)($c["attr_type"] ?? ""));
            $s = trim((string)($c["attr_size"] ?? ""));
            if ($t !== "" && !in_array($t, $types, true)) {
                $types[] = $t;
            }
            if ($s !== "" && !in_array($s, $sizes, true)) {
                $sizes[] = $s;
            }
        }
        return count($types) > 1 || count($sizes) > 1;
    }

    private function parseRelatedItems(mixed $raw): array
    {
        if (!is_array($raw)) {
            return [];
        }
        $out = [];
        foreach ($raw as $item) {
            if (!is_array($item)) {
                continue;
            }
            $id = (int)($item["f_id"] ?? $item["id"] ?? 0);
            $name = trim((string)($item["f_name"] ?? $item["name"] ?? ""));
            if ($id <= 0) {
                continue;
            }
            $out[] = [
                "id" => $id,
                "name" => $name,
            ];
        }
        return $out;
    }

    private function bjuFromData(array $data): array
    {
        $bju = is_array($data["f_bju"] ?? null) ? $data["f_bju"] : [];
        return [
            "kcal" => (float)($bju["kcal"] ?? 0),
            "protein" => (float)($bju["protein"] ?? 0),
            "fat" => (float)($bju["fat"] ?? 0),
            "carbs" => (float)($bju["carbs"] ?? 0),
        ];
    }

    private function dietaryFromData(array $data): array
    {
        $badge = is_array($data["f_dietary_badge"] ?? null) ? $data["f_dietary_badge"] : [];
        return [
            "gluten_free" => $this->toBool($badge["gluten_free"] ?? false),
            "vegetarian" => $this->toBool($badge["vegetarian"] ?? false),
            "vegan" => $this->toBool($badge["vegan"] ?? false),
            "no_gmo" => $this->toBool($badge["no_gmo"] ?? false),
            "no_lactose" => $this->toBool($badge["no_lactose"] ?? false),
            "no_sugar" => $this->toBool($badge["no_sugar"] ?? false),
            "contains_nuts" => $this->toBool($badge["contains_nuts"] ?? false),
            "halal_kosher" => $this->toBool($badge["halal_kosher"] ?? false),
        ];
    }

    private function parseModificators(array $data): array
    {
        $raw = $data["f_modificators"] ?? null;
        if (!is_array($raw)) {
            return [];
        }
        $out = [];
        foreach ($raw as $item) {
            if (!is_array($item)) {
                continue;
            }
            $id = (int)($item["f_id"] ?? $item["id"] ?? 0);
            $name = trim((string)($item["f_name"] ?? $item["name"] ?? ""));
            if ($id <= 0 || $name === "") {
                continue;
            }
            $out[] = [
                "id" => $id,
                "name" => $name,
                "price" => (float)($item["f_price"] ?? $item["price"] ?? 0),
                "required" => $this->toBool($item["f_required"] ?? $item["required"] ?? false),
            ];
        }
        return $out;
    }
}
