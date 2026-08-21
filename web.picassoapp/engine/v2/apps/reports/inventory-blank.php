<?php
# © 2026 , Kudryashov Vasili
# Inventory blanks for Menu review (sys_data f_type=1)

require_once __DIR__ . "/../../worker/auth.php";

class InventoryBlank extends Auth
{
    private const TYPE_INVENTORY_BLANK = 1;

    public function List($params)
    {
        $rows = $this->select(
            "SELECT f_id, f_data FROM sys_data WHERE f_type = ? ORDER BY f_id DESC",
            "i",
            [self::TYPE_INVENTORY_BLANK]
        )->fetch_all(MYSQLI_ASSOC);

        $items = [];
        foreach ($rows as $row) {
            $data = json_decode($row["f_data"] ?? "{}", true);
            if (!is_array($data)) {
                $data = [];
            }
            $items[] = [
                "id" => (int)$row["f_id"],
                "title" => (string)($data["title"] ?? ("#" . $row["f_id"])),
                "created" => (string)($data["created"] ?? ""),
            ];
        }

        $this->result["items"] = $items;
        $this->echoResult();
    }

    public function Get($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("Invalid id", 400);
        }

        $row = $this->select(
            "SELECT f_id, f_data FROM sys_data WHERE f_id = ? AND f_type = ? LIMIT 1",
            "ii",
            [$id, self::TYPE_INVENTORY_BLANK]
        )->fetch_assoc();

        if (empty($row)) {
            dieWithCode("Not found", 404);
        }

        $data = json_decode($row["f_data"] ?? "{}", true);
        if (!is_array($data)) {
            $data = [];
        }
        $data["id"] = (int)$row["f_id"];
        $this->result["blank"] = $data;
        $this->echoResult();
    }

    public function Save($params)
    {
        $id = (int)($params->id ?? 0);
        $title = trim((string)($params->title ?? ""));
        if ($title === "") {
            $title = Translator::t("Inventory blank");
        }

        $storesIn = $params->stores ?? [];
        if (is_object($storesIn)) {
            $storesIn = json_decode(json_encode($storesIn), true);
        }
        if (!is_array($storesIn)) {
            $storesIn = [];
        }

        $stores = [];
        foreach ($storesIn as $store) {
            if (is_object($store)) {
                $store = json_decode(json_encode($store), true);
            }
            if (!is_array($store)) {
                continue;
            }
            $items = [];
            $rawItems = $store["items"] ?? [];
            if (is_object($rawItems)) {
                $rawItems = json_decode(json_encode($rawItems), true);
            }
            if (!is_array($rawItems)) {
                $rawItems = [];
            }
            foreach ($rawItems as $item) {
                if (is_object($item)) {
                    $item = json_decode(json_encode($item), true);
                }
                if (!is_array($item)) {
                    continue;
                }
                $name = trim((string)($item["name"] ?? ""));
                if ($name === "") {
                    continue;
                }
                $items[] = [
                    "goods_id" => (int)($item["goods_id"] ?? 0),
                    "code" => (string)($item["code"] ?? ""),
                    "name" => $name,
                    "unit" => (string)($item["unit"] ?? ""),
                    "sku" => (string)($item["sku"] ?? ""),
                    "group_id" => (int)($item["group_id"] ?? 0),
                    "group_name" => (string)($item["group_name"] ?? ""),
                ];
            }
            $stores[] = [
                "store_id" => (int)($store["store_id"] ?? 0),
                "store_name" => (string)($store["store_name"] ?? ""),
                "items" => $items,
            ];
        }

        $created = trim((string)($params->created ?? ""));
        if ($created === "") {
            $created = date("Y-m-d H:i:s");
        }

        $payload = [
            "title" => $title,
            "created" => $created,
            "stores" => $stores,
        ];
        $json = json_encode($payload, JSON_UNESCAPED_UNICODE);

        if ($id > 0) {
            $exists = $this->select(
                "SELECT f_id FROM sys_data WHERE f_id = ? AND f_type = ? LIMIT 1",
                "ii",
                [$id, self::TYPE_INVENTORY_BLANK]
            )->fetch_assoc();
            if (empty($exists)) {
                dieWithCode("Not found", 404);
            }
            $this->update("sys_data", ["f_data" => $json], $id);
        } else {
            $id = $this->insert("sys_data", [
                "f_type" => self::TYPE_INVENTORY_BLANK,
                "f_data" => $json,
            ]);
        }

        $payload["id"] = $id;
        $this->result["blank"] = $payload;
        $this->echoResult();
    }

    public function Remove($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("Invalid id", 400);
        }
        $this->select(
            "DELETE FROM sys_data WHERE f_id = ? AND f_type = ?",
            "ii",
            [$id, self::TYPE_INVENTORY_BLANK],
            true
        );
        $this->result["deleted"] = $id;
        $this->echoResult();
    }

    /**
     * Distinct stores that participate in recipes for current filters.
     */
    public function Stores($params)
    {
        $where = $this->buildFilterWhere($params);
        $sql = <<<EOD
        SELECT
            COALESCE(mm.f_store, 0) AS store_id,
            MAX(COALESCE(s.f_name, '')) AS store_name
        FROM c_menu mm
        INNER JOIN c_goods g ON g.f_id = mm.f_dish
        INNER JOIN c_goods_complectation c ON c.f_base = g.f_id
        LEFT JOIN c_storages s ON s.f_id = mm.f_store
        $where
        GROUP BY COALESCE(mm.f_store, 0)
        HAVING COUNT(DISTINCT c.f_goods) > 0
        ORDER BY store_name
        EOD;

        $rows = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $items = [];
        foreach ($rows as $row) {
            $items[] = [
                "store_id" => (int)$row["store_id"],
                "store_name" => (string)$row["store_name"],
            ];
        }
        $this->result["stores"] = $items;
        $this->echoResult();
    }

    /**
     * Build blank draft from dish recipes, grouped by c_menu.f_store.
     * Accepts same filters as menu-review: menu, status, dishgroup, dish, goods.
     * store_id > 0 — only that store; common_all=true — one merged blank for all stores.
     */
    public function Build($params)
    {
        $where = $this->buildFilterWhere($params);
        $storeId = (int)($params->store_id ?? 0);
        $commonAll = !empty($params->common_all) || $storeId < 0;

        if (!$commonAll && $storeId > 0) {
            $where .= " AND mm.f_store = {$storeId} ";
        }

        if ($commonAll) {
            $sql = <<<EOD
            SELECT
                0 AS store_id,
                '' AS store_name,
                COALESCE(ig.f_group, 0) AS group_id,
                MAX(COALESCE(gr.f_name, '')) AS group_name,
                ig.f_id AS goods_id,
                ig.f_id AS code,
                MAX(ig.f_name) AS name,
                MAX(COALESCE(u.f_name, '')) AS unit,
                MAX(COALESCE(ig.f_scancode, '')) AS sku
            FROM c_menu mm
            INNER JOIN c_goods g ON g.f_id = mm.f_dish
            INNER JOIN c_goods_complectation c ON c.f_base = g.f_id
            INNER JOIN c_goods ig ON ig.f_id = c.f_goods
            LEFT JOIN c_units u ON u.f_id = ig.f_unit
            LEFT JOIN c_groups gr ON gr.f_id = ig.f_group
            $where
            GROUP BY COALESCE(ig.f_group, 0), ig.f_id
            ORDER BY group_name, name
            EOD;
        } else {
            $sql = <<<EOD
            SELECT
                COALESCE(mm.f_store, 0) AS store_id,
                MAX(COALESCE(s.f_name, '')) AS store_name,
                COALESCE(ig.f_group, 0) AS group_id,
                MAX(COALESCE(gr.f_name, '')) AS group_name,
                ig.f_id AS goods_id,
                ig.f_id AS code,
                MAX(ig.f_name) AS name,
                MAX(COALESCE(u.f_name, '')) AS unit,
                MAX(COALESCE(ig.f_scancode, '')) AS sku
            FROM c_menu mm
            INNER JOIN c_goods g ON g.f_id = mm.f_dish
            INNER JOIN c_goods_complectation c ON c.f_base = g.f_id
            INNER JOIN c_goods ig ON ig.f_id = c.f_goods
            LEFT JOIN c_units u ON u.f_id = ig.f_unit
            LEFT JOIN c_storages s ON s.f_id = mm.f_store
            LEFT JOIN c_groups gr ON gr.f_id = ig.f_group
            $where
            GROUP BY COALESCE(mm.f_store, 0), COALESCE(ig.f_group, 0), ig.f_id
            ORDER BY store_name, group_name, name
            EOD;
        }

        $rows = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $byStore = [];
        foreach ($rows as $row) {
            $sid = (int)$row["store_id"];
            if (!isset($byStore[$sid])) {
                $storeName = (string)$row["store_name"];
                if ($commonAll) {
                    $storeName = Translator::t("All stores");
                }
                $byStore[$sid] = [
                    "store_id" => $sid,
                    "store_name" => $storeName,
                    "items" => [],
                ];
            }
            $byStore[$sid]["items"][] = [
                "goods_id" => (int)$row["goods_id"],
                "code" => (string)$row["code"],
                "name" => (string)$row["name"],
                "unit" => (string)$row["unit"],
                "sku" => (string)$row["sku"],
                "group_id" => (int)$row["group_id"],
                "group_name" => (string)$row["group_name"],
            ];
        }

        $title = Translator::t("Inventory blank") . " " . date("Y-m-d");
        if (!$commonAll && $storeId > 0 && !empty($byStore[$storeId]["store_name"])) {
            $title .= " — " . $byStore[$storeId]["store_name"];
        }

        $this->result["blank"] = [
            "id" => 0,
            "title" => $title,
            "created" => date("Y-m-d H:i:s"),
            "stores" => array_values($byStore),
        ];
        $this->echoResult();
    }

    private function buildFilterWhere($params): string
    {
        $where = "WHERE g.f_type IN (1, 2, 5) ";
        if (strlen($params->status ?? "") > 0) {
            $where .= " AND mm.f_state IN ({$params->status}) ";
        }
        if (!empty($params->menu)) {
            $where .= " AND mm.f_menu IN ({$params->menu}) ";
        }
        if (!empty($params->dish)) {
            $where .= " AND mm.f_dish IN ({$params->dish}) ";
        }
        if (!empty($params->dishgroup)) {
            $where .= " AND g.f_group IN ({$params->dishgroup}) ";
        }
        if (!empty($params->goods)) {
            $where .= " AND g.f_id IN (
                SELECT c.f_base
                FROM c_goods_complectation c
                WHERE c.f_goods IN ({$params->goods})
            ) ";
        }
        return $where;
    }
}
