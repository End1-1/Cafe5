<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-11-27 09:51:33
# Last Modified: 2026-08-21
require_once __DIR__ . "/index.php";

class Api extends Auth
{
    public function stock($params)
    {
        $storeIds = [1];
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
        $allow_store = [0, 2, 3, 5, 24];
        $session = uuid_v4();

        $total = 0;
        $goodsRows = [];

        if (empty($params->items)) {
            dieWithCode("Empty list of goods");
        }

        $order_id = $params->{"query-id"} ?? "";
        if ($order_id == "auto") {
            $order_id = uuid_v4();
        }

        foreach ($params->items as $i => $item) {
            if ((int)$item->store == 0) {
                $item->store = 2;
            }
            if (!in_array((int)$item->store, $allow_store, true)) {
                dieWithCode("Store not allowed: " . $item->store);
            }
            $res = $this->select(
                "select f_id from c_goods where f_scancode=? limit 1",
                "s",
                [$item->sku]
            );

            $row = $res->fetch_assoc();
            if (!$row) {
                dieWithCode("Goods not found: " . $item->sku);
            }

            $goodsId = $row['f_id'];

            $rowTotal = $item->qty * $item->price;
            $total += $rowTotal;

            $goodsRows[] = [
                "f_id" => uuid_v4(),
                "f_header" => $order_id,
                "f_store" => $item->store,
                "f_goods" => $goodsId,
                "f_qty" => $item->qty,
                "f_price" => $item->price,
                "f_total" => $rowTotal,
                "f_tax" => 0,
                "f_sign" => 1,
                "f_taxdebt" => 0,
                "f_row" => $i,
                "f_discountfactor" => 0,
                "f_discountmode" => 0,
                "f_discountamount" => 0,
                "f_return" => 0,
                "f_isservice" => 0,
                "f_amountaccumulate" => 0,
                "f_emarks" => ""
            ];
        }

        // ----------------------------------
        // распределяем оплату
        // ----------------------------------
        $amountCash = 0;
        $amountCard = 0;
        $amountIdram = 0;
        $amountTelcell = 0;

        switch ($params->{"payment-method"}) {
            case 1:
                $amountCash = $total;
                break;
            case 2:
                $amountCard = $total;
                break;
            case 4:
                $amountIdram = $total;
                break;
            case 7:
                $amountTelcell = $total;
                break;
        }

        // ----------------------------------
        // header
        // ----------------------------------
        $header = [
            "f_id" => $order_id,
            "f_state" => 2,
            "f_cashier" => 1,
            "f_staff" => 1,
            "f_source" => 2,
            "f_saletype" => 1,
            "f_currency" => 1,
            "f_dateopen" => date("Y-m-d"),
            "f_dateclose" => date("Y-m-d"),
            "f_datecash" => date("Y-m-d"),
            "f_timeopen" => date("H:i:s"),
            "f_timeclose" => date("H:i:s"),
            "f_amounttotal" => $total,
            "f_amountcash" => $amountCash,
            "f_amountcard" => $amountCard,
            "f_amountidram" => $amountIdram,
            "f_amounttelcell" => $amountTelcell,
            "f_cash" => $total,
            "f_change" => 0
        ];

        $jdoc = [
            "session" => $session,
            "header" => $header,
            "goods" => $goodsRows,
            "flags" => [
                "f_1" => 0,
                "f_2" => 0,
                "f_3" => 0,
                "f_4" => 0,
                "f_5" => 0
            ]
        ];

        $json = json_encode($jdoc, JSON_UNESCAPED_UNICODE);

        // ----------------------------------
        // вызов процедуры
        // ----------------------------------
        $this->callJsonProcedure("sf_create_shop_order", $json, true);

        // ----------------------------------
        // читаем результат
        // ----------------------------------
        $res = $this->select(
            "select f_result from a_result where f_session=?",
            "s",
            [$session]
        );

        $row = $res->fetch_assoc();
        if (!$row) {
            dieWithCode("Session result not found");
        }

        $this->result["result"] = $row['f_result'];
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
}
