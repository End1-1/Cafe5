<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class GoodsList extends Auth
{
    public function get($params)
    {
        $storeId = (int)($params->store ?? 0);
        $currencyId = (int)($params->currency ?? 1);
        if ($currencyId <= 0) {
            $currencyId = 1;
        }

        $loadAll = $this->toBool($params->all ?? false);
        $q = trim((string)($params->q ?? ""));

        $searchSql = "1=1";
        $bindTypes = "ii";
        $bindValues = [$currencyId, $storeId];
        $limitSql = "";

        if (!$loadAll) {
            if (mb_strlen($q) <= 1) {
                $this->result["rows"] = [];
                $this->echoResult();
                return;
            }

            $searchName = !isset($params->search_name) || $this->toBool($params->search_name);
            $searchScancode = !isset($params->search_scancode) || $this->toBool($params->search_scancode);
            if (!$searchName && !$searchScancode) {
                $this->result["rows"] = [];
                $this->echoResult();
                return;
            }

            $like = "%" . $q . "%";
            $searchParts = [];
            if ($searchName) {
                $searchParts[] = "g.f_name LIKE ?";
                $bindTypes .= "s";
                $bindValues[] = $like;
            }
            if ($searchScancode) {
                $searchParts[] = "g.f_scancode LIKE ?";
                $bindTypes .= "s";
                $bindValues[] = $like;
            }
            $searchSql = "(" . implode(" OR ", $searchParts) . ")";
            $limitSql = "LIMIT 200";
        }

        $sql = <<<SQL
            SELECT
                g.f_id AS f_goods,
                gg.f_name AS f_groupname,
                g.f_scancode,
                g.f_name,
                COALESCE(ss.f_qty, 0) AS f_qty,
                IF(COALESCE(gp.f_price1disc, 0) > 0, gp.f_price1disc, gp.f_price1) AS f_price1,
                gp.f_price2,
                0 AS f_draftqty,
                0 AS f_reserve
            FROM c_goods g
            LEFT JOIN c_groups gg ON gg.f_id = g.f_group
            LEFT JOIN c_goods_prices gp ON gp.f_goods = g.f_id AND gp.f_currency = ?
            LEFT JOIN (
                SELECT f_item_id, SUM(f_qty_left) AS f_qty
                FROM store_stock
                WHERE f_store_id = ?
                GROUP BY f_item_id
            ) ss ON ss.f_item_id = g.f_id
            WHERE g.f_enabled = 1
              AND (
                    COALESCE(g.f_service, 0) = 1
                    OR COALESCE(ss.f_qty, 0) > 0
                  )
              AND {$searchSql}
            ORDER BY gg.f_name, g.f_name
            {$limitSql}
        SQL;

        $rows = [];
        $result = $this->select($sql, $bindTypes, $bindValues);
        while ($row = $result->fetch_assoc()) {
            $rows[] = [
                "f_goods" => (int)$row["f_goods"],
                "f_groupname" => $row["f_groupname"],
                "f_scancode" => $row["f_scancode"],
                "f_name" => $row["f_name"],
                "f_qty" => (float)$row["f_qty"],
                "f_price1" => (float)$row["f_price1"],
                "f_price2" => (float)$row["f_price2"],
                "f_draftqty" => (float)$row["f_draftqty"],
                "f_reserve" => (float)$row["f_reserve"],
            ];
        }

        $this->result["rows"] = $rows;
        $this->echoResult();
    }

    private function toBool($v): bool
    {
        if (is_bool($v)) {
            return $v;
        }
        if (is_numeric($v)) {
            return ((int)$v) !== 0;
        }
        $s = strtolower(trim((string)$v));
        return !in_array($s, ["", "0", "false", "no", "off"], true);
    }
}
