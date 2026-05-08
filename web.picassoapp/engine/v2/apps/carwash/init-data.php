<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-29
# Last Modified: 2026-03-29

require_once __DIR__ . "/index.php";

/**
 * Логика MySQL FUNCTION sf_vip_init(params JSON) на PHP (см. Db).
 */
class InitData extends Auth
{
    /**
     * @param object $params ожидается f_menu (идентификатор меню)
     */
    public function Get($params)
    {
        $params = $this->ValidateParams($params, [
            'f_menu' => 'required|integer',
        ]);

        $fMenu = (int)$params->f_menu;

        $part1 = $this->select(
            "SELECT f_id, f_name FROM d_part1"
        )->fetch_all(MYSQLI_ASSOC);

        $part2 = $this->select(
            <<<SQL
            SELECT p.f_id, p.f_class as f_part, p.f_name, COALESCE(s.f_data, '') AS f_image
            FROM c_groups p
            LEFT JOIN s_images s ON s.f_id = p.f_image
            SQL
        )->fetch_all(MYSQLI_ASSOC);

        $dish = $this->select(
            <<<SQL
            SELECT
            d.f_group as f_part,
            m.f_dish,
            d.f_name as f_dish_name,
            m.f_price,
            m.f_print1,
            m.f_print2,
            '' AS f_image,
            COALESCE(d.f_description, '') AS f_comment,
            cast(coalesce(JSON_VALUE(d.f_data, '$.f_cooking_time') , 60) as unsigned) as f_cooking_time,
            COALESCE(d.f_adg, p2.f_adgcode) AS f_adgt
            FROM c_menu m
            LEFT JOIN c_goods d ON d.f_id = m.f_dish
            LEFT JOIN c_groups p2 ON p2.f_id = d.f_group
            WHERE m.f_state = 1 AND m.f_menu=?
            SQL,
            'i',
            [$fMenu]
        )->fetch_all(MYSQLI_ASSOC);

        if (empty($dish)) {
            dieWithCode("No dishes");
        }

        $tables = $this->select(
            <<<SQL
            SELECT t.f_id, COALESCE(h.f_id, '') AS f_order, t.f_name
            FROM h_tables t
            LEFT JOIN o_header h ON h.f_table = t.f_id AND h.f_state = 1
            WHERE t.f_hall = 1
            SQL
        )->fetch_all(MYSQLI_ASSOC);

        $payload = [
            'part1' => $part1,
            'part2' => $part2,
            'dish' => $dish,
            'tables' => $tables,
        ];

        $this->result['data'] = $payload;
        $this->echoResult();
    }

    /**
     * Аналог MariaDB JSON_DETAILED: отформатированная JSON-строка utf8.
     */
    private function jsonDetailed(?array $rows): string
    {
        if ($rows === null || $rows === []) {
            return json_encode([], JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
        }
        return json_encode($rows, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
    }
}
