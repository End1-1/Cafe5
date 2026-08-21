<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../worker/dict-payment.php";
require_once __DIR__ . "/../worker/dict-cash-operation-type.php";
require_once __DIR__ . "/../../worker/uuid.php";

class ViewOrder extends Auth
{
    private const ORDER_STATE_CLOSE = 2;
    private const SALE_RETURN = 4;
    private const MAX_RETURN_DAYS = 14;

    /**
     * Поиск заказа по номеру чека (o_header.f_prefix или CONCAT(f_prefix, f_hallid)).
     */
    public function GetByPrefix($params)
    {
        $prefix = trim((string)($params->f_prefix ?? $params->prefix ?? ""));
        $prefix = str_replace([" ", ";", "?"], "", $prefix);
        if ($prefix === "") {
            dieWithCode(Translator::t("Enter receipt number"));
        }

        $sql = <<<SQL
            SELECT f_id, f_prefix, f_hallid, f_state, f_datecash, f_amounttotal
            FROM o_header
            WHERE f_prefix = ?
               OR CONCAT(COALESCE(f_prefix, ''), COALESCE(f_hallid, '')) = ?
            ORDER BY (f_state = 2) DESC, f_datecash DESC, f_id DESC
            LIMIT 1
        SQL;
        $row = $this->select($sql, "ss", [$prefix, $prefix])->fetch_assoc();
        if (!$row) {
            dieWithCode(Translator::t("Document is not exists"));
        }

        $this->result["id"] = $row["f_id"];
        $this->result["f_prefix"] = $row["f_prefix"];
        $this->result["f_number"] = trim((string)($row["f_prefix"] ?? "") . (string)($row["f_hallid"] ?? ""));
        $this->result["f_state"] = (int)$row["f_state"];
        $this->echoResult();
    }

    public function Get($params)
    {
        $id = (string)($params->id ?? "");
        if ($id === "") {
            dieWithCode(Translator::t("Missing id"));
        }

        $sql = <<<SQL
            SELECT
                oh.f_id,
                oh.f_state,
                oh.f_prefix,
                oh.f_hallid,
                oh.f_hall,
                oh.f_table,
                oh.f_datecash,
                oh.f_timeclose,
                oh.f_amounttotal,
                oh.f_saletype,
                oh.f_partner,
                oh.f_staff,
                oh.f_cashier,
                oh.f_comment,
                oh.f_data,
                oh.f_cash_session_id,
                CONCAT(COALESCE(u.f_last, ''), ' ', COALESCE(u.f_first, '')) AS f_saler,
                CONCAT_WS(' ', p.f_taxcode, p.f_taxname, p.f_contact, p.f_phone) AS f_buyer
            FROM o_header oh
            LEFT JOIN s_user u ON u.f_id = oh.f_staff
            LEFT JOIN c_partners p ON p.f_id = oh.f_partner
            WHERE oh.f_id = ?
        SQL;
        $header = $this->select($sql, "s", [$id])->fetch_assoc();
        if (!$header) {
            dieWithCode(Translator::t("Document is not exists"));
        }

        $odata = json_decode($header["f_data"] ?? "{}", true) ?: [];
        $header["f_amount_cash"] = (float)($odata["f_amount_cash"] ?? 0);
        $header["f_amount_card"] = (float)($odata["f_amount_card"] ?? 0);
        $header["f_amount_idram"] = (float)($odata["f_amount_idram"] ?? 0);
        $header["f_amount_debt"] = (float)($odata["f_amount_debt"] ?? 0);
        $header["f_amount_bank"] = (float)($odata["f_amount_bank"] ?? 0);
        $header["f_amount_telcell"] = (float)($odata["f_amount_telcell"] ?? 0);
        $header["f_amount_prepaid"] = (float)($odata["f_amount_prepaid"] ?? 0);
        $header["f_fiscal"] = $odata["f_fiscal"] ?? new stdClass();
        $header["f_fiscal_taxback_at"] = $odata["f_fiscal_taxback_at"] ?? "";
        $header["f_time_close"] = $odata["f_time_close"] ?? ($header["f_timeclose"] ?? "");
        $header["f_number"] = trim((string)($header["f_prefix"] ?? "") . (string)($header["f_hallid"] ?? ""));
        unset($header["f_data"]);

        $dm = $this->select(
            "SELECT CONCAT(' ', u.f_last, u.f_first) AS f_deliveryman
             FROM o_header_options o
             LEFT JOIN s_user u ON u.f_id = o.f_deliveryman
             WHERE o.f_id = ?",
            "s",
            [$id]
        )->fetch_assoc();
        $header["f_deliveryman"] = $dm["f_deliveryman"] ?? "";

        $sqlGoods = <<<SQL
            SELECT
                b.f_id,
                b.f_row,
                b.f_goods AS f_goodsid,
                b.f_qty,
                b.f_price,
                b.f_total,
                b.f_store,
                b.f_return,
                COALESCE(b.f_returnedqty, 0) AS f_returnedqty,
                b.f_type,
                b.f_data,
                g.f_name,
                g.f_scancode,
                COALESCE(JSON_VALUE(b.f_data, '$.f_is_service'), g.f_service, 0) AS f_service
            FROM o_goods b
            INNER JOIN c_goods g ON g.f_id = b.f_goods
            WHERE b.f_header = ? AND b.f_state = 1
            ORDER BY b.f_row
        SQL;
        $rows = $this->select($sqlGoods, "s", [$id])->fetch_all(MYSQLI_ASSOC);
        $goods = [];
        foreach ($rows as $i => $row) {
            $gdata = json_decode($row["f_data"] ?? "{}", true) ?: [];
            $qty = (float)$row["f_qty"];
            $returned = (float)$row["f_returnedqty"];
            $goods[] = [
                "f_id" => $row["f_id"],
                "f_row" => (int)$row["f_row"],
                "f_fiscal_row" => $i,
                "f_goodsid" => (int)$row["f_goodsid"],
                "f_name" => $row["f_name"],
                "f_scancode" => $row["f_scancode"],
                "f_qty" => $qty,
                "f_price" => (float)$row["f_price"],
                "f_total" => (float)$row["f_total"],
                "f_store" => (int)$row["f_store"],
                "f_return" => (int)$row["f_return"],
                "f_returnedqty" => $returned,
                "f_qty_available" => max(0.0, $qty - $returned),
                "f_service" => (int)$row["f_service"],
                "f_type" => (int)$row["f_type"],
                "f_is_service" => !empty($gdata["f_is_service"]) || ((int)$row["f_service"] !== 0),
            ];
        }

        $this->result["header"] = $header;
        $this->result["goods"] = $goods;
        $this->echoResult();
    }

    public function ReturnReasons($params)
    {
        unset($params);
        $rows = $this->select("SELECT f_id, f_name FROM o_goods_return_reason ORDER BY f_id")->fetch_all(MYSQLI_ASSOC);
        $out = [];
        foreach ($rows as $r) {
            $out[] = ["f_id" => (int)$r["f_id"], "f_name" => $r["f_name"]];
        }
        $this->result["reasons"] = $out;
        $this->echoResult();
    }

    public function CreateReturn($params)
    {
        $orderId = (string)($params->id ?? "");
        $reasonId = (int)($params->reason_id ?? 0);
        $itemsIn = $params->items ?? [];
        $cashSessionId = (int)($params->cash_session_id ?? 0);
        $cashboxId = (int)($params->cashbox_id ?? 0);
        $storeId = (int)($params->store_id ?? 0);

        if ($orderId === "") {
            dieWithCode(Translator::t("Missing id"));
        }
        if ($reasonId <= 0) {
            dieWithCode(Translator::t("The return reason must be specified."));
        }
        if (empty($itemsIn) || !is_array($itemsIn)) {
            dieWithCode(Translator::t("Nothing to return"));
        }
        if ($cashSessionId <= 0) {
            dieWithCode(Translator::t("Cashbox session is not open"));
        }
        if ($cashboxId <= 0) {
            dieWithCode("cashbox_id is required");
        }
        if ($storeId <= 0) {
            dieWithCode(Translator::t("Store is not defined"));
        }

        $sql = "SELECT * FROM o_header WHERE f_id=?";
        $src = $this->select($sql, "s", [$orderId])->fetch_assoc();
        if (!$src) {
            dieWithCode(Translator::t("Document is not exists"));
        }
        if ((int)$src["f_state"] !== self::ORDER_STATE_CLOSE) {
            dieWithCode(Translator::t("Order is not closed"));
        }
        if ((int)($src["f_saletype"] ?? 0) === self::SALE_RETURN) {
            dieWithCode(Translator::t("You cannot return this item"));
        }

        $dateCash = (string)($src["f_datecash"] ?? "");
        if ($dateCash !== "") {
            $days = (int)((strtotime(date("Y-m-d")) - strtotime($dateCash)) / 86400);
            if ($days > self::MAX_RETURN_DAYS) {
                dieWithCode(Translator::t("You cannot return this item"));
            }
        }

        $odata = json_decode($src["f_data"] ?? "{}", true) ?: [];
        static $paymentDict = null;
        if (!is_array($paymentDict)) {
            $paymentDict = require __DIR__ . "/../worker/dict-payment.php";
        }
        $payment = $paymentDict;

        $goodsMap = [];
        $goodsRows = $this->select(
            "SELECT og.*, g.f_name, g.f_scancode, g.f_lastinputprice,
                    COALESCE(JSON_VALUE(og.f_data, '$.f_is_service'), g.f_service, 0) AS f_service
             FROM o_goods og
             INNER JOIN c_goods g ON g.f_id = og.f_goods
             WHERE og.f_header=? AND og.f_state=1
             ORDER BY og.f_row",
            "s",
            [$orderId]
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($goodsRows as $i => $g) {
            $g["f_fiscal_row"] = $i;
            $goodsMap[$g["f_id"]] = $g;
        }

        $returnLines = [];
        $returnTotal = 0.0;
        foreach ($itemsIn as $it) {
            $it = (array)$it;
            $gid = (string)($it["id"] ?? "");
            $qty = (float)($it["qty"] ?? 0);
            if ($gid === "" || $qty < 0.0001) {
                continue;
            }
            if (!isset($goodsMap[$gid])) {
                dieWithCode(Translator::t("Wrong goods line"));
            }
            $srcLine = $goodsMap[$gid];
            $available = (float)$srcLine["f_qty"] - (float)($srcLine["f_returnedqty"] ?? 0);
            if ($qty > $available + 0.0001) {
                dieWithCode(Translator::t("Invalid qty") . ": " . ($srcLine["f_name"] ?? ""));
            }
            if ((float)$srcLine["f_price"] < 0) {
                dieWithCode(Translator::t("You cannot return this item"));
            }
            $lineTotal = round($qty * (float)$srcLine["f_price"], 2);
            $returnTotal += $lineTotal;
            $returnLines[] = [
                "src" => $srcLine,
                "qty" => $qty,
                "total" => $lineTotal,
            ];
        }
        if (empty($returnLines) || $returnTotal < 0.01) {
            dieWithCode(Translator::t("Nothing to return"));
        }

        require_once __DIR__ . "/../waiter/cashbox.php";
        $cc = new Cashbox();
        $cashSession = $cc->GetRawCashboxSession($cashSessionId);
        if (!$cashSession || (int)$cashSession["f_state"] !== 1) {
            dieWithCode(Translator::t("Cashbox session is not open"));
        }
        if ((int)$cashSession["f_cashbox_id"] !== $cashboxId) {
            dieWithCode("cash_session_id does not match cashbox");
        }

        $alloc = $this->allocateReturnPayments($odata, $payment, $returnTotal);
        $partnerId = (int)($src["f_partner"] ?? 0);
        if (($odata["f_guest"]["f_guest_id"] ?? null)) {
            $partnerId = (int)$odata["f_guest"]["f_guest_id"];
        }
        $deferred = (float)($alloc["f_amount_debt"] ?? 0) + (float)($alloc["f_amount_bank"] ?? 0);
        if ($deferred > 0.00001 && $partnerId <= 0) {
            dieWithCode(Translator::t("A partner must be selected for deferred payments"));
        }

        $this->beginTransaction();

        $newId = uuid_v4();
        $saleNumber = trim((string)($src["f_prefix"] ?? "") . (string)($src["f_hallid"] ?? ""));
        $comment = Translator::t("Return from") . " " . $saleNumber;

        $newData = [
            "f_date_open" => date("Y-m-d"),
            "f_time_open" => date("H:i:s"),
            "f_date_close" => date("Y-m-d"),
            "f_time_close" => date("H:i:s"),
            "f_return_from" => $orderId,
            "f_currency_id" => (int)($odata["f_currency_id"] ?? 1),
            "f_amount_cash" => -1 * (float)($alloc["f_amount_cash"] ?? 0),
            "f_amount_card" => -1 * (float)($alloc["f_amount_card"] ?? 0),
            "f_amount_idram" => -1 * (float)($alloc["f_amount_idram"] ?? 0),
            "f_amount_telcell" => -1 * (float)($alloc["f_amount_telcell"] ?? 0),
            "f_amount_bank" => -1 * (float)($alloc["f_amount_bank"] ?? 0),
            "f_amount_debt" => -1 * (float)($alloc["f_amount_debt"] ?? 0),
            "f_amount_prepaid" => -1 * (float)($alloc["f_amount_prepaid"] ?? 0),
            "f_amount_paid" => -1 * $returnTotal,
            "f_amount_change" => 0,
            "log" => [["action" => "sale return", "from" => $orderId, "at" => date("Y-m-d H:i:s")]],
        ];
        if (!empty($odata["f_guest"])) {
            $newData["f_guest"] = $odata["f_guest"];
        }
        if (!empty($params->fiscal->out)) {
            $newData["f_fiscal"] = json_decode(json_encode($params->fiscal->out), true);
        }

        $hallCounter = $this->nextReturnPrefix((int)$src["f_hall"], (int)$src["f_table"]);

        $vHeader = [
            "f_id" => $newId,
            "f_state" => self::ORDER_STATE_CLOSE,
            "f_prefix" => $hallCounter["prefix"],
            "f_hallid" => $hallCounter["hallid"],
            "f_hall" => (int)$src["f_hall"],
            "f_table" => (int)$src["f_table"],
            "f_datecash" => date("Y-m-d"),
            "f_timeclose" => date("H:i:s"),
            "f_amounttotal" => -1 * $returnTotal,
            "f_saletype" => self::SALE_RETURN,
            "f_partner" => $partnerId > 0 ? $partnerId : null,
            "f_staff" => $this->userid,
            "f_cashier" => $this->userid,
            "f_comment" => $comment,
            "f_cash_session_id" => $cashSessionId,
            "f_data" => json_encode($newData, JSON_UNESCAPED_UNICODE),
        ];
        $this->insert("o_header", $vHeader);

        $storeItems = [];
        $rowNo = 0;
        foreach ($returnLines as $line) {
            $srcLine = $line["src"];
            $qty = $line["qty"];
            $rowNo += 100;
            $newGoodsId = uuid_v4();
            $gdata = json_decode($srcLine["f_data"] ?? "{}", true) ?: [];
            $gdata["f_return_from_header"] = $orderId;
            $gdata["f_printed"] = true;

            $this->insert("o_goods", [
                "f_id" => $newGoodsId,
                "f_header" => $newId,
                "f_state" => 1,
                "f_type" => (int)$srcLine["f_type"],
                "f_store" => (int)($srcLine["f_store"] ?: $storeId),
                "f_goods" => (int)$srcLine["f_goods"],
                "f_qty" => $qty,
                "f_price" => (float)$srcLine["f_price"],
                "f_total" => $line["total"],
                "f_row" => $rowNo,
                "f_return" => $reasonId,
                "f_returnfrom" => $srcLine["f_id"],
                "f_returnedqty" => 0,
                "f_data" => json_encode($gdata, JSON_UNESCAPED_UNICODE),
            ]);

            $this->select(
                "UPDATE o_goods SET f_return=?, f_returnedqty=COALESCE(f_returnedqty,0)+? WHERE f_id=?",
                "ids",
                [$reasonId, $qty, $srcLine["f_id"]],
                true
            );

            $isService = ((int)$srcLine["f_service"] !== 0) || !empty($gdata["f_is_service"]);
            if (!$isService) {
                $components = $this->returnStoreComponents($orderId, $srcLine["f_id"], $qty, (float)$srcLine["f_qty"]);
                foreach ($components as $c) {
                    $storeItems[] = $c;
                }
            }
        }

        if (!empty($storeItems)) {
            $this->postStoreInput($storeItems, $storeId, $newId, $comment);
        }

        $this->postReturnMoney($alloc, $payment, $partnerId, $newId, $cashboxId, $cashSessionId, $comment, (int)($odata["f_currency_id"] ?? 1));

        if (!empty($params->fiscal->in) || !empty($params->fiscal->out) || isset($params->fiscal->result)) {
            $taxLog = [
                "f_id" => uuid_v4(),
                "f_order" => $newId,
                "f_date" => date("Y-m-d"),
                "f_time" => date("H:i:s"),
                "f_elapsed" => 0,
                "f_in" => is_string($params->fiscal->in ?? null)
                    ? $params->fiscal->in
                    : json_encode($params->fiscal->in ?? new stdClass(), JSON_UNESCAPED_UNICODE),
                "f_out" => is_string($params->fiscal->out ?? null)
                    ? $params->fiscal->out
                    : json_encode($params->fiscal->out ?? new stdClass(), JSON_UNESCAPED_UNICODE),
                "f_err" => (string)($params->fiscal->err ?? $params->fiscal->error ?? ""),
                "f_result" => (int)($params->fiscal->result ?? -1),
                "f_state" => ((int)($params->fiscal->result ?? -1) === 0) ? 1 : 0,
            ];
            $machineId = (int)($params->fiscal->f_fiscal_machine_id
                ?? $params->f_fiscal_machine_id
                ?? 0);
            if ($machineId > 0) {
                $taxLog["f_fiscal_machine_id"] = $machineId;
            }
            $this->insert("o_tax_log", $taxLog);
        }

        $this->commit();

        $this->result["return_order_id"] = $newId;
        $this->result["return_total"] = $returnTotal;
        $this->result["order"] = $this->select("SELECT f_id, f_prefix, f_hallid, f_amounttotal FROM o_header WHERE f_id=?", "s", [$newId])->fetch_assoc();
        $this->echoResult();
    }

    /** @param array $odata @param array $payment @return array<string,float> */
    private function allocateReturnPayments(array $odata, array $payment, float $returnTotal): array
    {
        $fields = [];
        $sumPos = 0.0;
        foreach ($payment["types"] as $pt) {
            $fn = $payment["fields"][$pt];
            $v = max(0.0, (float)($odata[$fn] ?? 0));
            $fields[$fn] = $v;
            $sumPos += $v;
        }
        $out = [];
        foreach ($fields as $fn => $v) {
            $out[$fn] = 0.0;
        }
        if ($sumPos < 0.00001) {
            $out["f_amount_cash"] = $returnTotal;
            return $out;
        }
        $assigned = 0.0;
        $keys = array_keys($fields);
        $last = count($keys) - 1;
        foreach ($keys as $i => $fn) {
            if ($i === $last) {
                $out[$fn] = round($returnTotal - $assigned, 2);
            } else {
                $part = round($returnTotal * ($fields[$fn] / $sumPos), 2);
                $out[$fn] = $part;
                $assigned += $part;
            }
        }
        return $out;
    }

    private function nextReturnPrefix(int $hallId, int $tableId): array
    {
        $sql = <<<EOD
        SELECT h.f_id, c.f_counter+1 as f_counter, h.f_counterhall, h.f_prefix as f_department
        FROM h_tables t
        LEFT JOIN h_halls h ON h.f_id = t.f_hall
        LEFT JOIN h_halls c ON c.f_id=h.f_counterhall
        WHERE t.f_id=?
        FOR UPDATE
        EOD;
        $hall = $this->select($sql, "i", [$tableId > 0 ? $tableId : 0])->fetch_assoc();
        if (!$hall && $hallId > 0) {
            $hall = $this->select(
                "SELECT h.f_id, c.f_counter+1 as f_counter, h.f_counterhall, h.f_prefix as f_department
                 FROM h_halls h
                 LEFT JOIN h_halls c ON c.f_id=h.f_counterhall
                 WHERE h.f_id=? FOR UPDATE",
                "i",
                [$hallId]
            )->fetch_assoc();
        }
        if (!$hall) {
            return ["prefix" => "R" . date("ymd"), "hallid" => random_int(1, 9999)];
        }
        $this->update("h_halls", ["f_counter" => $hall["f_counter"]], $hall["f_counterhall"]);
        return [
            "prefix" => $hall["f_department"] . $hall["f_counter"],
            "hallid" => (int)$hall["f_counter"],
        ];
    }

    /** Expand returned line into store components (recipes), proportional to returned qty. */
    private function returnStoreComponents(string $orderId, string $rowId, float $returnQty, float $srcQty): array
    {
        $ratio = ($srcQty > 0.0001) ? ($returnQty / $srcQty) : 1.0;
        $rows = $this->select(
            "SELECT f_store_id, f_item_id, f_qty, f_price
             FROM store_calc_queue
             WHERE f_doc_sale_id=? AND f_row_sale_id=?",
            "ss",
            [$orderId, $rowId]
        )->fetch_all(MYSQLI_ASSOC);

        $items = [];
        if (!empty($rows)) {
            foreach ($rows as $r) {
                $qty = round((float)$r["f_qty"] * $ratio, 4);
                if ($qty < 0.0001) {
                    continue;
                }
                $price = (float)$r["f_price"];
                if ($price <= 0) {
                    $g = $this->select("SELECT f_lastinputprice FROM c_goods WHERE f_id=?", "i", [(int)$r["f_item_id"]])->fetch_assoc();
                    $price = (float)($g["f_lastinputprice"] ?? 0);
                }
                $items[] = [
                    "id" => uuid_v4(),
                    "item_id" => (int)$r["f_item_id"],
                    "qty" => $qty,
                    "price" => $price,
                    "store_id" => (int)$r["f_store_id"],
                    "row" => count($items) + 1,
                    "comment" => "sale_return",
                ];
            }
            return $items;
        }

        $line = $this->select("SELECT f_goods, f_store FROM o_goods WHERE f_id=?", "s", [$rowId])->fetch_assoc();
        if (!$line) {
            return [];
        }
        $g = $this->select("SELECT f_lastinputprice, f_service FROM c_goods WHERE f_id=?", "i", [(int)$line["f_goods"]])->fetch_assoc();
        if (!$g || (int)($g["f_service"] ?? 0) !== 0) {
            return [];
        }
        return [[
            "id" => uuid_v4(),
            "item_id" => (int)$line["f_goods"],
            "qty" => $returnQty,
            "price" => (float)($g["f_lastinputprice"] ?? 0),
            "store_id" => (int)$line["f_store"],
            "row" => 1,
            "comment" => "sale_return",
        ]];
    }

    private function postStoreInput(array $items, int $defaultStoreId, string $returnOrderId, string $comment): void
    {
        $byStore = [];
        foreach ($items as $it) {
            $sid = (int)($it["store_id"] ?? 0);
            if ($sid <= 0) {
                $sid = $defaultStoreId;
            }
            $byStore[$sid][] = $it;
        }

        foreach ($byStore as $storeId => $storeItems) {
            $docSum = 0.0;
            $payloadItems = [];
            $row = 1;
            foreach ($storeItems as $it) {
                $docSum += (float)$it["qty"] * (float)$it["price"];
                $payloadItems[] = [
                    "id" => $it["id"],
                    "item_id" => $it["item_id"],
                    "qty" => $it["qty"],
                    "price" => $it["price"],
                    "row" => $row++,
                    "comment" => $it["comment"] ?? "sale_return",
                ];
            }
            $doc = [
                "doc_uuid" => uuid_v4(),
                "doc_date" => date("Y-m-d H:i:s"),
                "doc_status" => 1,
                "doc_type" => 1,
                "doc_store_in" => $storeId,
                "doc_user_id" => (string)$this->userid,
                "doc_create_user" => $this->userid,
                "doc_version" => 0,
                "doc_partner" => 0,
                "doc_sum" => 0,
                "cashbox_id" => 0,
                "payment_type_id" => 1,
                "currency_id" => 1,
                "doc_data" => ["comment" => $comment, "return_order" => $returnOrderId],
                "items" => $payloadItems,
            ];
            $res = $this->select("SELECT sf_store2_input(?) AS result", "s", [json_encode($doc, JSON_UNESCAPED_UNICODE)])->fetch_assoc();
            $decoded = json_decode($res["result"] ?? "{}", true);
            if (!is_array($decoded) || (int)($decoded["status"] ?? 1) !== 0) {
                $this->rollback();
                dieWithCode(Translator::t("Store input failed") . ": " . ($decoded["msg"] ?? json_encode($decoded)));
            }
            // sf_store2_input may write empty cash_debts when cashbox=0 and doc_sum=0
            $this->select(
                "DELETE FROM cash_debts WHERE f_doc_uuid=? AND ABS(COALESCE(f_credit,0))+ABS(COALESCE(f_debit,0)) < 0.00001",
                "s",
                [$doc["doc_uuid"]],
                true
            );
        }
    }

    private function postReturnMoney(
        array $alloc,
        array $payment,
        int $partnerId,
        string $returnOrderId,
        int $cashboxId,
        int $cashSessionId,
        string $comment,
        int $currencyId
    ): void {
        $deferredAmount = (float)($alloc["f_amount_debt"] ?? 0) + (float)($alloc["f_amount_bank"] ?? 0);
        if ($deferredAmount > 0.00001) {
            // Write-off customer debt (sale wrote f_debit; return writes f_credit)
            $this->insert("cash_debts", [
                "f_date" => date("Y-m-d"),
                "f_partner" => $partnerId,
                "f_doc_type" => 2,
                "f_doc_uuid" => $returnOrderId,
                "f_credit" => $deferredAmount,
                "f_debit" => 0,
                "f_currency_id" => $currencyId,
            ]);
        }

        $cashOutTypes = [
            PAYMENT_TYPE_CASH,
            PAYMENT_TYPE_CARD,
            PAYMENT_TYPE_IDRAM,
            PAYMENT_TELCELL,
            PAYMENT_PREPAID,
        ];
        $sessionDelta = 0.0;
        foreach ($cashOutTypes as $pt) {
            $fn = $payment["fields"][$pt];
            $amount = (float)($alloc[$fn] ?? 0);
            if ($amount < 0.00001) {
                continue;
            }
            $this->insert("cash_operations", [
                "f_cashbox_id" => $cashboxId,
                "f_session_id" => $cashSessionId,
                "f_order_id" => $returnOrderId,
                "f_user" => $this->userid,
                "f_operation_type" => CASH_OP_TOTAL_EXPENSES,
                "f_payment_type_id" => $pt,
                "f_datetime" => date("Y-m-d H:i:s"),
                "f_debit" => 0,
                "f_credit" => $amount,
                "f_currency_id" => $currencyId,
                "f_comment" => $comment,
            ]);
            $sessionDelta += $amount;
        }
        if ($sessionDelta > 0.00001) {
            $this->select(
                "UPDATE cash_session SET f_amount_expected=f_amount_expected-? WHERE f_id=?",
                "di",
                [$sessionDelta, $cashSessionId],
                true
            );
        }
    }

    /**
     * Clear sale fiscal after successful KKM taxback (recovery / make-draft step 1).
     */
    public function ClearFiscal($params)
    {
        $orderId = (string)($params->id ?? "");
        if ($orderId === "") {
            dieWithCode(Translator::t("Missing id"));
        }
        $header = $this->select("select f_id, f_data from o_header where f_id=?", "s", [$orderId])->fetch_assoc();
        if (!$header) {
            dieWithCode(Translator::t("Document is not exists"));
        }
        $odata = json_decode($header["f_data"] ?? "{}", true) ?: [];
        unset($odata["f_fiscal"]);
        $odata["f_fiscal_taxback_at"] = date("Y-m-d H:i:s");
        $odata["log"][] = [
            "ts" => date("Y-m-d H:i:s"),
            "action" => "clear fiscal after taxback",
            "user" => $this->fullName(),
        ];
        $this->update(
            "o_header",
            ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)],
            $orderId
        );
        $this->result["cleared"] = 1;
        $this->echoResult();
    }

    /**
     * Shop: reopen closed sale as editable draft (same UUID), after client KKM taxback if any.
     */
    public function MakeDraft($params)
    {
        $orderId = (string)($params->id ?? "");
        if ($orderId === "") {
            dieWithCode(Translator::t("Missing id"));
        }

        $header = $this->select("select * from o_header where f_id=?", "s", [$orderId])->fetch_assoc();
        if (!$header) {
            dieWithCode(Translator::t("Document is not exists"));
        }
        if ((int)$header["f_state"] !== self::ORDER_STATE_CLOSE) {
            dieWithCode(Translator::t("Order is not closed"));
        }
        if ((int)($header["f_saletype"] ?? 0) === self::SALE_RETURN) {
            dieWithCode(Translator::t("You cannot return this item"));
        }

        $partialReturn = $this->select(
            "select f_id from o_goods where f_header=? and coalesce(f_returnedqty, 0) > 0 limit 1",
            "s",
            [$orderId]
        )->fetch_assoc();
        if ($partialReturn) {
            dieWithCode(Translator::t("Cannot make draft: goods were already returned"));
        }

        $returnDoc = $this->select(
            "select f_id from o_header
             where f_saletype=? and JSON_UNQUOTE(JSON_EXTRACT(f_data, '$.f_return_from'))=?
             limit 1",
            "is",
            [self::SALE_RETURN, $orderId]
        )->fetch_assoc();
        if ($returnDoc) {
            dieWithCode(Translator::t("Cannot make draft: return document exists"));
        }

        require_once __DIR__ . "/../waiter/order.php";
        $ord = new Order();
        if (!$ord->auth()) {
            dieWithCode("Unauthorized");
        }

        $preferredTable = (int)($params->table ?? $params->preferred_table ?? 0);
        $hallId = (int)($params->hall ?? $header["f_hall"] ?? 0);

        $order = $ord->performReopen($orderId, [
            "allow_reassign_table" => true,
            "preferred_table" => $preferredTable,
            "clear_fiscal" => true,
            "hall_id" => $hallId,
        ]);

        $this->result["order"] = $order;
        $this->result["table_id"] = (int)($order["f_table"] ?? $preferredTable);
        $this->echoResult();
    }
}
