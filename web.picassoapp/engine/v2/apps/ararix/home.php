<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/locale.php";

class Home extends ArarixAuth
{
    private function mapRestaurants(array $restaurants, float $clientLat, float $clientLng): array
    {
        foreach ($restaurants as &$r) {
            $r["id"] = (int)$r["id"];
            $r["score"] = (int)$r["score"];
            $natId = isset($r["nationality_id"]) ? (int)$r["nationality_id"] : 0;
            $r["nationality_id"] = $natId > 0 ? $natId : null;
            $lat = isset($r["lat"]) ? (float)$r["lat"] : null;
            $lng = isset($r["lng"]) ? (float)$r["lng"] : null;
            $r["lat"] = $lat;
            $r["lng"] = $lng;
            $r["distance_m"] = ($lat !== null && $lng !== null)
                ? (int)round($this->haversineMeters($clientLat, $clientLng, $lat, $lng))
                : null;
            $r["image_url"] = $this->absoluteMediaUrl($r["image_url"] ?? null);
            $r["logo_url"] = $this->absoluteMediaUrl($r["logo_url"] ?? null);
            $r["eta_min"] = (int)($r["eta_min"] ?? 55);
        }
        unset($r);
        return $restaurants;
    }

    private function restaurantSelectSql(string $where = ""): string
    {
        return "SELECT r.f_id AS id,
                       r.f_name AS name,
                       r.f_score AS score,
                       r.f_image_url AS image_url,
                       r.f_logo_url AS logo_url,
                       r.f_category AS category,
                       r.f_nationality_id AS nationality_id,
                       n.f_name AS nationality,
                       r.f_eta_min AS eta_min,
                       IF(r.f_location IS NULL, NULL, ST_Y(r.f_location)) AS lat,
                       IF(r.f_location IS NULL, NULL, ST_X(r.f_location)) AS lng
                FROM ararix_restaurants r
                LEFT JOIN ararix_restaurant_nationality n ON n.f_id = r.f_nationality_id
                $where
                ORDER BY r.f_score DESC, r.f_name";
    }

    public function Get($params)
    {
        $this->result["address"] = $this->clientActiveAddress();

        $this->result["promo"] = [
            "title" => "Order Salmon Steak Today",
            "subtitle" => "And Save Up To",
            "placeholder" => true,
        ];

        $restaurants = $this->select(
            $this->restaurantSelectSql() . " LIMIT 30"
        )->fetch_all(MYSQLI_ASSOC);

        $clientLat = (float)($this->result["address"]["lat"] ?? $this->defaultMapCenter()["lat"]);
        $clientLng = (float)($this->result["address"]["lng"] ?? $this->defaultMapCenter()["lng"]);
        $restaurants = $this->mapRestaurants($restaurants, $clientLat, $clientLng);
        $this->result["top_restaurants"] = $restaurants;

        $this->result["goods_groups"] = $this->select(
            "SELECT f_id AS id, f_name AS name, f_image_url AS image_url
             FROM ararix_goods_groups
             ORDER BY f_sort, f_name"
        )->fetch_all(MYSQLI_ASSOC);

        foreach ($this->result["goods_groups"] as &$g) {
            $g["id"] = (int)$g["id"];
            $g["image_url"] = $this->absoluteMediaUrl($g["image_url"] ?? null);
        }
        unset($g);

        // Cuisine / nationality chips for home quick filter.
        $this->result["countries"] = $this->select(
            "SELECT f_id AS id, f_name AS name
             FROM ararix_restaurant_nationality
             ORDER BY f_sort, f_name"
        )->fetch_all(MYSQLI_ASSOC);

        foreach ($this->result["countries"] as &$c) {
            $c["id"] = (int)$c["id"];
        }
        unset($c);

        // Top offer of the day — stub card until dedicated table exists.
        $top = $restaurants[0] ?? null;
        $this->result["top_offer"] = [
            "placeholder" => true,
            "title" => "10% Off",
            "dish_name" => "Cheese Pizza",
            "restaurant_name" => $top["name"] ?? "Pizza Lab",
            "restaurant_id" => $top["id"] ?? null,
            "category" => $top["category"] ?? "Pizzeria / Mexican",
            "rating" => 4.5,
            "distance_m" => $top["distance_m"] ?? 250,
            "image_url" => null,
        ];

        $this->echoResult();
    }

    /**
     * Full restaurant catalog for Menu tab (not limited to home "top").
     */
    public function Restaurants($params)
    {
        $limit = (int)($params->limit ?? 200);
        if ($limit < 1) {
            $limit = 200;
        }
        if ($limit > 500) {
            $limit = 500;
        }

        $sql = $this->restaurantSelectSql() . " LIMIT {$limit}";
        $rows = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $addr = $this->clientActiveAddress();
        $this->result["restaurants"] = $this->mapRestaurants(
            $rows,
            (float)($addr["lat"] ?? $this->defaultMapCenter()["lat"]),
            (float)($addr["lng"] ?? $this->defaultMapCenter()["lng"])
        );
        $this->echoResult();
    }

    public function Search($params)
    {
        $q = trim((string)($params->q ?? $params->query ?? ''));

        $nationalityIds = [];
        if (isset($params->nationality_ids) && is_array($params->nationality_ids)) {
            foreach ($params->nationality_ids as $nid) {
                $nid = (int)$nid;
                if ($nid > 0) {
                    $nationalityIds[] = $nid;
                }
            }
        } elseif (!empty($params->nationality_id)) {
            $nid = (int)$params->nationality_id;
            if ($nid > 0) {
                $nationalityIds[] = $nid;
            }
        }
        $nationalityIds = array_values(array_unique($nationalityIds));

        $groupId = (int)($params->group_id ?? 0);

        $where = [];
        $types = '';
        $binds = [];

        if ($q !== '') {
            $where[] = 'r.f_name LIKE ?';
            $types .= 's';
            $binds[] = '%' . $q . '%';
        }
        if ($nationalityIds !== []) {
            $placeholders = implode(',', array_fill(0, count($nationalityIds), '?'));
            $where[] = "r.f_nationality_id IN ($placeholders)";
            $types .= str_repeat('i', count($nationalityIds));
            foreach ($nationalityIds as $nid) {
                $binds[] = $nid;
            }
        }
        if ($groupId > 0) {
            // Restaurants that actually sell dishes in this goods group.
            $where[] = 'EXISTS (
                SELECT 1 FROM ararix_menu m
                WHERE m.f_restaurant_id = r.f_id
                  AND m.f_group_id = ?
                  AND m.f_state = 1
            )';
            $types .= 'i';
            $binds[] = $groupId;
        }

        if ($where === []) {
            $this->result["restaurants"] = [];
            $this->echoResult();
            return;
        }

        $sql = $this->restaurantSelectSql('WHERE ' . implode(' AND ', $where)) . ' LIMIT 100';
        $rows = $this->select($sql, $types, $binds)->fetch_all(MYSQLI_ASSOC);

        $addr = $this->clientActiveAddress();
        $this->result["restaurants"] = $this->mapRestaurants(
            $rows,
            (float)($addr["lat"] ?? $this->defaultMapCenter()["lat"]),
            (float)($addr["lng"] ?? $this->defaultMapCenter()["lng"])
        );
        $this->echoResult();
    }

    /**
     * Autocomplete for home search.
     * Suggestions order: matching restaurants first, then matching dishes.
     */
    public function Suggest($params)
    {
        $q = trim((string)($params->q ?? $params->query ?? ''));
        if (mb_strlen($q) < 1) {
            $this->result["suggestions"] = [];
            $this->echoResult();
            return;
        }

        $locale = resolve_menu_locale($params, strtolower((string)($this->client["f_locale"] ?? "en")));
        $like = '%' . $q . '%';
        $limitRestaurants = 8;
        $limitDishes = 8;

        $restaurants = $this->select(
            "SELECT r.f_id AS id,
                    r.f_name AS name,
                    r.f_logo_url AS image_url,
                    r.f_category AS subtitle
             FROM ararix_restaurants r
             WHERE r.f_name LIKE ?
             ORDER BY
               CASE WHEN r.f_name LIKE ? THEN 0 ELSE 1 END,
               r.f_score DESC,
               r.f_name
             LIMIT {$limitRestaurants}",
            'ss',
            [$like, $q . '%']
        )->fetch_all(MYSQLI_ASSOC);

        $dishes = $this->select(
            "SELECT m.f_id AS id,
                    COALESCE(NULLIF(TRIM(tr.f_name), ''), m.f_name) AS name,
                    m.f_image_url AS image_url,
                    m.f_restaurant_id AS restaurant_id,
                    r.f_name AS restaurant_name,
                    m.f_price AS price
             FROM ararix_menu m
             INNER JOIN ararix_restaurants r ON r.f_id = m.f_restaurant_id
             LEFT JOIN ararix_menu_tr tr ON tr.f_menu_id = m.f_id AND tr.f_lang = ?
             WHERE m.f_state = 1
               AND (
                    m.f_name LIKE ?
                    OR tr.f_name LIKE ?
               )
             ORDER BY
               CASE
                 WHEN COALESCE(NULLIF(TRIM(tr.f_name), ''), m.f_name) LIKE ? THEN 0
                 ELSE 1
               END,
               m.f_sort,
               name
             LIMIT {$limitDishes}",
            'ssss',
            [$locale, $like, $like, $q . '%']
        )->fetch_all(MYSQLI_ASSOC);

        $suggestions = [];
        foreach ($restaurants as $row) {
            $image = trim((string)($row['image_url'] ?? ''));
            $suggestions[] = [
                'type' => 'restaurant',
                'id' => (int)$row['id'],
                'name' => (string)$row['name'],
                'subtitle' => (string)($row['subtitle'] ?? ''),
                'image_url' => $image !== '' ? $this->absoluteMediaUrl($image) : null,
                'restaurant_id' => (int)$row['id'],
                'restaurant_name' => (string)$row['name'],
            ];
        }
        foreach ($dishes as $row) {
            $image = trim((string)($row['image_url'] ?? ''));
            $suggestions[] = [
                'type' => 'dish',
                'id' => (int)$row['id'],
                'name' => (string)$row['name'],
                'subtitle' => (string)($row['restaurant_name'] ?? ''),
                'image_url' => $image !== '' ? $this->absoluteMediaUrl($image) : null,
                'restaurant_id' => (int)$row['restaurant_id'],
                'restaurant_name' => (string)($row['restaurant_name'] ?? ''),
                'price' => (float)($row['price'] ?? 0),
            ];
        }

        $this->result["suggestions"] = $suggestions;
        $this->result["locale"] = $locale;
        $this->echoResult();
    }
}
