<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-28 14:59:24
# Last Modified: 2026-03-14 09:46:32
require_once __DIR__ . "/index.php";

class Menu extends Auth
{
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

        // Already base64 PNG (how ce5goodsgroup saves f_data).
        if (strncmp($trim, 'iVBORw0KGgo', 11) === 0) {
            return $trim;
        }

        // Raw PNG bytes in DB.
        if (strlen($payload) >= 8 && strncmp($payload, "\x89PNG\r\n\x1a\n", 8) === 0) {
            return base64_encode($payload);
        }

        return $trim;
    }

    /**
     * SelfBoard package (f_type=5): component lines from c_goods_complectation.
     */
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
        foreach ($rows as &$row) {
            $row['f_image'] = $this->normalizeMenuImagePayload($row['f_image'] ?? '');
        }
        unset($row);
        return $rows;
    }

    public function get($params)
    {
        $sql = <<<EOD
        SELECT
        gr.f_id,
        gr.f_parent,
        gr.f_name,
        gr.f_taxdept as f_fiscal_department,
        gr.f_order as f_queue,
        COALESCE(g.f_qty, 0) AS f_qty,
        CAST(gr.f_class AS INT) AS f_class,
        gr.f_color,
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
        foreach ($groups as &$group) {
            $group['f_image'] = $this->normalizeMenuImagePayload($group['f_image'] ?? '');
        }
        unset($group);
        $this->result["groups"] = $groups;

        $sql = <<< EOD
        SELECT m.f_id AS f_menu, m.f_name, mm.f_price, 
        g.f_type,
        gr.f_taxdept as f_fiscal_department,
        gr.f_name as f_group_name,
        mm.f_dish, g.f_group, g.f_name AS f_goods_name, g.f_color,gr.f_taxdept,
        mm.f_store, mm.f_print1, mm.f_print2, mm.f_state,
        if (length(coalesce(g.f_adg, ''))>0, g.f_adg, gr.f_adgcode) AS f_adgt, gr.f_taxdept,
        mm.f_recent, g.f_emark_required, cast(coalesce(sl.f_qty, -999) AS INT) as f_stoplist,
        g.f_data, g.f_scancode, g.f_description,
        COALESCE(NULLIF(TRIM(gi.f_image), ''), '') AS f_image
        FROM c_menu_names m
        LEFT JOIN c_menu mm ON mm.f_menu=m.f_id 
        LEFT JOIN c_goods g ON g.f_id=mm.f_dish
        LEFT JOIN c_groups gr ON gr.f_id=g.f_group
        LEFT JOIN c_goods_images gi ON gi.f_id = g.f_id
        left join c_stoplist sl on sl.f_dish=mm.f_dish
        WHERE m.f_enabled=1 AND mm.f_state=1 AND g.f_type IN (1, 2, 5)
        order by coalesce(g.f_queue,9999)
        EOD;
        $dishes = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        foreach ($dishes as &$dish) {
            $dish['f_image'] = $this->normalizeMenuImagePayload($dish['f_image'] ?? '');
            if ((int)($dish['f_type'] ?? 0) === 5) {
                $dish['f_complectation'] = $this->loadPackageComplectation((int)$dish['f_dish']);
            }
        }
        unset($dish);
        $this->result["dishes"] = $dishes;
        $this->echoResult();
    }

    public function GetComments($params)
    {
        $this->result["data"] = $this->select("select f_name from c_goods_comment")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetRemoveReason($params)
    {
        $this->result["data"] = $this->select("select f_name from o_dish_remove_reason")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetMenuNames($params)
    {
        $this->result["names"] = $this->select("select f_id, f_name from c_menu_names where f_enabled=1")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
