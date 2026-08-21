<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-07-06 13:35:02
# Last Modified: 2026-03-29 17:04:12

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/uuid.php";
require_once __DIR__ . "/../worker/dict-cash-operation-type.php";

const ORDER_STATE_OPEN = 1;
const ORDER_STATE_CLOSED = 2;
const ORDER_STATE_EMPTY = 3; /* cancel/void empty — same numeric as C++ ORDER_STATE_VOID */
const ORDER_STATE_MOVED = 6; /* align with Cafe5/c5utils.h ORDER_STATE_MOVED */
const ORDER_STATE_PREORDER = 5;

const FORMAT_DATE_TO_STR = "d/m/Y";
const FORMAT_TIME_TO_STR = "H:i";

class Order extends Auth
{
    public function GetPreorders($params)
    {
        $sql = <<<EOD
        select oh.f_id, oh.f_prefix, oh.f_hall, oh.f_table,
        oh.f_amounttotal as f_amounttotal,
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
            $row["f_amounttotal"] = (float)($row["f_amounttotal"] ?? 0);
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
        $order = $this->performReopen((string)$params->id, [
            "allow_reassign_table" => false,
            "clear_fiscal" => false,
        ]);
        $this->result["order"] = $order;
        $this->echoResult();
    }

    /**
     * Reopen a closed sale: rollback cash/store/loyalty, set f_state=1.
     * @param array $opts allow_reassign_table, preferred_table, clear_fiscal, hall_id
     */
    public function performReopen(string $orderId, array $opts = []): array
    {
        $allowReassign = !empty($opts["allow_reassign_table"]);
        $preferredTable = (int)($opts["preferred_table"] ?? 0);
        $clearFiscal = !empty($opts["clear_fiscal"]);
        $hallId = (int)($opts["hall_id"] ?? 0);

        $header = $this->select("select * from o_header where f_id=?", "s", [$orderId])->fetch_assoc();
        if (!$header) {
            dieWithCode("Do you want to try hack reopen order?");
        }
        if ((int)$header["f_state"] != ORDER_STATE_CLOSED) {
            dieWithCode("Why you want to reopen order with illegal state?");
        }

        $tableId = (int)($header["f_table"] ?? 0);
        $busy = $this->select(
            "select f_id from o_header where f_state=1 and f_table=? and f_id<>? limit 1",
            "is",
            [$tableId, $orderId]
        )->fetch_assoc();
        if ($busy) {
            if (!$allowReassign) {
                dieWithCode(Translator::t("This table busy now"));
            }
            $tableId = $this->resolveFreeTableForReopen($header, $preferredTable, $hallId);
        }

        $odata = json_decode($header["f_data"] ?? "{}", true) ?: [];
        // Partner may live only in o_header.f_partner (view-order join); keep f_guest for Shop UI.
        $guestId = (int)($odata["f_guest"]["f_guest_id"] ?? 0);
        if ($guestId <= 0) {
            $guestId = (int)($header["f_partner"] ?? 0);
        }
        if ($guestId > 0) {
            $needGuest = empty($odata["f_guest"]["f_guest_id"])
                || (empty($odata["f_guest"]["f_guest_tin"]) && empty($odata["f_guest"]["f_guest_name"]));
            if ($needGuest) {
                $p = $this->select(
                    "select f_id, f_taxcode, f_taxname, f_contact, f_phone from c_partners where f_id=?",
                    "i",
                    [$guestId]
                )->fetch_assoc();
                if ($p) {
                    $parts = array_values(array_filter([
                        trim((string)($p["f_taxname"] ?? "")),
                        trim((string)($p["f_contact"] ?? "")),
                        trim((string)($p["f_phone"] ?? "")),
                    ], static fn($s) => $s !== ""));
                    $odata["f_guest"] = [
                        "f_guest_id" => (int)$p["f_id"],
                        "f_guest_tin" => (string)($p["f_taxcode"] ?? ""),
                        "f_guest_name" => implode(", ", $parts),
                    ];
                }
            }
        }
        $this->beginTransaction();
        $rollbackAmount = 0;
        if (!empty($header["f_cash_session_id"])) {
            $sumRow = $this->select(
                "select coalesce(sum(f_debit), 0) as f_amount from cash_operations where f_order_id=? and f_session_id=? and f_operation_type=?",
                "sii",
                [$orderId, (int)$header["f_cash_session_id"], CASH_OP_SALES_REVENUE]
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
                [$orderId, (int)$header["f_cash_session_id"], CASH_OP_SALES_REVENUE],
                true
            );

            $deliveryRow = $this->select(
                "select coalesce(sum(f_credit), 0) as f_amount from cash_operations where f_order_id=? and f_session_id=? and f_operation_type=?",
                "sii",
                [$orderId, (int)$header["f_cash_session_id"], CASH_OP_DELIVERY_FEE]
            )->fetch_assoc();
            $deliveryAmount = (float)($deliveryRow["f_amount"] ?? 0);
            if ($deliveryAmount > 0) {
                $this->select(
                    "update cash_session set f_amount_expected=f_amount_expected+? where f_id=?",
                    "di",
                    [$deliveryAmount, (int)$header["f_cash_session_id"]],
                    true
                );
                $this->select(
                    "delete from cash_operations where f_order_id=? and f_session_id=? and f_operation_type=?",
                    "sii",
                    [$orderId, (int)$header["f_cash_session_id"], CASH_OP_DELIVERY_FEE],
                    true
                );
            }
        }
        $storeRollback = $this->rollbackStoreOutputForOrder($orderId, $header);
        $this->reverseLoyaltyOps($orderId);
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
        $odata["f_first_close"] = false;
        if ($clearFiscal) {
            unset($odata["f_fiscal"]);
        }
        $odata["log"][] = ["ts" => date("Y-m-d H:i:s"), "action" => "reopen", "user" => $this->fullName()];
        $odata["log"][] = ["ts" => date("Y-m-d H:i:s"), "action" => "rollback cash operations", "amount" => $rollbackAmount, "user" => $this->fullName()];
        if ($storeRollback["docs"] > 0) {
            $odata["log"][] = [
                "ts" => date("Y-m-d H:i:s"),
                "action" => "rollback store output",
                "docs" => $storeRollback["docs"],
                "user" => $this->fullName()
            ];
        }
        $upd = [
            "f_state" => ORDER_STATE_OPEN,
            "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE),
        ];
        if ($tableId > 0 && $tableId !== (int)($header["f_table"] ?? 0)) {
            $upd["f_table"] = $tableId;
            $odata["log"][] = [
                "ts" => date("Y-m-d H:i:s"),
                "action" => "reassign table",
                "from" => (int)($header["f_table"] ?? 0),
                "to" => $tableId,
                "user" => $this->fullName()
            ];
            $upd["f_data"] = json_encode($odata, JSON_UNESCAPED_UNICODE);
        }
        $this->update("o_header", $upd, $orderId);
        $this->commit();
        return $this->GetOrder($orderId);
    }

    private function resolveFreeTableForReopen(array $header, int $preferredTable, int $hallId): int
    {
        if ($hallId <= 0) {
            $hallId = (int)($header["f_hall"] ?? 0);
        }
        if ($preferredTable > 0) {
            $busy = $this->select(
                "select f_id from o_header where f_state=1 and f_table=? limit 1",
                "i",
                [$preferredTable]
            )->fetch_assoc();
            if (!$busy) {
                return $preferredTable;
            }
        }
        if ($hallId <= 0) {
            dieWithCode(Translator::t("This table busy now"));
        }
        $tables = $this->select(
            "select f_id from h_tables where f_hall=? order by f_id",
            "i",
            [$hallId]
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($tables as $t) {
            $tid = (int)$t["f_id"];
            $busy = $this->select(
                "select f_id from o_header where f_state=1 and f_table=? limit 1",
                "i",
                [$tid]
            )->fetch_assoc();
            if (!$busy) {
                return $tid;
            }
        }
        dieWithCode(Translator::t("No free tables"));
    }

    public function AddDish($params)
    {
        //first , check stoplist
        $stoplistTouched = false;
        $this->beginTransaction();
        $stoplist = $this->select("select * from c_stoplist where f_dish=? for update", "i", [$params->dish])->fetch_assoc();
        if (!empty($stoplist)) {
            if ($stoplist["f_qty"] - $params->qty < 0) {
                $this->rollback();
                dieWithCode(Translator::t("Stoplist limit reached"));
            }
            $this->select("update c_stoplist set f_qty=f_qty-? where f_dish=?", "ii", [$params->qty, $params->dish], true);
            $this->result["stoplist"] = $stoplist["f_qty"] - $params->qty;
            $stoplistTouched = true;
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
        $data = array_merge($data, (array)($params->f_data ?? []));
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
        $createProcess = $this->shouldCreateGoodsProcess($params);
        if ($createProcess) {
            $cd["f_id"] =  $this->result["focused_dish"];
            $cd["f_header"] = $oheader["f_id"];
            $cd["f_status"] = 1;
            $cd["f_daily_number"] = $odata["f_daily_number"] ?? "-";
            $ogp = ["f_status_1_1_time" => date("Y-m-d H:i:s"), "f_cooking_start" => $params->f_data?->f_cooking_start ?? date("1981-09-05 00:01:00"), "f_cooking_end" =>   $params->f_data?->f_cooking_end ?? date("Y-M-d H:i:s"), "f_substatus" => 1];
            $cd["f_data"] = json_encode($ogp, JSON_UNESCAPED_UNICODE);
            $this->insert("o_goods_process", $cd);
        }
        $this->CountAmounts($oheader["f_id"],  $this->LogRecord("add dish", ["comment" => $params->dish_name . " (" . $params->qty . ") "]));
        if ($this->shouldBlockNegativeRemains()) {
            $shortages = $this->stockShortagesForSale($oheader["f_id"]);
            if (!empty($shortages)) {
                $this->select("DELETE FROM o_goods WHERE f_id=?", "s", [$this->result["focused_dish"]], true);
                $this->select("DELETE FROM o_goods_process WHERE f_id=?", "s", [$this->result["focused_dish"]], true);
                if ($stoplistTouched) {
                    $this->select(
                        "UPDATE c_stoplist SET f_qty = f_qty + ? WHERE f_dish = ?",
                        "di",
                        [(float)$params->qty, (int)$params->dish],
                        true
                    );
                }
                $this->CountAmounts($oheader["f_id"]);
                dieWithCode(Translator::t("Insufficient stock") . ": " . implode(", ", $shortages));
            }
        }
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
        $oldQty = (float)$row["f_qty"];
        $oldState = (int)$row["f_state"];
        $oldDataJson = $row["f_data"] ?? "{}";
        $this->update("o_goods", $v, $params->id);
        $this->CountAmounts($params->order_id, $log);
        if ($this->shouldBlockNegativeRemains() && (int)$params->new_state === 1) {
            $shortages = $this->stockShortagesForSale((string)$params->order_id);
            if (!empty($shortages)) {
                $this->update("o_goods", [
                    "f_qty" => $oldQty,
                    "f_state" => $oldState,
                    "f_data" => $oldDataJson,
                ], $params->id);
                if ($stoplistqty != 0) {
                    $this->select(
                        "UPDATE c_stoplist SET f_qty = f_qty - ? WHERE f_dish = ?",
                        "di",
                        [(float)$params->restore_stoplist, (int)$params->dish],
                        true
                    );
                }
                $this->CountAmounts($params->order_id);
                dieWithCode(Translator::t("Insufficient stock") . ": " . implode(", ", $shortages));
            }
        }
        $this->result["order"] = $this->GetOrder($params->order_id);
        $this->echoResult();
    }

    public function SetDishPrice($params)
    {
        $sql = <<<EOD
        select g.f_name as f_dish_name, og.*
        from o_goods og
        left join c_goods g on g.f_id=og.f_goods
        where og.f_id=?
        EOD;
        $row = $this->select($sql, "s", [$params->id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Are you hacker of o_goods row? Code:" . $params->id);
        }
        $price = (float)($params->new_price ?? 0);
        if ($price < 0) {
            dieWithCode(Translator::t("Invalid price"));
        }
        $qty = (float)$row["f_qty"];
        $v = [
            "f_price" => $price,
            "f_total" => $price * $qty,
        ];
        $this->update("o_goods", $v, $params->id);
        $log = [$this->LogRecord("dish price", ["comment" => ($params->dish_name ?? $row["f_dish_name"]) . " = " . $price])];
        $this->CountAmounts($params->order_id, $log);
        $this->result["order"] = $this->GetOrder($params->order_id);
        $this->echoResult();
    }

    /**
     * Вернуть количество в стоп-лист при удалении строки заказа (аналогично SetDishQty).
     */
    private function restoreStoplistOnRemove(int $catalogDishId, float $qty): void
    {
        if ($qty <= 0.00001) {
            return;
        }
        $stoplist = $this->select("select * from c_stoplist where f_dish=? for update", "i", [$catalogDishId])->fetch_assoc();
        if (empty($stoplist)) {
            return;
        }
        $v = [];
        $v["f_qty"] = $stoplist["f_qty"] + $qty;
        if ($v["f_qty"] < 0) {
            $this->rollback();
            dieWithCode(Translator::t("Stoplist limit reached"));
        }
        $this->update("c_stoplist", $v, $catalogDishId, "f_dish");
    }

    /**
     * Удаление пакета (f_type=5) вместе со всеми дочерними строками (f_parent = id пакета).
     * Параметры как у set-dish-qty для строки пакета: order_id, id, dish, dish_name, new_state, new_qty,
     * data, remove_emarks, restore_stoplist, remove_reason (для new_state 2/3).
     *
     * @param object $params
     */
    public function RemovePackageWithChildren($params)
    {
        $dish_states = require_once __DIR__ . "/../worker/dict-dish-state.php";
        $orderId = (string)($params->order_id ?? "");
        $packageId = (string)($params->id ?? "");
        if ($orderId === "" || $packageId === "") {
            dieWithCode(Translator::t("Invalid request"));
        }

        $newState = (int)($params->new_state ?? 0);
        $newQtyPkg = (float)($params->new_qty ?? 0);
        $removeEmarks = (bool)($params->remove_emarks ?? false);
        $restorePkg = (float)($params->restore_stoplist ?? 0);
        $catalogDishPkg = (int)($params->dish ?? 0);
        $dishNamePkg = (string)($params->dish_name ?? "");
        $removeReason = trim((string)($params->remove_reason ?? ""));

        $this->beginTransaction();

        $sqlPkg = <<<EOD
select g.f_name as f_dish_name, og.*
from o_goods og
left join c_goods g on g.f_id=og.f_goods
where og.f_id=? and og.f_header=?
for update
EOD;
        $pkg = $this->select($sqlPkg, "ss", [$packageId, $orderId])->fetch_assoc();
        if (!$pkg) {
            $this->rollback();
            dieWithCode(Translator::t("Order line not found"));
        }
        if ((int)$pkg["f_type"] !== 5) {
            $this->rollback();
            dieWithCode(Translator::t("Not a package"));
        }
        if ((int)$pkg["f_state"] !== DISH_STATE_NORMAL) {
            $this->rollback();
            dieWithCode(Translator::t("Invalid dish state"));
        }

        $sqlCh = <<<EOD
select g.f_name as f_dish_name, og.*
from o_goods og
left join c_goods g on g.f_id=og.f_goods
where og.f_header=? and og.f_parent=? and og.f_state=1
for update
EOD;
        $children = $this->select($sqlCh, "ss", [$orderId, $packageId])->fetch_all(MYSQLI_ASSOC);

        foreach ($children as $ch) {
            $this->restoreStoplistOnRemove((int)$ch["f_goods"], (float)$ch["f_qty"]);
        }
        if ($catalogDishPkg > 0 && $restorePkg > 0.00001) {
            $this->restoreStoplistOnRemove($catalogDishPkg, $restorePkg);
        }

        $log = [];
        $removedForPrint = [];
        $isPrintedRemove = ($newState === DISH_STATE_MISTAKE || $newState === DISH_STATE_VOID);

        foreach ($children as $ch) {
            $data = json_decode($ch["f_data"] ?? "{}", true);
            if (!is_array($data)) {
                $data = [];
            }
            $rowQty = (float)$ch["f_qty"];
            $lineQty = $isPrintedRemove ? $rowQty : 0.0;
            if ($isPrintedRemove) {
                $entry = [
                    "print1" => $data["f_print1"] ?? "",
                    "print2" => $data["f_print2"] ?? "",
                    "f_store_out" => $newState === DISH_STATE_VOID,
                    "f_removed_qty" => $lineQty,
                    "f_removed_dish_name" => $ch["f_dish_name"] ?? "",
                ];
                if ($removeReason !== "") {
                    $data["f_remove_reason"] = $removeReason;
                    $data["f_removed_time"] = date("y-M-d H:i:s");
                    $entry["f_removed_comment"] = $removeReason;
                }
                $removedForPrint[] = $entry;
                $log[] = $this->LogRecord("dishes removed", [
                    "comment" => ($ch["f_dish_name"] ?? "") . " (" . $lineQty . ") " . ($dish_states["names"][$newState] ?? ""),
                    "important" => true,
                ]);
            } else {
                $log[] = $this->LogRecord("dish quantity", [
                    "comment" => ($ch["f_dish_name"] ?? "") . " (0) " . ($dish_states["names"][$newState] ?? ""),
                ]);
            }
            $v = [];
            $v["f_qty"] = $lineQty;
            $v["f_state"] = $newState;
            $v["f_data"] = json_encode($data, JSON_UNESCAPED_UNICODE);
            if ($removeEmarks) {
                $v["f_emarks"] = null;
            }
            $this->update("o_goods", $v, $ch["f_id"]);
        }

        $pkgData = json_decode($pkg["f_data"] ?? "{}", true);
        if (!is_array($pkgData)) {
            $pkgData = [];
        }
        $pkgData = array_merge((array)($params->data ?? []), $pkgData);
        $pkgLineQty = $isPrintedRemove ? $newQtyPkg : 0.0;
        if ($isPrintedRemove) {
            $entry = [
                "print1" => $pkgData["f_print1"] ?? "",
                "print2" => $pkgData["f_print2"] ?? "",
                "f_store_out" => $newState === DISH_STATE_VOID,
                "f_removed_qty" => $pkgLineQty,
                "f_removed_dish_name" => $pkg["f_dish_name"] ?? "",
            ];
            if ($removeReason !== "") {
                $pkgData["f_remove_reason"] = $removeReason;
                $pkgData["f_removed_time"] = date("y-M-d H:i:s");
                $entry["f_removed_comment"] = $removeReason;
            }
            $removedForPrint[] = $entry;
            $log[] = $this->LogRecord("dishes removed", [
                "comment" => $dishNamePkg . " (" . $pkgLineQty . ") " . ($dish_states["names"][$newState] ?? ""),
                "important" => true,
            ]);
        } else {
            $log[] = $this->LogRecord("dish quantity", [
                "comment" => $dishNamePkg . " (0) " . ($dish_states["names"][$newState] ?? ""),
            ]);
        }
        $v = [];
        $v["f_qty"] = $pkgLineQty;
        $v["f_state"] = $newState;
        $v["f_data"] = json_encode($pkgData, JSON_UNESCAPED_UNICODE);
        if ($removeEmarks) {
            $v["f_emarks"] = null;
        }
        $this->update("o_goods", $v, $packageId);

        $this->commit();

        if (!empty($removedForPrint)) {
            $this->result["removed_dishes"] = $removedForPrint;
        }
        $this->CountAmounts($orderId, $log);
        $this->result["order"] = $this->GetOrder($orderId);
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
     * Kitchen / customer-notification queue (o_goods_process).
     * Explicit create_process wins; otherwise shop without printers skips;
     * Waiter must pass create_process=true when customer_notification is on.
     */
    private function shouldCreateGoodsProcess(object $params): bool
    {
        if (isset($params->create_process)) {
            return (bool)$params->create_process;
        }
        $p1 = trim((string)($params->print1 ?? ""));
        $p2 = trim((string)($params->print2 ?? ""));
        // Shop retail without kitchen printers: skip process queue.
        if ($p1 === "" && $p2 === "" && isset($params->shop_mode) && (int)$params->shop_mode === 1) {
            return false;
        }
        // Waiter / other: omit create_process → do not enqueue (opt-in via client flag).
        if (!isset($params->shop_mode) || (int)$params->shop_mode !== 1) {
            return false;
        }
        return true;
    }

    /**
     * Одна копия напечатанного пакета: новая строка пакета + дочерние строки с теми же количествами/ценами (без f_printed).
     *
     * @param array<string,mixed> $pkg
     * @param array<int,array<string,mixed>> $children
     */
    private function duplicateOnePrintedPackageCopy(string $headerId, array $pkg, array $children, bool $createProcess = false): string
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
        if ($createProcess) {
            $this->insertOgProcessRow($headerId, $newPkgId);
        }

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
            if ($createProcess) {
                $this->insertOgProcessRow($headerId, $childId);
            }
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
        $createProcess = $this->shouldCreateGoodsProcess($params);

        try {
            for ($i = 0; $i < $copies; $i++) {
                $lastNewId = $this->duplicateOnePrintedPackageCopy($headerId, $pkg, $children, $createProcess);
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
        if (empty($hall)) {
            $this->rollback();
            dieWithCode(Translator::t("Wrong table id") . ": " . (int)($params->table ?? 0)
                . ". " . Translator::t("Set default table in shop workstation settings (f_default_table_id) and create the table in h_tables."));
        }
        $data = json_decode($hall["f_config"] ?? "{}", true);
        if (!is_array($data)) {
            $data = [];
        }
        /* Hall settings use service_factor (FrontDesk); orders store f_service_factor. */
        $hallService = (float)($data["f_service_factor"] ?? $data["service_factor"] ?? 0);
        /* Honor explicit 0 from client (empty(0) is true in PHP). */
        if (property_exists($params, "service_factor") && $params->service_factor !== null && $params->service_factor !== "") {
            $service_factor = (float)$params->service_factor;
        } else {
            $service_factor = $hallService;
        }

        $discount_factor = abs((float)($params->discount_factor ?? 0));
        $nv["f_counter"] = $hall["f_counter"];
        $this->update("h_halls", $nv, $hall["f_counterhall"]);

        /*daily counter */
        require_once __DIR__ . "/cashbox.php";
        $cc = new Cashbox();
        $cashboxId = $cc->resolveCashboxId($params);
        $cash_session = $cc->GetOpenedCashboxSessionId($cashboxId);
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
        $staffId = (int)($params->staff_id ?? 0);
        if ($staffId <= 0) {
            $staffId = (int)$this->userid;
        }
        $v["f_id"] = $id;
        $v["f_prefix"] = $hall["f_department"] . $hall["f_counter"];
        $v["f_state"] = !empty($params->create_as_preorder) ? ORDER_STATE_PREORDER : ORDER_STATE_OPEN;
        $v["f_hall"] = $hall["f_id"];
        $v["f_table"] = $params->table;
        $v["f_datecash"] = date("Y-m-d");
        $v["f_staff"] = $staffId;
        $v["f_cashier"] = $this->userid;
        $v["f_currentstaff"] = $staffId;
        $v["f_cash_session_id"] = $cash_session["f_id"];
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
        $row = $this->select("select f_data, f_state from o_header where f_id=?", "s", [$id])->fetch_assoc();
        if (!$row) {
            return;
        }
        $odata = json_decode($row["f_data"] ?? "{}", true) ?: [];
        $odata["log"] ??= [];
        if (!empty($odata["f_discount_factor"])) {
            $odata["f_discount_factor"] = abs((float)$odata["f_discount_factor"]);
        }
        $isPreorder = ((int)($row["f_state"] ?? 0) === ORDER_STATE_PREORDER);

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

            if (($ddata["f_complimentary"] ?? 0) == 1) {
                continue;
            }
            /* Open order: only kitchen-printed lines; preorder: full draft (ignore f_printed). */
            if (!$isPreorder && !($ddata["f_printed"] ?? false)) {
                continue;
            }

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
                $discountFactor = abs((float)($odata["f_discount_factor"] ?? 0));
                $ddata["f_discount_factor"] = $discountFactor;
                $priceModificator -= $discountFactor;
                $discountCounted += $d["f_price"] * $discountFactor * $d["f_qty"];
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

    /**
     * Пересчёт сумм для предзаказа: делегирует CountAmounts().
     * Для ORDER_STATE_PREORDER внутри CountAmounts в сумму входят все строки (без требования f_printed);
     * для открытого заказа — только напечатанные на кухню.
     */
    public function CountAmountsOfPreorder($id, $extraLog = [])
    {
        $this->CountAmounts($id, $extraLog);
    }

    public function SetAmount($params)
    {
        $this->assertOrderKitchenPrinted($params->id);
        $oheader = $this->GetHeader($params->id);
        $odata = json_decode($oheader["f_data"] ?? "{}", true);
        $odata[$params->payment_field] = $params->amount;
        $this->applyDepositPrepaidAndRecalcTotals($odata, (float)($oheader["f_amounttotal"] ?? 0));
        $this->update("o_header", ["f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    public function SetAmounts($params)
    {
        $this->assertOrderKitchenPrinted($params->id);
        if ($this->shouldBlockNegativeRemains()) {
            $this->assertStockAvailableForSale($params->id);
        }
        $oheader = $this->GetHeader($params->id);
        $odata = json_decode($oheader["f_data"] ?? "{}", true);

        $payment = $this->paymentDict();

        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            $odata[$pn] = $params->$pn ?? 0;
        }

        if (!empty($params->f_pinpad_response)) {
            $odata["f_pinpad_response"] = json_decode(json_encode($params->f_pinpad_response), true);
        }

        $this->applyDepositPrepaidAndRecalcTotals($odata, (float)($oheader["f_amounttotal"] ?? 0));
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
        oh.f_cashier, concat(u2.f_last, ' ', left(u2.f_first, 1), '.') as f_cashier_name,
        oh.f_cash_session_id
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
        $oheader["calc_queue"] = $this->GetCalcQueue($oheader["f_id"]);
        $this->result["order"] = $oheader;
        $this->echoResult();
    }

    public function GetCalcQueue($orderId)
    {
        $sql = <<<EOD
        SELECT
            scq.f_id,
            scq.f_row_sale_id,
            scq.f_item_id,
            scq.f_qty,
            scq.f_price,
            scq.f_status,
            scq.f_store_id,
            COALESCE(dish_g.f_name, '') AS f_dish_name,
            COALESCE(og.f_qty, 0) AS f_sold_qty,
            COALESCE(comp.f_name, '') AS f_component_name
        FROM store_calc_queue scq
        LEFT JOIN o_goods og ON og.f_id = scq.f_row_sale_id
        LEFT JOIN c_goods dish_g ON dish_g.f_id = og.f_goods
        LEFT JOIN c_goods comp ON comp.f_id = scq.f_item_id
        WHERE scq.f_doc_sale_id = ?
        ORDER BY og.f_row, dish_g.f_name, comp.f_name
        EOD;

        return $this->select($sql, "s", [$orderId])->fetch_all(MYSQLI_ASSOC);
    }

    /**
     * Edit kitchen status times (o_goods_process.f_data) and optional append/print (o_goods.f_data).
     *
     * params: id (o_goods uuid), process_times { "1": "Y-m-d H:i:s", ... }, f_append_time, f_print_time (empty = clear)
     */
    public function SetDishKitchenTimes($params)
    {
        $id = (string)($params->id ?? '');
        if ($id === '') {
            dieWithCode(Translator::t('Missing id'));
        }

        $row = $this->select(
            <<<EOD
            SELECT og.f_id, og.f_header, og.f_data, ogp.f_id AS process_id, ogp.f_data AS process_data
            FROM o_goods og
            LEFT JOIN o_goods_process ogp ON ogp.f_id = og.f_id
            WHERE og.f_id = ?
            EOD,
            's',
            [$id]
        )->fetch_assoc();

        if (!$row) {
            dieWithCode(Translator::t('Record not found'));
        }

        $headerId = (string)($row['f_header'] ?? '');
        $ogData = json_decode($row['f_data'] ?? '{}', true);
        if (!is_array($ogData)) {
            $ogData = [];
        }

        $procData = json_decode($row['process_data'] ?? '{}', true);
        if (!is_array($procData)) {
            $procData = [];
        }

        if (property_exists($params, 'process_times')) {
            $times = (array)($params->process_times ?? []);
            for ($st = 1; $st <= 4; ++$st) {
                $key = "f_status_{$st}_1_time";
                $stKey = (string)$st;
                if (!array_key_exists($stKey, $times) && !array_key_exists($st, $times)) {
                    continue;
                }
                $raw = trim((string)($times[$stKey] ?? $times[$st] ?? ''));
                if ($raw === '') {
                    unset($procData[$key]);
                    for ($ss = 2; $ss <= 4; ++$ss) {
                        unset($procData["f_status_{$st}_{$ss}_time"]);
                    }
                    continue;
                }
                $ts = strtotime($raw);
                if ($ts === false) {
                    dieWithCode(Translator::t('Invalid kitchen time for status ') . $st);
                }
                $procData[$key] = date('Y-m-d H:i:s', $ts);
            }
        }

        if (property_exists($params, 'f_append_time')) {
            $raw = trim((string)$params->f_append_time);
            if ($raw === '') {
                unset($ogData['f_append_time']);
            } else {
                $ts = strtotime($raw);
                if ($ts === false) {
                    dieWithCode(Translator::t('Invalid append time'));
                }
                $ogData['f_append_time'] = date('Y-m-d H:i:s', $ts);
            }
        }

        if (property_exists($params, 'f_print_time')) {
            $raw = trim((string)$params->f_print_time);
            if ($raw === '') {
                unset($ogData['f_print_time']);
            } else {
                $ts = strtotime($raw);
                if ($ts === false) {
                    dieWithCode(Translator::t('Invalid print time'));
                }
                $ogData['f_print_time'] = date('Y-m-d H:i:s', $ts);
            }
        }

        $this->update('o_goods', [
            'f_data' => json_encode($ogData, JSON_UNESCAPED_UNICODE),
        ], $id);

        if (empty($row['process_id'])) {
            $this->ensureGoodsProcessRows($headerId);
        }

        if (!empty($procData)) {
            $procData['f_substatus'] = (int)($procData['f_substatus'] ?? 1);
        } else {
            $procData = ['f_substatus' => 1];
        }

        $this->update('o_goods_process', [
            'f_data' => json_encode($procData, JSON_UNESCAPED_UNICODE),
        ], $id);

        if ($headerId !== '') {
            $this->result['order'] = $this->GetOrder($headerId);
        }
        $this->echoResult();
    }

    public function SetCalcQueuePrice($params)
    {
        $id = (string)($params->id ?? '');
        if ($id === '') {
            dieWithCode(Translator::t('Missing id'));
        }
        if (!property_exists($params, 'price')) {
            dieWithCode(Translator::t('Missing price'));
        }

        $row = $this->select(
            "SELECT f_id, f_doc_sale_id FROM store_calc_queue WHERE f_id = ?",
            "s",
            [$id]
        )->fetch_assoc();
        if (!$row) {
            dieWithCode(Translator::t('Record not found'));
        }

        $price = round((float)$params->price, 2);
        $this->update('store_calc_queue', ['f_price' => $price], $id);

        $orderId = (string)($row['f_doc_sale_id'] ?? '');
        if ($orderId !== '') {
            $this->result['calc_queue'] = $this->GetCalcQueue($orderId);
        }
        $this->echoResult();
    }

    /**
     * Kitchen times live in o_goods_process.f_data (not in o_goods.f_data).
     * Backfill rows for legacy lines that were added before o_goods_process existed.
     */
    private function ensureGoodsProcessRows(string $headerId): void
    {
        $sql = <<<EOD
        INSERT INTO o_goods_process (f_id, f_header, f_status, f_daily_number, f_data)
        SELECT
            og.f_id,
            og.f_header,
            1,
            COALESCE(NULLIF(JSON_UNQUOTE(JSON_EXTRACT(oh.f_data, '$.f_daily_number')), ''), '-'),
            JSON_OBJECT(
                'f_status_1_1_time',
                COALESCE(
                    NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_append_time'))), ''),
                    DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s')
                ),
                'f_substatus', 1,
                'f_cooking_start', '1981-09-05 00:01:00',
                'f_cooking_end', DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s')
            )
        FROM o_goods og
        INNER JOIN o_header oh ON oh.f_id = og.f_header
        LEFT JOIN o_goods_process ogp ON ogp.f_id = og.f_id
        WHERE og.f_header = ?
          AND og.f_state = 1
          AND ogp.f_id IS NULL
        EOD;
        $this->select($sql, 's', [$headerId], true);
    }

    public function GetDishes($header)
    {
        $this->ensureGoodsProcessRows($header);
        $sql = <<<EOD
        select og.f_id, og.f_state, og.f_type, og.f_parent, og.f_emarks, og.f_data,
        og.f_row, og.f_qty, og.f_price, og.f_total, og.f_store,
        og.f_goods as f_dish, g.f_name as f_dish_name,
        if (length(COALESCE(g.f_adg, ''))>0, g.f_adg, p2.f_adgcode) AS f_adgt,
        p2.f_taxdept as f_fiscal_department,
        og.f_header,
        ogp.f_status AS f_process_status,
        JSON_DETAILED(ogp.f_data) AS f_process_data
        from o_goods og 
        left join c_goods g on g.f_id=og.f_goods
        left join c_groups p2 on p2.f_id=g.f_group
        left join o_goods_process ogp on ogp.f_id = og.f_id
        where og.f_header=?
        order by og.f_row
        EOD;
        return $this->normalizeDishesKitchenFields(
            $this->select($sql, "s", [$header])->fetch_all(MYSQLI_ASSOC)
        );
    }

    /**
     * Kitchen queue data lives in o_goods_process (not o_goods.f_data / o_header.f_data).
     *
     * @param array<int,array<string,mixed>> $rows
     * @return array<int,array<string,mixed>>
     */
    private function normalizeDishesKitchenFields(array $rows): array
    {
        foreach ($rows as &$row) {
            $st = $row['f_process_status'] ?? null;
            $row['f_process_status'] = ($st === null || $st === '') ? 0 : (int)$st;

            $raw = $row['f_process_data'] ?? null;
            if ($raw === null || $raw === '') {
                $row['f_process_data'] = [];
                continue;
            }

            if (is_string($raw)) {
                $decoded = json_decode($raw, true);
                $row['f_process_data'] = is_array($decoded) ? $decoded : [];
            } elseif (is_array($raw)) {
                $row['f_process_data'] = $raw;
            } else {
                $row['f_process_data'] = [];
            }
        }
        unset($row);

        return $rows;
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
            } else {
                if (empty($oheader)) {
                    $oheader = $this->select(
                        "select t.f_id as f_table, t.f_name as f_table_name, h.f_name as f_hall_name "
                            . "from h_tables t left join h_halls h on h.f_id=t.f_hall where t.f_id=?",
                        "i",
                        [$params->table]
                    )->fetch_assoc() ?: [];
                }
                $oheader["f_id"] = "";
                $oheader["f_state"] = ORDER_STATE_OPEN;
                $oheader["f_amounttotal"] = 0;
                $oheader["f_data"] = "{}";
                $oheader["dishes"] = [];
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

    /**
     * Record the delivery courier fee as a cash-out tied to the order.
     * Idempotent: re-closing the same order rolls back the previous delivery operation first,
     * so the cash drawer expected amount stays correct. Linked by f_order_id so order removal cleans it up.
     */
    private function applyDeliveryCashout($orderId, $cashSessionId, $cashboxId, array $odata, string $prefix)
    {
        $cashSessionId = (int)$cashSessionId;
        $deliveryAmount = (float)($odata["f_delivery_amount"] ?? 0);

        $prev = $this->select(
            "select coalesce(sum(f_credit), 0) as f_amount from cash_operations where f_order_id=? and f_operation_type=?",
            "si",
            [$orderId, CASH_OP_DELIVERY_FEE]
        )->fetch_assoc();
        $prevAmount = (float)($prev["f_amount"] ?? 0);
        if ($prevAmount > 0.00001) {
            $this->select("update cash_session set f_amount_expected=f_amount_expected+? where f_id=?", "di", [$prevAmount, $cashSessionId], true);
        }
        $this->select("delete from cash_operations where f_order_id=? and f_operation_type=?", "si", [$orderId, CASH_OP_DELIVERY_FEE], true);

        if ($deliveryAmount <= 0.00001) {
            return;
        }

        $this->insert("cash_operations", [
            "f_cashbox_id" => $cashboxId,
            "f_session_id" => $cashSessionId,
            "f_order_id" => $orderId,
            "f_user" => $this->userid,
            "f_operation_type" => CASH_OP_DELIVERY_FEE,
            "f_payment_type_id" => PAYMENT_TYPE_CASH,
            "f_datetime" => date("Y-m-d H:i:s"),
            "f_credit" => $deliveryAmount,
            "f_comment" => str_replace("%ordernumber", $prefix, Translator::t("Delivery for %ordernumber")),
        ]);
        $this->select("update cash_session set f_amount_expected=f_amount_expected-? where f_id=?", "di", [$deliveryAmount, $cashSessionId], true);
    }

    public function CloseOrder($params)
    {
        $this->assertOrderKitchenPrinted($params->id);
        $row = $this->select("select * from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (!$row) {
            dieWithCode("Are you hacker of close order row?");
        }
        // Block closing when warehouse lines cannot be covered by stock (only if configured).
        if ($this->shouldBlockNegativeRemains()) {
            $this->assertStockAvailableForSale($params->id);
        }
        $odata = json_decode($row["f_data"] ?? "{}", true);
        $odata["f_first_close"] = !isset($odata["f_first_close"]);
        $payment = $this->paymentDict();
        require_once __DIR__ . "/cashbox.php";
        $cc = new Cashbox();
        $cashboxId = $cc->resolveCashboxId($params);
        $cash_session_id = (int)($params->cash_session_id ?? 0);
        $cash_session_row = $cash_session_id > 0 ? $cc->GetRawCashboxSession($cash_session_id) : null;
        if (!$cash_session_row || (int)$cash_session_row["f_state"] !== 1
            || (int)$cash_session_row["f_cashbox_id"] !== $cashboxId) {
            $opened = $cc->GetOpenedCashboxSessionId($cashboxId);
            if (!$opened) {
                dieWithCode(Translator::t("No active cashbox session"));
            }
            $cash_session_id = (int)$opened["f_id"];
            $cash_session_row = $cc->GetRawCashboxSession($cash_session_id);
            if (!$cash_session_row || (int)$cash_session_row["f_state"] !== 1) {
                dieWithCode("Cashbox session is not open");
            }
        }

        $totalDue = (float)($row["f_amounttotal"] ?? 0);
        $this->applyDepositPrepaidAndRecalcTotals($odata, $totalDue);
        $paymentDebits = $this->cashboxPaymentDebits($odata);

        $total = 0;
        $partnerId = $odata["f_guest"]["f_guest_id"] ?? null;
        $debtField = $payment["fields"][PAYMENT_DEBT];
        $debtAmount = (float)($odata[$debtField] ?? 0);

        $bankField = $payment["fields"][PAYMENT_TYPE_BANK];
        $bankAmount = (float)($odata[$bankField] ?? 0);

        // Both "Debt" and "Bank transfer" are deferred customer settlements,
        // so we must know the partner to put the record into cash_debts.
        if (($debtAmount > 0 || $bankAmount > 0) && empty($partnerId)) {
            dieWithCode(Translator::t("A partner must be selected for deferred payments"));
        }
        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            if ($pt == PAYMENT_DEBT) {
                // Insert debt row only when the debt amount is actually > 0.
                // Otherwise we end up with empty debt operations (e.g. "0 dram").
                if ($debtAmount > 0.00001) {
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
            } elseif ($pt == PAYMENT_TYPE_BANK) {
                // Bank transfer is also treated as deferred settlement.
                // Store it the same way as "Debt" in cash_debts, so it counts as customer debt.
                if ($bankAmount > 0.00001) {
                    $this->insert("cash_debts", [
                        "f_date"        => date("Y-m-d"),
                        "f_partner"     => $partnerId,
                        "f_doc_type"    => 2,
                        "f_doc_uuid"    => $params->id,
                        "f_credit"      => 0,
                        "f_debit"       => $bankAmount,
                        "f_currency_id" => $odata["f_currency_id"] ?? 1
                    ]);
                }
            }
            $pcash = $payment["cashbox"][$pt];
            if (!$pcash) {
                continue;
            }
            $debit = (float)($paymentDebits[$pn] ?? 0);
            if ($debit > 0.00001) {
                $total += $debit;
                $this->insert("cash_operations", [
                    "f_cashbox_id" => $params->cashbox_id,
                    "f_session_id" => $cash_session_id,
                    "f_order_id" => $params->id,
                    "f_user" => $this->userid,
                    "f_operation_type" => CASH_OP_SALES_REVENUE,
                    "f_payment_type_id" => $pt,
                    "f_datetime" => date("Y-m-d H:i:s"),
                    "f_debit" => $debit,
                    "f_comment" => $row["f_prefix"]
                ]);
            }
        }

        $this->select("update cash_session set f_amount_expected=f_amount_expected+? where f_id=?", "di", [$total, $cash_session_id], true);
        $this->applyDeliveryCashout($params->id, $cash_session_id, $params->cashbox_id ?? null, $odata, (string)$row["f_prefix"]);
        $this->writeLoyaltyOps($params->id, $params->loyalty ?? null);
        $odata["f_date_close"] = date("Y-m-d");
        $odata["f_time_close"] = date("H:i:s");
        $odata["f_fiscal"] = $params->fiscal->out ?? [];
        $this->AppendLog("order closed", $odata);
        if (!empty($params->fiscal->in)) {
            require_once __DIR__ . "/../common/fiscal.php";
            $fiscallog = new Fiscal();
            $fiscallog->InsertLog((object) array_merge((array) $params->fiscal, ["f_id" => uuid_v4(), "f_order" => $params->id,  "err" => $params->fiscal->err ?? ""]));
        }
        $staffId = (int)($params->staff_id ?? 0);
        if ($staffId <= 0) {
            $staffId = (int)($row["f_staff"] ?? 0);
        }
        if ($staffId <= 0) {
            $staffId = (int)$this->userid;
        }
        $v["f_state"] = 2;
        $v["f_datecash"] = date("Y-m-d");
        $v["f_timeclose"] = $odata["f_time_close"];
        $v["f_data"] = json_encode($odata, JSON_UNESCAPED_UNICODE);
        $v["f_cashier"]  = $this->userid;
        $v["f_staff"] = $staffId;
        $v["f_cash_session_id"] = $cash_session_id;
        $v["f_partner"] = $partnerId;
        $this->update("o_header", $v, $params->id);
        $this->CalculationOutput($params->id, $this->resolveCostDependOnServiceAndDiscount($params));
        $this->result["order"] = $this->GetOrder($params->id);
        $this->result["order"]["precheck_dishes"] = $this->GetPrecheckDishes($params->id);
        $this->echoResult();
    }

    /**
     * Записывает операции лояльности заказа в отдельные журналы (новый контур).
     * Все движения по картам создаются только при закрытии заказа и привязаны к f_order_id.
     */
    private function writeLoyaltyOps($orderId, $loyalty): void
    {
        if (empty($loyalty)) {
            return;
        }
        $loyalty = json_decode(json_encode($loyalty), true);
        if (!is_array($loyalty)) {
            return;
        }

        /* idempotency: не дублировать при повторном закрытии */
        $this->select("delete from b_discount_ops where f_order_id=?", "s", [$orderId], true);
        $this->select("delete from b_gift_card_ops where f_order_id=?", "s", [$orderId], true);
        $this->select("delete from b_accumulate_ops where f_order_id=?", "s", [$orderId], true);

        if (!empty($loyalty["discount"])) {
            $d = $loyalty["discount"];
            $this->insert("b_discount_ops", [
                "f_id"         => uuid_v4(),
                "f_order_id"   => $orderId,
                "f_card_id"    => !empty($d["card_id"]) ? (int)$d["card_id"] : null,
                "f_partner_id" => !empty($d["partner_id"]) ? (int)$d["partner_id"] : null,
                "f_type"       => (int)($d["type"] ?? 0),
                "f_factor"     => (float)($d["factor"] ?? 0),
                "f_amount"     => (float)($d["amount"] ?? 0),
                "f_comment"    => (string)($d["comment"] ?? ""),
            ]);
        }

        if (!empty($loyalty["gift"]) && !empty($loyalty["gift"]["card_id"])) {
            $g = $loyalty["gift"];
            $spend = (float)($g["spend"] ?? 0);
            if ($spend > 0.00001) {
                $this->insert("b_gift_card_ops", [
                    "f_id"       => uuid_v4(),
                    "f_card_id"  => (int)$g["card_id"],
                    "f_order_id" => $orderId,
                    "f_amount"   => -1 * $spend,
                    "f_op_type"  => "spend",
                    "f_comment"  => "",
                ]);
            }
        }

        if (!empty($loyalty["accumulate"]) && !empty($loyalty["accumulate"]["card_id"])) {
            $a = $loyalty["accumulate"];
            $cardId = (int)$a["card_id"];
            $percent = (float)($a["percent"] ?? 0);
            $spend = (float)($a["spend"] ?? 0);
            $earn = (float)($a["earn"] ?? 0);
            if ($spend > 0.00001) {
                $this->insert("b_accumulate_ops", [
                    "f_id"       => uuid_v4(),
                    "f_card_id"  => $cardId,
                    "f_order_id" => $orderId,
                    "f_amount"   => -1 * $spend,
                    "f_op_type"  => "spend",
                    "f_percent"  => $percent,
                    "f_comment"  => "",
                ]);
            }
            if ($earn > 0.00001) {
                $this->insert("b_accumulate_ops", [
                    "f_id"       => uuid_v4(),
                    "f_card_id"  => $cardId,
                    "f_order_id" => $orderId,
                    "f_amount"   => $earn,
                    "f_op_type"  => "earn",
                    "f_percent"  => $percent,
                    "f_comment"  => "",
                ]);
            }
        }
    }

    /** Сторнирует операции лояльности заказа (используется при откате/удалении документа). */
    private function reverseLoyaltyOps($orderId): void
    {
        $this->select("delete from b_discount_ops where f_order_id=?", "s", [$orderId], true);
        $this->select("delete from b_gift_card_ops where f_order_id=?", "s", [$orderId], true);
        $this->select("delete from b_accumulate_ops where f_order_id=?", "s", [$orderId], true);
    }

    public function ModifyOrder($params)
    {
        $this->assertOrderKitchenPrinted($params->id);
        $payment = $this->paymentDict();
        require_once __DIR__ . "/cashbox.php";
        $cc = new Cashbox();
        $cashboxId = $cc->resolveCashboxId($params);
        $cash_session = $cc->GetOpenedCashboxSessionId($cashboxId);
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
        $totalDue = (float)($row["f_amounttotal"] ?? 0);
        $this->applyDepositPrepaidAndRecalcTotals($odata, $totalDue);
        $paymentDebits = $this->cashboxPaymentDebits($odata);

        $total = 0;
        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            $pcash = $payment["cashbox"][$pt];
            if (!$pcash) {
                continue;
            }
            $debit = (float)($paymentDebits[$pn] ?? 0);
            if ($debit > 0.00001) {
                $total += $debit;
                $this->insert("cash_operations", [
                    "f_cashbox_id" => $cash_session["f_cashbox_id"],
                    "f_session_id" => $cash_session["f_id"],
                    "f_order_id" => $params->id,
                    "f_user" => $this->userid,
                    "f_operation_type" => CASH_OP_SALES_REVENUE,
                    "f_payment_type_id" => $pt,
                    "f_datetime" => date("Y-m-d H:i:s"),
                    "f_debit" => $debit,
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
        $staffId = (int)($params->staff_id ?? 0);
        if ($staffId <= 0) {
            $staffId = (int)($row["f_staff"] ?? 0);
        }
        if ($staffId <= 0) {
            $staffId = (int)$this->userid;
        }
        $v["f_state"] = 2;
        $v["f_datecash"] = date("Y-m-d");
        $v["f_timeclose"] = $odata["f_time_close"];
        $v["f_data"] = json_encode($odata, JSON_UNESCAPED_UNICODE);
        $v["f_cashier"]  = $this->userid;
        $v["f_staff"] = $staffId;
        // $v["f_cash_session_id"] = $cash_session["f_id"];
        $this->update("o_header", $v, $params->id);
        $this->CalculationOutput($params->id, $this->resolveCostDependOnServiceAndDiscount($params));
        $this->result["order"] = $this->GetOrder($params->id);
        $this->result["order"]["precheck_dishes"] = $this->GetPrecheckDishes($params->id);

        $gp = (object)($params->o_goods_process_status ?? ["f_status" => 0, "f_substatus" => 0]);
        $closeKitchenStatus = (int)($gp->f_status ?? 0);
        if ($closeKitchenStatus >= 1 && $closeKitchenStatus <= 4) {
            $ss = max(1, (int)($gp->f_substatus ?? 1));
            $jsonTimePath = sprintf('$.f_status_%d_%d_time', $closeKitchenStatus, $ss);
            $sql = <<<EOD
            UPDATE o_goods_process
            SET f_status = ?,
            f_data = JSON_SET(
                COALESCE(f_data, '{}'),
                '{$jsonTimePath}',
                COALESCE(
                    NULLIF(JSON_UNQUOTE(JSON_EXTRACT(f_data, '{$jsonTimePath}')), ''),
                    DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s')
                ),
                '$.f_substatus', ?
            )
            WHERE f_header = ?
            EOD;
            $this->select($sql, 'iis', [$closeKitchenStatus, $ss, $params->id], true);
        }
        $this->echoResult();
    }


    /**
     * Common workstation (type 5): dont_allow_negative_remains.
     * Default false = allow stock to go negative on sale.
     */
    private function shouldBlockNegativeRemains(): bool
    {
        $row = $this->select(
            "SELECT f_config FROM workstations WHERE f_type = 5 ORDER BY f_id ASC LIMIT 1"
        )->fetch_assoc();
        if (!$row) {
            return false;
        }
        $cfg = json_decode($row["f_config"] ?? "{}", true);
        if (!is_array($cfg)) {
            return false;
        }
        return !empty($cfg["dont_allow_negative_remains"]);
    }

    /**
     * @return list<string> human-readable shortage lines; empty if OK
     */
    private function stockShortagesForSale(string $orderId): array
    {
        $sql = <<<EOD
WITH RECURSIVE recipe_expander AS (
    SELECT
        g.f_storeid AS item_id,
        og.f_qty AS qty,
        (SELECT COUNT(*) FROM c_goods_complectation WHERE f_base = og.f_goods) AS has_recipe,
        CAST(IFNULL(og.f_isservice, 0) AS UNSIGNED) AS is_service,
        CAST(og.f_store AS UNSIGNED) AS fixed_store_id,
        1 AS depth
    FROM o_goods og
    INNER JOIN c_goods g ON g.f_id = og.f_goods
    WHERE og.f_header = ?
      AND og.f_state = 1
      AND ((og.f_type = 1 OR og.f_type = 2)
           OR EXISTS(SELECT 1 FROM c_goods_complectation WHERE f_base = og.f_goods))

    UNION ALL

    SELECT
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
),
need AS (
    SELECT
        re.fixed_store_id AS f_store_id,
        re.item_id AS f_item_id,
        SUM(re.qty) AS f_need
    FROM recipe_expander re
    WHERE re.has_recipe = 0
      AND re.is_service = 0
      AND re.fixed_store_id > 0
    GROUP BY re.fixed_store_id, re.item_id
)
SELECT
    need.f_store_id,
    need.f_item_id,
    g.f_name,
    need.f_need,
    COALESCE(SUM(ss.f_qty_left), 0) AS f_have
FROM need
LEFT JOIN c_goods g ON g.f_id = need.f_item_id
LEFT JOIN store_stock ss ON ss.f_store_id = need.f_store_id AND ss.f_item_id = need.f_item_id
GROUP BY need.f_store_id, need.f_item_id, g.f_name, need.f_need
HAVING need.f_need > COALESCE(SUM(ss.f_qty_left), 0) + 0.0001
EOD;

        $rows = $this->select($sql, "s", [$orderId])->fetch_all(MYSQLI_ASSOC);
        if (empty($rows)) {
            return [];
        }

        $parts = [];
        foreach ($rows as $r) {
            $name = trim((string)($r["f_name"] ?? ""));
            if ($name === "") {
                $name = "#" . (int)$r["f_item_id"];
            }
            $have = (float)$r["f_have"];
            $need = (float)$r["f_need"];
            $parts[] = $name . " ("
                . rtrim(rtrim(number_format($have, 3, ".", ""), "0"), ".")
                . " / "
                . rtrim(rtrim(number_format($need, 3, ".", ""), "0"), ".")
                . ")";
        }
        return $parts;
    }

    /**
     * Refuse to close/change a sale when any warehouse line needs more qty than available on stock.
     * Same recipe expansion as CalculationOutput; lines with store_id=0 are skipped (no WH).
     */
    private function assertStockAvailableForSale(string $orderId): void
    {
        $parts = $this->stockShortagesForSale($orderId);
        if (empty($parts)) {
            return;
        }
        dieWithCode(Translator::t("Insufficient stock") . ": " . implode(", ", $parts));
    }

    public function CalculationOutput($id, $costDependOnServiceAndDiscount = false)
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

        if ($costDependOnServiceAndDiscount) {
            $this->applyCostServiceAndDiscountToQueue($id);
        }

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
                    $paramsDecoded = json_decode($params, true);
                    $docUuid = $paramsDecoded['doc_uuid'] ?? null;

                    $this->select(
                        "update store_calc_queue set f_status=1, f_doc_store_out_id=? WHERE f_doc_sale_id = ? AND f_store_id = ?",
                        "sss",
                        [$docUuid, $id, $row['f_store_id']],
                        true
                    );
                } else {
                    $msg = $json_res['msg'] ?? 'Unknown error';
                    error_log("Discharge error for store " . ($row['f_store_id'] ?? 'unknown') . ": " . $msg);
                    dieWithCode(Translator::t("Insufficient stock") . " (" . $msg . ")");
                }
            }
        }
    }

    /** Cancel posted store write-offs, rebuild queue from recipes, post store output again. */
    public function RecalculateStoreOutput($id, $costDependOnServiceAndDiscount = false)
    {
        $header = $this->select(
            "SELECT f_id, f_prefix, f_datecash, f_state FROM o_header WHERE f_id = ? LIMIT 1",
            "s",
            [$id]
        )->fetch_assoc();
        if (empty($header)) {
            dieWithCode(Translator::t("Order not found"));
        }
        if ((int)($header["f_state"] ?? 0) !== ORDER_STATE_CLOSED) {
            dieWithCode(Translator::t("Order is not closed"));
        }

        $this->rollbackStoreOutputForOrder($id, $header);
        $this->CalculationOutput($id, $costDependOnServiceAndDiscount);
        $this->result["recalculated"] = $id;
        return $this->result;
    }

    /** Cancel posted store write-offs for a closed sale (reopen order). */
    private function rollbackStoreOutputForOrder(string $orderId, array $header): array
    {
        $docRows = $this->select(
            "SELECT DISTINCT f_doc_store_out_id AS doc_id
             FROM store_calc_queue
             WHERE f_doc_sale_id = ? AND f_status = 1
               AND f_doc_store_out_id IS NOT NULL AND f_doc_store_out_id <> ''",
            "s",
            [$orderId]
        )->fetch_all(MYSQLI_ASSOC);

        if (empty($docRows)) {
            $saleComment = Translator::t("Sale") . " " . ($header["f_prefix"] ?? "");
            $docRows = $this->select(
                "SELECT DISTINCT sd.f_id AS doc_id
                 FROM store_document sd
                 INNER JOIN (
                     SELECT DISTINCT f_store_id
                     FROM store_calc_queue
                     WHERE f_doc_sale_id = ? AND f_status = 1
                 ) q ON q.f_store_id = sd.f_store_out
                 WHERE sd.f_doc_type = 2
                   AND sd.f_status = 1
                   AND JSON_UNQUOTE(JSON_EXTRACT(sd.f_data, '$.comment')) = ?
                   AND DATE(sd.f_doc_date) = ?",
                "sss",
                [$orderId, $saleComment, $header["f_datecash"] ?? date("Y-m-d")]
            )->fetch_all(MYSQLI_ASSOC);

            if (empty($docRows)) {
                $docRows = $this->select(
                    "SELECT DISTINCT sd.f_id AS doc_id
                     FROM store_document sd
                     WHERE sd.f_doc_type = 2
                       AND sd.f_status = 1
                       AND JSON_UNQUOTE(JSON_EXTRACT(sd.f_data, '$.comment')) = ?
                       AND DATE(sd.f_doc_date) = ?",
                    "ss",
                    [$saleComment, $header["f_datecash"] ?? date("Y-m-d")]
                )->fetch_all(MYSQLI_ASSOC);
            }
        }

        $rolledBack = 0;
        foreach ($docRows as $docRow) {
            $docId = $docRow["doc_id"] ?? "";
            if ($docId === "") {
                continue;
            }

            $res = $this->select("SELECT sf_store2_output_delete(?) AS result", "s", [$docId]);
            $row = $res->fetch_assoc();
            $result = json_decode($row["result"] ?? "", true);
            if (($result["status"] ?? 1) > 0) {
                $this->rollback();
                dieWithCode($result["msg"] ?? Translator::t("Store rollback failed"));
            }
            $rolledBack++;
        }

        if ($rolledBack > 0) {
            $this->select(
                "UPDATE store_calc_queue SET f_status = 0, f_doc_store_out_id = NULL WHERE f_doc_sale_id = ? AND f_status = 1",
                "s",
                [$orderId],
                true
            );
        }

        return ["docs" => $rolledBack];
    }

    private function resolveCostDependOnServiceAndDiscount($params): bool
    {
        if (!isset($params->cost_depend_on_service_and_discount)) {
            return false;
        }

        $v = $params->cost_depend_on_service_and_discount;

        return $v === true || $v === 1 || $v === '1' || $v === 'true';
    }

    /**
     * Same price modificator as CountAmounts: service + discount on dish lines that count them.
     */
    private function costPriceModificatorForDish(array $ddata, array $odata): float
    {
        $modificator = 0.0;

        if (!empty($ddata['f_count_service'])) {
            $modificator += (float)($odata['f_service_factor'] ?? 0);
        }

        if (!empty($ddata['f_count_discount'])) {
            $modificator -= abs((float)($odata['f_discount_factor'] ?? 0));
        }

        return $modificator;
    }

    /** Apply order service/discount factors to store_calc_queue.f_price (COGS). */
    private function applyCostServiceAndDiscountToQueue(string $orderId): void
    {
        $header = $this->select("select f_data from o_header where f_id=?", "s", [$orderId])->fetch_assoc();
        if (!$header) {
            return;
        }

        $odata = json_decode($header['f_data'] ?? '{}', true);
        if (!is_array($odata)) {
            $odata = [];
        }

        $sql = <<<EOD
        SELECT scq.f_id, scq.f_price, og.f_data
        FROM store_calc_queue scq
        INNER JOIN o_goods og ON og.f_id = scq.f_row_sale_id
        WHERE scq.f_doc_sale_id = ?
        EOD;

        $rows = $this->select($sql, "s", [$orderId])->fetch_all(MYSQLI_ASSOC);

        foreach ($rows as $row) {
            $ddata = json_decode($row['f_data'] ?? '{}', true);
            if (!is_array($ddata)) {
                $ddata = [];
            }

            $modificator = $this->costPriceModificatorForDish($ddata, $odata);
            if (abs($modificator) < 1e-9) {
                continue;
            }

            $basePrice = (float)($row['f_price'] ?? 0);
            $newPrice = round($basePrice * (1 + $modificator), 2);
            $this->update('store_calc_queue', ['f_price' => $newPrice], $row['f_id']);
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

    /**
     * Service check print: same payload as PrintServiceCheck, optionally without persisting
     * f_printed / f_print_time on lines (preorder — unlimited reprints).
     *
     * @param object $params must include header_id
     */
    private function printServiceCheckCore($params, bool $persistPrinted): void
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
    and (og.f_type = 5 or ((og.f_type = 1 or og.f_type = 2) and not (og.f_parent is not null and og.f_parent != '' and pkg.f_type = 5)))
    order by og.f_row
    for update
    EOD;

        $print_data = $this->select($sql, "s", [$params->header_id])->fetch_all(MYSQLI_ASSOC);

        $result = [];
        $printed = [];
        $log = [];
        $reprint = !empty($params->reprint ?? false);

        foreach ($print_data as $pd) {
            if ($persistPrinted && !empty($pd['f_printed']) && !$reprint) {
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
                if ($persistPrinted) {
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
                }
                $printed[] = $rid;
            }
        }

        $this->commit();

        $this->result["printed"] = $printed;
        $this->CountAmounts($params->header_id, $log);
        $this->result["reprint"] = $params->reprint ?? false;
        $this->result["header"] = $header;
        $this->result["order"] = $this->GetOrder($params->header_id);
        $this->result["print_data"] = $result;
        $this->echoResult();
    }

    public function PrintServiceCheck($params)
    {
        $this->printServiceCheckCore($params, true);
    }

    public function PrintServiceOfPreorder($params)
    {
        $header = $this->GetHeader($params->header_id);
        if (empty($header)) {
            dieWithCode(Translator::t("Order not exists"));
        }
        if ((int)$header["f_state"] !== ORDER_STATE_PREORDER) {
            dieWithCode(Translator::t("Not a preorder"));
        }
        $this->printServiceCheckCore($params, false);
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
                "error" => $params->fiscal->error ?? "",
                "result" => $params->fiscal->result,
                "id" => $params->id,
                "f_fiscal_machine_id" => $params->fiscal->f_fiscal_machine_id
                    ?? $params->f_fiscal_machine_id
                    ?? 0,
            ], true);
        }
        $this->result["order"] = $this->GetOrder($params->id);
        $this->echoResult();
    }

    /** Open order: all goods/dish/package lines must be printed on kitchen before precheck or payment. */
    private function assertOrderKitchenPrinted(string $orderId): void
    {
        $header = $this->select("select f_state from o_header where f_id=?", "s", [$orderId])->fetch_assoc();
        if (!$header) {
            dieWithCode(Translator::t("Wrong order id"));
        }
        if ((int)($header["f_state"] ?? 0) === ORDER_STATE_PREORDER) {
            return;
        }

        $rows = $this->select(
            "select f_data from o_goods where f_header=? and f_state=1 and f_type in (1, 2, 5)",
            "s",
            [$orderId]
        )->fetch_all(MYSQLI_ASSOC);

        foreach ($rows as $row) {
            $ddata = json_decode($row["f_data"] ?? "{}", true) ?: [];
            $print1 = trim((string)($ddata["f_print1"] ?? ""));
            $print2 = trim((string)($ddata["f_print2"] ?? ""));
            if ($print1 === "" && $print2 === "") {
                continue;
            }
            if (!($ddata["f_printed"] ?? false)) {
                dieWithCode(Translator::t("Print service check before payment or precheck"));
            }
        }
    }

    /** @return array Payment dictionary from dict-payment.php (cached; safe for repeated calls). */
    private function paymentDict(): array
    {
        static $payment = null;
        if (!is_array($payment)) {
            $payment = require __DIR__ . "/../worker/dict-payment.php";
        }
        return $payment;
    }

    /** Перенос депозита в строку оплаты предоплатой; пересчёт f_amount_paid по всем полям dict-payment. */
    private function applyDepositPrepaidAndRecalcTotals(array &$data, float $totalDue): void
    {
        $deposit = (float)($data["f_deposit_prepaid"] ?? 0);
        if ($deposit > 0.00001) {
            $data["f_amount_prepaid"] = (float)($data["f_amount_prepaid"] ?? 0) + $deposit;
            $data["f_deposit_prepaid"] = 0;
        }

        $payment = $this->paymentDict();
        $totalPaid = 0.0;
        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            $totalPaid += (float)($data[$pn] ?? 0);
        }

        $data["f_amount_paid"] = $totalPaid;
        $data["f_amount_change"] = ($totalPaid > $totalDue) ? ($totalPaid - $totalDue) : 0;
    }

    /**
     * Sums to post into cashbox per payment field (tender minus change; change is taken from cash first).
     *
     * @return array<string, float> field name => net debit
     */
    private function cashboxPaymentDebits(array $odata): array
    {
        $payment = $this->paymentDict();
        $debits = [];

        foreach ($payment["types"] as $pt) {
            $pn = $payment["fields"][$pt];
            $debits[$pn] = (float)($odata[$pn] ?? 0);
        }

        $change = (float)($odata["f_amount_change"] ?? 0);
        if ($change <= 0.00001) {
            return $debits;
        }

        $cashField = $payment["fields"][PAYMENT_TYPE_CASH];
        $cashTendered = $debits[$cashField] ?? 0.0;
        $deduct = min($change, $cashTendered);
        $debits[$cashField] = $cashTendered - $deduct;
        $change -= $deduct;

        if ($change <= 0.00001) {
            return $debits;
        }

        foreach ($payment["types"] as $pt) {
            if (!$payment["cashbox"][$pt] || $pt === PAYMENT_TYPE_CASH) {
                continue;
            }
            $pn = $payment["fields"][$pt];
            $deduct = min($change, $debits[$pn]);
            $debits[$pn] -= $deduct;
            $change -= $deduct;
            if ($change <= 0.00001) {
                break;
            }
        }

        return $debits;
    }

    public function PrintPrecheck($params)
    {
        $this->assertOrderKitchenPrinted($params->id);
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
        $data["f_amount_cash"] = 0;
        $data["f_amount_card"] = 0;
        $data["f_amount_bank"] = 0;
        $data["f_amount_idram"] = 0;
        $data["f_amount_complimentary"] = 0;
        $data["f_amount_other"] = 0;
        $data["f_amount_telcell"] = 0;
        $data["f_amount_debt"] = 0;
        $data["f_amount_prepaid"] = 0;
        $data["f_amount_paid"] = 0;
        $data["f_amount_change"] = 0;
        $this->AppendLog("cancel precheck", $data);
        $this->update("o_header", ["f_data" => json_encode($data, JSON_UNESCAPED_UNICODE)], $params->id);
        $this->CountAmounts($params->id);
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
        $v["f_in"] = is_string($params->in ?? null) ? $params->in : json_encode($params->in ?? new stdClass(), JSON_UNESCAPED_UNICODE);
        $v["f_out"] = is_string($params->out ?? null) ? $params->out : json_encode($params->out ?? new stdClass(), JSON_UNESCAPED_UNICODE);
        $v["f_err"] = $params->error ?? "";
        $v["f_result"] = $params->result;
        $v["f_state"] = $params->result === 0 ? 1 : 0;
        $machineId = (int)($params->f_fiscal_machine_id ?? $params->f_machine ?? 0);
        if ($machineId > 0) {
            $v["f_fiscal_machine_id"] = $machineId;
        }
        $this->insert("o_tax_log", $v);
        if (!$noecho) {
            $this->echoResult();
        }
    }

    public function GetServiceValues($params)
    {
        $this->result["values"] = $this->select("select f_value, coalesce(f_comment, '') as f_name from o_service_values where f_value>=0 order by f_value ")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetDiscountValues($params)
    {
        /* Reference rows use f_value<=0; order math expects a positive discount factor. */
        $this->result["values"] = $this->select(
            "select abs(f_value) as f_value, coalesce(f_comment, '') as f_name from o_service_values where f_value<=0 order by f_value "
        )->fetch_all(MYSQLI_ASSOC);
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

    public function ChangeDiscountValue($params)
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
        $discountFactor = abs((float)($params->value ?? 0));
        $odata["f_discount_factor"] = $discountFactor;
        $odata["f_discount_comment"] = $params->comment;
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
            if (($ddata->f_count_discount ?? false) === true) {
                $ddata->f_discount_factor = $discountFactor;
            }

            $v["f_data"]  = json_encode($ddata, JSON_UNESCAPED_UNICODE);
            $this->update("o_goods", $v, $d["f_id"]);
        }
        $this->CountAmounts($id, $this->LogRecord("discount value", ["comment" => $discountFactor * 100 . "%"]));
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
        require_once __DIR__ . "/../worker/dict-dish-state.php";
        $this->TryLock($params->destination, $params->locksrc);

        $destTable = $this->select(
            "select f_id, f_hall, f_name from h_tables where f_id=?",
            "i",
            [$params->destination]
        )->fetch_assoc();
        if (empty($destTable)) {
            dieWithCode(Translator::t("Wrong table id"));
        }

        $odst = $this->select(
            "select f_id, f_data from o_header where f_table=? and f_state=?",
            "ii",
            [$params->destination, ORDER_STATE_OPEN]
        )->fetch_assoc();
        $src_dishes = $this->GetDishes($params->id);
        if (!$src_dishes) {
            dieWithCode("Are you src dish hacker?");
        }

        $transferComment = ($params->source_table_name ?? "") . " => " . ($params->destination_table_name ?? $destTable["f_name"]);

        if (empty($odst)) {
            /* Empty destination: keep source order, move table/hall, preserve f_data (service/discount). */
            $odata = $this->GetHeaderData($params->id);
            if (!is_array($odata)) {
                $odata = [];
            }
            $odata["log"] ??= [];
            $this->AppendLog("transfer table", $odata, ["comment" => $transferComment, "important" => true]);
            $this->update("o_header", [
                "f_table" => (int)$params->destination,
                "f_hall" => (int)$destTable["f_hall"],
                "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE),
            ], $params->id);
            $this->CountAmounts($params->id);
            $this->result["order"] = $this->GetOrder($params->id);
            $this->echoResult();
            return;
        }

        /* Merge into existing open order on destination: dishes move, service/discount always from source. */
        $source_data = $this->GetHeaderData($params->id);
        if (!is_array($source_data)) {
            $source_data = [];
        }
        $source_data["log"] ??= [];
        $dest_data = json_decode($odst["f_data"] ?? "{}", true);
        if (!is_array($dest_data)) {
            $dest_data = [];
        }
        $dest_data["log"] ??= [];

        $this->beginTransaction();
        foreach ($src_dishes as $d) {
            if ((int)$d["f_state"] != DISH_STATE_NORMAL) {
                continue;
            }
            $ddata = json_decode($d["f_data"] ?? "{}", true);
            if (!is_array($ddata)) {
                $ddata = [];
            }
            $ddata["f_from_table"] = $params->source_table_name ?? "";
            $itemComment = ($d["f_dish_name"] ?? "") . " (" . ($d["f_qty"] ?? 0) . ") " . $transferComment;
            $this->AppendLog("transfer items", $dest_data, ["comment" => $itemComment, "important" => true]);
            $this->AppendLog("transfer items", $source_data, ["comment" => $itemComment, "important" => true]);
            $this->update("o_goods", [
                "f_header" => $odst["f_id"],
                "f_data" => json_encode($ddata, JSON_UNESCAPED_UNICODE),
            ], $d["f_id"]);
        }

        /* Full table move carries source service/discount even if dest hall default is 0. */
        $dest_data["f_service_factor"] = $source_data["f_service_factor"] ?? 0;
        if (array_key_exists("f_service_comment", $source_data)) {
            $dest_data["f_service_comment"] = $source_data["f_service_comment"];
        }
        $dest_data["f_discount_factor"] = abs((float)($source_data["f_discount_factor"] ?? 0));
        if (array_key_exists("f_discount_comment", $source_data)) {
            $dest_data["f_discount_comment"] = $source_data["f_discount_comment"];
        }
        $this->AppendLog("transfer table", $dest_data, ["comment" => $transferComment, "important" => true]);
        $this->update("o_header", [
            "f_data" => json_encode($dest_data, JSON_UNESCAPED_UNICODE),
        ], $odst["f_id"]);

        $this->AppendLog("transfer table", $source_data, ["comment" => $transferComment, "important" => true]);
        $remaining = $this->select(
            "select f_id from o_goods where f_header=? and f_state=? limit 1",
            "si",
            [$params->id, DISH_STATE_NORMAL]
        )->fetch_assoc();
        $srcUpdate = ["f_data" => json_encode($source_data, JSON_UNESCAPED_UNICODE)];
        if (empty($remaining)) {
            $srcUpdate["f_state"] = ORDER_STATE_MOVED;
        }
        $this->update("o_header", $srcUpdate, $params->id);
        $this->commit();

        $this->CountAmounts($odst["f_id"]);
        if (!empty($remaining)) {
            $this->CountAmounts($params->id);
        }

        $this->result["order"] = $this->GetOrder($odst["f_id"]);
        $this->echoResult();
    }

    public function TransferItems($params)
    {
        require_once __DIR__ . "/../worker/dict-dish-state.php";
        $logs1 = [];
        $logs2 = [];
        $id1 = (string)($params->id1 ?? "");
        $id2 = (string)($params->id2 ?? "");

        $normalCount = function (string $hid): int {
            if ($hid === "") {
                return 0;
            }
            $r = $this->select(
                "select count(*) as c from o_goods where f_header=? and f_state=?",
                "si",
                [$hid, DISH_STATE_NORMAL]
            )->fetch_assoc();
            return (int)($r["c"] ?? 0);
        };
        /* Live counts: empty destination inherits donor service/discount (split / move onto new table). */
        $before = [
            $id1 => $normalCount($id1),
            $id2 => $normalCount($id2),
        ];
        $inheritFrom = [];

        foreach ($params->data as $d) {
            $v = [];
            $v["f_header"] = $d->f_header;
            $dish = $this->select("select f_header, f_data from o_goods where f_id=?", "s", [$d->f_id])->fetch_assoc();
            if (empty($dish)) {
                continue;
            }
            $oldHeader = (string)($dish["f_header"] ?? "");
            $newHeader = (string)($d->f_header ?? "");
            if ($oldHeader !== "" && $newHeader !== "" && $oldHeader !== $newHeader) {
                if (($before[$newHeader] ?? 0) === 0 && !isset($inheritFrom[$newHeader])) {
                    $inheritFrom[$newHeader] = $oldHeader;
                }
                if (array_key_exists($oldHeader, $before)) {
                    $before[$oldHeader] = max(0, $before[$oldHeader] - 1);
                }
                $before[$newHeader] = ($before[$newHeader] ?? 0) + 1;
            }
            $ddata = json_decode($dish["f_data"] ?? "{}", true);
            if (!is_array($ddata)) {
                $ddata = [];
            }
            $ddata["f_from_table"] = $d->f_from_table;
            $logEntry = $this->LogRecord("transfer items", [
                "comment" => $d->f_dish_name . " (" . $d->f_qty . ") " . $params->source_table_name . " => " . $params->destination_table_name,
            ]);
            /* Log on both sides via CountAmounts extraLog — do not overwrite f_data afterwards. */
            $logs1[] = $logEntry;
            $logs2[] = $logEntry;
            $v["f_data"] = json_encode($ddata, JSON_UNESCAPED_UNICODE);
            $this->update("o_goods", $v, $d->f_id);
        }

        foreach ($inheritFrom as $destId => $srcId) {
            $source_data = $this->GetHeaderData($srcId);
            if (!is_array($source_data)) {
                $source_data = [];
            }
            $dest_data = $this->GetHeaderData($destId);
            if (!is_array($dest_data)) {
                $dest_data = [];
            }
            $dest_data["f_service_factor"] = $source_data["f_service_factor"] ?? 0;
            if (array_key_exists("f_service_comment", $source_data)) {
                $dest_data["f_service_comment"] = $source_data["f_service_comment"];
            }
            $dest_data["f_discount_factor"] = abs((float)($source_data["f_discount_factor"] ?? 0));
            if (array_key_exists("f_discount_comment", $source_data)) {
                $dest_data["f_discount_comment"] = $source_data["f_discount_comment"];
            }
            $this->update("o_header", [
                "f_data" => json_encode($dest_data, JSON_UNESCAPED_UNICODE),
            ], $destId);
        }

        $this->CountAmounts($params->id1, $logs1);
        $this->CountAmounts($params->id2, $logs2);
        $this->echoResult();
    }

    public function SetHeaderComment($params)
    {
        $oheader = $this->select("select f_data from o_header where f_id=?", "s", [$params->id])->fetch_assoc();
        if (empty($oheader)) {
            dieWithCode("Order not found");
        }
        $odata = json_decode($oheader["f_data"] ?? "{}", true) ?: [];
        $comment = (string)($params->comment ?? "");
        $odata["f_comment"] = $comment;
        $this->update("o_header", [
            "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE),
            "f_comment" => $comment,
        ], $params->id);
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

    public function ActivatePreorder($params)
    {
        $orderId = (string)($params->id ?? "");
        if ($orderId === "") {
            dieWithCode(Translator::t("Missing order id"));
        }
        $this->beginTransaction();
        $header = $this->select("select * from o_header where f_id=? for update", "s", [$orderId])->fetch_assoc();
        if (!$header) {
            $this->rollback();
            dieWithCode(Translator::t("Order not found"));
        }
        if ((int)$header["f_state"] !== ORDER_STATE_PREORDER) {
            $this->rollback();
            dieWithCode(Translator::t("Not a preorder"));
        }
        $tableId = (int)$header["f_table"];
        $busy = $this->select(
            "select f_id from o_header where f_table=? and f_state=? and f_id<>? for update",
            "iis",
            [$tableId, ORDER_STATE_OPEN, $orderId]
        )->fetch_assoc();
        if ($busy) {
            $this->rollback();
            dieWithCode(Translator::t("This table busy now"));
        }
        $odata = json_decode($header["f_data"] ?? "{}", true) ?: [];
        $odata["log"] ??= [];
        $this->AppendLog("activate preorder", $odata);
        $this->update("o_header", [
            "f_state" => ORDER_STATE_OPEN,
            "f_data" => json_encode($odata, JSON_UNESCAPED_UNICODE),
        ], $orderId);
        $this->commit();
        $this->result["order"] = $this->GetOrder($orderId);
        $this->echoResult();
    }

    /**
     * Aggregate dishes ordered in the last N minutes, grouped by print1 station + dish.
     * params: minutes (int > 0)
     */
    public function RecentDishes($params)
    {
        $minutes = (int)($params->minutes ?? 0);
        if ($minutes <= 0) {
            dieWithCode(Translator::t("Minutes required"));
        }

        $sql = <<<EOD
        SELECT
            TRIM(JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_print1'))) AS print1,
            og.f_goods AS dish_id,
            g.f_name AS dish_name,
            SUM(og.f_qty) AS qty
        FROM o_goods og
        INNER JOIN c_goods g ON g.f_id = og.f_goods
        LEFT JOIN o_goods pkg ON pkg.f_id = og.f_parent AND pkg.f_header = og.f_header AND pkg.f_state = 1
        WHERE og.f_state = 1
          AND LENGTH(TRIM(IFNULL(JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_print1')), ''))) > 0
          AND JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_append_time'))
              >= DATE_FORMAT(DATE_SUB(NOW(), INTERVAL ? MINUTE), '%Y-%m-%d %H:%i:%s')
          AND (
                og.f_type = 5
             OR ((og.f_type = 1 OR og.f_type = 2)
                 AND NOT (og.f_parent IS NOT NULL AND og.f_parent != '' AND pkg.f_type = 5))
          )
        GROUP BY TRIM(JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_print1'))), og.f_goods, g.f_name
        ORDER BY print1, g.f_name
        EOD;

        $rows = $this->select($sql, "i", [$minutes])->fetch_all(MYSQLI_ASSOC);
        $stations = [];
        $out = [];
        foreach ($rows as $r) {
            $print1 = (string)($r["print1"] ?? "");
            if ($print1 === "") {
                continue;
            }
            if (!in_array($print1, $stations, true)) {
                $stations[] = $print1;
            }
            $out[] = [
                "print1" => $print1,
                "dish_id" => (int)$r["dish_id"],
                "dish_name" => (string)($r["dish_name"] ?? ""),
                "qty" => (float)$r["qty"],
            ];
        }

        $this->result["minutes"] = $minutes;
        $this->result["stations"] = $stations;
        $this->result["rows"] = $out;
        $this->echoResult();
    }
}
