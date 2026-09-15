<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

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
        // Address: stub until client addresses are stored.
        $this->result["address"] = $this->clientAddressStub();

        $this->result["promo"] = [
            "title" => "Order Salmon Steak Today",
            "subtitle" => "And Save Up To",
            "placeholder" => true,
        ];

        $restaurants = $this->select(
            $this->restaurantSelectSql() . " LIMIT 30"
        )->fetch_all(MYSQLI_ASSOC);

        $clientLat = (float)($this->result["address"]["lat"] ?? 0);
        $clientLng = (float)($this->result["address"]["lng"] ?? 0);
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

    public function Search($params)
    {
        $q = trim((string)($params->q ?? $params->query ?? ''));
        $nationalityId = (int)($params->nationality_id ?? 0);

        $where = [];
        $types = '';
        $binds = [];

        if ($q !== '') {
            $where[] = 'r.f_name LIKE ?';
            $types .= 's';
            $binds[] = '%' . $q . '%';
        }
        if ($nationalityId > 0) {
            $where[] = 'r.f_nationality_id = ?';
            $types .= 'i';
            $binds[] = $nationalityId;
        }

        if ($where === []) {
            $this->result["restaurants"] = [];
            $this->echoResult();
            return;
        }

        $sql = $this->restaurantSelectSql('WHERE ' . implode(' AND ', $where)) . ' LIMIT 100';
        $rows = $this->select($sql, $types, $binds)->fetch_all(MYSQLI_ASSOC);

        $addr = $this->clientAddressStub();
        $this->result["restaurants"] = $this->mapRestaurants(
            $rows,
            (float)($addr["lat"] ?? 0),
            (float)($addr["lng"] ?? 0)
        );
        $this->echoResult();
    }
}
