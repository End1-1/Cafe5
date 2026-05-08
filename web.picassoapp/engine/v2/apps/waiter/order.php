<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-07-06 13:35:02
# Last Modified: 2026-03-29 17:04:12

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";

const ORDER_STATE_OPEN = 1;
const ORDER_STATE_CLOSED = 2;
const ORDER_STATE_EMPTY = 3;
const ORDER_STATE_MOVED = 4;
const ORDER_STATE_PREORDER = 5;

const FORMAT_DATE_TO_STR = "d/m/Y";
const FORMAT_TIME_TO_STR = "H:i";

const CASHBOX_IN = 1;

class Order extends Auth
{
    public function GetPreorders($params)
    {
        $sql = <<<EOD
        select oh.f_id, oh.f_prefix, oh.f_hall, oh.f_table,
        h.f_name as f_hall_name, t.f_name as f_table_name,
        concat(u.f_last, ' ', left(u.f_first, 1), '.') as f_staff_name,
        json_value(oh.f_data, '$.f_date_open') as f_date_open,
        json_value(oh.f_data, '$.f_time_open') as f_time_open,
        json_value(oh.f_data, '$.f_preorder_datetime') as f_preorder_datetime,
        json_value(oh.f_data, '$.f_deposit_prepaid') as f_deposit_prepaid,
        json_value(oh.f_data, '$.f_guest.f_guest_name') as f_guest_name,
        json_value(oh.f_data, '$.f_guest.f_guest_phone') as f_guest_phone,
        json_value(oh.f_data, '$.f_guest.f_guest_address') as f_guest_address
        from o_header oh
        left join h_tables t on t.f_id = oh.f_table
        left join h_halls h on h.f_id = t.f_hall
        left join s_user u on u.f_id = oh.f_staff
        where oh.f_state = ?
        order by oh.f_id desc
        EOD;
        $rows = $this->select($sql, "i", [ORDER_STATE_PREORDER])->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$row) {
            $rawDeposit = $row["f_deposit_prepaid"] ?? null;
            if ($rawDeposit === null || $rawDeposit === "") {
                $row["f_deposit_prepaid"] = 0.0;
            } else {
                $row["f_deposit_prepaid"] = (float)$rawDeposit;
            }
        }
        unset($row);
        $this->result["preorders"] = $rows;
        $this->echoResult();
    }

    public function OpenOrder($params)
    {
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }
    public function ReopenOrder($params)
    {
        $header = $this->select("select * from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$header) {
            dieWithCode("Do you want to try hack reopen order?");
        }
        if ($header["f_state"]  != ORDER_STATE_CLOSED) {
            dieWithCode("Why you want to reopen order with illegal state?");
        }
        $check = $this->select("select * from o_header where f_state=1 and f_table=?", "i", [$header["f_table"]])->fetch_row();
        if ($check) {
            dieWithCode(Translator::t("This table busy now"));
        }
        $odata = json_decode($header["f_data"] ?? "{}", true);
        $this->beginTransaction();
        $rollbackAmount = 0;
        if (!empty($header["f_cash_session_id"])) {
            $sumRow = $this->select(
                "select coalesce(sum(f_debit), 0) as f_amount from cash_operations where f_order_id=? and f_session_id=? and f_operation_type=?",
                "sii",
                [$params->id, (int)$header["f_cash_session_id"], CASHBOX_IN]
            )->fetch_assoc();
            $rollbackAmount = (float)($sumRow["f_amount"] ?? 0);
            if ($rollbackAmount > 0) {
                $this->select(
                    "update cash_session set f_amount_expected=f_amount_expected-? where f_id=?",
                    "di",
                    [$rollbackAmount, (int)$header["f_cash_session_id"]],
                    true
                );
            }
            $this->select(
                "delete from cash_operations where f_order_id=? and f_session_id=? and f_operation_type=?",
                "sii",
                [$params->id, (int)$header["f_cash_session_id"], CASHBOX_IN],
                true
            );
        }
        $odata["f_amount_cash"] = 0;
        $odata["f_amount_card"] = 0;
        $odata["f_amount_bank"] = 0;
        $odata["f_amount_idram"] = 0;
        $odata["f_amount_complimentary"] = 0;
        $odata["f_amount_other"] = 0;
        $odata["f_amount_telcell"] = 0;
        $odata["f_amount_debt"] = 0;
        $odata["f_amount_prepaid"] = 0;
        $odata["f_amount_paid"] = 0;
        $odata["f_amount_change"] = 0;
        $odata["log"][] = ["ts" => date("Y-m-d H:i:s"), "action" => "reopen", "user" => $this->fullName()];
        $odata["log"][] = ["ts" => date("Y-m-d H:i:s"), "action" => "rollback cash operations", "amount" => $rollbackAmount, "user" => $this->fullName()];
        $this->update("o_header", [
            "f_state" => 1,
            "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)
        ], $params->id);
        $this->commit();
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function AddDish($params)
    {
        //first , check stoplist
        $this->beginTransaction();
        $stoplist = $this->select("select * from c_stoplist where f_dish=? for update", "i", [$params->dish])->fetch_assoc();
        if (!empty($stoplist)) {
            if ($stoplist["f_qty"] - $params->qty < 0) {
                $this->rollback();
                dieWithCode(Translator::t("Stoplist limit reached"));
            }
            $this->select("update c_stoplist set f_qty=f_qty-? where f_dish=?", "ii", [$params->qty, $params->dish], true);
            $this->result["stoplist"] = $stoplist["f_qty"] - $params->qty;
        }
        $this->commit();
        $isPreorder = (int)($params->is_preorder ?? 0) === 1;
        $orderId = (string)($params->order_id ?? "");
        $oheader = null;

        if (!empty($orderId)) {
            $oheader = $this->GetHeader($orderId);
            if (!$oheader) {
                dieWithCode("Invalid order id for add-dish");
            }
        } else if ($isPreorder) {
            // For preorder mode always create a new preorder when order_id is not provided.
            // One table can have unlimited preorder documents.
            $params->create_as_preorder = true;
            $oheader = $this->CreateOrder($params);
        } else {
            $oheader = $this->GetOpenHeader($params->table);
            if (empty($oheader) || empty($oheader["f_id"])) {
                $oheader = $this->CreateOrder($params);
            }
        }

        $data = [
            "f_count_service" => $params->count_service ?? 0,
            "f_count_discount" => $params->count_discount ?? 0,
            "f_service_factor" => $params->service_factor ?? 0,
            "f_discount_factor" => $params->discount_factor ?? 0,
            "f_append_time" => date("Y-m-d H:i:s"),
            "f_append_user" => $this->fullName(),
            "f_print1" => $params->print1,
            "f_print2" => $params->print2,
        ];
        $data = array_merge($data, (array) $params->f_data ?? []);
        $this->result["focused_dish"] = uuid_v4();
        $v["f_id"] = $this->result["focused_dish"];
        $v["f_header"] = $oheader["f_id"];
        $v["f_state"] = 1;
        $v["f_type"] = $params->type;
        $v["f_parent"] = $params->parent ?? null;
        $v["f_store"] = $params->store;
        $v["f_goods"] = $params->dish;
        $v["f_qty"] = $params->qty;
        $v["f_price"] = $params->price;
        $v["f_total"] = $params->price * $params->qty;
        $v["f_data"] = json_encode($data, JSON_UNESCAPED_UNICODE);
        $insertRow = (int) $params->row;
        if (!empty($params->parent)) {
            $parentId = $params->parent;
            $hid = $oheader["f_id"];
            $prow = $this->select(
                "select f_row from o_goods where f_id=? and f_header=?",
                "ss",
                [$parentId, $hid]
            )->fetch_assoc();
            if (!$prow) {
                dieWithCode(Translator::t("Invalid parent dish line"));
            }
            $pr = (int) $prow["f_row"];
            $maxR = $this->select(
                "select coalesce(max(f_row), 0) as mx from o_goods where f_header=? and f_parent=?",
                "ss",
                [$hid, $parentId]
            )->fetch_assoc();
            $mxChild = (int) ($maxR["mx"] ?? 0);
            $candidate = ($mxChild > 0) ? ($mxChild + 100) : ($pr + 100);

            /* Flat list uses ORDER BY f_row. Child lines used parent_row+100 which often equals the next
             * root line (second package at row 100 vs first package child at 0+100). Shift tail rows up to free a slot. */
            $guard = 0;
            while ($guard++ < 500) {
                $nrRow = $this->select(
                    "select coalesce(min(f_row), 2147483647) as mr from o_goods where f_header=? and (f_parent is null or f_parent = '') and f_row > ?",
                    "si",
                    [$hid, $pr]
                )->fetch_assoc();
                $nextRootRow = (int) ($nrRow["mr"] ?? 2147483647);
                if ($candidate < $nextRootRow || $nextRootRow >= 2147483647) {
                    break;
                }
                $this->select(
                    "update o_goods set f_row=f_row+100 where f_header=? and f_row>=?",
                    "si",
                    [$hid, $nextRootRow],
                    true
                );
            }
            $insertRow = $candidate;
        } elseif (empty($params->shift_rows)) {
            $maxRow = $this->select(
                "select coalesce(max(f_row), 0) as mx from o_goods where f_header=?",
                "s",
                [$oheader["f_id"]]
            )->fetch_assoc();
            $mxGlobal = (int) ($maxRow["mx"] ?? 0);
            $insertRow = max((int) $params->row, $mxGlobal + 100);
        }
        $v["f_row"] = $insertRow;
        $this->insert("o_goods", $v);
        if (!empty($params->shift_rows)) {
            $this->select("update o_goods set f_row=f_row+100 where f_row>=? and f_header=?", "is", [$insertRow, $oheader["f_id"]], true);
        }
        $odata = json_decode($oheader["f_data"] ?? "{}", true);
        $cd["f_id"] =  $this->result["focused_dish"];
        $cd["f_header"] = $oheader["f_id"];
        $cd["f_status"] = 1;
        $cd["f_daily_number"] = $odata["f_daily_number"] ?? "-";
        $ogp = ["f_status_1_1_time" => date("Y-m-d H:i:s"), "f_cooking_start" => $params->f_data?->f_cooking_start ?? date("1981-09-05 00:01:00"), "f_cooking_end" =>   $params->f_data?->f_cooking_end ?? date("Y-M-d H:i:s"), "f_substatus" => 1];
        $cd["f_data"] = json_encode($ogp, JSON_UNESCAPED_UNICODE);
        $this->insert("o_goods_process", $cd);
        $this->CountAmounts($oheader["f_id"],  $this->LogRecord("add dish", ["comment" => $params->dish_name . " (" . $params->qty . ") "]));
        $this->result["order"] = $this->GetOrder($oheader["f_id"]);
        $this->echoResult();
    }

    public function SetDishQty($params)
    {
        $dish_states = require_once __DIR__ . "/../worker/dict-dish-state.php";
        $sql = <<<EOD
        select g.f_name as f_dish_name, og.* 
        from o_goods og
        left join c_goods g on g.f_id=og.f_goods
        where og.f_id=?
        EOD;
        $row = $this->select($sql, "s", [$params->id])->fetch_assoc();
        if (!$row) {
            var_dump($params);
            dieWithCode("Are you hacker of o_goods row? Code:" . $params->id);
        }
        $data = json_decode($row["f_data"] ?? "{}", true);
        $data = array_merge((array)$params->data, $data);
        $stoplistqty = $params->restore_stoplist ?? 0;
        $log = [];
        if ($stoplistqty != 0) {
            $this->beginTransaction();
            $stoplist = $this->select("select * from c_stoplist where f_dish=? for update", "i", [$params->dish])->fetch_assoc();
            if (!empty($stoplist)) {
                $v["f_qty"] = $stoplist["f_qty"] + $params->restore_stoplist;
                if ($v["f_qty"] < 0) {
                    $this->rollback();
                    dieWithCode(Translator::t("Stoplist limit reached"));
                }
                $this->update("c_stoplist", $v, $params->dish, "f_dish");
                $this->result["restore_stoplist_qty"] = $v["f_qty"];
                $this->result["restore_stoplist_dish"] = $params->dish;
            }
            $this->commit();
        }
        if ($params->new_state == 2 || $params->new_state == 3) {
            $this->result["print1"] = $data["f_print1"] ?? "";
            $this->result["print2"] = $data["f_print2"] ?? "";
            $this->result["f_store_out"] = $params->new_state == DISH_STATE_VOID;
            $this->result["f_removed_qty"] = $params->new_qty;
            $this->result["f_removed_dish_name"] = $row["f_dish_name"];
            if (!empty($params->remove_reason)) {
                $data["f_remove_reason"] = $params->remove_reason;
                $data["f_removed_time"] = date("y-M-d H:i:s");
                $this->result["f_removed_comment"] = $params->remove_reason;
            }
            $log[] = $this->LogRecord("dishes removed",  ["comment" => $params->dish_name . " (" . $params->new_qty . ") " . $dish_states["names"][$params->new_state], "important" => true]);
        } else {
            $log[] = $this->LogRecord("dish quantity", ["comment" => $params->dish_name . " (" . $params->new_qty . ") " . $dish_states["names"][$params->new_state]]);
        }
        $v = [];
        $v["f_qty"] = $params->new_qty;
        $v["f_state"] = $params->new_state;
        $v["f_data"] = json_encode($data, JSON_UNESCAPED_UNICODE);
        if ($params->remove_emarks ?? false) {
            $v["f_emarks"] = null;
        }
        $this->update("o_goods", $v, $params->id);
        $this->CountAmounts($params->order_id, $log);
        $this->result["order"] = $this->GetOrder($params->order_id);
        $this->echoResult();
    }

    /** Убираем признак сервисной печати у клона строки заказа. */
    private function stripPrintedFlagsFromOgData(?string $json): string
    {
        $d = json_decode($json ?? '{}', true);
        if (!is_array($d)) {
            $d = [];
        }
        unset($d['f_printed'], $d['f_print_time']);

        return json_encode($d, JSON_UNESCAPED_UNICODE);
    }

    private function consumeStoplistForDish(int $dishId, float $qty): bool
    {
        if ($qty <= 0.00001) {
            return true;
        }
        $stoplist = $this->select("select * from c_stoplist where f_dish=? for update", "i", [$dishId])->fetch_assoc();
        if (empty($stoplist)) {
            return true;
        }
        if ($stoplist["f_qty"] - $qty < 0) {
            return false;
        }
        $this->select("update c_stoplist set f_qty=f_qty-? where f_dish=?", "di", [$qty, $dishId], true);

        return true;
    }

    /**
     * @param array<string,mixed> $src
     * @param array<string,mixed> $overrides
     */
    private function insertClonedOgRow(array $src, array $overrides): string
    {
        unset($src['f_id']);
        $v = array_merge($src, $overrides);
        $newId = uuid_v4();
        $v['f_id'] = $newId;
        $this->insert('o_goods', $v);

        return $newId;
    }

    private function insertOgProcessRow(string $headerId, string $ogId): void
    {
        $oheader = $this->select("select f_data from o_header where f_id=?", "s", [$headerId])->fetch_assoc();
        $odata = json_decode($oheader["f_data"] ?? "{}", true) ?: [];
        $cd = [];
        $cd["f_id"] = $ogId;
        $cd["f_header"] = $headerId;
        $cd["f_status"] = 1;
        $cd["f_daily_number"] = $odata["f_daily_number"] ?? "-";
        $ogp = [
            "f_status_1_1_time" => date("Y-m-d H:i:s"),
            "f_cooking_start" => date("1981-09-05 00:01:00"),
            "f_cooking_end" => date("Y-m-d H:i:s"),
            "f_substatus" => 1,
        ];
        $cd["f_data"] = json_encode($ogp, JSON_UNESCAPED_UNICODE);
        $this->insert("o_goods_process", $cd);
    }

    /**
     * Одна копия напечатанного пакета: новая строка пакета + дочерние строки с теми же количествами/ценами (без f_printed).
     *
     * @param array<string,mixed> $pkg
     * @param array<int,array<string,mixed>> $children
     */
    private function duplicateOnePrintedPackageCopy(string $headerId, array $pkg, array $children): string
    {
        if (!$this->consumeStoplistForDish((int)$pkg['f_goods'], (float)$pkg['f_qty'])) {
            throw new \RuntimeException('stoplist');
        }

        $mxRow = $this->select("select coalesce(max(f_row), 0) as mx from o_goods where f_header=?", "s", [$headerId])->fetch_assoc();
        $newPkgRow = (int)($mxRow["mx"] ?? 0) + 100;

        $newPkgId = $this->insertClonedOgRow($pkg, [
            'f_header' => $headerId,
            'f_parent' => null,
            'f_row' => $newPkgRow,
            'f_state' => 1,
            'f_data' => $this->stripPrintedFlagsFromOgData($pkg['f_data'] ?? '{}'),
        ]);
        $this->insertOgProcessRow($headerId, $newPkgId);

        $pr = (int)$pkg['f_row'];

        foreach ($children as $ch) {
            if (!$this->consumeStoplistForDish((int)$ch['f_goods'], (float)$ch['f_qty'])) {
                throw new \RuntimeException('stoplist');
            }
            $newRow = $newPkgRow + ((int)$ch['f_row'] - $pr);
            $childId = $this->insertClonedOgRow($ch, [
                'f_header' => $headerId,
                'f_parent' => $newPkgId,
                'f_row' => $newRow,
                'f_state' => 1,
                'f_data' => $this->stripPrintedFlagsFromOgData($ch['f_data'] ?? '{}'),
            ]);
            $this->insertOgProcessRow($headerId, $childId);
        }

        return $newPkgId;
    }

    /** Дублирование напечатанного пакета со всем составом (после сервисной печати, кнопка + на официанте). */
    public function DuplicatePrintedPackage($params)
    {
        $headerId = (string)($params->header_id ?? $params->order_id ?? '');
        $packageId = (string)($params->package_id ?? '');
        $copies = (int)($params->copies ?? 1);
        if ($headerId === '' || $packageId === '') {
            dieWithCode('Invalid duplicate package request');
        }
        if ($copies < 1) {
            $copies = 1;
        }
        if ($copies > 50) {
            $copies = 50;
        }

        $pkg = $this->select("select * from o_goods where f_id=? and f_header=?", "ss", [$packageId, $headerId])->fetch_assoc();
        if (!$pkg) {
            dieWithCode(Translator::t("Order line not found"));
        }
        if ((int)$pkg['f_type'] !== 5) {
            dieWithCode(Translator::t("Not a package"));
        }
        if ((int)$pkg['f_state'] !== 1) {
            dieWithCode(Translator::t("Invalid dish state"));
        }

        $pdata = json_decode($pkg['f_data'] ?? '{}', true);
        if (!is_array($pdata)) {
            $pdata = [];
        }
        $pv = $pdata['f_printed'] ?? false;
        $isPrinted = ($pv === true || $pv === 1 || $pv === '1' || $pv === 'true');
        if (!$isPrinted) {
            dieWithCode(Translator::t("Print the package before adding another"));
        }

        $children = $this->select(
            "select * from o_goods where f_header=? and f_parent=? and f_state=1 order by f_row",
            "ss",
            [$headerId, $packageId]
        )->fetch_all(MYSQLI_ASSOC);

        $this->beginTransaction();
        $lastNewId = null;

        try {
            for ($i = 0; $i < $copies; $i++) {
                $lastNewId = $this->duplicateOnePrintedPackageCopy($headerId, $pkg, $children);
            }
            $this->commit();
        } catch (\RuntimeException $e) {
            $this->rollback();
            if ($e->getMessage() === 'stoplist') {
                dieWithCode(Translator::t("Stoplist limit reached"));
            }
            dieWithCode($e->getMessage());
        }

        $this->result["focused_dish"] = $lastNewId;
        $this->CountAmounts($headerId, [$this->LogRecord("duplicate printed package", ["comment" => "copies={$copies}"])]);
        $this->result["order"] = $this->GetOrder($headerId);
        $this->echoResult();
    }

    public function SetDishComment($params)
    {
        $row = $this->select("select * from o_goods where f_id=?", "s", [$params->id])->fetch_assoc();
        if (empty($row)) {
            dieWithCode("Are your hacker of row?");
        }
        $data = json_decode($row["f_data"] ?? "{}", true);
        $data["f_comment"] = $params->comment;
        $this->update("o_goods", ["f_data" => json_encode($data, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->result["order"] = $this->GetOrder($row["f_header"]);
        $this->echoResult();
    }

    public function StopPlay($params)
    {
        $dish = $this->select("select * from o_goods where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$dish) {
            dieWithCode("Do you wanna hack stop play? ");
        }
        $ddata = json_decode($dish["f_data"] ?? [], true);
        $ddata["f_stopped"] = true;
        $this->update("o_goods", ["f_data" => json_encode($ddata, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->CountAmounts($dish["f_header"]);
        $this->result["order"] = $this->GetOrder($dish["f_header"]);
        $this->echoResult();
    }

    public function SetEndDatetime($params)
    {
        $dish = $this->select("select f_id, f_header, f_data from o_goods where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$dish) {
            dieWithCode("Do you wanna hack stop play?");
        }

        // 1. Декодируем JSON из колонки f_data
        $ddata = json_decode($dish["f_data"] ?? "[]", true);

        // 2. Достаем f_append_time из распарсенного массива
        $startTimeStr = $ddata["f_append_time"] ?? '';
        if (empty($startTimeStr)) {
            // Если вдруг в JSON нет даты начала, это косяк данных
            dieWithCode("Error: f_append_time not found in service data");
        }

        $startTime = strtotime($startTimeStr);
        $endTime = strtotime($params->datetime);

        // 3. Проверка: дата остановки не может быть раньше даты добавления
        if ($endTime < $startTime) {
            dieWithCode("Error: End time (" . $params->datetime . ") cannot be earlier than start time (" . $startTimeStr . ")");
        }

        // 4. Обновляем данные
        $ddata["f_stopped"] = true;
        $ddata["f_play_time"] = $params->datetime;

        $this->update("o_goods", [
            "f_data" => json_encode($ddata, JSON_UNESCAPED_UNICODE)
        ], $params->id);

        // 5. Пересчет и результат
        $this->CountAmounts($dish["f_header"]);
        $this->result["order"] = $this->GetOrder($dish["f_header"]);
        $this->echoResult();
    }

    public function CreateOrder($params)
    {
        $this->beginTransaction();
        /* total counter */
        $sql = <<<EOD
        SELECT h.f_id, c.f_counter+1 as f_counter, h.f_counterhall, h.f_prefix as f_department,
        CONCAT(
            LEFT(h.f_prefix, LENGTH(h.f_prefix) - LENGTH(REGEXP_SUBSTR(h.f_prefix, '[0-9]+$'))),
            LPAD(
                c.f_counter + 1,
                LENGTH(REGEXP_SUBSTR(h.f_prefix, '[0-9]+$')),
                '0')
        ) AS f_prefix,
        if (t.f_special_config > 0, s2.f_config, s1.f_config) as f_config 
        FROM h_tables t
        LEFT JOIN h_halls h ON h.f_id = t.f_hall
        LEFT JOIN h_halls c ON c.f_id=h.f_counterhall
        left join sys_json_config s1 on s1.f_id=h.f_settings
        left join sys_json_config s2 on s2.f_id=t.f_special_config
        WHERE t.f_id=?
        for update
        EOD;
        $hall = $this->select($sql, "i", [$params->table])->fetch_assoc();
        $data = json_decode($hall["f_config"] ?? "{}", true);
        $service_factor = $service_factor = !empty($params->service_factor) ? $params->service_factor : ($data["f_service_factor"] ?? 0);

        $discount_factor = $params->discount_factor ?? 0;
        if (empty($hall)) {
            $this->rollback();
            dieWithCode("Oh, why tell me wrong id of table?");
        }
        $nv["f_counter"] = $hall["f_counter"];
        $this->update("h_halls", $nv, $hall["f_counterhall"]);

        /*daily counter */
        require_once __DIR__ . "/cashbox.php";
        $cc = new Cashbox();
        $cash_session = $cc->GetOpenedCashboxSessionId($params->cashbox_id ?? 0);
        if (!$cash_session) {
            dieWithCode("Cashbox session is empty");
        }
        $sql_counter = <<<EOD
        INSERT INTO o_daily_counter (f_cash_session_id, f_department, f_counter)
        VALUES (?, ?, 1)
        ON DUPLICATE KEY UPDATE f_counter = f_counter + 1
        EOD;
        $this->select($sql_counter, "is", [$cash_session["f_id"], $hall["f_department"]], true);
        $res_c = $this->select(
            "SELECT concat(f_department, f_counter) as f_counter FROM o_daily_counter WHERE f_cash_session_id=? AND f_department=?",
            "is",
            [$cash_session["f_id"], $hall["f_department"]]
        )->fetch_assoc();
        $daily_number = $res_c['f_counter'];
        $this->commit();

        $id = uuid_v4();
        $v["f_id"] = $id;
        $v["f_prefix"] = $hall["f_department"] . $hall["f_counter"];
        $v["f_state"] = !empty($params->create_as_preorder) ? ORDER_STATE_PREORDER : ORDER_STATE_OPEN;
        $v["f_hall"] = $hall["f_id"];
        $v["f_table"] = $params->table;
        $v["f_datecash"] = date("Y-m-d");
        $v["f_staff"] = $this->userid;
        $v["f_cashier"] = $this->userid;
        $v["f_currentstaff"] = $this->userid;
        $v["f_data"] = json_encode([
            "f_service_factor" => $service_factor,
            "f_discount_factor" => $discount_factor,
            "f_date_open" => date("Y-m-d"),
            "f_time_open" => date("H:i:s"),
            "f_daily_number_prefix"  => $hall["f_prefix"],
            "f_daily_number" => $daily_number,
            "log" => [
                $this->LogRecord("created")
            ]
        ], JSON_UNESCAPED_UNICODE);
        $this->insert("o_header", $v);
        return $this->GetHeader($id);
    }

    public function UpdateAmounts($params)
    {
        $this->CountAmounts($params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function CountAmounts($id, $extraLog = [])
    {
        $row = $this->select("select f_data from o_header where f_id=?", "s", [$id])->fetch_assoc();
        $odata = json_decode($row["f_data"] ?? "{}", true) ?: [];
        $odata["log"] ??= [];

        /* Skip package children (parent f_type = GOODS_TYPE_PACKAGE) from bill totals */
        $sqlCountDishes = <<<EOD
select og.*
from o_goods og
left join o_goods pkg on pkg.f_id = og.f_parent and pkg.f_header = og.f_header
where og.f_header = ?
  and og.f_state = 1
  and not (og.f_parent is not null and og.f_parent != '' and pkg.f_type = 5)
EOD;
        $dishes = $this->select($sqlCountDishes, "s", [$id])->fetch_all(MYSQLI_ASSOC);

        $subtotal = $totaldue = $serviceCounted = $discountCounted = 0;

        foreach ($dishes as $d) {
            $ddata = json_decode($d["f_data"] ?? "{}", true) ?: [];

            if (($ddata["f_complimentary"] ?? 0) == 1) continue;
            if (!($ddata["f_printed"] ?? false)) continue;
            //var_dump("DDD");

            $priceModificator = 0;

            if ($ddata["f_hourly_payment"] ?? false) {
                $startTime = strtotime($ddata["f_append_time"]);

                // 1. Определяем точку "До какого момента считаем"
                if ($ddata["f_stopped"] ?? false) {
                    // Если остановлено, считаем строго до зафиксированного f_play_time
                    // Если f_play_time вдруг пустой (ошибка логики), берем текущее время как фоллбэк
                    $endTime = !empty($ddata["f_play_time"]) ? strtotime($ddata["f_play_time"]) : time();
                } else {
                    // Если не остановлено, считаем до "сейчас" и ОБНОВЛЯЕМ f_play_time
                    $endTime = time();
                    $ddata["f_play_time"] = date("Y-m-d H:i:s", $endTime);
                }

                // 2. Считаем разницу в секундах
                $diffSeconds = $endTime - $startTime;
                if ($diffSeconds < 0) $diffSeconds = 0;

                // 3. Правило интервала (шага)
                $ruleIntervalMinutes = (int)($ddata["f_hourly_rule"] ?: 60);
                $ruleIntervalSeconds = $ruleIntervalMinutes * 60;

                // 4. Считаем количество шагов (ceil — даже 1 секунда нового интервала считается за полный шаг)
                $steps = ceil($diffSeconds / $ruleIntervalSeconds);
                if ($steps < 1) $steps = 1;

                // 5. Расчет цены
                if (!isset($ddata["f_initial_price"])) {
                    $ddata["f_initial_price"] = $d["f_price"];
                }

                $initialPrice = (float)($ddata["f_initial_price"] ?? 0);
                // Цена за один интервал (шаг)
                $pricePerStep = $initialPrice / (60 / $ruleIntervalMinutes);

                $rawPrice = $steps * $pricePerStep;

                // Округление до сотен вниз (например, 150 -> 100, 290 -> 200)
                $d["f_price"] = floor($rawPrice / 100) * 100;

                // 6. Формируем текст для чека/отчета
                $startStr = date("d/m/Y H:i", $startTime);
                $endStr   = date("d/m/Y H:i", $endTime);
                $ddata["f_comment"] = $startStr . " - " . $endStr;
            }

            if (($ddata["f_count_service"] ?? false)) {
                $ddata["f_service_factor"] = $odata["f_service_factor"] ?? 0;
                $priceModificator += $ddata["f_service_factor"];
                $serviceCounted += $d["f_price"] * $ddata["f_service_factor"] * $d["f_qty"];
            }

            if (($ddata["f_count_discount"] ?? false)) {
                $ddata["f_discount_factor"] = $odata["f_discount_factor"] ?? 0;
                $priceModificator -= $ddata["f_discount_factor"];
                $discountCounted += $d["f_price"] * $ddata["f_discount_factor"] * $d["f_qty"];
            }

            $subtotal += $d["f_price"] * $d["f_qty"];
            $price = $d["f_price"] * (1 + $priceModificator);
            $totaldue += $d["f_qty"] * $price;

            $this->update("o_goods", [
                "f_price" => $d["f_price"],
                "f_total" => $d["f_qty"] * $price,
                "f_data"  => json_encode($ddata, JSON_UNESCAPED_UNICODE)
            ], $d["f_id"]);
        }

        /* ROUND BILL */
        /*
        $roundedTotal = floor($totaldue / 100) * 100;
        $diff = $totaldue - $roundedTotal;
        if ($diff > 0) {
            if ($serviceCounted > $diff) {
                $serviceCounted -= $diff;
                $totaldue -= $diff;
            }
        }
            */


        $odata["f_sub_total"] = $subtotal;
        $odata["f_service_amount"] = $serviceCounted;
        $odata["f_discount_amount"] = $discountCounted;

        if (!empty($extraLog)) {
            if (array_keys($extraLog) !== range(0, count($extraLog) - 1)) {
                $extraLog = [$extraLog];
            }
            $odata["log"] = array_merge($odata["log"], $extraLog);
        }

        // var_dump($id);
        $this->update("o_header", [
            "f_amounttotal" => $totaldue,
            "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)
        ], $id);
    }

    public function CountAmountsOfPreorder($id, $extraLog = [])
    {
        $row = $this->select("select f_data from o_header where f_id=?", "s", [$id])->fetch_assoc();
        $odata = json_decode($row["f_data"] ?? "{}", true) ?: [];
        $odata["log"] ??= [];

        $dishes = $this->select("select * from o_goods where f_header=? and f_state=1", "s", [$id])->fetch_all(MYSQLI_ASSOC);

        $subtotal = $totaldue = $serviceCounted = $discountCounted = 0;

        foreach ($dishes as $d) {
            $ddata = json_decode($d["f_data"] ?? "{}", true) ?: [];

            if (($ddata["f_complimentary"] ?? 0) == 1) continue;
            // Preorder totals are calculated from NOT printed rows
            if (($ddata["f_printed"] ?? false)) continue;

            $priceModificator = 0;

            if (($ddata["f_count_service"] ?? false)) {
                $ddata["f_service_factor"] = $odata["f_service_factor"] ?? 0;
                $priceModificator += $ddata["f_service_factor"];
                $serviceCounted += $d["f_price"] * $ddata["f_service_factor"] * $d["f_qty"];
            }

            if (($ddata["f_count_discount"] ?? false)) {
                $ddata["f_discount_factor"] = $odata["f_discount_factor"] ?? 0;
                $priceModificator -= $ddata["f_discount_factor"];
                $discountCounted += $d["f_price"] * $ddata["f_discount_factor"] * $d["f_qty"];
            }

            $subtotal += $d["f_price"] * $d["f_qty"];
            $price = $d["f_price"] * (1 + $priceModificator);
            $totaldue += $d["f_qty"] * $price;
        }

        $odata["f_sub_total"] = $subtotal;
        $odata["f_service_amount"] = $serviceCounted;
        $odata["f_discount_amount"] = $discountCounted;

        if (!empty($extraLog)) {
            if (array_keys($extraLog) !== range(0, count($extraLog) - 1)) {
                $extraLog = [$extraLog];
            }
            $odata["log"] = array_merge($odata["log"], $extraLog);
        }

        $this->update("o_header", [
            "f_amounttotal" => $totaldue,
            "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)
        ], $id);
    }


    public function SetAmount($params)
    {
        $oheader = $this->GetHeader($params->id);
        $odata = json_decode($oheader["f_data"] ?? "{}", true);
        $odata[$params->payment_field] = $params->amount;
        $odata["f_amount_paid"] =
            ($odata["f_amount_cash"] ?? 0)
            + ($odata["f_amount_card"] ?? 0)
            + ($odata["f_amount_bank"] ?? 0)
            + ($odata["f_amount_idram"] ?? 0)
            + ($odata["f_amount_complimentary"] ?? 0);

        if ($odata["f_amount_paid"] > $oheader["f_amounttotal"]) {
            $odata["f_amount_change"] = $odata["f_amount_paid"] - $oheader["f_amounttotal"];
        }
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function SetAmounts($params)
    {
        $oheader = $this->GetHeader($params->id);
        $odata = json_decode($oheader["f_data"] ?? "{}", true);
        $odata["f_amount_paid"] = 0;


        $payment = require_once __DIR__ . "/../worker/dict-payment.php";

        $total = 0;
        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            $odata[$pn] = $params->$pn ?? 0;
            $total +=  $odata[$pn];
        }

        $odata["f_amount_paid"] = $total;
        if ($odata["f_amount_paid"] > $oheader["f_amounttotal"]) {
            $odata["f_amount_change"] = $odata["f_amount_paid"] - $oheader["f_amounttotal"];
        }




        if ($odata["f_amount_paid"] > $oheader["f_amounttotal"]) {
            $odata["f_amount_change"] = $odata["f_amount_paid"] - $oheader["f_amounttotal"];
        }
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function GetOpenHeader($table)
    {
        $sql = <<<EOD
        select oh.*, coalesce(oh.f_table, t.f_id) as f_table,  t.f_name as f_table_name, oh.f_data
        from h_tables t
        left join o_header oh on t.f_id=oh.f_table and  oh.f_state=1 
        where t.f_id=?
        EOD;
        return  $this->select($sql, "i", [$table])->fetch_assoc();
    }

    public function GetHeader($id)
    {
        $sql = <<<EOD
        select oh.f_id, oh.f_state, oh.f_prefix, oh.f_table, oh.f_amounttotal, oh.f_data,
        h.f_name as f_hall_name, t.f_name as f_table_name, oh.f_cash_session_id,
        oh.f_staff, concat(u1.f_last, ' ', left(u1.f_first, 1) , '.') as f_staff_name,
        oh.f_cashier, concat(u2.f_last, ' ', left(u2.f_first, 1), '.') as f_cashier_name
        from o_header oh
        left join h_tables t on t.f_id=oh.f_table 
        left join h_halls h on h.f_id=t.f_hall
        left join s_user u1 on u1.f_id = oh.f_staff
        left join s_user u2 on u2.f_id = oh.f_cashier
        where oh.f_id=?
        EOD;
        $oheader = $this->select($sql, "s", [$id])->fetch_assoc();
        return $oheader;
    }

    public function GetOrder($id)
    {
        $oheader = $this->GetHeader($id);
        $oheader["dishes"] = $this->GetDishes($oheader["f_id"]);
        return $oheader;
    }

    public function QueryOrder($params)
    {
        $oheader = $this->GetHeader($params->id);
        $oheader["dishes"] = $this->GetDishes($oheader["f_id"]);
        $this->result["order"] = $oheader;
        $this->echoResult();
    }

    public function GetDishes($header)
    {
        $sql = <<<EOD
        select og.f_id, og.f_state, og.f_type, og.f_parent, og.f_emarks, og.f_data,
        og.f_row, og.f_qty, og.f_price, og.f_total, og.f_store,
        og.f_goods as f_dish, g.f_name as f_dish_name,
        COALESCE(g.f_adg, p2.f_adgcode) AS f_adgt,
        og.f_header
        from o_goods og 
        left join c_goods g on g.f_id=og.f_goods
        left join c_groups p2 on p2.f_id=g.f_group
        where og.f_header=?
        order by og.f_row
        EOD;
        return $this->select($sql, "s", [$header])->fetch_all(MYSQLI_ASSOC);
    }

    public function GetHeaderData($id)
    {
        $row = $this->select("select f_data from o_header where f_id=?", "s", [$id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Do you want to hack get header data?");
        }
        $odata = json_decode($row["f_data"] ?? "{}", true);
        return $odata;
    }

    private function TryLock($table, $locksrc)
    {
        $this->beginTransaction();
        $lc = $this->select("select f_locksrc, f_name from h_tables where f_id=? for update", "i", [$table])->fetch_assoc();
        if (!empty($locksrc) && !empty($lc["f_locksrc"]) && $lc["f_locksrc"] !== $locksrc) {
            $this->rollback();
            dieWithCode(Translator::t("Table locked by ") . $lc["f_locksrc"]);
        }
        $v["f_locksrc"] = $locksrc;
        $this->update("h_tables", $v, $table);
        $this->commit();
        return $lc;
    }

    public function OpenTable($params)
    {
        $this->TryLock($params->table, $params->locksrc);
        if (!empty($params->create_as_preorder)) {
            $row = $this->select(
                "select t.f_id as f_table, t.f_name as f_table_name, h.f_name as f_hall_name from h_tables t left join h_halls h on h.f_id=t.f_hall where t.f_id=?",
                "i",
                [$params->table]
            )->fetch_assoc();
            $this->result["order"] = [
                "f_id" => "",
                "f_state" => ORDER_STATE_PREORDER,
                "f_table" => (int)($row["f_table"] ?? $params->table),
                "f_table_name" => $row["f_table_name"] ?? "",
                "f_hall_name" => $row["f_hall_name"] ?? "",
                "f_data" => "{}",
                "dishes" => []
            ];
            $this->echoResult();
            return;
        }
        $oheader = $this->GetOpenHeader($params->table);
        if (empty($oheader["f_id"])) {
            if (!empty($params->create_empty)) {
                $oheader = $this->CreateOrder($params);
            }
        } else {
            $oheader["dishes"] = $this->GetDishes($oheader["f_id"]);
            $data = json_decode($oheader["f_data"], true);
            $this->AppendLog("opened for edit", $data);
            $this->update("o_header", ["f_data" => json_encode($data, JSON_UNESCAPED_UNICODE)], $oheader["f_id"]);
        }
        $this->result["order"] = $oheader;
        $this->echoResult();
    }

    public function SaveData($params)
    {
        if (!empty($params->create_guest)) {
            $v["f_address"] = $params->data->f_guest->f_guest_address;
            $v["f_phone"] = $params->data->f_guest->f_guest_phone;
            $v["f_taxname"] = $params->data->f_guest->f_guest_name;
            $v["f_contact"] = $params->data->f_guest->f_guest_name;
            $guest = $this->select("select * from c_partners where f_phone=?", "s", [$params->data->f_guest->f_guest_phone])->fetch_assoc();
            $notify = require_once __DIR__ . "/../../worker/ws-notify.php";
            if ($guest) {
                $params->data->f_guest->f_guest_id =  $guest["f_id"];
                $this->update("c_partners", $v, $guest["f_id"]);
                $notify->notify("partners", $params->data->f_guest->f_guest_id, false);
            } else {
                $params->data->f_guest->f_guest_id = $this->insert("c_partners", $v);
                $notify->notify("partners", $params->data->f_guest->f_guest_id, true);
            }
        }
        $row = $this->select("select f_data from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        $odata = json_decode($row["f_data"] ?? "{}", true) ?: [];
        $odata = array_merge($odata, (array) $params->data);
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function CloseOrder($params)
    {
        $payment = require_once __DIR__ . "/../worker/dict-payment.php";
        require_once __DIR__ . "/cashbox.php";
        $cc = new Cashbox();
        $cash_session = $cc->GetOpenedCashboxSessionId($params->cashbox_id ?? 0);
        if (!$cash_session) {
            dieWithCode("Cashbox session is empty");
        }
        $row = $this->select("select * from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Are you hacker of close order row?");
        }
        $odata = json_decode($row["f_data"] ?? "{}", true);
        $total = 0;
        $partnerId = $odata["f_guest"]["f_guest_id"] ?? null;
        $debtField = $payment["fields"][PAYMENT_DEBT];
        $debtAmount = (float)($odata[$debtField] ?? 0);
        if ($debtAmount > 0 && empty($partnerId)) {
            dieWithCode(Translator::t("A partner must be selected for debt payments"));
        }
        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            if ($pt == PAYMENT_DEBT) {
                $this->insert("cash_debts", [
                    "f_date"        => date("Y-m-d"),
                    "f_partner"     => $partnerId,
                    "f_doc_type"    => 2,
                    "f_doc_uuid"    => $params->id,
                    "f_credit"      => 0,
                    "f_debit"       => $debtAmount,
                    "f_currency_id" => $odata["f_currency_id"] ?? 1
                ]);
            }
            $pcash = $payment["cashbox"][$pt];
            if (!$pcash) {
                continue;
            }
            if (($odata[$pn] ?? 0) > 0) {
                $total +=  $odata[$pn];
                $this->insert("cash_operations", [
                    "f_cashbox_id" => $cash_session["f_cashbox_id"],
                    "f_session_id" => $cash_session["f_id"],
                    "f_order_id" => $params->id,
                    "f_user" => $this->userid,
                    "f_operation_type" => CASHBOX_IN,
                    "f_payment_type_id" => $pt,
                    "f_datetime" => date("Y-m-d H:i:s"),
                    "f_debit" => $odata[$pn],
                    "f_comment" => $row["f_prefix"]
                ]);
            }
        }

        $this->select("update cash_session set f_amount_expected=f_amount_expected+? where f_id=?", "di", [$total, $cash_session["f_id"]], true);

        $odata["f_date_close"] = date("Y-m-d");
        $odata["f_time_close"] = date("H:i:s");
        $odata["f_fiscal"] = $params->fiscal->out ?? [];
        $this->AppendLog("order closed", $odata);
        if (!empty($params->fiscal->in)) {
            require_once __DIR__ . "/../common/fiscal.php";
            $fiscallog = new Fiscal();
            $fiscallog->InsertLog((object) array_merge((array) $params->fiscal, ["f_id" => uuid_v4(), "f_order" => $params->id,  "err" => $params->fiscal->err ?? ""]));
        }
        $v["f_state"] = 2;
        $v["f_datecash"] = date("Y-m-d");
        $v["f_data"] = json_encode($odata, JSON_UNESCAPED_UNICODE);
        $v["f_cashier"]  = $this->userid;
        $v["f_staff"] = $this->userid;
        $v["f_cash_session_id"] = $cash_session["f_id"];
        $v["f_partner"] = $partnerId;
        $this->update("o_header", $v, $params->id);
        $this->CalculationOutput($params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->result["order"]["precheck_dishes"] = $this->GetPrecheckDishes($params->id);
        $this->echoResult();
    }

    public function ModifyOrder($params)
    {
        $payment = require_once __DIR__ . "/../worker/dict-payment.php";
        require_once __DIR__ . "/cashbox.php";
        $cc = new Cashbox();
        $cash_session = $cc->GetOpenedCashboxSessionId($params->cashbox_id ?? 0);
        if (!$cash_session) {
            dieWithCode("Cashbox session is empty");
        }
        $row = $this->select("select * from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Are you hacker of close order row?");
        }
        $odata = json_decode($row["f_data"] ?? "{}", true);
        foreach ($payment["fields"] as $ptId => $fieldName) {
            if (isset($params->$fieldName)) {
                $odata[$fieldName] = (float)$params->$fieldName ?? 0;
            }
        }
        $prefix = $row["f_prefix"];
        $total = 0;
        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            $pcash = $payment["cashbox"][$pt];
            if (!$pcash) {
                continue;
            }
            if (($odata[$pn] ?? 0) > 0) {
                $total +=  $odata[$pn];
                $this->insert("cash_operations", [
                    "f_cashbox_id" => $cash_session["f_cashbox_id"],
                    "f_session_id" => $cash_session["f_id"],
                    "f_order_id" => $params->id,
                    "f_user" => $this->userid,
                    "f_operation_type" => CASHBOX_IN,
                    "f_payment_type_id" => $pt,
                    "f_datetime" => date("Y-m-d H:i:s"),
                    "f_debit" => $odata[$pn],
                    "f_comment" => $prefix
                ]);
            }
        }

        $this->select("update cash_session set f_amount_expected=f_amount_expected+? where f_id=?", "di", [$total, $cash_session["f_id"]], true);

        $odata["f_date_close"] = date("Y-m-d");
        $odata["f_time_close"] = date("H:i:s");
        $odata["f_fiscal"] = $params->fiscal->out ?? [];
        $odata["f_amount_cash"] = $params->f_amount_cash ?? 0;
        $odata["f_amount_card"] = $params->f_amount_card ?? 0;
        $odata["f_amount_idram"] = $params->f_amount_idram ?? 0;
        $odata["f_amount_other"] = $params->f_amount_other ?? 0;
        $this->AppendLog("order closed", $odata);
        if (!empty($params->fiscal->in)) {
            require_once __DIR__ . "/../common/fiscal.php";
            $fiscallog = new Fiscal();
            $fiscallog->InsertLog((object) array_merge((array) $params->fiscal, ["f_id" => uuid_v4(), "f_order" => $params->id,  "err" => $params->fiscal->err ?? ""]));
        }
        $v["f_state"] = 2;
        $v["f_datecash"] = date("Y-m-d");
        $v["f_data"] = json_encode($odata, JSON_UNESCAPED_UNICODE);
        $v["f_cashier"]  = $this->userid;
        $v["f_staff"] = $this->userid;
        // $v["f_cash_session_id"] = $cash_session["f_id"];
        $this->update("o_header", $v, $params->id);
        $this->CalculationOutput($params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->result["order"]["precheck_dishes"] = $this->GetPrecheckDishes($params->id);

        $gp =  (object)($params->o_goods_process_status ?? ["f_status" => 0, "f_substatus" => 0]);
        if (($gp->f_status ?? 0) !== 0) {
            $this->select("update o_goods_process set f_status=? where f_header=?", "is", [$gp->f_status, $params->id], true);
        }
        $this->echoResult();
    }


    public function CalculationOutput($id)
    {
        $oheader = $this->select("select f_prefix from o_header where f_id=?", "s", [$id])->fetch_assoc();
        // 1. Очистка старой очереди
        $this->select("DELETE FROM store_calc_queue WHERE f_doc_sale_id = ?", "s", [$id], true);

        // 2. РАСКРЫТИЕ РЕЦЕПТОВ
        $sql_expand = <<<EOD
    INSERT INTO store_calc_queue (f_id, f_doc_sale_id, f_row_sale_id, f_store_id, f_item_id, f_qty, f_status, f_price)
    WITH RECURSIVE recipe_expander AS (
    -- Уровень 1: Позиции из чека
    SELECT 
        og.f_header AS doc_id,
        og.f_id AS row_id,
        g.f_storeid AS item_id,
        og.f_qty AS qty,
        (SELECT COUNT(*) FROM c_goods_complectation WHERE f_base = og.f_goods) AS has_recipe,
        CAST(IFNULL(og.f_isservice, 0) AS UNSIGNED) AS is_service,
        CAST(og.f_store AS UNSIGNED) AS fixed_store_id,
        1 as depth
    FROM o_goods og
    inner join c_goods g on g.f_id=og.f_goods
    WHERE og.f_header = ? 
      AND og.f_state = 1
      AND ((og.f_type=1 or og.f_type=2) 
           OR EXISTS(SELECT 1 FROM c_goods_complectation WHERE f_base = og.f_goods))

    UNION ALL

    -- Уровень 2+: Компоненты техкарт
    SELECT 
        re.doc_id,
        re.row_id,
        cc.f_goods AS sub_item_id,
        re.qty * cc.f_qty AS sub_qty,
        (CASE WHEN g_sub.f_component_exit = 1 
              AND EXISTS(SELECT 1 FROM c_goods_complectation WHERE f_base = g_sub.f_id) 
              THEN 1 ELSE 0 END) AS sub_has_recipe,
        0 AS is_service,
        re.fixed_store_id,
        re.depth + 1
    FROM recipe_expander re
    INNER JOIN c_goods_complectation cc ON cc.f_base = re.item_id
    INNER JOIN c_goods g_sub ON g_sub.f_id = cc.f_goods
    WHERE re.has_recipe >= 1
      AND re.depth < 10
)
-- ФИНАЛЬНЫЙ ОТБОР С ЦЕНОЙ
SELECT 
    UUID(), 
    re.doc_id, 
    re.row_id, 
    re.fixed_store_id, 
    re.item_id, 
    SUM(re.qty),
    0,
    MAX(g.f_lastinputprice) -- Берем цену из справочника товаров
FROM recipe_expander re
LEFT JOIN c_goods g ON g.f_id = re.item_id -- Джоиним для получения цены
WHERE re.has_recipe = 0 
  AND re.is_service = 0 
GROUP BY re.doc_id, re.row_id, re.fixed_store_id, re.item_id;
EOD;

        $this->select($sql_expand, "s", [$id], true);

        $doc_date = date('Y-m-d H:i:s');
        // 1. Готовим комментарий заранее
        $comment = Translator::t("Sale") . " " . ($oheader["f_prefix"] ?? '');

        $sql_json = <<<SQL
    SELECT 
        q.f_store_id,
        JSON_OBJECT(
            'doc_uuid', UUID(), 
            'doc_date', ?,
            'doc_status', 1,
            'doc_store_out', q.f_store_id,
            'doc_user_id', 1,
            'doc_version', 1,
            'doc_data', JSON_OBJECT('comment', '$comment'),
            'items', JSON_ARRAYAGG(
                JSON_OBJECT(
                    'id', UUID(),
                    'item_id', q.f_item_id,
                    'qty', q.f_qty,
                    'comment', 'sale_row'
                )
            )
        ) as params
    FROM store_calc_queue q
    WHERE q.f_doc_sale_id = ?
    GROUP BY q.f_store_id;
SQL;

        $res_obj = $this->select($sql_json, "ss", [$doc_date, $id]);
        $results = $res_obj->fetch_all(MYSQLI_ASSOC);

        if (!empty($results)) {
            foreach ($results as $row) {
                $params = $row['params'];

                // 3. Вызываем функцию. select возвращает mysqli_result.
                $func_res_obj = $this->select("SELECT sf_store2_output(?) as result", "s", [$params]);

                // Достаем одну строку
                $func_row = $func_res_obj->fetch_assoc();
                $json_res = json_decode($func_row['result'] ?? '', true);

                if (isset($json_res['status']) && $json_res['status'] == 0) {

                    $this->select(
                        "update store_calc_queue set f_status=1 WHERE f_doc_sale_id = ? AND f_store_id = ?",
                        "ss",
                        [$id, $row['f_store_id']],
                        true
                    );
                } else {
                    error_log("Discharge error for store " . ($row['f_store_id'] ?? 'unknown') . ": " . ($json_res['msg'] ?? 'Unknown error'));
                }
            }
        }
    }

    public function UnlockTable($params)
    {
        if (!empty($params->id)) {
            if ($params->empty_order) {
                $v["f_state"] = ORDER_STATE_EMPTY;
                $this->update("o_header", $v, $params->id);
            }
        }
        $this->select("update h_tables set f_locksrc=null where f_locksrc=?", "s", [$params->locksrc], true);
        if (!empty($params->id)) {
            $header = $this->GetHeader($params->id);
            $data = json_decode($header["f_data"], true);
            $this->AppendLog("table closed", $data);
            $this->update("o_header", ["f_data" => json_encode($data, JSON_UNESCAPED_UNICODE)], $params->id);
        }
        $this->echoResult();
    }

    /** Количество по строке состава пакета: «в одном пакете × число пакетов = итого». */
    private function formatServicePackageQtyLine(float $perPack, float $packCount, float $total): string
    {
        $fmt = static function ($x): string {
            $x = (float)$x;
            if (abs($x - round($x)) < 0.00001) {
                return (string)(int)round($x);
            }
            $s = number_format($x, 3, '.', '');

            return rtrim(rtrim($s, '0'), '.') ?: '0';
        };

        return $fmt($perPack) . ' х ' . $fmt($packCount) . ' = ' . $fmt($total);
    }

    /**
     * @param array<string,mixed> $row
     * @param array<int,mixed>    $log
     */
    private function appendServicePrintDish(array &$result, ?string $printerId, string $sideLabel, array $row, array &$log): void
    {
        if ($printerId === null || $printerId === '') {
            return;
        }
        if (empty($result[$printerId])) {
            $result[$printerId] = [
                'dishes' => [],
                'printer' => $printerId,
                'side' => $sideLabel,
            ];
        }
        $result[$printerId]['dishes'][] = $row;
        $qtyPart = !empty($row['f_qty_line']) ? $row['f_qty_line'] : ($row['f_qty'] ?? '');
        $log[] = $this->LogRecord(
            'printed service ' . $sideLabel . ' on ' . $printerId,
            ['comment' => ($row['f_dish_name'] ?? '') . ' (' . $qtyPart . ') ']
        );
    }

    public function PrintServiceCheck($params)
    {
        $header = $this->GetHeader($params->header_id);
        if (empty($header)) {
            dieWithCode(Translator::t("Order not exists"));
        }

        $this->beginTransaction();

        $sql = <<<EOD
    select og.f_id, og.f_type, g.f_name as f_dish_name, og.f_qty,
    coalesce(json_value(og.f_data, '$.f_printed'), false) as f_printed,
    json_value(og.f_data, '$.f_print1') as f_print1,
    json_value(og.f_data ,'$.f_print2') as f_print2,
    json_value(og.f_data, '$.f_comment') as f_comment,
    og.f_data
    from o_goods og
    left join c_goods g on g.f_id=og.f_goods
    left join o_goods pkg on pkg.f_id = og.f_parent and pkg.f_header = og.f_header and pkg.f_state = 1
    where og.f_header=?
    and og.f_state=1
    and (og.f_type = 5 or (og.f_type = 1 and not (og.f_parent is not null and og.f_parent != '' and pkg.f_type = 5)))
    order by og.f_row
    for update
    EOD;

        $print_data = $this->select($sql, "s", [$params->header_id])->fetch_all(MYSQLI_ASSOC);

        $result = [];
        $printed = [];
        $log = [];
        $reprint = !empty($params->reprint ?? false);

        foreach ($print_data as $pd) {
            // Пропускаем, если уже напечатано и это не повторная печать
            if (!empty($pd['f_printed']) && !$reprint) {
                continue;
            }

            $ftype = (int)($pd['f_type'] ?? 1);
            $idsToMarkPrinted = [$pd['f_id']];
            $children = [];
            $dataById = [$pd['f_id'] => $pd['f_data'] ?? '{}'];

            if ($ftype === 5) {
                $packageQty = (float)$pd['f_qty'];
                $this->appendServicePrintDish($result, $pd['f_print1'] ?? null, '[1]', $pd, $log);
                $this->appendServicePrintDish($result, $pd['f_print2'] ?? null, '[2]', $pd, $log);

                $sqlCh = <<<EOD
    select og.f_id, g.f_name as f_dish_name, og.f_qty,
    coalesce(json_value(og.f_data, '$.f_printed'), false) as f_printed,
    json_value(og.f_data, '$.f_print1') as f_print1,
    json_value(og.f_data, '$.f_print2') as f_print2,
    json_value(og.f_data, '$.f_comment') as f_comment,
    og.f_data
    from o_goods og
    left join c_goods g on g.f_id=og.f_goods
    where og.f_header=? and og.f_parent=? and og.f_state=1
    order by og.f_row
    EOD;
                $children = $this->select($sqlCh, 'ss', [$params->header_id, $pd['f_id']])->fetch_all(MYSQLI_ASSOC);

                foreach ($children as $ch) {
                    $idsToMarkPrinted[] = $ch['f_id'];
                    $dataById[$ch['f_id']] = $ch['f_data'] ?? '{}';
                    $perPack = (float)$ch['f_qty'];
                    $total = $perPack * $packageQty;
                    $compRow = array_merge($ch, [
                        'f_is_package_component' => true,
                        'f_qty_line' => $this->formatServicePackageQtyLine($perPack, $packageQty, $total),
                        'f_qty' => $total,
                    ]);
                    /* Вариант А: те же принтеры, что у пакета. */
                    $this->appendServicePrintDish($result, $pd['f_print1'] ?? null, '[1]', $compRow, $log);
                    $this->appendServicePrintDish($result, $pd['f_print2'] ?? null, '[2]', $compRow, $log);
                }
            } else {
                $this->appendServicePrintDish($result, $pd['f_print1'] ?? null, '[1]', $pd, $log);
                $this->appendServicePrintDish($result, $pd['f_print2'] ?? null, '[2]', $pd, $log);
            }

            foreach ($idsToMarkPrinted as $rid) {
                $raw = $dataById[$rid] ?? '{}';
                $itemData = json_decode($raw, true);
                if (!is_array($itemData)) {
                    $itemData = [];
                }
                $itemData['f_printed'] = true;
                $itemData['f_print_time'] = date('Y-m-d H:i:s');
                $this->update('o_goods', [
                    'f_data' => json_encode($itemData, JSON_UNESCAPED_UNICODE),
                ], $rid);
                $printed[] = $rid;
            }
        }

        $this->commit();

        // Заполняем результат для фронтенда
        $this->result["printed"] = $printed;
        $this->CountAmounts($params->header_id, $log);
        $this->result["reprint"] = $params->reprint ?? false;
        $this->result["header"] = $header;
        $this->result["order"] = $this->GetOrder($params->header_id);
        $this->result["print_data"] = $result;
        $this->echoResult();
    }

    public function PrintServiceOfPreorder($params)
    {
        // Keep separate endpoint for preorder workflow; currently same print logic.
        $this->PrintServiceCheck($params);
    }

    private function GetPrecheckDishes($id)
    {
        $sql = <<<EOD
        select og.f_state,og.f_parent, og.f_emarks, og.f_data,
        og.f_row, sum(og.f_qty)as f_qty, og.f_price, og.f_store,
        og.f_goods as f_dish, g.f_name as f_dish_name
        from o_goods og 
        left join c_goods g on g.f_id=og.f_goods
        where og.f_header=? AND og.f_state=1 AND LENGTH(coalesce(og.f_parent, '')) =0
        group by og.f_goods, og.f_state, og.f_price
        order by og.f_row
        EOD;
        return $this->select($sql, "s", [$id])->fetch_all(MYSQLI_ASSOC);
    }
    public function FiscalPrinted($params)
    {
        $header = $this->select("select * from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$header) {
            dieWithCode("Do you try to hack fiscal printed?");
        }
        $odata = json_decode($header["f_data"] ?? "{}", true);
        $odata["f_fiscal"] = $params->fiscal ?? [];
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $params->id);
        if (!empty($params->fiscal)) {
            $this->FiscalLog((object)[
                "in" => $params->fiscal->in,
                "out" => $params->fiscal->out,
                "error" => $params->fiscal->error,
                "result" => $params->fiscal->result,
                "id" => $params->id
            ], true);
        }
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    /** Перенос депозита в строку оплаты предоплатой; пересчёт f_amount_paid по всем полям dict-payment. */
    private function applyDepositPrepaidAndRecalcTotals(array &$data, float $totalDue): void
    {
        $deposit = (float)($data["f_deposit_prepaid"] ?? 0);
        if ($deposit > 0.00001) {
            $data["f_amount_prepaid"] = (float)($data["f_amount_prepaid"] ?? 0) + $deposit;
            $data["f_deposit_prepaid"] = 0;
        }

        $payment = require_once __DIR__ . "/../worker/dict-payment.php";
        $totalPaid = 0.0;
        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            $totalPaid += (float)($data[$pn] ?? 0);
        }

        $data["f_amount_paid"] = $totalPaid;
        $data["f_amount_change"] = ($totalPaid > $totalDue) ? ($totalPaid - $totalDue) : 0;
    }

    public function PrintPrecheck($params)
    {
        /* THIS IS TEMPORARY UNTILL FIND BUG WITHY INCORECT COUNTING */
        $this->CountAmounts($params->id);

        if (!$this->select("select f_id from o_header where f_id=?", "s", [$params->id])->fetch_assoc()) {
            dieWithCode(Translator::t("Wrong order id"));
        }
        $currentDateTime = date("Y-m-d H:i:s");
        $dishesdata = $this->select("select f_id, f_data from o_goods where f_header=? and f_state=1", "s", [$params->id])->fetch_all(MYSQLI_ASSOC);

        foreach ($dishesdata as $dd) {
            $ddata = json_decode($dd["f_data"] ?? "[]", true);

            // 1. Проверяем, что это почасовая услуга
            if ($ddata["f_hourly_payment"] ?? false) {

                // 2. Проверяем, что она ЕЩЕ НЕ остановлена (используем скобки для приоритета)
                if (!($ddata["f_stopped"] ?? false)) {

                    $ddata["f_stopped"] = true;
                    $ddata["f_play_time"] = $currentDateTime;

                    // Обновляем в базе по f_id
                    $this->update("o_goods", [
                        "f_data" => json_encode($ddata, JSON_UNESCAPED_UNICODE)
                    ], $dd["f_id"]);
                }
            }
        }

        // Не забудь после цикла вызвать пересчет всего заказа
        $this->CountAmounts($params->id);

        $row = $this->select("select f_data, f_amounttotal from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$row) {
            dieWithCode(Translator::t("Wrong order id"));
        }

        $data = json_decode($row["f_data"] ?? "{}", true) ?: [];
        $this->applyDepositPrepaidAndRecalcTotals($data, (float)($row["f_amounttotal"] ?? 0));

        $precheck = abs(($data["f_precheck"] ?? 0)) + 1;
        $printcount = ($data["f_print_count"] ?? 0) + 1;
        $data["f_precheck"] = $precheck;
        $data["f_print_count"] = $printcount;
        $this->AppendLog("print precheck", $data);
        $this->update("o_header", ["f_data" => json_encode($data, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->result["order"] = $this->GetOrder($params->id);

        $this->result["order"]["precheck_dishes"] = $this->GetPrecheckDishes($params->id);
        $this->echoResult();
    }

    public function PrintPrecheckOfPreorder($params)
    {
        // For preorder we do not mark rows as printed; allow unlimited printing.
        $this->CountAmountsOfPreorder($params->id);

        $row = $this->select("select f_data, f_amounttotal from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$row) {
            dieWithCode(Translator::t("Wrong order id"));
        }

        $data = json_decode($row["f_data"] ?? "{}", true) ?: [];
        $this->applyDepositPrepaidAndRecalcTotals($data, (float)($row["f_amounttotal"] ?? 0));

        $precheck = abs(($data["f_precheck"] ?? 0)) + 1;
        $printcount = ($data["f_print_count"] ?? 0) + 1;
        $data["f_precheck"] = $precheck;
        $data["f_print_count"] = $printcount;
        $this->AppendLog("print preorder", $data);
        $this->update("o_header", ["f_data" => json_encode($data, JSON_UNESCAPED_UNICODE)], $params->id);

        $this->result["order"] = $this->GetOrder($params->id);
        $this->result["order"]["precheck_dishes"] = $this->GetPrecheckDishes($params->id);
        $this->echoResult();
    }

    public function CancelPrecheck($params)
    {
        $row = $this->select("select f_data from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$row) {
            dieWithCode(Translator::t("Wrong order id"));
        }
        $data = json_decode($row["f_data"] ?? "{}", true);

        $precheck = ($data["f_precheck"] ?? 0) * -1;
        $data["f_precheck"] = $precheck;
        $this->AppendLog("cancel precheck", $data);
        $this->update("o_header", ["f_data" => json_encode($data, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    private function LogRecord($action, array $extra = [])
    {
        $record = array_merge(["ts" => date("Y-m-d H:i:s"), "host" => $this->remote_host, "action" => $action, "user" => $this->fullName()], $extra);
        return $record;
    }

    private function AppendLog($action, &$log, array $extra = [])
    {
        $log["log"][] = $this->LogRecord($action, $extra);
    }

    public function FiscalLog($params, $noecho = false)
    {
        $odata = json_decode($this->select("select f_data from o_header where f_id=?", "s", [$params->id])->fetch_assoc()["f_data"] ??  "{}", true);
        $this->AppendLog("fiscal fail", $odata, ["comment" => $params->error]);
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $params->id);
        $v["f_id"] = uuid_v4();
        $v["f_order"] = $params->id;
        $v["f_date"] = date("Y-m-d");
        $v["f_time"] = date("H:i:s");
        $v["f_elapsed"] = $params->elapsed ?? 0;
        $v["f_in"] = json_encode($params->in, JSON_UNESCAPED_UNICODE);
        $v["f_out"] = json_encode($params->out, JSON_UNESCAPED_UNICODE);
        $v["f_err"] = $params->error;
        $v["f_result"] = $params->result;
        $v["f_state"] = $params->result === 0 ? 1 : 0;
        $this->insert("o_tax_log", $v);
        if (!$noecho) {
            $this->echoResult();
        }
    }

    public function GetServiceValues($params)
    {
        $this->result["values"] = $this->select("select f_value, coalesce(f_comment, '') as f_name from o_service_values order by f_value ")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function ChangeServiceValue($params)
    {
        $id = trim((string)($params->id ?? ""));
        if ($id === "") {
            dieWithCode("Order id is required");
        }

        $oheader = $this->GetHeader($id);
        if (!$oheader || empty($oheader["f_id"])) {
            dieWithCode("Order not found");
        }

        $odata = json_decode($oheader["f_data"] ?? "{}", true);
        $odata["f_service_factor"] = $params->value;
        $odata["f_service_comment"] = $params->comment;
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $id);
        $dishes = $this->GetDishes($id);
        foreach ($dishes as $d) {
            $v = [];
            $ddata = json_decode($d["f_data"] ?? "{}");
            if (!$ddata) {
                $ddata = (object)[];
            }
            if ($ddata->f_complimentary ?? 0 == 1) {
                continue;
            }
            if (($ddata->f_count_service ?? false) === true) {
                $ddata->f_service_factor = $params->value;
            }

            $v["f_data"]  = json_encode($ddata, JSON_UNESCAPED_UNICODE);
            $this->update("o_goods", $v, $d["f_id"]);
        }
        $this->CountAmounts($id, $this->LogRecord("service value", ["comment" => $params->value * 100 . "%"]));
        $this->result["order"] = $this->GetOrder($id);
        $this->echoResult();
    }

    public function RemoveArrayOfDishes($params)
    {
        $dishes = [];
        foreach ($params->dishes as $d) {
            $data = (array)$d->f_data;
            if ($d->f_state == 2 || $d->f_state == 3) {
                $d->print1 = $data["f_print1"] ?? "";
                $d->print2 = $data["f_print2"] ?? "";
                $d->f_store_out = $d->f_state == 3;
                $d->f_removed_qty = $d->f_qty;
                $d->f_removed_dish_name = $d->dishName;
                if (!empty($d->remove_reason)) {
                    $data["f_remove_reason"] = $d->remove_reason;
                    $d->f_remove_reason = $d->remove_reason;
                    $data["f_removed_time"] = date("y-M-d H:i:s");
                }
            }
            $v = [];
            $v["f_qty"] = $d->f_qty;
            $v["f_state"] = $d->f_state;
            $v["f_data"] = json_encode($data, JSON_UNESCAPED_UNICODE);
            $v["f_emarks"] = null;
            $dishes[] = $v;
            $this->update("o_goods", $v, $d->f_id);
            $d->f_data = json_encode((array)$data, JSON_UNESCAPED_UNICODE);
            $dishes[] = $d;
        }
        $this->result["removed_dishes"] = $dishes;
        $this->CountAmounts($params->order_id);
        $this->result["order"] = $this->GetOrder($params->order_id);
        $this->echoResult();
    }

    public function TransferTable($params)
    {
        $dish_states = require_once __DIR__ . "/../worker/dict-dish-state.php";
        $table = $this->TryLock($params->destination, $params->locksrc);

        $odst = $this->select("select f_id, f_data from o_header where f_table=? and f_state=1", "i", [$params->destination])->fetch_assoc();
        $src_dishes = $this->GetDishes($params->id);
        if (!$src_dishes) {
            dieWithCode("Are you src dish hacker?");
        }
        if (empty($odst)) {
            /* just move */
            $odata = $this->select("select f_data from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
            $odata = json_decode($odata["f_data"] ?? "{}", true);
            $this->AppendLog("transfer table", $odata, ["comment" => $params->source_table_name . " => " . $params->destination_table_name, "important" => true]);
            $v = ["f_table" => $params->destination, "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)];
            $this->update("o_header", $v, $params->id);
        } else {
            /* merge */
            $source_data = $this->GetHeaderData($params->id);
            $this->beginTransaction();
            foreach ($src_dishes as $d) {
                if ($d["f_state"] != DISH_STATE_NORMAL) {
                    continue;
                }
                $v = [];
                $ddata = json_decode($d["f_data"] ?? "{}", true);
                $ddata["f_from_table"] = $table["f_name"];
                $this->AppendLog("transfer items", $source_data, ["comment" => $d["f_dish_name"] . " (" . $d["f_qty"] . ") " . $params->source_table_name . " => " . $params->destination_table_name, "important" => true]);
                $v["f_header"] = $odst["f_id"];
                $v["f_data"] = json_encode($ddata, JSON_UNESCAPED_UNICODE);
                $this->update("o_goods", $v, $d["f_id"]);
            }
            $this->update("o_header", ["f_data" => json_encode($source_data, JSON_UNESCAPED_UNICODE)], $params->id);
            $this->commit();
        }
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function TransferItems($params)
    {
        $odata = $this->GetHeaderData($params->id1);
        foreach ($params->data as $d) {
            $v = [];
            $v["f_header"] = $d->f_header;
            $dish = $this->select("select f_data from o_goods where f_id=?", "s", [$d->f_id])->fetch_assoc();
            $ddata = json_decode($dish["f_data"] ?? "{}", true);
            $ddata["f_from_table"] = $d->f_from_table;
            $this->AppendLog("transfer items", $odata, ["comment" => $d->f_dish_name . " (" . $d->f_qty . ") " . $params->source_table_name . " => " . $params->destination_table_name]);
            $v["f_data"] = json_encode($ddata, JSON_UNESCAPED_UNICODE);
            $this->update("o_goods", $v, $d->f_id);
        }
        $this->CountAmounts($params->id1);
        $this->CountAmounts($params->id2);
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $params->id1);
        $this->echoResult();
    }

    public function SetHeaderComment($params)
    {
        $oheader = $this->select("select f_data from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        $odata = json_decode($oheader["f_data"] ?? "{}", true);
        $odata["f_comment"] = $params->comment;
        $v["f_data"] = json_encode($odata, JSON_UNESCAPED_UNICODE);
        $this->update("o_header", $v, $params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function SetDataValue($params)
    {
        $oheader = $this->select("select f_data from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        $odata = json_decode($oheader["f_data"] ?? "{}", true) ?: [];
        $key = $params->key;
        if (property_exists($params, 'value') && $params->value === null) {
            unset($odata[$key]);
        } else {
            $val = $params->value;
            if ($key === 'f_preorder_datetime' && $val !== null && $val !== '') {
                $ts = strtotime((string)$val);
                if ($ts !== false && $ts < time()) {
                    dieWithCode(Translator::t('Preorder time cannot be in the past'));
                }
            }
            $odata[$key] = $val;
        }
        $v["f_data"] = json_encode($odata, JSON_UNESCAPED_UNICODE);
        $this->update("o_header", $v, $params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function ComplimentaryItems($params)
    {
        foreach ($params->items ?? [] as $i) {
            $row = $this->select("select f_data from o_goods where f_id=?", "s", [$i->f_id])->fetch_assoc();
            $data = json_decode($row["f_data"] ?? "{}", true);
            $data["f_complimentary"] = true;
            $v = [];
            $v["f_data"] = json_encode($data, JSON_UNESCAPED_UNICODE);
            $this->update("o_goods", $v, $i->f_id);
        }

        $this->CountAmounts($params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function Cancelation($params)
    {
        $this->select("update o_header set f_state=3 where f_id=?", "s", [$params->id], true);
        $this->select("update o_goods set f_state=3 where f_header=?", "s", [$params->id], true);
        $this->select("update o_goods_process set f_status=6 where f_header=?", "s", [$params->id], true);
        /* correct cash ops */
        $this->echoResult();
    }
}
