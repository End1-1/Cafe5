<?php
# Guest order from the table booklet.
# POST /engine/v2/ararix/order/create
# POST /engine/v2/ararix/order/validate
# Table comes from the booklet key (one key per table).

require_once __DIR__ . "/../../worker/auth.php";
require_once __DIR__ . "/../../worker/selfboard-menu.php";
require_once __DIR__ . "/guestkey.php";
require_once __DIR__ . "/../waiter/order.php";

function guest_attr_part($dyn, string $key): array
{
    if (!is_array($dyn) || !array_key_exists($key, $dyn)) {
        return ["value" => "", "price" => 0.0, "measurement" => ""];
    }
    $raw = $dyn[$key];
    if (is_array($raw)) {
        return [
            "value" => trim((string)($raw["value"] ?? "")),
            "price" => (float)($raw["price"] ?? 0),
            "measurement" => trim((string)($raw["measurement"] ?? "")),
        ];
    }
    return ["value" => trim((string)$raw), "price" => 0.0, "measurement" => ""];
}

function guest_request_ids($raw): array
{
    if ($raw === null) {
        return [];
    }
    if (is_object($raw)) {
        $raw = (array)$raw;
    }
    if (!is_array($raw)) {
        guest_fail(422, "UNKNOWN_MODIFIER", Translator::t("Unknown modifier"));
    }
    $ids = [];
    foreach ($raw as $item) {
        if (is_object($item)) {
            $item = (array)$item;
        }
        if (is_array($item)) {
            $id = (int)($item["f_id"] ?? $item["id"] ?? 0);
        } else {
            $id = (int)$item;
        }
        if ($id > 0) {
            $ids[$id] = $id;
        }
    }
    return array_values($ids);
}

function guest_selected_modifiers(array $catalog, array $wantedIds): array
{
    $byId = [];
    foreach ($catalog as $item) {
        if (!is_array($item)) {
            continue;
        }
        $id = (int)($item["f_id"] ?? 0);
        $name = trim((string)($item["f_name"] ?? ""));
        if ($id <= 0 || $name === "") {
            continue;
        }
        $byId[$id] = $item;
    }

    foreach ($byId as $id => $item) {
        $required = $item["f_required"] ?? false;
        $isRequired = $required === true || $required === 1 || $required === "1";
        if ($isRequired && !in_array($id, $wantedIds, true)) {
            guest_fail(422, "REQUIRED_MODIFIER", Translator::t("Required modifier is missing") . ": " . trim((string)$item["f_name"]), [
                "f_id" => $id,
            ]);
        }
    }

    $selected = [];
    $sum = 0.0;
    foreach ($wantedIds as $id) {
        if (!isset($byId[$id])) {
            guest_fail(422, "UNKNOWN_MODIFIER", Translator::t("Unknown modifier") . ": " . $id, [
                "f_id" => $id,
            ]);
        }
        $price = (float)($byId[$id]["f_price"] ?? 0);
        $selected[] = [
            "f_id" => $id,
            "f_name" => trim((string)$byId[$id]["f_name"]),
            "f_price" => $price,
        ];
        $sum += $price;
    }
    return [$selected, $sum];
}

class GuestOrder extends Auth
{
    public function auth()
    {
        guest_require_table_key($this);
        return true;
    }

    public function Create($params)
    {
        if (isset($params->table) || isset($params->table_id) || isset($params->f_table)) {
            guest_fail(400, "TABLE_NOT_ALLOWED", "Table is taken from the booklet key and must not be sent");
        }

        $keyRow = guest_require_table_key($this);
        $idemKey = trim((string)($params->idempotency_key ?? ""));
        if ($idemKey === "" || strlen($idemKey) < 8 || strlen($idemKey) > 64) {
            guest_fail(400, "VALIDATION_FAILED", "idempotency_key is required (8..64 chars)");
        }
        $idemHash = hash("sha256", (int)$keyRow["f_id"] . "|" . $idemKey);
        $existingIdem = $this->select(
            "SELECT f_order_id FROM guest_order_idempotency WHERE f_key_hash = ? LIMIT 1",
            "s",
            [$idemHash]
        )->fetch_assoc();
        if ($existingIdem) {
            $this->echoExistingOrder((string)$existingIdem["f_order_id"], true);
            return;
        }

        $built = $this->buildCart($params, true);
        $place = $this->guestPlace($keyRow);

        $ord = new Order();
        $ord->bindSessionUser($place["staff_id"]);
        $header = $ord->CreateOrder((object)[
            "table" => $place["table_id"],
            "cashbox_id" => $place["cashbox_id"],
            "staff_id" => $place["staff_id"],
        ]);
        $orderId = (string)($header["f_id"] ?? "");
        if ($orderId === "") {
            guest_fail(500, "VALIDATION_FAILED", Translator::t("Order not exists"));
        }
        $headerData = json_decode($header["f_data"] ?? "{}", true);
        if (!is_array($headerData)) {
            $headerData = [];
        }
        $serviceFactor = (float)($headerData["f_service_factor"] ?? 0);

        foreach ($built["lines"] as $line) {
            $ord->AddDish((object)[
                "silent" => true,
                "order_id" => $orderId,
                "order_source" => "guest",
                "create_process" => true,
                "dish" => $line["dish"],
                "dish_name" => $line["name"],
                "qty" => $line["qty"],
                "type" => $line["type"],
                "price" => $line["price"],
                "store" => $line["store"],
                "print1" => $line["print1"],
                "print2" => $line["print2"],
                "count_service" => $line["count_service"],
                "count_discount" => $line["count_discount"],
                "service_factor" => $serviceFactor,
                "table" => $place["table_id"],
                "cashbox_id" => $place["cashbox_id"],
                "row" => 0,
                "f_data" => (object)$line["f_data"],
            ]);
        }

        $ord->printServiceCheckSilent((object)["header_id" => $orderId]);

        $token = bin2hex(random_bytes(24));
        $ord->update("o_header", ["f_public_token" => $token], $orderId);

        try {
            $this->insert("guest_order_idempotency", [
                "f_key_hash" => $idemHash,
                "f_order_id" => $orderId,
                "f_table_id" => $place["table_id"],
                "f_api_key_id" => (int)$keyRow["f_id"],
            ]);
        } catch (Exception $e) {
            // Race: another request stored the same key — return that order.
            $again = $this->select(
                "SELECT f_order_id FROM guest_order_idempotency WHERE f_key_hash = ? LIMIT 1",
                "s",
                [$idemHash]
            )->fetch_assoc();
            if ($again && (string)$again["f_order_id"] !== $orderId) {
                $this->echoExistingOrder((string)$again["f_order_id"], true);
                return;
            }
        }

        $this->echoExistingOrder($orderId, false);
    }

    public function Validate($params)
    {
        guest_require_table_key($this);
        $built = $this->buildCart($params, false);
        $this->result["ok"] = empty($built["price_changed"]);
        $this->result["amount_total"] = $built["amount_total"];
        $this->result["dishes"] = $built["response_dishes"];
        if (!empty($built["price_changed"])) {
            $this->result["error"] = [
                "code" => "PRICE_CHANGED",
                "message" => Translator::t("Price has changed"),
                "details" => ["lines" => $built["price_changed"]],
            ];
        }
        $this->echoResult();
    }

    private function echoExistingOrder(string $orderId, bool $replay): void
    {
        $ord = new Order();
        $header = $ord->GetHeader($orderId);
        if (!$header) {
            guest_fail(404, "VALIDATION_FAILED", Translator::t("Order not found"));
        }
        $rows = $ord->select(
            "SELECT og.f_goods AS f_dish, g.f_name AS f_name, og.f_qty, og.f_price, og.f_total, og.f_data
             FROM o_goods og
             LEFT JOIN c_goods g ON g.f_id = og.f_goods
             WHERE og.f_header = ? AND og.f_state = 1
             ORDER BY og.f_row",
            "s",
            [$orderId]
        )->fetch_all(MYSQLI_ASSOC);

        $dishes = [];
        foreach ($rows as $row) {
            $data = selfboard_decode_goods_data($row["f_data"] ?? null);
            $dishes[] = [
                "f_dish" => (int)$row["f_dish"],
                "f_name" => (string)$row["f_name"],
                "qty" => (float)$row["f_qty"],
                "f_price" => (float)$row["f_price"],
                "f_total" => (float)$row["f_total"],
                "f_comment" => (string)($data["f_comment"] ?? ""),
                "f_modificators" => $data["f_modificators"] ?? [],
                "f_package_id" => (int)($data["f_package_id"] ?? 0),
                "f_package_name" => (string)($data["f_package_name"] ?? ""),
            ];
        }

        $this->result["order"] = [
            "f_id" => $orderId,
            "f_prefix" => (string)($header["f_prefix"] ?? ""),
            "f_state" => (int)($header["f_state"] ?? 0),
            "f_amounttotal" => (float)($header["f_amounttotal"] ?? 0),
            "f_public_token" => (string)($header["f_public_token"] ?? ""),
            "table" => [
                "f_id" => (int)($header["f_table"] ?? 0),
                "f_name" => (string)($header["f_table_name"] ?? ""),
            ],
            "hall" => [
                "f_id" => (int)($header["f_hall"] ?? 0),
                "f_name" => (string)($header["f_hall_name"] ?? ""),
            ],
            "dishes" => $dishes,
        ];
        if ($replay) {
            $this->result["idempotent_replay"] = true;
        }
        $this->echoResult();
    }

    /**
     * @return array{lines:array,response_dishes:array,amount_total:float,price_changed:array}
     */
    private function buildCart($params, bool $failOnPriceChange): array
    {
        $raw = $params->dishes ?? null;
        if (is_object($raw)) {
            $raw = array_values((array)$raw);
        }
        if (!is_array($raw) || $raw === []) {
            guest_fail(400, "EMPTY_DISHES", Translator::t("Empty dish list"));
        }
        $locale = Translator::$locale ?? "hy";
        if (!in_array($locale, ["hy", "ru"], true)) {
            $locale = "hy";
        }

        $lines = [];
        $response = [];
        $priceChanged = [];
        $amount = 0.0;
        foreach ($raw as $idx => $item) {
            if (is_object($item)) {
                $item = (array)$item;
            }
            if (!is_array($item)) {
                guest_fail(422, "DISH_NOT_FOUND", Translator::t("Dish is not on the menu"));
            }
            $built = $this->buildLine($item, $locale);
            $clientPrice = array_key_exists("f_price", $item) ? (float)$item["f_price"]
                : (array_key_exists("price", $item) ? (float)$item["price"] : null);
            if ($clientPrice !== null && abs($clientPrice - $built["price"]) > 0.009) {
                $priceChanged[] = [
                    "index" => (int)$idx,
                    "f_dish" => (int)($item["f_dish"] ?? $item["dish"] ?? 0),
                    "old_price" => $clientPrice,
                    "new_price" => $built["price"],
                ];
                if ($failOnPriceChange) {
                    guest_fail(409, "PRICE_CHANGED", Translator::t("Price has changed"), [
                        "lines" => $priceChanged,
                    ]);
                }
            }
            $total = round($built["price"] * $built["qty"], 2);
            $amount += $total;
            $lines[] = $built;
            $response[] = [
                "f_dish" => $built["request_dish"],
                "f_goods" => $built["dish"],
                "f_name" => $built["name"],
                "qty" => $built["qty"],
                "f_price" => $built["price"],
                "f_total" => $total,
                "f_comment" => (string)($built["f_data"]["f_comment"] ?? ""),
                "f_modificators" => $built["f_data"]["f_modificators"] ?? [],
                "f_package_id" => (int)($built["f_data"]["f_package_id"] ?? 0),
                "f_package_name" => (string)($built["f_data"]["f_package_name"] ?? ""),
                "type" => $built["type_wanted"],
                "size" => $built["size_wanted"],
            ];
        }
        return [
            "lines" => $lines,
            "response_dishes" => $response,
            "amount_total" => round($amount, 2),
            "price_changed" => $priceChanged,
        ];
    }

    private function guestPlace(array $keyRow): array
    {
        global $webapi_config_id;
        $wsId = (int)($webapi_config_id ?? 0);
        if ($wsId <= 0) {
            guest_fail(500, "VALIDATION_FAILED", Translator::t("Guest order workstation is not configured"));
        }
        $row = $this->select("SELECT f_config FROM workstations WHERE f_id=?", "i", [$wsId])->fetch_assoc();
        if (!$row) {
            guest_fail(500, "VALIDATION_FAILED", Translator::t("Guest order workstation is not configured"));
        }
        $cfg = json_decode($row["f_config"] ?? "{}", true);
        if (!is_array($cfg)) {
            $cfg = [];
        }
        $cashboxId = (int)($cfg["f_cashbox_id"] ?? 0);
        if ($cashboxId <= 0) {
            guest_fail(500, "VALIDATION_FAILED", Translator::t("Guest order workstation is not configured"));
        }
        $session = $this->select(
            "SELECT f_user_open FROM cash_session WHERE f_state=1 AND f_cashbox_id=? ORDER BY f_id DESC LIMIT 1",
            "i",
            [$cashboxId]
        )->fetch_assoc();
        $staffId = (int)($session["f_user_open"] ?? 0);
        if ($staffId <= 0) {
            guest_fail(409, "SESSION_CLOSED", Translator::t("No active cashbox session"));
        }
        $tableId = (int)$keyRow["table"]["f_id"];
        return [
            "table_id" => $tableId,
            "cashbox_id" => $cashboxId,
            "staff_id" => $staffId,
        ];
    }

    private function buildLine(array $line, string $locale): array
    {
        $dishId = (int)($line["f_dish"] ?? $line["dish"] ?? 0);
        $qty = (float)($line["qty"] ?? $line["f_qty"] ?? 0);
        if ($dishId <= 0) {
            guest_fail(422, "DISH_NOT_FOUND", Translator::t("Dish is not on the menu"));
        }
        if ($qty <= 0) {
            guest_fail(422, "VALIDATION_FAILED", Translator::t("Quantity must be greater than zero"));
        }
        $comment = trim((string)($line["comment"] ?? $line["f_comment"] ?? ""));
        if (mb_strlen($comment) > 200) {
            $comment = mb_substr($comment, 0, 200);
        }
        $wanted = guest_request_ids($line["modifiers"] ?? $line["f_modificators"] ?? null);
        $typeWanted = trim((string)($line["type"] ?? ""));
        $sizeWanted = trim((string)($line["size"] ?? ""));

        $card = $this->select(
            "SELECT mm.f_dish, mm.f_price, mm.f_store, mm.f_print1, mm.f_print2, g.f_type, g.f_data,
                    COALESCE(NULLIF(TRIM(gdtr.f_name), ''), g.f_name) AS f_name,
                    cast(coalesce(sl.f_qty, -999) AS SIGNED) AS f_stoplist
             FROM c_menu mm
             INNER JOIN c_menu_names mn ON mn.f_id = mm.f_menu AND mn.f_enabled = 1
             INNER JOIN c_goods g ON g.f_id = mm.f_dish
             LEFT JOIN c_goods_tr gdtr ON gdtr.f_goods_id = g.f_id AND gdtr.f_lang = ?
             LEFT JOIN c_stoplist sl ON sl.f_dish = mm.f_dish
             WHERE mm.f_state = 1 AND mm.f_dish = ? AND g.f_type IN (1, 2, 5)
             LIMIT 1",
            "si",
            [$locale, $dishId]
        )->fetch_assoc();
        if (!$card) {
            guest_fail(422, "DISH_NOT_FOUND", Translator::t("Dish is not on the menu") . ": " . $dishId, [
                "f_dish" => $dishId,
            ]);
        }
        $stoplist = (int)($card["f_stoplist"] ?? -999);
        if ($stoplist !== -999) {
            guest_fail(409, "STOPLIST", Translator::t("Dish is on the stop list"), [
                "f_dish" => $dishId,
                "f_stoplist" => $stoplist,
            ]);
        }

        $data = selfboard_decode_goods_data($card["f_data"] ?? null);
        [$modifiers, $modifierSum] = guest_selected_modifiers($data["f_modificators"] ?? [], $wanted);
        $price = (float)$card["f_price"] + $modifierSum;
        $goodsId = (int)$card["f_dish"];
        $goodsType = (int)$card["f_type"];
        $name = (string)$card["f_name"];
        $fdata = [];
        if ($comment !== "") {
            $fdata["f_comment"] = $comment;
        }
        if ($modifiers !== []) {
            $fdata["f_modificators"] = $modifiers;
        }

        if ($goodsType === 5) {
            $resolved = $this->resolvePackage($dishId, $data, $typeWanted, $sizeWanted, $locale);
            $goodsId = $resolved["goods_id"];
            $goodsType = 1;
            $price = (float)$card["f_price"] + $resolved["attr_price"] + $modifierSum;
            $fdata["f_package_id"] = $dishId;
            $fdata["f_package_name"] = $name;
            if ($resolved["dynamic"] !== []) {
                $fdata["f_dynamic_attributes"] = $resolved["dynamic"];
            }
        }

        return [
            "request_dish" => $dishId,
            "dish" => $goodsId,
            "name" => $name,
            "qty" => $qty,
            "type" => $goodsType,
            "price" => round($price, 2),
            "store" => (int)$card["f_store"],
            "print1" => (string)($card["f_print1"] ?? ""),
            "print2" => (string)($card["f_print2"] ?? ""),
            "count_service" => !empty($data["f_count_service"]) ? 1 : 0,
            "count_discount" => !empty($data["f_count_discount"]) ? 1 : 0,
            "f_data" => $fdata,
            "type_wanted" => $typeWanted,
            "size_wanted" => $sizeWanted,
        ];
    }

    private function resolvePackage(int $packageId, array $packageData, string $typeWanted, string $sizeWanted, string $locale): array
    {
        unset($packageData);
        $rows = selfboard_load_package_complectation($this, $packageId, $locale);
        if ($rows === []) {
            guest_fail(422, "COMBO_UNAVAILABLE", Translator::t("This combination is not available"));
        }
        $parsed = [];
        $types = [];
        $sizes = [];
        foreach ($rows as $row) {
            $data = selfboard_decode_goods_data($row["f_data"] ?? null);
            $dyn = is_array($data["f_dynamic_attributes"] ?? null) ? $data["f_dynamic_attributes"] : [];
            $type = guest_attr_part($dyn, "Type");
            $size = guest_attr_part($dyn, "Size");
            if ($size["measurement"] === "" && isset($dyn["Measurement"])) {
                $size["measurement"] = trim((string)$dyn["Measurement"]);
            }
            if ($type["value"] !== "") {
                $types[$type["value"]] = true;
            }
            if ($size["value"] !== "") {
                $sizes[$size["value"]] = true;
            }
            $parsed[] = ["row" => $row, "type" => $type, "size" => $size];
        }
        if (count($types) > 1 && $typeWanted === "") {
            guest_fail(422, "TYPE_REQUIRED", Translator::t("Type is required"));
        }
        if (count($sizes) > 1 && $sizeWanted === "") {
            guest_fail(422, "SIZE_REQUIRED", Translator::t("Size is required"));
        }
        $match = null;
        foreach ($parsed as $item) {
            if ($typeWanted !== "" && $item["type"]["value"] !== $typeWanted) {
                continue;
            }
            if ($sizeWanted !== "" && $item["size"]["value"] !== $sizeWanted) {
                continue;
            }
            $match = $item;
            break;
        }
        if ($match === null) {
            guest_fail(422, "COMBO_UNAVAILABLE", Translator::t("This combination is not available"));
        }
        $dynamic = [];
        if ($match["type"]["value"] !== "") {
            $dynamic["Type"] = $match["type"]["value"];
        }
        if ($match["size"]["value"] !== "") {
            $dynamic["Size"] = $match["size"]["value"];
        }
        if ($match["size"]["measurement"] !== "") {
            $dynamic["Measurement"] = $match["size"]["measurement"];
        }
        return [
            "goods_id" => (int)$match["row"]["f_goods"],
            "attr_price" => (float)$match["type"]["price"] + (float)$match["size"]["price"],
            "dynamic" => $dynamic,
        ];
    }
}
