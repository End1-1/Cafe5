<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Home extends ArarixAuth
{
    public function Get($params)
    {
        // Address: stub until client addresses are stored.
        $this->result["address"] = [
            "label" => "Komitas Avenue, 8",
            "lat" => 40.1872,
            "lng" => 44.5121,
        ];

        $this->result["promo"] = [
            "title" => "Order Salmon Steak Today",
            "subtitle" => "And Save Up To",
            "placeholder" => true,
        ];

        $restaurants = $this->select(
            "SELECT f_id AS id,
                    f_name AS name,
                    f_score AS score,
                    f_image_url AS image_url,
                    f_category AS category,
                    IF(f_location IS NULL, NULL, ST_Y(f_location)) AS lat,
                    IF(f_location IS NULL, NULL, ST_X(f_location)) AS lng
             FROM ararix_restaurants
             ORDER BY f_score DESC, f_name
             LIMIT 30"
        )->fetch_all(MYSQLI_ASSOC);

        $clientLat = (float)($this->result["address"]["lat"] ?? 0);
        $clientLng = (float)($this->result["address"]["lng"] ?? 0);
        foreach ($restaurants as &$r) {
            $r["id"] = (int)$r["id"];
            $r["score"] = (int)$r["score"];
            $lat = isset($r["lat"]) ? (float)$r["lat"] : null;
            $lng = isset($r["lng"]) ? (float)$r["lng"] : null;
            $r["lat"] = $lat;
            $r["lng"] = $lng;
            $r["distance_m"] = ($lat !== null && $lng !== null)
                ? (int)round($this->haversineMeters($clientLat, $clientLng, $lat, $lng))
                : null;
        }
        unset($r);
        $this->result["top_restaurants"] = $restaurants;

        $this->result["goods_groups"] = $this->select(
            "SELECT f_id AS id, f_name AS name, f_image_url AS image_url
             FROM ararix_goods_groups
             ORDER BY f_sort, f_name"
        )->fetch_all(MYSQLI_ASSOC);

        foreach ($this->result["goods_groups"] as &$g) {
            $g["id"] = (int)$g["id"];
        }
        unset($g);

        $this->result["countries"] = $this->select(
            "SELECT f_id AS id, f_name AS name
             FROM ararix_goods_country
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

    private function haversineMeters(float $lat1, float $lng1, float $lat2, float $lng2): float
    {
        $earth = 6371000.0;
        $dLat = deg2rad($lat2 - $lat1);
        $dLng = deg2rad($lng2 - $lng1);
        $a = sin($dLat / 2) ** 2
            + cos(deg2rad($lat1)) * cos(deg2rad($lat2)) * sin($dLng / 2) ** 2;
        return 2 * $earth * asin(min(1.0, sqrt($a)));
    }
}
