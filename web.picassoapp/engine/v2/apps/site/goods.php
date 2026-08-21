<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Goods extends Auth
{
    /**
     * Catalog for the public site.
     * Optional params->lu (last update): filter groups/goods changed after this timestamp.
     * Units are always returned in full.
     */
    public function Catalog($params)
    {
        $lu = trim((string)($params->lu ?? ""));

        $groupsSql = <<<SQL
            SELECT
                gr.f_id,
                gr.f_name AS f_hy,
                gr.f_name AS f_ru,
                gr.f_name AS f_en,
                gr.lu
            FROM c_groups gr
            WHERE CAST(COALESCE(JSON_VALUE(gr.f_data, '$.f_online_sale'), '0') AS UNSIGNED) = 1
        SQL;
        $groupsTypes = "";
        $groupsBinds = [];
        if ($lu !== "") {
            $groupsSql .= " AND gr.lu > ?";
            $groupsTypes = "s";
            $groupsBinds[] = $lu;
        }
        $groupsSql .= " ORDER BY COALESCE(gr.f_order, 9999), gr.f_name";

        $goodsSql = <<<SQL
            SELECT
                g.f_id,
                g.f_group,
                g.f_name AS f_hy,
                g.f_name AS f_ru,
                g.f_name AS f_en,
                g.f_unit,
                g.f_scancode as f_sku,
                gp.f_price1 AS f_retail,
                gp.f_price1disc AS f_retail_discounted,
                JSON_VALUE(g.f_data, '$.f_big_image_url') AS f_big_image_url,
                JSON_VALUE(g.f_data, '$.f_small_image_url') AS f_small_image_url,
                g.lu
            FROM c_goods g
            LEFT JOIN c_goods_prices gp ON gp.f_goods = g.f_id AND gp.f_currency = 1
            WHERE g.f_enabled = 1
              AND CAST(COALESCE(JSON_VALUE(g.f_data, '$.f_online_sale'), '0') AS UNSIGNED) = 1
        SQL;
        $goodsTypes = "";
        $goodsBinds = [];
        if ($lu !== "") {
            $goodsSql .= " AND g.lu > ?";
            $goodsTypes = "s";
            $goodsBinds[] = $lu;
        }
        $goodsSql .= " ORDER BY g.f_name";

        $unitsSql = <<<SQL
            SELECT
                u.f_id,
                u.f_name AS f_hy,
                u.f_name AS f_ru,
                u.f_name AS f_en
            FROM c_units u
            ORDER BY u.f_name
        SQL;

        $this->result["groups"] = $this->select($groupsSql, $groupsTypes, $groupsBinds)->fetch_all(MYSQLI_ASSOC);
        $this->result["goods"] = $this->select($goodsSql, $goodsTypes, $goodsBinds)->fetch_all(MYSQLI_ASSOC);
        $this->result["units"] = $this->select($unitsSql)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
