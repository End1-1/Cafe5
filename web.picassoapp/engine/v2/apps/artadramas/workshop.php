<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-10-28 14:56:00
# Last Modified: 2026-10-28 14:56:00
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

class Workshop extends Auth
{
    public function PinLogin($params)
    {
        global $bearerToken;
        $pin = trim((string)($params->pin ?? ""));
        if ($pin === "") {
            dieWithCode($this->msg("bad_pin"), 401);
        }
        $userdata = $this->select(
            "select f_id, f_first, f_last from s_user where f_altpassword=md5(?)",
            "s",
            [$pin]
        )->fetch_assoc();
        if (empty($userdata)) {
            dieWithCode($this->msg("bad_pin"), 401);
        }
        $bearerToken = uuid_v4();
        $name = trim(($userdata["f_last"] ?? "") . " " . ($userdata["f_first"] ?? ""));
        $this->insert("s_login_session", [
            "f_session" => $bearerToken,
            "f_user" => $userdata["f_id"],
            "f_datestart" => date("Y-m-d"),
            "f_timestart" => date("H:i:s"),
            "f_iplogin" => json_encode([
                $_SERVER["HTTP_X_FORWARDED_FOR"] ?? "0.0.0.0",
                $_SERVER["REMOTE_ADDR"] ?? "",
            ]),
        ]);
        $this->result["token"] = $bearerToken;
        $this->result["user"] = [
            "f_id" => (int)$userdata["f_id"],
            "f_name" => $name,
        ];
        $this->echoResult();
    }

    public function Catalog($params)
    {
        $this->writeStorages();
    }

    public function Storages($params)
    {
        $this->writeStorages();
    }

    private function writeStorages(): void
    {
        $rows = $this->select("select f_id, f_name from c_storages order by f_name")->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$row) {
            $row["f_id"] = (int)$row["f_id"];
            $row["f_name"] = (string)($row["f_name"] ?? "");
        }
        unset($row);
        $this->result["storages"] = $rows;
        $this->echoResult();
    }

    public function Goods($params)
    {
        $art = (int)($params->artnumber ?? 0);
        if ($art <= 0) {
            dieWithCode($this->msg("art"));
        }
        $lang = $this->lang();
        $sql = <<<EOD
        select g.f_id,
               coalesce(nullif(tr.f_name, ''), g.f_name) as f_name,
               coalesce(u.f_name, '') as f_unit_name
        from c_goods g
        left join c_goods_tr tr on tr.f_goods_id = g.f_id and tr.f_lang = ?
        left join c_units u on u.f_id = g.f_unit
        where cast(json_unquote(json_extract(g.f_data, '$.f_artnumber')) as unsigned) = ?
          and exists (select 1 from c_goods_complectation c where c.f_base = g.f_id)
        order by coalesce(nullif(tr.f_name, ''), g.f_name)
        EOD;
        $rows = $this->select($sql, "si", [$lang, $art])->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$row) {
            $row["f_id"] = (int)$row["f_id"];
            $row["f_name"] = (string)($row["f_name"] ?? "");
            $row["f_unit_name"] = (string)($row["f_unit_name"] ?? "");
        }
        unset($row);
        $this->result["goods"] = $rows;
        $this->echoResult();
    }

    public function Drafts($params)
    {
        $lang = $this->lang();
        $sql = <<<EOD
        select sd.f_id, sd.f_user_id, sd.f_doc_date, sd.f_status, sd.f_version, sd.f_data,
               coalesce(nullif(tr.f_name, ''), g.f_name) as f_name
        from store_document sd
        left join c_goods g on g.f_id = cast(json_unquote(json_extract(sd.f_data, '$.complect_goods')) as unsigned)
        left join c_goods_tr tr on tr.f_goods_id = g.f_id and tr.f_lang = ?
        where sd.f_doc_type = 4
          and sd.f_status = 0
          and json_unquote(json_extract(sd.f_data, '$.source')) = 'artadramas'
          and cast(json_unquote(json_extract(sd.f_data, '$.worker_id')) as unsigned) = ?
        order by sd.f_doc_date desc
        limit 100
        EOD;
        $rows = $this->select($sql, "si", [$lang, (int)$this->userid])->fetch_all(MYSQLI_ASSOC);
        $drafts = [];
        foreach ($rows as $row) {
            $data = $this->decodeData($row["f_data"] ?? null);
            $drafts[] = [
                "id" => (string)$row["f_id"],
                "doc_num" => (string)($row["f_user_id"] ?? ""),
                "doc_date" => (string)($row["f_doc_date"] ?? ""),
                "status" => (int)$row["f_status"],
                "version" => (int)$row["f_version"],
                "goods_id" => (int)($data["complect_goods"] ?? 0),
                "goods_name" => (string)($row["f_name"] ?? ""),
                "qty" => (float)($data["complect_qty"] ?? 0),
            ];
        }
        $this->result["drafts"] = $drafts;
        $this->echoResult();
    }

    public function Create($params)
    {
        $goodsId = (int)($params->goods_id ?? 0);
        $qty = (float)($params->qty ?? 0);
        $storeOut = (int)($params->store_out ?? 0);
        $storeIn = (int)($params->store_in ?? 0);
        $art = (int)($params->artnumber ?? 0);
        if ($goodsId <= 0 || $qty < 0.0001 || $storeOut <= 0 || $storeIn <= 0 || $art <= 0) {
            dieWithCode($this->msg("fill"));
        }
        $this->assertWorkshopGoods($goodsId, $art);
        $items = $this->recipeItems($goodsId, $qty);
        if (empty($items)) {
            dieWithCode($this->msg("recipe"));
        }

        $docUuid = uuid_v4();
        $rowId = uuid_v4();
        $now = date("Y-m-d H:i:s");
        $docData = [
            "kind" => "complect",
            "complect_goods" => $goodsId,
            "complect_qty" => $qty,
            "complect_row_id" => $rowId,
            "comment" => "",
            "create_user" => $this->fullName(),
            "create_date" => $now,
            "worker_id" => (int)$this->userid,
            "source" => "artadramas",
        ];

        $saved = $this->runStoreOperation([$storeOut, $storeIn], function () use ($storeOut, $storeIn, $docUuid, $goodsId, $qty, $rowId, $now, $docData, $items) {
            $docNum = $this->nextComplectNumber($storeOut);
            $res = $this->postComplect([
                "doc_uuid" => $docUuid,
                "doc_user_id" => $docNum,
                "doc_date" => $now,
                "doc_status" => 0,
                "doc_type" => 4,
                "doc_store_out" => $storeOut,
                "doc_store_in" => $storeIn,
                "doc_sum" => 0,
                "doc_create_user" => (int)$this->userid,
                "doc_version" => 0,
                "complect_goods" => $goodsId,
                "complect_qty" => $qty,
                "complect_row_id" => $rowId,
                "doc_data" => $docData,
                "items" => $items,
            ]);
            $res["doc_num"] = $docNum;
            return $res;
        });

        $this->result["id"] = $docUuid;
        $this->result["doc_num"] = $saved["doc_num"];
        $this->result["version"] = (int)($saved["version"] ?? 1);
        $this->result["qty"] = $qty;
        $this->result["doc_date"] = $now;
        $this->echoResult();
    }

    public function UpdateQty($params)
    {
        $id = trim((string)($params->id ?? ""));
        $qty = (float)($params->qty ?? 0);
        $version = (int)($params->version ?? -1);
        if ($id === "" || $qty < 0.0001) {
            dieWithCode($this->msg("qty"));
        }
        $doc = $this->select(
            "select f_id, f_user_id, f_status, f_doc_type, f_doc_date, f_store_out, f_store_in, f_version, f_data
             from store_document where f_id=?",
            "s",
            [$id]
        )->fetch_assoc();
        if (empty($doc) || (int)$doc["f_doc_type"] !== 4) {
            dieWithCode($this->msg("missing"));
        }
        if ((int)$doc["f_status"] !== 0) {
            dieWithCode($this->msg("posted"));
        }
        $data = $this->decodeData($doc["f_data"] ?? null);
        if ((int)($data["worker_id"] ?? 0) !== (int)$this->userid || ($data["source"] ?? "") !== "artadramas") {
            dieWithCode($this->msg("foreign"));
        }
        if ((int)$doc["f_version"] !== $version) {
            dieWithCode($this->msg("version"));
        }
        $goodsId = (int)($data["complect_goods"] ?? 0);
        $items = $this->recipeItems($goodsId, $qty);
        if (empty($items)) {
            dieWithCode($this->msg("recipe"));
        }
        $rowId = (string)($data["complect_row_id"] ?? "");
        if ($rowId === "") {
            $rowId = uuid_v4();
        }
        $data["kind"] = "complect";
        $data["complect_goods"] = $goodsId;
        $data["complect_qty"] = $qty;
        $data["complect_row_id"] = $rowId;
        $storeOut = (int)$doc["f_store_out"];
        $storeIn = (int)$doc["f_store_in"];
        $saved = $this->runStoreOperation([$storeOut, $storeIn], function () use ($doc, $id, $storeOut, $storeIn, $goodsId, $qty, $rowId, $version, $data, $items) {
            return $this->postComplect([
                "doc_uuid" => $id,
                "doc_user_id" => (string)($doc["f_user_id"] ?? ""),
                "doc_date" => (string)$doc["f_doc_date"],
                "doc_status" => 0,
                "doc_type" => 4,
                "doc_store_out" => $storeOut,
                "doc_store_in" => $storeIn,
                "doc_sum" => 0,
                "doc_create_user" => (int)$this->userid,
                "doc_version" => $version,
                "complect_goods" => $goodsId,
                "complect_qty" => $qty,
                "complect_row_id" => $rowId,
                "doc_data" => $data,
                "items" => $items,
            ]);
        });
        $this->result["id"] = $id;
        $this->result["version"] = (int)($saved["version"] ?? ($version + 1));
        $this->result["qty"] = $qty;
        $this->echoResult();
    }

    public function Remove($params)
    {
        $id = trim((string)($params->id ?? ""));
        if ($id === "") {
            dieWithCode($this->msg("missing"));
        }
        $doc = $this->ownDraft($id);
        $storeOut = (int)$doc["f_store_out"];
        $storeIn = (int)$doc["f_store_in"];
        $this->runStoreOperation([$storeOut, $storeIn], function () use ($id) {
            $res = $this->callStoreFunction("sf_store2_complect_delete", $id);
            if (($res["status"] ?? 1) > 0) {
                dieWithCode($this->storeError($res));
            }
            return $res;
        });
        $this->result["id"] = $id;
        $this->echoResult();
    }

    private function ownDraft(string $id): array
    {
        $doc = $this->select(
            "select f_id, f_user_id, f_status, f_doc_type, f_doc_date, f_store_out, f_store_in, f_version, f_data
             from store_document where f_id=?",
            "s",
            [$id]
        )->fetch_assoc();
        if (empty($doc) || (int)$doc["f_doc_type"] !== 4) {
            dieWithCode($this->msg("missing"));
        }
        if ((int)$doc["f_status"] !== 0) {
            dieWithCode($this->msg("posted"));
        }
        $data = $this->decodeData($doc["f_data"] ?? null);
        if ((int)($data["worker_id"] ?? 0) !== (int)$this->userid || ($data["source"] ?? "") !== "artadramas") {
            dieWithCode($this->msg("foreign"));
        }
        return $doc;
    }

    private function assertWorkshopGoods(int $goodsId, int $art): void
    {
        $row = $this->select("select f_data from c_goods where f_id=?", "i", [$goodsId])->fetch_assoc();
        if (empty($row)) {
            dieWithCode($this->msg("goods"));
        }
        $data = $this->decodeData($row["f_data"] ?? null);
        if ((int)($data["f_artnumber"] ?? 0) !== $art) {
            dieWithCode($this->msg("goods"));
        }
    }

    private function recipeItems(int $goodsId, float $qty): array
    {
        $recipe = $this->select(
            "select f_goods, f_qty from c_goods_complectation where f_base=? order by f_id",
            "i",
            [$goodsId]
        )->fetch_all(MYSQLI_ASSOC);
        $items = [];
        $row = 0;
        foreach ($recipe as $line) {
            $lineQty = round((float)$line["f_qty"] * $qty, 4);
            $itemId = (int)$line["f_goods"];
            if ($itemId <= 0 || $lineQty < 0.0001) {
                continue;
            }
            $items[] = [
                "id" => uuid_v4(),
                "item_id" => $itemId,
                "qty" => $lineQty,
                "price" => 0,
                "expire_date" => null,
                "comment" => "",
                "row" => $row++,
            ];
        }
        return $items;
    }

    private function nextComplectNumber(int $storeId): string
    {
        $row = $this->select(
            "select f_complectcounter from c_storages where f_id=? for update",
            "i",
            [$storeId]
        )->fetch_assoc();
        if (empty($row)) {
            dieWithCode($this->msg("store"));
        }
        $next = (int)($row["f_complectcounter"] ?? 0) + 1;
        $this->select(
            "update c_storages set f_complectcounter=? where f_id=?",
            "ii",
            [$next, $storeId],
            true
        );
        return str_pad((string)$next, 5, "0", STR_PAD_LEFT);
    }

    private function postComplect(array $payload): array
    {
        $res = $this->callStoreFunction("sf_store2_complect", json_encode($payload, JSON_UNESCAPED_UNICODE));
        if (($res["status"] ?? 1) > 0) {
            dieWithCode($this->storeError($res));
        }
        return $res;
    }

    private function storeError(array $result): string
    {
        $msg = (string)($result["msg"] ?? "");
        if ($msg === "version_error") {
            return $this->msg("version");
        }
        if ($msg === "insufficient_stock") {
            return $this->msg("stock");
        }
        if ($msg === "already_sold_cannot_delete") {
            return $this->msg("posted");
        }
        return $msg !== "" ? $msg : $this->msg("save");
    }

    private function decodeData($raw): array
    {
        if (is_array($raw)) {
            return $raw;
        }
        if (is_object($raw)) {
            $decoded = json_decode(json_encode($raw), true);
            return is_array($decoded) ? $decoded : [];
        }
        if ($raw === null || $raw === "") {
            return [];
        }
        $decoded = json_decode((string)$raw, true);
        return is_array($decoded) ? $decoded : [];
    }

    private function lang(): string
    {
        $locale = Translator::$locale ?? "ru";
        return $locale === "hy" ? "hy" : "ru";
    }

    private function msg(string $key): string
    {
        $ru = [
            "bad_pin" => "Неверный PIN",
            "art" => "Укажите номер цеха",
            "fill" => "Укажите товар, количество и оба склада",
            "recipe" => "У товара нет состава комплектации",
            "goods" => "Товар не входит в этот номер цеха",
            "store" => "Склад не найден",
            "qty" => "Количество должно быть больше нуля",
            "missing" => "Документ не найден",
            "posted" => "Документ уже сохранён бухгалтером",
            "foreign" => "Этот черновик создал другой работник",
            "version" => "Документ уже изменён, обновите список",
            "stock" => "Недостаточно остатка",
            "save" => "Не удалось записать документ",
        ];
        $hy = [
            "bad_pin" => "Սխալ PIN",
            "art" => "Նշեք արտադրամասի համարը",
            "fill" => "Նշեք ապրանքը, քանակը և երկու պահեստները",
            "recipe" => "Ապրանքը կոմպլեկտացիայի բաղադրություն չունի",
            "goods" => "Ապրանքը այս արտադրամասի համարում չէ",
            "store" => "Պահեստը չի գտնվել",
            "qty" => "Քանակը պետք է մեծ լինի զրոյից",
            "missing" => "Փաստաթուղթը չի գտնվել",
            "posted" => "Փաստաթուղթն արդեն պահպանել է հաշվապահը",
            "foreign" => "Այս սևագիրը ստեղծել է այլ աշխատող",
            "version" => "Փաստաթուղթն արդեն փոխվել է, թարմացրեք ցանկը",
            "stock" => "Մնացորդը բավարար չէ",
            "save" => "Փաստաթուղթը գրանցել չհաջողվեց",
        ];
        $dict = $this->lang() === "hy" ? $hy : $ru;
        return $dict[$key] ?? $ru[$key] ?? $key;
    }
}
