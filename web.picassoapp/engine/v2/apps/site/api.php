<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-11-27 09:51:33
# Last Modified: 2026-08-25
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../waiter/order.php";
require_once __DIR__ . "/../waiter/cashbox.php";
require_once __DIR__ . "/../shop/view-order.php";

class Api extends Auth
{
    /**
     * Workstation for site sales: SELECT * FROM workstations WHERE f_id = $webapi_config_id
     * @return array{row: array, config: array}
     */
    private function loadWebApiWorkstation(): array
    {
        global $webapi_config_id;
        $id = (int)($webapi_config_id ?? 0);
        if ($id <= 0) {
            dieWithCode("webapi_config_id is not set in cnf.php");
        }
        $row = $this->select("SELECT * FROM workstations WHERE f_id=?", "i", [$id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Workstation not found: " . $id);
        }
        $cfg = json_decode($row["f_config"] ?? "{}", true);
        if (!is_array($cfg)) {
            $cfg = [];
        }
        if ((int)($cfg["f_cashbox_id"] ?? 0) <= 0) {
            dieWithCode("f_cashbox_id is not set in workstation config " . $id);
        }
        return ["row" => $row, "config" => $cfg];
    }

    public function stock($params)
    {
        $ws = $this->loadWebApiWorkstation();
        $defaultStore = (int)($ws["config"]["f_default_store_id"] ?? 1);
        $storeIds = [$defaultStore > 0 ? $defaultStore : 1];
        $storePlaceholders = implode(",", array_fill(0, count($storeIds), "?"));
        $types = str_repeat("i", count($storeIds));
        $binds = $storeIds;

        $skuFilter = "";
        if (!empty($params->sku) && is_array($params->sku)) {
            $skuList = array_values(array_filter(
                $params->sku,
                fn($x) => is_string($x) && $x !== ""
            ));
            if (!empty($skuList)) {
                $skuPlaceholders = implode(",", array_fill(0, count($skuList), "?"));
                $skuFilter = " AND g.f_scancode IN ($skuPlaceholders)";
                $types .= str_repeat("s", count($skuList));
                array_push($binds, ...$skuList);
            }
        }

        $sql = <<<SQL
            SELECT
            g.f_id as f_goods_id,
                g.f_scancode AS sku,
                st.f_store_id AS store,
                SUM(st.f_qty_left) AS qty,
                cp.f_price1 AS price1,
                cp.f_price1disc AS price1disc,
                g.f_name AS name,
                gr.f_name AS groupname
            FROM store_stock st
            INNER JOIN c_goods g ON g.f_id = st.f_item_id
            LEFT JOIN c_goods_prices cp ON cp.f_goods = g.f_id AND cp.f_currency = 1
            INNER JOIN c_groups gr ON gr.f_id = g.f_group
            WHERE st.f_store_id IN ($storePlaceholders)
              AND CAST(COALESCE(JSON_VALUE(gr.f_data, '$.f_online_sale'), '0') AS UNSIGNED) = 1
              AND CAST(COALESCE(JSON_VALUE(g.f_data, '$.f_online_sale'), '0') AS UNSIGNED) = 1
              {$skuFilter}
            GROUP BY g.f_id, g.f_scancode, st.f_store_id, cp.f_price1, cp.f_price1disc, g.f_name, gr.f_name
            HAVING SUM(st.f_qty_left) > 0
        SQL;

        $this->result["data"] = $this->select($sql, $types, $binds)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function ReserveCreate($params)
    {
        $this->echoResult();
    }

    public function ReserveCancel($params)
    {
        $this->echoResult();
    }

    public function PurchaseCreate($params)
    {
        $allow_store = [0, 1, 2, 3, 5, 24];
        $ws = $this->loadWebApiWorkstation();
        $cfg = $ws["config"];
        $defaultStore = (int)($cfg["f_default_store_id"] ?? 1);
        if ($defaultStore <= 0) {
            $defaultStore = 1;
        }

        if (empty($params->items)) {
            dieWithCode("Empty list of goods");
        }

        $order_id = (string)($params->{"query-id"} ?? "");
        if ($order_id === "" || $order_id === "auto") {
            $order_id = "auto";
        }

        $items = [];
        foreach ($params->items as $item) {
            $item = is_array($item) ? (object)$item : $item;
            $store = (int)($item->store ?? 0);
            if ($store === 0) {
                $store = $defaultStore;
            }
            if (!in_array($store, $allow_store, true)) {
                dieWithCode("Store not allowed: " . $store);
            }

            $row = $this->select(
                "SELECT g.f_id, g.f_name, g.f_scancode,
                        CAST(COALESCE(JSON_VALUE(gr.f_data, '$.f_online_sale'), '0') AS UNSIGNED) AS f_group_online,
                        CAST(COALESCE(JSON_VALUE(g.f_data, '$.f_online_sale'), '0') AS UNSIGNED) AS f_goods_online
                 FROM c_goods g
                 INNER JOIN c_groups gr ON gr.f_id = g.f_group
                 WHERE g.f_scancode=? LIMIT 1",
                "s",
                [$item->sku]
            )->fetch_assoc();
            if (!$row) {
                dieWithCode("Goods not found: " . $item->sku);
            }
            if ((int)$row["f_group_online"] !== 1) {
                dieWithCode("Goods group is not available for online sale: " . $item->sku);
            }
            if ((int)$row["f_goods_online"] !== 1) {
                dieWithCode("Goods not available for online sale: " . $item->sku);
            }

            $items[] = (object)[
                "goods_id" => (int)$row["f_id"],
                "dish" => (int)$row["f_id"],
                "name" => $row["f_name"],
                "store" => $store,
                "qty" => (float)$item->qty,
                "price" => (float)$item->price,
            ];
        }

        $table = (int)($params->table ?? $cfg["f_default_table_id"] ?? 0);
        if ($table <= 0) {
            $hallId = (int)($cfg["f_default_hall_id"] ?? 0);
            if ($hallId > 0) {
                $trow = $this->select(
                    "SELECT f_id FROM c_tables WHERE f_hall=? ORDER BY f_id LIMIT 1",
                    "i",
                    [$hallId]
                )->fetch_assoc();
                if ($trow) {
                    $table = (int)$trow["f_id"];
                }
            }
            if ($table <= 0) {
                $table = 1;
            }
        }
        $cashboxId = (int)($params->cashbox_id ?? $cfg["f_cashbox_id"] ?? 0);
        $staffId = (int)($params->staff_id ?? 0);

        $ord = new Order();
        if (!$ord->auth()) {
            dieWithCode("Unauthorized");
        }

        $cc = new Cashbox();
        if (!$cc->auth()) {
            dieWithCode("Unauthorized");
        }
        $session = $cc->ensureDailyOpenSession($cashboxId, (int)$this->userid, 0);
        if (!$session || empty($session["f_id"])) {
            dieWithCode(Translator::t("No active cashbox session"));
        }

        $saleParams = (object)[
            "table" => $table,
            "cashbox_id" => $cashboxId,
            "cash_session_id" => (int)$session["f_id"],
            "staff_id" => $staffId,
            "order_id" => $order_id,
            "items" => $items,
            "payment_method" => (int)($params->{"payment-method"} ?? $params->payment_method ?? 0),
            "service_factor" => 0,
            "discount_factor" => 0,
            "costumer" => $params->costumer ?? $params->customer ?? null,
        ];

        $order = $ord->createClosedSaleFromItems($saleParams);

        $this->result["f_id"] = (string)($order["f_id"] ?? "");
        $this->echoResult();
    }

    public function printFiscal($params)
    {
        $this->echoResult();
    }

    public function get($params)
    {
        $this->result["data"] = $this->select("select * from c_goods where f_id=?", "i", [$params->f_id])->fetch_assoc();
        $this->echoResult();
    }

    public function cancelPurchase($params)
    {
        $orderId = (string)($params->{"query-id"} ?? $params->f_id ?? $params->order_id ?? $params->id ?? "");
        if ($orderId === "") {
            dieWithCode("Missing order id");
        }

        $ws = $this->loadWebApiWorkstation();
        $cfg = $ws["config"];
        $defaultStore = (int)($cfg["f_default_store_id"] ?? 1);
        if ($defaultStore <= 0) {
            $defaultStore = 1;
        }
        $cashboxId = (int)($params->cashbox_id ?? $cfg["f_cashbox_id"] ?? 0);

        $src = $this->select("SELECT * FROM o_header WHERE f_id=?", "s", [$orderId])->fetch_assoc();
        if (!$src) {
            dieWithCode(Translator::t("Document is not exists"));
        }
        if ((int)$src["f_state"] !== 2 && (int)$src["f_state"] !== 1) {
            dieWithCode(Translator::t("Order is not closed"));
        }
        if ((int)($src["f_saletype"] ?? 0) === 4) {
            dieWithCode(Translator::t("You cannot return this item"));
        }

        $odata = json_decode($src["f_data"] ?? "{}", true) ?: [];
        if (($odata["f_source"] ?? "") !== "site") {
            dieWithCode("Not a site purchase");
        }

        $partialReturn = $this->select(
            "SELECT f_id FROM o_goods WHERE f_header=? AND COALESCE(f_returnedqty, 0) > 0 LIMIT 1",
            "s",
            [$orderId]
        )->fetch_assoc();
        if ($partialReturn) {
            dieWithCode("Purchase was already partially returned");
        }

        $returnDoc = $this->select(
            "SELECT f_id FROM o_header
             WHERE f_saletype=4 AND JSON_UNQUOTE(JSON_EXTRACT(f_data, '$.f_return_from'))=?
             LIMIT 1",
            "s",
            [$orderId]
        )->fetch_assoc();
        if ($returnDoc) {
            dieWithCode("Purchase was already cancelled");
        }

        $goodsRows = $this->select(
            "SELECT f_id, f_qty, COALESCE(f_returnedqty, 0) AS f_returnedqty, f_price, f_store
             FROM o_goods
             WHERE f_header=? AND f_state=1",
            "s",
            [$orderId]
        )->fetch_all(MYSQLI_ASSOC);

        $items = [];
        $storeId = $defaultStore;
        foreach ($goodsRows as $row) {
            $available = (float)$row["f_qty"] - (float)$row["f_returnedqty"];
            if ($available < 0.0001 || (float)$row["f_price"] < 0) {
                continue;
            }
            $lineStore = (int)($row["f_store"] ?? 0);
            if ($lineStore > 0) {
                $storeId = $lineStore;
            }
            $items[] = (object)[
                "id" => (string)$row["f_id"],
                "qty" => $available,
            ];
        }
        if (empty($items)) {
            dieWithCode(Translator::t("Nothing to return"));
        }

        $vo = new ViewOrder();
        if (!$vo->auth()) {
            dieWithCode("Unauthorized");
        }

        $cc = new Cashbox();
        if (!$cc->auth()) {
            dieWithCode("Unauthorized");
        }
        $session = $cc->ensureDailyOpenSession($cashboxId, (int)$this->userid, 0);
        if (!$session || empty($session["f_id"])) {
            dieWithCode(Translator::t("No active cashbox session"));
        }

        $result = $vo->performCreateReturn((object)[
            "id" => $orderId,
            "reason_id" => 0,
            "reason" => trim((string)($params->reason ?? "")),
            "items" => $items,
            "cash_session_id" => (int)$session["f_id"],
            "cashbox_id" => $cashboxId,
            "store_id" => $storeId,
        ]);

        // Pending site print uses f_state=1; after cancel mark closed so Shop won't print.
        if ((int)$src["f_state"] === 1) {
            $odata = json_decode($src["f_data"] ?? "{}", true) ?: [];
            unset($odata["f_site_print_pending"]);
            $this->select(
                "UPDATE o_header SET f_state=2, f_data=? WHERE f_id=? AND f_state=1",
                "ss",
                [json_encode($odata, JSON_UNESCAPED_UNICODE), $orderId],
                true
            );
        } elseif (!empty(json_decode($src["f_data"] ?? "{}", true)["f_site_print_pending"])) {
            $odata = json_decode($src["f_data"] ?? "{}", true) ?: [];
            unset($odata["f_site_print_pending"]);
            $this->update(
                "o_header",
                ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)],
                $orderId
            );
        }

        $this->result["f_id"] = (string)($result["return_order_id"] ?? "");
        $this->echoResult();
    }
}
