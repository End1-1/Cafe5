<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-06 02:32:31
# Last Modified: 2026-03-21 17:57:42
require_once __DIR__ . "/index.php";

class StoreMove extends Auth
{
    public function input($params)
    {
        $this->beginTransaction();
        $res = $this->select("select sf_store2_input(?) as result", "s", [json_encode($params->doc, JSON_UNESCAPED_UNICODE)]);
        $row = $res->fetch_assoc();

        if (!$row || !isset($row['result'])) {
            $this->rollback();
            dieWithCode("Database function returned nothing");
        }
        $result = json_decode($row["result"], true);
        if ($result["status"] > 0) {
            $this->rollback();
            dieWithCode("Exit with {$result["status"]}");
        }
        $this->commit();
        if ($params->doc->doc_status == 1) {
            $items = $params->doc->items;
            $worker = require_once __DIR__ . "/../../worker/ws-notify.php";
            $worker->updatePrices($items);
        }
        $this->echoResult();
    }

    public function output($params)
    {
        $this->beginTransaction();
        $res = $this->select("select sf_store2_output(?) as result", "s", [json_encode($params->doc, JSON_UNESCAPED_UNICODE)]);
        $row = $res->fetch_assoc();

        if (!$row || !isset($row['result'])) {
            $this->rollback();
            dieWithCode("Database function returned nothing");
        }
        $result = json_decode($row["result"], true);
        if ($result["status"] > 0) {
            $this->rollback();
            dieWithCode("Exit with {$result["status"]}");
        }
        $this->commit();
        $this->echoResult();
    }

    public function open($params)
    {

        $sql = <<<EOD
        select sd.f_id , sd.f_user_id, sd.f_doc_date, sd.f_status, sd.f_doc_type, 
        sd.f_store_in, st1.f_name as f_store_in_name, sd.f_store_out, st2.f_name as f_store_out_name,
        sd.f_sum, sd.f_partner, sd.f_version, sd.f_data
        from store_document sd
        left join c_storages st1 on st1.f_id=sd.f_store_in
        left join c_storages st2 on st2.f_id=sd.f_store_out
        where sd.f_id=?
        EOD;
        $doc = $this->select($sql, "s", [$params->id])->fetch_assoc();
        if (empty($doc)) {
            die(Translator::t("Document not found"));
        }
        $sql = <<<EOD
        select su.f_id, su.f_item_id, g.f_name f_item_name, su.f_qty, su.f_price, su.f_total,
        su.f_comment, if (length(g.f_adg)>0, g.f_adg, gr.f_adgcode) as f_adg,
        u.f_name as f_unit_name, st.f_expiry_date
        from store_user su
        left join c_goods g on g.f_id=su.f_item_id
        left join c_groups gr on gr.f_id=g.f_group
        left join c_units u on u.f_id=g.f_unit
        left join store_stock st on st.f_doc_row_id=su.f_id
        where su.f_doc=?
        order by su.f_row
        EOD;
        $goods = $this->select($sql, "s", [$params->id])->fetch_all(MYSQLI_ASSOC);
        $doc["items"] = $goods;
        $this->result["doc"] = $doc;
        $this->echoResult();
    }

    public function remove($params)
    {
        // 1. Начинаем транзакцию, так как удаление затрагивает много таблиц
        $this->beginTransaction();

        // 2. Вызываем универсальную функцию удаления
        $res = $this->select("select sf_store2_doc_delete(?) as result", "s", [$params->id]);
        $row = $res->fetch_assoc();

        if (!$row || !isset($row['result'])) {
            $this->rollback();
            dieWithCode("Database function returned nothing");
        }

        $result = json_decode($row["result"], true);

        // 3. Проверяем статус возврата из SQL функции
        // Если status > 0, значит сработал триггер (например, "уже продано")
        if ($result["status"] > 0) {
            $this->rollback();
            // Выводим сообщение об ошибке из базы (msg), чтобы в Qt было понятно, почему не удалилось
            dieWithCode($result["msg"] ?? "Error code: " . $result["status"]);
        }

        // 4. Если всё ок — фиксируем изменения
        $this->commit();

        // Возвращаем результат (Qt ожидает успешный JSON)
        $this->echoResult();
    }
}
