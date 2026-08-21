<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-06 02:32:31
# Last Modified: 2026-07-30
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

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
            $msg = (string)($result["msg"] ?? "");
            if ($msg === "paid_exceeds_sum") {
                dieWithCode(Translator::t("Paid amount cannot exceed document total"));
            }
            if ($msg === "cashbox_required_for_paid") {
                dieWithCode(Translator::t("Payment type not specified"));
            }
            if ($msg === "partner_required_for_debt") {
                dieWithCode(Translator::t("Partner not selected"));
            }
            if ($msg === "version_conflict") {
                dieWithCode(Translator::t("Document was changed by another user"));
            }
            dieWithCode("Exit with {$result["status"]}" . ($msg !== "" ? ": {$msg}" : ""));
        }
        $this->commit();

        $items = [];
        foreach ($params->doc->items ?? [] as $item) {
            $item = (array)$item;
            $itemId = (int)($item["item_id"] ?? 0);
            $price = (float)($item["price"] ?? $item["f_price"] ?? 0);
            if ($itemId > 0 && $price > 0) {
                $items[] = ["item_id" => $itemId, "price" => $price];
            }
        }
        if (!empty($items)) {
            $worker = require __DIR__ . "/../../worker/ws-notify.php";
            $worker->updatePrices($items);
        }
        $this->result["version"] = (int)($result["version"] ?? 0);
        $this->result["paid_amount"] = (float)($result["paid_amount"] ?? 0);
        $this->echoResult();
    }

    /**
     * Change paid/debt split on a posted input document without reversing stock.
     */
    public function inputPaid($params)
    {
        $doc = $params->doc ?? $params;
        if (is_object($doc)) {
            $payload = [
                "doc_uuid" => (string)($doc->doc_uuid ?? $doc->f_id ?? ""),
                "doc_version" => (int)($doc->doc_version ?? $doc->f_version ?? 0),
                "doc_create_user" => (int)($doc->doc_create_user ?? $this->userid ?? 0),
                "cashbox_id" => (int)($doc->cashbox_id ?? 0),
                "payment_type_id" => (int)($doc->payment_type_id ?? 0),
                "currency_id" => (int)($doc->currency_id ?? 1),
                "paid_amount" => (float)($doc->paid_amount ?? 0),
            ];
        } else {
            $doc = (array)$doc;
            $payload = [
                "doc_uuid" => (string)($doc["doc_uuid"] ?? $doc["f_id"] ?? ""),
                "doc_version" => (int)($doc["doc_version"] ?? $doc["f_version"] ?? 0),
                "doc_create_user" => (int)($doc["doc_create_user"] ?? $this->userid ?? 0),
                "cashbox_id" => (int)($doc["cashbox_id"] ?? 0),
                "payment_type_id" => (int)($doc["payment_type_id"] ?? 0),
                "currency_id" => (int)($doc["currency_id"] ?? 1),
                "paid_amount" => (float)($doc["paid_amount"] ?? 0),
            ];
        }
        if ($payload["doc_uuid"] === "") {
            dieWithCode(Translator::t("Document id is required"));
        }
        if ($payload["paid_amount"] < 0) {
            dieWithCode(Translator::t("Paid amount is not valid"));
        }

        $this->beginTransaction();
        $res = $this->select("select sf_store2_input_set_paid(?) as result", "s",
            [json_encode($payload, JSON_UNESCAPED_UNICODE)]);
        $row = $res->fetch_assoc();
        if (!$row || !isset($row["result"])) {
            $this->rollback();
            dieWithCode("Database function returned nothing");
        }
        $result = json_decode($row["result"], true);
        if (($result["status"] ?? 1) > 0) {
            $this->rollback();
            $msg = (string)($result["msg"] ?? "error");
            if ($msg === "paid_exceeds_sum") {
                dieWithCode(Translator::t("Paid amount cannot exceed document total"));
            }
            if ($msg === "cashbox_required_for_paid") {
                dieWithCode(Translator::t("Payment type not specified"));
            }
            if ($msg === "version_conflict") {
                dieWithCode(Translator::t("Document was changed by another user"));
            }
            if ($msg === "document_not_posted") {
                dieWithCode(Translator::t("Document is not saved"));
            }
            dieWithCode("Exit with {$result["status"]}: {$msg}");
        }
        $this->commit();
        $this->result["version"] = (int)($result["version"] ?? 0);
        $this->result["paid_amount"] = (float)($result["paid_amount"] ?? 0);
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

    /**
     * Movement: single store_document (f_doc_type=3).
     * FIFO write-off from store_out + same items input to store_in at cost.
     */
    public function transfer($params)
    {
        $doc = $params->doc;
        $storeOut = (int)($doc->doc_store_out ?? 0);
        $storeIn = (int)($doc->doc_store_in ?? 0);
        if ($storeOut <= 0 || $storeIn <= 0) {
            dieWithCode(Translator::t("Both stores must be selected"));
        }
        if ($storeOut === $storeIn) {
            dieWithCode(Translator::t("Stores must be different"));
        }

        $docUuid = (string)($doc->doc_uuid ?? "");
        if ($docUuid === "") {
            $docUuid = uuid_v4();
        }

        $baseData = is_object($doc->doc_data ?? null)
            ? json_decode(json_encode($doc->doc_data), true)
            : (array)($doc->doc_data ?? []);
        if (!empty($baseData["pair_out"]) || !empty($baseData["pair_in"])) {
            dieWithCode(Translator::t("Invalid document data"));
        }
        $docData = array_merge($baseData, ["kind" => "move"]);

        $items = [];
        $rowIdx = 0;
        $clientItems = $doc->items ?? null;
        if (is_object($clientItems)) {
            $clientItems = (array)$clientItems;
        }
        if (is_array($clientItems)) {
            foreach ($clientItems as $it) {
                if (is_object($it)) {
                    $it = (array)$it;
                }
                $itemId = (int)($it["item_id"] ?? $it["f_item_id"] ?? 0);
                $qty = (float)($it["qty"] ?? $it["f_qty"] ?? 0);
                if ($itemId <= 0 || $qty < 0.0001) {
                    continue;
                }
                $items[] = [
                    "id" => (string)($it["id"] ?? uuid_v4()),
                    "item_id" => $itemId,
                    "qty" => $qty,
                    "price" => (float)($it["price"] ?? 0),
                    "expire_date" => $it["expire_date"] ?? null,
                    "comment" => (string)($it["comment"] ?? ""),
                    "row" => $rowIdx++,
                ];
            }
        }
        if (empty($items)) {
            dieWithCode(Translator::t("Empty document"));
        }

        $payload = [
            "doc_uuid" => $docUuid,
            "doc_user_id" => $doc->doc_user_id ?? "",
            "doc_date" => $doc->doc_date,
            "doc_status" => (int)($doc->doc_status ?? 0),
            "doc_type" => 3,
            "doc_store_out" => $storeOut,
            "doc_store_in" => $storeIn,
            "doc_sum" => 0,
            "doc_create_user" => (int)($doc->doc_create_user ?? 0),
            "doc_version" => (int)($doc->doc_version ?? 0),
            "doc_data" => $docData,
            "items" => $items,
        ];

        $this->beginTransaction();
        $result = $this->callStore2("sf_store2_move", $payload);
        if (($result["status"] ?? 1) > 0) {
            $this->rollback();
            dieWithCode($this->store2ErrorMessage($result));
        }
        $this->commit();

        $this->result["id"] = $docUuid;
        $this->result["cost"] = (float)($result["cost"] ?? 0);
        $this->result["version"] = (int)($result["version"] ?? 0);
        $this->echoResult();
    }

    /**
     * Complectation: single store_document (f_doc_type=4).
     * Materials write-off from store_out + finished goods input to store_in (stores may match).
     */
    public function complect($params)
    {
        $doc = $params->doc;
        $storeOut = (int)($doc->doc_store_out ?? 0);
        $storeIn = (int)($doc->doc_store_in ?? 0);
        $complectGoods = (int)($doc->complect_goods ?? 0);
        $complectQty = (float)($doc->complect_qty ?? 0);
        if ($storeOut <= 0 || $storeIn <= 0) {
            dieWithCode(Translator::t("Both stores must be selected"));
        }
        if ($complectGoods <= 0 || $complectQty < 0.0001) {
            dieWithCode(Translator::t("Complectation goods and qty required"));
        }

        $docUuid = (string)($doc->doc_uuid ?? "");
        if ($docUuid === "") {
            $docUuid = uuid_v4();
        }
        $complectRowId = (string)($doc->complect_row_id ?? "");
        if ($complectRowId === "") {
            $baseDataTmp = is_object($doc->doc_data ?? null)
                ? json_decode(json_encode($doc->doc_data), true)
                : (array)($doc->doc_data ?? []);
            $complectRowId = (string)($baseDataTmp["complect_row_id"] ?? "");
        }
        if ($complectRowId === "") {
            $complectRowId = uuid_v4();
        }

        $baseData = is_object($doc->doc_data ?? null)
            ? json_decode(json_encode($doc->doc_data), true)
            : (array)($doc->doc_data ?? []);
        if (!empty($baseData["pair_out"]) || !empty($baseData["pair_in"])) {
            dieWithCode(Translator::t("Invalid document data"));
        }
        $docData = array_merge($baseData, [
            "kind" => "complect",
            "complect_goods" => $complectGoods,
            "complect_qty" => $complectQty,
            "complect_row_id" => $complectRowId,
        ]);

        $items = [];
        $rowIdx = 0;
        $clientItems = $doc->items ?? null;
        if (is_object($clientItems)) {
            $clientItems = (array)$clientItems;
        }
        if (is_array($clientItems) && count($clientItems) > 0) {
            foreach ($clientItems as $it) {
                if (is_object($it)) {
                    $it = (array)$it;
                }
                $itemId = (int)($it["item_id"] ?? $it["f_goods"] ?? 0);
                $qty = (float)($it["qty"] ?? $it["f_qty"] ?? 0);
                if ($itemId <= 0 || $qty < 0.0001) {
                    continue;
                }
                $items[] = [
                    "id" => (string)($it["id"] ?? uuid_v4()),
                    "item_id" => $itemId,
                    "qty" => $qty,
                    "price" => 0,
                    "expire_date" => null,
                    "comment" => (string)($it["comment"] ?? ""),
                    "row" => $rowIdx++,
                ];
            }
        }
        if (empty($items)) {
            $recipe = $this->select(
                "select f_goods, f_qty from c_goods_complectation where f_base=? order by f_id",
                "i",
                [$complectGoods]
            )->fetch_all(MYSQLI_ASSOC);
            foreach ($recipe as $r) {
                $items[] = [
                    "id" => uuid_v4(),
                    "item_id" => (int)$r["f_goods"],
                    "qty" => (float)$r["f_qty"] * $complectQty,
                    "price" => 0,
                    "expire_date" => null,
                    "comment" => "",
                    "row" => $rowIdx++,
                ];
            }
        }
        if (empty($items)) {
            dieWithCode(Translator::t("Empty document"));
        }

        $payload = [
            "doc_uuid" => $docUuid,
            "doc_user_id" => $doc->doc_user_id ?? "",
            "doc_date" => $doc->doc_date,
            "doc_status" => (int)($doc->doc_status ?? 0),
            "doc_type" => 4,
            "doc_store_out" => $storeOut,
            "doc_store_in" => $storeIn,
            "doc_sum" => 0,
            "doc_create_user" => (int)($doc->doc_create_user ?? 0),
            "doc_version" => (int)($doc->doc_version ?? 0),
            "complect_goods" => $complectGoods,
            "complect_qty" => $complectQty,
            "complect_row_id" => $complectRowId,
            "doc_data" => $docData,
            "items" => $items,
        ];

        $this->beginTransaction();
        $result = $this->callStore2("sf_store2_complect", $payload);
        if (($result["status"] ?? 1) > 0) {
            $this->rollback();
            dieWithCode($this->store2ErrorMessage($result));
        }
        $this->commit();

        $unitPrice = (float)($result["unit_price"] ?? 0);
        if ($unitPrice > 0) {
            $worker = require __DIR__ . "/../../worker/ws-notify.php";
            $worker->updatePrices([["item_id" => $complectGoods, "price" => $unitPrice]]);
        }

        $this->result["id"] = $docUuid;
        $this->result["complect_goods"] = $complectGoods;
        $this->result["complect_qty"] = $complectQty;
        $this->result["complect_row_id"] = $result["complect_row_id"] ?? $complectRowId;
        $this->result["unit_price"] = $unitPrice;
        $this->result["version"] = (int)($result["version"] ?? 0);
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
        left join store_stock st on st.f_id=su.f_id
        where su.f_doc=?
        order by su.f_row
        EOD;
        $goods = $this->select($sql, "s", [$params->id])->fetch_all(MYSQLI_ASSOC);
        $doc["items"] = $goods;

        $data = [];
        if (!empty($doc["f_data"])) {
            $data = is_string($doc["f_data"]) ? json_decode($doc["f_data"], true) : $doc["f_data"];
            if (!is_array($data)) {
                $data = [];
            }
        }
        $kind = $data["kind"] ?? "";
        $docType = (int)($doc["f_doc_type"] ?? 0);

        $doc["f_cashbox_id"] = (int)($data["cashbox_id"] ?? 0);
        $doc["f_payment_type_id"] = (int)($data["payment_type_id"] ?? 0);
        $doc["f_currency_id"] = (int)($data["currency_id"] ?? 0);
        $doc["f_cashbox_name"] = (string)($data["cashbox_name"] ?? "");
        $doc["f_payment_type_name"] = (string)($data["payment_type_name"] ?? "");
        $doc["f_currency_name"] = (string)($data["currency_name"] ?? "");
        if (array_key_exists("paid_amount", $data)) {
            $doc["paid_amount"] = (float)$data["paid_amount"];
        } else {
            $paidRow = $this->select(
                "select coalesce(sum(f_credit),0) as paid, max(f_cashbox_id) as cashbox_id,
                        max(f_payment_type_id) as payment_type_id, max(f_currency_id) as currency_id
                 from cash_operations where f_order_id=?",
                "s",
                [$params->id]
            )->fetch_assoc();
            $doc["paid_amount"] = (float)($paidRow["paid"] ?? 0);
            if ($doc["f_cashbox_id"] <= 0 && (int)($paidRow["cashbox_id"] ?? 0) > 0) {
                $doc["f_cashbox_id"] = (int)$paidRow["cashbox_id"];
                $doc["f_payment_type_id"] = (int)($paidRow["payment_type_id"] ?? 0);
                $doc["f_currency_id"] = (int)($paidRow["currency_id"] ?? 1);
            }
        }
        if (!empty($doc["f_cashbox_id"]) && $doc["f_cashbox_name"] === "") {
            $cb = $this->select("select f_name from cash_box where f_id=?", "i", [$doc["f_cashbox_id"]])->fetch_assoc();
            if (!$cb) {
                $cb = $this->select("select f_name from e_cash_names where f_id=?", "i", [$doc["f_cashbox_id"]])->fetch_assoc();
            }
            $doc["f_cashbox_name"] = (string)($cb["f_name"] ?? "");
        }

        if ($docType === 3) {
            $doc["kind"] = "move";
        } elseif ($docType === 4) {
            $doc["kind"] = "complect";
            $doc["complect_goods"] = (int)($data["complect_goods"] ?? 0);
            $doc["complect_qty"] = (float)($data["complect_qty"] ?? 0);
            $doc["complect_row_id"] = (string)($data["complect_row_id"] ?? "");
            if (!empty($doc["complect_goods"])) {
                $cg = $this->select(
                    "select g.f_id, g.f_name, g.f_scancode, u.f_name as f_unit_name
                     from c_goods g left join c_units u on u.f_id=g.f_unit where g.f_id=?",
                    "i",
                    [$doc["complect_goods"]]
                )->fetch_assoc();
                $doc["complect_goods_name"] = $cg["f_name"] ?? "";
                $doc["complect_scancode"] = $cg["f_scancode"] ?? "";
                $doc["complect_unit_name"] = $cg["f_unit_name"] ?? "";
            }
        }

        $this->result["doc"] = $doc;
        $this->echoResult();
    }

    public function relatedOutputs($params)
    {
        $id = $params->id ?? '';
        if ($id === '') {
            dieWithCode(Translator::t("Document id is required"));
        }

        $locale = Translator::$locale;
        $sql = <<<EOD
        SELECT DISTINCT sd.f_id,
               sd.f_doc_type,
               ld_type.f_value AS f_type_name,
               ld_status.f_value AS f_status_name,
               date_fmt(sd.f_doc_date) AS f_doc_date,
               sd.f_user_id,
               COALESCE(NULLIF(TRIM(sd.f_user_id), ''), LEFT(sd.f_id, 8)) AS f_doc_number,
               COALESCE(so.f_name, '') AS f_store_out_name,
               COALESCE(si.f_name, '') AS f_store_in_name,
               sd.f_sum
        FROM store_stock ss
        INNER JOIN store_moves sm ON sm.f_batch_id = ss.f_id AND sm.f_doc <> ss.f_doc
        INNER JOIN store_document sd ON sd.f_id = sm.f_doc
        LEFT JOIN c_storages so ON so.f_id = sd.f_store_out
        LEFT JOIN c_storages si ON si.f_id = sd.f_store_in
        LEFT JOIN l_dictionary ld_status ON ld_status.f_dict_id = sd.f_status
            AND ld_status.f_dict = 'store_statuses' AND ld_status.f_lang = ?
        LEFT JOIN l_dictionary ld_type ON ld_type.f_dict_id = sd.f_doc_type
            AND ld_type.f_dict = 'store_types' AND ld_type.f_lang = ?
        WHERE ss.f_doc = ?
          AND sm.f_qty_out > 0
        ORDER BY sd.f_doc_date DESC, sd.f_user_id
        EOD;

        $rows = $this->select($sql, "sss", [$locale, $locale, $id])->fetch_all(MYSQLI_ASSOC);
        $this->result["rows"] = $rows;
        $this->echoResult();
    }

    public function FindByInvoiceNumber($params)
    {
        $series = trim((string)($params->f_series ?? ""));
        $number = trim((string)($params->f_number ?? ""));
        $userId = trim((string)($params->f_user_id ?? ""));
        if ($userId === "" && ($series !== "" || $number !== "")) {
            $userId = $series . $number;
        }
        if ($userId === "") {
            dieWithCode("f_user_id or f_series/f_number is required");
        }

        $docTypeInput = 1;
        $sql = <<<SQL
            SELECT
                sd.f_id,
                sd.f_user_id,
                date_fmt(sd.f_doc_date) AS f_doc_date,
                sd.f_status,
                COALESCE(ld.f_value, CAST(sd.f_status AS CHAR)) AS f_status_name
            FROM store_document sd
            LEFT JOIN l_dictionary ld
                ON ld.f_dict = 'store_statuses'
               AND ld.f_dict_id = sd.f_status
               AND ld.f_lang = ?
            WHERE sd.f_doc_type = {$docTypeInput}
              AND sd.f_user_id = ?
            ORDER BY sd.f_doc_date DESC
        SQL;
        $rows = $this->select($sql, "ss", [Translator::$locale, $userId])->fetch_all(MYSQLI_ASSOC);
        $this->result["exists"] = !empty($rows);
        $this->result["docs"] = $rows;
        $this->echoResult();
    }

    public function remove($params)
    {
        $this->beginTransaction();

        $doc = $this->select("select f_id, f_doc_type from store_document where f_id=?", "s", [$params->id])->fetch_assoc();
        if (empty($doc)) {
            $this->rollback();
            dieWithCode(Translator::t("Document not found"));
        }

        $docType = (int)($doc["f_doc_type"] ?? 0);
        switch ($docType) {
            case 1:
                $fn = "sf_store2_input_delete";
                break;
            case 2:
                $fn = "sf_store2_output_delete";
                break;
            case 3:
                $fn = "sf_store2_move_delete";
                break;
            case 4:
                $fn = "sf_store2_complect_delete";
                break;
            default:
                $this->rollback();
                dieWithCode(Translator::t("Unknown document type") . " (" . $docType . ")");
        }

        $res = $this->select("select {$fn}(?) as result", "s", [$params->id]);
        $row = $res->fetch_assoc();
        if (!$row || !isset($row['result'])) {
            $this->rollback();
            dieWithCode("Database function returned nothing");
        }
        $result = json_decode($row["result"], true);
        if (!is_array($result) || ($result["status"] ?? 1) > 0) {
            $this->rollback();
            dieWithCode($this->store2ErrorMessage(is_array($result) ? $result : ["status" => 1, "msg" => ""]));
        }

        $this->commit();
        $this->echoResult();
    }

    private function callStore2(string $fn, array $doc): array
    {
        $res = $this->select("select {$fn}(?) as result", "s", [json_encode($doc, JSON_UNESCAPED_UNICODE)]);
        $row = $res->fetch_assoc();
        if (!$row || !isset($row["result"])) {
            return ["status" => 1, "msg" => "Database function returned nothing"];
        }
        $result = json_decode($row["result"], true);
        if (!is_array($result)) {
            return ["status" => 1, "msg" => "Invalid function result"];
        }
        return $result;
    }

    private function store2ErrorMessage(array $result): string
    {
        $msg = (string)($result["msg"] ?? "");
        if ($msg === "insufficient_stock") {
            $itemId = (int)($result["item_id"] ?? 0);
            $shortage = (float)($result["shortage"] ?? 0);
            $name = "";
            if ($itemId > 0) {
                $row = $this->select("select f_name from c_goods where f_id=?", "i", [$itemId])->fetch_assoc();
                $name = (string)($row["f_name"] ?? "");
            }
            $label = $name !== "" ? $name : ("#" . $itemId);
            return Translator::t("Insufficient stock") . ": " . $label
                . ($shortage > 0 ? (" (" . rtrim(rtrim(number_format($shortage, 4, ".", ""), "0"), ".") . ")") : "");
        }
        if ($msg === "version_error") {
            return Translator::t("Document was modified by another user");
        }
        if ($msg === "already_sold_cannot_edit") {
            return Translator::t("Cannot edit: goods already sold");
        }
        if ($msg === "already_sold_cannot_delete") {
            return Translator::t("Cannot delete: goods already sold");
        }
        if ($msg === "unknown_doc_type") {
            return Translator::t("Unknown document type") . " (" . (int)($result["doc_type"] ?? 0) . ")";
        }
        if ($msg === "stores_must_differ") {
            return Translator::t("Stores must be different");
        }
        if ($msg === "stores_required") {
            return Translator::t("Both stores must be selected");
        }
        return $msg !== "" ? $msg : ("Exit with " . ($result["status"] ?? "?"));
    }
}
