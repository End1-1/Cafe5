<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-02-05 09:04:22
# Last Modified: 2026-02-05 09:04:25

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class Stock extends Auth
{
    public function quickCheck($params)
    {
        if (!empty($params->barcode)) {
            $row = $this->select("select f_id from c_goods where f_scancode=?", "s", [$params->barcode])->fetch_assoc();
            if (!$row) {
                dieWithCode("Do you want to hack quickCheck?");
            }
            $goods_id = $row["f_id"];
        } else {
            $goods_id = $params->goods_id;
        }
        if (!$goods_id) {
            dieWithCode("Do you want to hack quickCheck?");
        }
        $sql = <<<EOD
        select sum(f_qty*f_type) as f_qty from a_store where f_goods=? and f_store=?
        EOD;
        $row = $this->select($sql, "ii", [$goods_id, $params->store_id])->fetch_assoc();
        $this->result["qty"] = $row["f_qty"];
        $this->result = array_merge((array)$params, (array)$this->result);
        $this->echoResult();
    }

    /* NEW STOCK */
    public function HistoricalStore($params)
    {
        $sql = <<<EOD
        SELECT 
            g.f_id AS item_id,
            g.f_name AS item_name,
            gr.f_name AS group_name,
            u.f_name as unit_name,
            -- Остаток: (приходы - расходы) до даты; raw number (не money_fmt — клиент парсит локаль)
            ROUND(SUM(IFNULL(sm.f_qty_in, 0)) - SUM(IFNULL(sm.f_qty_out, 0)), 3) AS qty_sys,
            ROUND(g.f_lastinputprice, 2) AS price
        FROM c_goods g
        join c_units u on u.f_id=g.f_unit
        JOIN c_groups gr ON g.f_group = gr.f_id
        LEFT JOIN store_moves sm ON sm.f_item_id = g.f_id 
            AND sm.f_store_id = ?
        JOIN store_document sd ON sm.f_doc = sd.f_id 
            AND sd.f_doc_date <= ? 
           
        GROUP BY g.f_id
        HAVING qty_sys <> 0
         order by gr.f_name, g.f_name
    EOD;
        $rows = $this->select($sql, "is", [$params->store, $params->date])->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$r) {
            $r["qty_sys"] = (float)$r["qty_sys"];
            $r["price"] = (float)$r["price"];
            $r["item_id"] = (int)$r["item_id"];
        }
        unset($r);
        $this->result["data"] = $rows;
        $this->echoResult();
    }

    /** Balance of the given goods on one store up to a date, including negatives and zeros. */
    public function Balances($params)
    {
        $store = (int)($params->store ?? 0);
        $date = trim((string)($params->date ?? ""));
        $ids = [];
        foreach ($params->items ?? [] as $id) {
            $id = (int)$id;
            if ($id > 0) {
                $ids[$id] = $id;
            }
        }
        if ($store <= 0 || $date === "" || $ids === []) {
            dieWithCode(Translator::t("Store, date and items are required"));
        }
        $in = implode(",", $ids);
        // Movements are stored on f_storeid when the card points at another goods.
        // Count both ids, and only documents up to the requested date (a plain LEFT JOIN
        // would also add later documents and turn a real minus into the wrong number).
        $sql = <<<EOD
        SELECT g.f_id AS item_id,
               ROUND(
                   COALESCE(SUM(CASE WHEN sd.f_id IS NOT NULL AND sd.f_doc_date <= ? THEN sm.f_qty_in ELSE 0 END), 0)
                   - COALESCE(SUM(CASE WHEN sd.f_id IS NOT NULL AND sd.f_doc_date <= ? THEN sm.f_qty_out ELSE 0 END), 0)
               , 3) AS qty_sys
        FROM c_goods g
        LEFT JOIN store_moves sm
            ON sm.f_store_id = ?
           AND sm.f_item_id IN (g.f_id, IF(IFNULL(g.f_storeid, 0) = 0, g.f_id, g.f_storeid))
        LEFT JOIN store_document sd ON sd.f_id = sm.f_doc
        WHERE g.f_id IN ($in)
        GROUP BY g.f_id
        EOD;
        $rows = $this->select($sql, "ssi", [$date, $date, $store])->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$r) {
            $r["item_id"] = (int)$r["item_id"];
            $r["qty_sys"] = (float)$r["qty_sys"];
        }
        unset($r);
        $this->result["data"] = $rows;
        $this->echoResult();
    }

    public function SaveInventory($params)
    {
        // 1. Сначала определяем ID документа
        $uid = (!empty($params->f_id)) ? $params->f_id : uuid_v4();


        // В валидации убираем f_id из required, так как мы его уже определили выше
        $v = $this->ValidateParams($params, [
            'f_store' => 'required|integer',
            'f_date' => 'required|string'
        ]);

        $this->beginTransaction();

        try {
            // 3. Удаляем старые данные
            // ВНИМАНИЕ: используем $uid (новый или старый), а не $v->f_id
            $this->delete("store_inventory_user", $uid, "f_doc");
            $this->delete("store_inventory_document", $uid, "f_id");

            // 4. Вставляем заголовок
            $this->insert("store_inventory_document", [
                "f_id" => $uid,
                "f_date" => $v->f_date,
                "f_store" => $v->f_store,
                "f_data" => json_encode(array_merge($params->f_data ?? [], [
                    "f_create_user" => $this->fullName(),
                    "f_create_date" => date("Y-m-d H:i:s")
                ]), JSON_UNESCAPED_UNICODE),

            ]);

            // 5. Вставляем строки
            if (isset($params->items) && is_array($params->items)) {
                foreach ($params->items as $index => $item) {
                    $this->insert("store_inventory_user", [
                        "f_id" => uuid_v4(), // Лучше использовать uuid_v4() раз он у тебя есть
                        "f_doc" => $uid,
                        "f_row" => $index + 1,
                        "f_item" => (int)$item->f_item,
                        "f_qty_sys" => (float)$item->f_qty_sys,
                        "f_qty_user" => (float)$item->f_qty_user,
                        "f_price" => (float)($item->f_price ?? 0)
                    ]);
                }
            }

            $this->commit();
            $this->result["uid"] = $uid;
            $this->echoResult();
        } catch (Exception $e) {
            $this->rollback();
            dieWithCode($e->getMessage());
        }
    }

    public function OpenInventory($params)
    {
        // Валидация входного ID
        $v = $this->ValidateParams($params, [
            'id' => 'required|string'
        ]);

        // 1. Получаем заголовок документа
        // Делаем JOIN со складом, чтобы получить f_store_in_name
        $sql = <<<EOD
         SELECT 
            d.f_id, 
            d.f_date AS f_date, 
            d.f_store AS f_store,
            s.f_name AS f_store_name,
            d.f_data
        FROM store_inventory_document d
        LEFT JOIN c_storages s ON s.f_id = d.f_store
        WHERE d.f_id = ?
        EOD;
        $row = $this->select($sql, "s", [$v->id])->fetch_assoc();

        if (!$row) {
            dieWithCode("Document not found");
        }

        // Декодируем дополнительные данные (комментарии и т.д.)
        $f_data = json_decode($row['f_data'], true) ?? [];

        // 2. Получаем строки товаров
        // JOIN-им справочники товаров (c_goods), групп (c_groups) и единиц (c_units)
        $sql = <<<EOD
        SELECT 
            i.f_id,
            g.f_id AS f_item_id,
            g.f_name AS f_item_name,
            gr.f_name AS f_group_name,
            u.f_name AS f_unit_name,
            i.f_qty_user,
            i.f_qty_sys,
            (i.f_qty_user - i.f_qty_sys) AS f_qty_diff,
            i.f_price
        FROM store_inventory_user i
        LEFT JOIN c_goods g ON g.f_id = i.f_item
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        LEFT JOIN c_units u ON u.f_id = g.f_unit
        WHERE i.f_doc = ?
        ORDER BY i.f_row ASC
        EOD;
        $items = $this->select($sql, "s", [$v->id])->fetch_all(MYSQLI_ASSOC);
        foreach ($items as &$it) {
            $it["f_qty_user"] = (float)$it["f_qty_user"];
            $it["f_qty_sys"] = (float)$it["f_qty_sys"];
            $it["f_qty_diff"] = (float)$it["f_qty_diff"];
            $it["f_price"] = (float)$it["f_price"];
            $it["f_item_id"] = (int)$it["f_item_id"];
        }
        unset($it);

        // 3. Формируем финальный ответ под структуру C++
        $result = [
            "f_id" => $row['f_id'],
            "f_date" => $row['f_date'],
            "f_store" => (int)$row['f_store'],
            "f_store_name" => $row['f_store_name'],
            "f_data" => $f_data, // parseData(jo) в C++ заберет данные отсюда
            "items" => $items
        ];

        $this->result["data"] = $result;
        $this->result["status"] = 1;
        $this->echoResult();
    }

    public function RemoveInventory($params)
    {
        $v = $this->ValidateParams($params, [
            'id' => 'required|string'
        ]);

        $row = $this->select(
            "SELECT f_id FROM store_inventory_document WHERE f_id=?",
            "s",
            [$v->id]
        )->fetch_assoc();
        if (!$row) {
            dieWithCode("Document not found");
        }

        $this->beginTransaction();
        try {
            $this->delete("store_inventory_user", $v->id, "f_doc");
            $this->delete("store_inventory_document", $v->id, "f_id");
            $this->commit();
            $this->echoResult();
        } catch (Exception $e) {
            $this->rollback();
            dieWithCode($e->getMessage());
        }
    }
}
