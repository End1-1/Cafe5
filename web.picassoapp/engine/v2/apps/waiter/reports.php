<?php
# © 2025 , Kudryashov Vasili
# Created: 2026-01-08 10:47:25

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/helper.php";
require_once __DIR__ . "/../worker/cash-ops-helper.php";

class Reports extends Auth
{
    public function GetOrders($params)
    {
        $sql = <<<EOD
        select oh.f_id, oh.f_prefix, oh.f_table as f_table, t.f_hall as f_hall,
        h.f_name as f_hall_name, t.f_name as f_table_name,
        date_fmt(json_value(oh.f_data, '$.f_date_close')) as f_date_open,
        json_value(oh.f_data, '$.f_time_open') as f_time_open,
        json_value(oh.f_data, '$.f_date_close') as f_date_close,
        json_value(oh.f_data, '$.f_time_close') as f_time_close,
        concat(u.f_last, ' ', left(u.f_first, 1), '.') as f_staff_name,
        money_fmt(oh.f_amounttotal) as f_amount_total, json_value(oh.f_data, '$.rseq') as f_fiscal
        from o_header oh 
        left join h_tables t on t.f_id=oh.f_table
        left join h_halls h on h.f_id=t.f_hall
        left join s_user u on u.f_id=oh.f_staff
        where oh.f_state=2 and oh.f_datecash between ? and ?
        order by  json_value(oh.f_data, '$.f_date_close'), json_value(oh.f_data, '$.f_time_close') 
        EOD;
        $this->result["orders"] = $this->select($sql, "ss", [$params->date1, $params->date2])->fetch_all(MYSQLI_ASSOC);
        $sql = <<<EOD
        select money_fmt(sum(oh.f_amounttotal)) as f_amount_total
        from o_header oh
        where oh.f_state=2 and oh.f_datecash between ? and ?
        EOD;
        $this->result["total"] = $this->select($sql, "ss", [$params->date1, $params->date2])->fetch_row()[0];
        $this->echoResult();
    }

    public function GetDaily($params)
    {
        // Aggregate payment fields via cash_ops_order_payment_debits so change (сдача)
        // is not counted as cash revenue (e.g. 10000 note for 4000 bill → cash 4000).
        $rows = $this->select(
            "SELECT f_amounttotal, f_data,
                    coalesce(JSON_VALUE(f_data, '$.f_service_amount'), 0) AS f_service_amount
             FROM o_header
             WHERE f_datecash BETWEEN ? AND ? AND f_state=2",
            "ss",
            [$params->date1, $params->date2]
        )->fetch_all(MYSQLI_ASSOC);
        $dict = require __DIR__ . "/../worker/dict-payment.php";
        $out = [
            "f_count_id" => count($rows),
            "f_amount_total" => 0.0,
            "f_service_amount" => 0.0,
            "f_amount_cash" => 0.0,
            "f_amount_card" => 0.0,
            "f_amount_bank" => 0.0,
            "f_amount_idram" => 0.0,
        ];
        foreach ($rows as $row) {
            $out["f_amount_total"] += (float)($row["f_amounttotal"] ?? 0);
            $out["f_service_amount"] += (float)($row["f_service_amount"] ?? 0);
            $odata = json_decode($row["f_data"] ?? "{}", true);
            if (!is_array($odata)) {
                $odata = [];
            }
            $debits = cash_ops_order_payment_debits($odata);
            foreach ($dict["fields"] as $pt => $field) {
                if (!array_key_exists($field, $out)) {
                    $out[$field] = 0.0;
                }
                $out[$field] += (float)($debits[(int)$pt] ?? 0);
            }
        }
        $this->result = array_merge($out, $this->result);
        $this->echoResult();
    }
    public function GetList($params)
    {
        $last_session_id = $this->select("select coalesce(max(f_id), 0) from cash_session")->fetch_row()[0];
        $list = [
            ["title" => "01. " . Translator::t("Daily revenue by shift"), "report_id" => 1, "params" => "last-30-sessions", "default_values" => [
                "session_id" => $last_session_id
            ]],
            ["title" => "02. " . Translator::t("Daily revenue by shift, by waiter"), "report_id" => 2, "params" => "last-30-sessions", "default_values" => [
                "session_id" => $last_session_id
            ]],
            ["title" => "03. " . Translator::t("Dish sales report by shift"), "report_id" => 3, "params" => "last-30-sessions", "default_values" => [
                "session_id" => $last_session_id
            ]],
            ["title" => "04. " . Translator::t("Dish and services, amounts, report by shift"), "report_id" => 4, "params" => "last-30-sessions", "default_values" => [
                "session_id" => $last_session_id
            ]],
            ["title" => "05. " . Translator::t("Cash flow"), "report_id" => 5, "params" => "last-30-sessions", "default_values" => [
                "session_id" => $last_session_id
            ]],
        ];
        $this->result["list"] = $list;
        $this->echoResult();
    }

    public function GetReport($params)
    {
        $output = [];
        match ($params->report_id ?? 0) {
            1 => $output = $this->PrintDailyRevenue($params),
            2 => $output =  $this->PrintDailyRevenueByWaiters($params),
            3 => $output = $this->PrintDishesByShift($params),
            4 => $output = $this->PrintDishesServicesAmountsByShift($params),
            5 => $output = $this->PrintSessionCashFlow($params),
            default => dieWithCode("Report id not exits " . $params->report_id ?? 0)
        };
        $this->result["printing"] = $output;
        $this->echoResult();
    }

    private function PrintDailyRevenue($params)
    {
        $cashbox = $this->select("select cb.f_name from cash_box cb left join cash_session cs on cs.f_cashbox_id=cb.f_id where cs.f_id=?", "i", [$params->params->session_id])->fetch_assoc();
        $dict = require __DIR__ . "/../worker/dict-payment.php";

        $orderRows = $this->select(
            "SELECT oh.f_amounttotal, oh.f_data FROM o_header oh WHERE oh.f_state = 2 AND oh.f_cash_session_id=?",
            "i",
            [$params->params->session_id]
        )->fetch_all(MYSQLI_ASSOC);
        $payTotals = [];
        foreach ($dict["fields"] as $type => $field) {
            $payTotals[$field] = 0.0;
        }
        $amountTotal = 0.0;
        foreach ($orderRows as $orow) {
            $amountTotal += (float)($orow["f_amounttotal"] ?? 0);
            $odata = json_decode($orow["f_data"] ?? "{}", true);
            if (!is_array($odata)) {
                $odata = [];
            }
            foreach (cash_ops_order_payment_debits($odata) as $pt => $amt) {
                $field = $dict["fields"][$pt] ?? null;
                if ($field === null) {
                    continue;
                }
                $payTotals[$field] = ($payTotals[$field] ?? 0) + (float)$amt;
            }
        }
        $totals = [
            "f_orders_count" => count($orderRows),
            "f_amount_total" => money_fmt($amountTotal),
        ];
        foreach ($payTotals as $field => $amt) {
            $totals[$field] = money_fmt($amt);
        }

        $sql = <<<EOD
    select money_fmt(SUM(coalesce(JSON_VALUE(f_data, '$.f_service_amount'), 0))) AS f_service_amount,
    datetime_fmt(cs.f_date_open, 0) as f_date_open,
    datetime_fmt(cs.f_date_close, 0) as f_date_close
    from o_header oh 
    left join cash_session cs  on cs.f_id=oh.f_cash_session_id
    where oh.f_state=2 and oh.f_cash_session_id=?
    EOD;
        $oh = $this->select($sql, "i", [$params->params->session_id])->fetch_assoc();

        $output = [
            ["cmd" => "fontsize", "size" => 20],
            ["cmd" => "ctext", "text" => $params->report_name ?? "Undefined"],
            ["cmd" => "br"],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Cashbox"), "right" => $cashbox["f_name"]],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Shift"), "right" => "#" . $params->params->session_id],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t(key: "Date open"), "right" =>  $oh["f_date_open"]],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Date close"), "right" =>  $oh["f_date_close"]],
            ["cmd" => "br"],
            ["cmd" => "line2", "width" => 1],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Orders") . ":", "right" => "{$totals["f_orders_count"]}"],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Total") . ":",  "right" => $totals["f_amount_total"]],
            ["cmd" => "line"],
            ["cmd" => "br"],
        ];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "line2", "width" => 1];

        $output[] = ["cmd" => "br"];
        foreach ($dict["fields"] as $type => $field) {
            $amount = $totals[$field] ?? "0";
            if ((float)$amount == 0) {
                continue;
            }
            $output[] = ["cmd" => "lrtext", "left" => payment_type_name($this, (int)$type), "right" => $amount];
            $output[] = ["cmd" => "br"];
        }
        $output[] = ["cmd" => "lrtext", "left" => Translator::t("Amount of service"), "right" => $oh["f_service_amount"]];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "line2", "width" => 1];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "fontsize", "size" => 16];
        $output[] = ["cmd" => "rtext", "text" => date("Y-m-d H:i:s")];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "ltext", "text" => "."];

        return $output;
    }

    private function PrintDailyRevenueByWaiters($params)
    {
        $cashbox = $this->select("select cb.f_name from cash_box cb left join cash_session cs on cs.f_cashbox_id=cb.f_id where cs.f_id=?", "i", [$params->params->session_id])->fetch_assoc();
        $dict = require __DIR__ . "/../worker/dict-payment.php";

        $sessionMeta = $this->select(
            "SELECT datetime_fmt(cs.f_date_open, 0) as f_date_open,
                    datetime_fmt(cs.f_date_close, 0) as f_date_close
             FROM cash_session cs WHERE cs.f_id=?",
            "i",
            [$params->params->session_id]
        )->fetch_assoc();

        $orderRows = $this->select(
            "SELECT oh.f_amounttotal, oh.f_data, oh.f_staff,
                    concat(u.f_last, ' ', left(u.f_first, 1), '.') as f_waiter
             FROM o_header oh
             LEFT JOIN s_user u ON u.f_id = oh.f_staff
             WHERE oh.f_state = 2 AND oh.f_cash_session_id=?",
            "i",
            [$params->params->session_id]
        )->fetch_all(MYSQLI_ASSOC);

        $byWaiter = [];
        $finaly = [
            "f_orders_count" => 0,
            "f_amount_total" => 0.0,
            "f_service_amount" => 0.0,
        ];
        foreach ($dict["fields"] as $field) {
            $finaly[$field] = 0.0;
        }
        foreach ($orderRows as $orow) {
            $waiter = trim((string)($orow["f_waiter"] ?? "")) ?: ("#" . (int)($orow["f_staff"] ?? 0));
            if (!isset($byWaiter[$waiter])) {
                $byWaiter[$waiter] = [
                    "f_waiter" => $waiter,
                    "f_date_open" => $sessionMeta["f_date_open"] ?? "",
                    "f_date_close" => $sessionMeta["f_date_close"] ?? "",
                    "f_orders_count" => 0,
                    "f_amount_total" => 0.0,
                    "f_service_amount" => 0.0,
                ];
                foreach ($dict["fields"] as $field) {
                    $byWaiter[$waiter][$field] = 0.0;
                }
            }
            $odata = json_decode($orow["f_data"] ?? "{}", true);
            if (!is_array($odata)) {
                $odata = [];
            }
            $byWaiter[$waiter]["f_orders_count"]++;
            $byWaiter[$waiter]["f_amount_total"] += (float)($orow["f_amounttotal"] ?? 0);
            $byWaiter[$waiter]["f_service_amount"] += (float)($odata["f_service_amount"] ?? 0);
            $finaly["f_orders_count"]++;
            $finaly["f_amount_total"] += (float)($orow["f_amounttotal"] ?? 0);
            $finaly["f_service_amount"] += (float)($odata["f_service_amount"] ?? 0);
            foreach (cash_ops_order_payment_debits($odata) as $pt => $amt) {
                $field = $dict["fields"][$pt] ?? null;
                if ($field === null) {
                    continue;
                }
                $byWaiter[$waiter][$field] += (float)$amt;
                $finaly[$field] += (float)$amt;
            }
        }

        $totals = [];
        foreach ($byWaiter as $h) {
            $row = $h;
            $row["f_amount_total"] = money_fmt($h["f_amount_total"]);
            $row["f_service_amount"] = money_fmt($h["f_service_amount"]);
            foreach ($dict["fields"] as $field) {
                $row[$field] = money_fmt($h[$field] ?? 0);
            }
            $totals[] = $row;
        }
        $finaly["f_amount_total"] = money_fmt($finaly["f_amount_total"]);
        $finaly["f_service_amount"] = money_fmt($finaly["f_service_amount"]);
        foreach ($dict["fields"] as $field) {
            $finaly[$field] = money_fmt($finaly[$field] ?? 0);
        }

        $first = true;
        foreach ($totals as $h) {
            if ($first) {
                $first = false;
                $output = [
                    ["cmd" => "fontsize", "size" => 20],
                    ["cmd" => "ctext", "text" => $params->report_name ?? "Undefined"],
                    ["cmd" => "br"],
                    ["cmd" => "br"],
                    ["cmd" => "lrtext", "left" => Translator::t("Cashbox"), "right" => $cashbox["f_name"]],
                    ["cmd" => "br"],
                    ["cmd" => "lrtext", "left" => Translator::t("Shift"), "right" => "#" . $params->params->session_id],
                    ["cmd" => "br"],
                    ["cmd" => "lrtext", "left" => Translator::t(key: "Date open"), "right" =>  $h["f_date_open"]],
                    ["cmd" => "br"],
                    ["cmd" => "lrtext", "left" => Translator::t("Date close"), "right" =>  $h["f_date_close"]],
                    ["cmd" => "br"],
                    ["cmd" => "line2", "width" => 1],


                ];
                $output[] = ["cmd" => "br"];
            }
            $output[] = ["cmd" => "br"];
            $output[] = ["cmd" => "ltext", "text" => $h["f_waiter"]];
            $output[] = ["cmd" => "br"];
            $output[] = ["cmd" => "lrtext", "left" => Translator::t("Orders"), "right" => "{$h["f_orders_count"]}"];
            $output[] = ["cmd" => "br"];
            foreach ($dict["fields"] as $type => $field) {
                $amount = $h[$field] ?? "0";
                if ((float)$amount == 0) {
                    continue;
                }
                $output[] = ["cmd" => "lrtext", "left" => payment_type_name($this, (int)$type), "right" => $amount];
                $output[] = ["cmd" => "br"];
            }
            $output[] = ["cmd" => "lrtext", "left" => Translator::t("Amount of service"), "right" => $h["f_service_amount"]];
            $output[] = ["cmd" => "br"];
            $output[] = ["cmd" => "line2", "width" => 1];
        }

        //TOTOAL
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "ltext", "text" => Translator::t("Total")];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "lrtext", "left" =>  Translator::t("Orders"), "right" => "{$finaly["f_orders_count"]}"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "lrtext", "left" => Translator::t("Amount"),  "right" => $finaly["f_amount_total"]];
        $output[] = ["cmd" => "br"];
        foreach ($dict["fields"] as $type => $field) {
            $amount = $finaly[$field] ?? "0";
            if ((float)$amount == 0) {
                continue;
            }
            $output[] = ["cmd" => "lrtext", "left" => payment_type_name($this, (int)$type), "right" => $amount];
            $output[] = ["cmd" => "br"];
        }
        $output[] = ["cmd" => "line"];
        $output[] = ["cmd" => "br"];


        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "fontsize", "size" => 16];
        $output[] = ["cmd" => "rtext", "text" => date("Y-m-d H:i:s")];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "ltext", "text" => "."];

        return $output;
    }

    public function PrintDishesByShift($params)
    {
        $sql = <<<EOD
        select cb.f_name, 
        datetime_fmt(cs.f_date_open, 0) as f_date_open,
        datetime_fmt(cs.f_date_close, 0) as f_date_close
        from cash_box cb 
        left join cash_session cs on cs.f_cashbox_id=cb.f_id 
        where cs.f_id=?
        EOD;
        $cashbox = $this->select($sql, "i", [$params->params->session_id])->fetch_assoc();
        $sql = <<<EOD
        select gr.f_name as f_group_name, g.f_name as f_goods_name,
        money_fmt(sum(og.f_qty)) as f_qty, 
        money_fmt(sum(og.f_total)) as f_amount_total
        from o_goods og
        left join o_header oh on oh.f_id=og.f_header
        left join c_goods g on g.f_id=og.f_goods
        left join c_groups gr on gr.f_id=g.f_group
        where oh.f_state=2 and og.f_state=1
        and oh.f_cash_session_id=?
        group by g.f_name
        order by gr.f_name
        EOD;
        $data = $this->select($sql, "i", [$params->params->session_id])->fetch_all(MYSQLI_ASSOC);
        $output = [
            ["cmd" => "fontsize", "size" => 20],
            ["cmd" => "ctext", "text" => $params->report_name ?? "Undefined"],
            ["cmd" => "br"],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Cashbox"), "right" => $cashbox["f_name"]],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Shift"), "right" => "#" . $params->params->session_id],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t(key: "Date open"), "right" =>  $cashbox["f_date_open"]],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Date close"), "right" =>  $cashbox["f_date_close"]],
            ["cmd" => "br"],
            ["cmd" => "line2", "width" => 1],
        ];
        $group_name = "";
        foreach ($data as $d) {
            if ($group_name != $d["f_group_name"]) {
                $group_name = $d["f_group_name"];
                $output[] = ["cmd" => "ctext", "text" => $group_name];
                $output[] =   ["cmd" => "br"];
            }
            $output[] =  ["cmd" => "ltext", "text" => $d["f_goods_name"], "x" => 0, "textwidth" => 50];
            $output[] =  ["cmd" => "ltext", "text" => $d["f_qty"], "x" => 51];
            $output[] = ["cmd" => "rtext", "text" => $d["f_amount_total"]];
            $output[] = ["cmd" => "br"];
        }
        return $output;
    }

    public function Last30Sessions($params)
    {
        $sql = <<<EOD
        SELECT 
            c.f_id, rpad(cb.f_name, 20, ' ') as f_name,
            datetime_fmt(c.f_date_open, 0)  AS f_date_open,
            datetime_fmt(c.f_date_close, 0) AS f_date_close,
            money_fmt(c.f_amount_expected)  AS f_amount_expected,
            CONCAT(u1.f_last, ' ', LEFT(u1.f_first, 1), '.') AS f_user_open_name,
            COUNT(DISTINCT CASE WHEN o.f_operation_type = 1 THEN o.f_order_id END) AS f_orders_count
        FROM cash_session c
        LEFT JOIN s_user u1 ON u1.f_id = c.f_user_open
        LEFT JOIN cash_operations o ON o.f_session_id = c.f_id
        left join cash_box cb on cb.f_id=c.f_cashbox_id
        GROUP BY c.f_id
        ORDER BY c.f_id DESC
        LIMIT 30;
        EOD;
        $data = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $list = [];
        foreach ($data as $d) {
            $list[] = [
                "text" => "#" . str_pad($d["f_id"], 6) . "  " . $d["f_name"]
                    . Translator::t("Date open") . " " . $d["f_date_open"] . "   "
                    . Translator::t("Date close") . " " . $d["f_date_close"] . "   "
                    . Translator::t("Amount")  . " " . $d["f_amount_expected"],
                "value" => $d["f_id"]
            ];
        }
        $this->result["values"] = $list;
        $this->echoResult();
    }

    public function PrintDishesServicesAmountsByShift($params)
    {
        $sql = <<<EOD
        select cb.f_name, 
        datetime_fmt(cs.f_date_open, 0) as f_date_open,
        datetime_fmt(cs.f_date_close, 0) as f_date_close
        from cash_box cb 
        left join cash_session cs on cs.f_cashbox_id=cb.f_id 
        where cs.f_id=?
        EOD;
        $cashbox = $this->select($sql, "i", [$params->params->session_id])->fetch_assoc();
        $sql = <<<EOD
        SELECT 
        g.f_service,
        money_fmt(sum(og.f_qty)) as f_qty, 
        money_fmt(sum(og.f_total)) as f_amount_total
        FROM o_goods og
        LEFT JOIN o_header oh ON oh.f_id = og.f_header
        LEFT JOIN c_goods g ON g.f_id = og.f_goods
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        WHERE oh.f_state = 2 
        AND og.f_state = 1
        AND oh.f_cash_session_id = ?
        GROUP BY g.f_service
        ORDER BY g.f_service
        EOD;
        $data = $this->select($sql, "i", [$params->params->session_id])->fetch_all(MYSQLI_ASSOC);
        $output = [
            ["cmd" => "fontsize", "size" => 20],
            ["cmd" => "ctext", "text" => $params->report_name ?? "Undefined"],
            ["cmd" => "br"],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Cashbox"), "right" => $cashbox["f_name"]],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Shift"), "right" => "#" . $params->params->session_id],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t(key: "Date open"), "right" =>  $cashbox["f_date_open"]],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Date close"), "right" =>  $cashbox["f_date_close"]],
            ["cmd" => "br"],
            ["cmd" => "line2", "width" => 1],
        ];

        foreach ($data as $d) {

            $output[] =  ["cmd" => "ltext", "text" => $d["f_service"] == 0 ? Translator::t("Goods") : Translator::t("Service"), "x" => 0, "textwidth" => 50];
            $output[] =  ["cmd" => "ltext", "text" => $d["f_qty"], "x" => 51];
            $output[] = ["cmd" => "rtext", "text" => $d["f_amount_total"]];
            $output[] = ["cmd" => "br"];
        }
        return $output;
    }

    public function PrintSessionCashFlow($params)
    {
        $sessionId = (int)$params->params->session_id;

        // --- 1. ИНФО О СЕССИИ (ШАПКА) ---
        // Добавляем f_date_close в выборку
        $sql = "SELECT cb.f_id as f_cashbox_id, cb.f_name, cs.f_date_open, cs.f_date_close 
            FROM cash_session cs 
            LEFT JOIN cash_box cb ON cb.f_id = cs.f_cashbox_id 
            WHERE cs.f_id = ?";
        $sessionInfo = $this->select($sql, "i", [$sessionId])->fetch_assoc();

        if (!$sessionInfo) {
            return [["cmd" => "ctext", "text" => "Session #$sessionId not found"]];
        }

        $cashboxId = (int)$sessionInfo['f_cashbox_id'];
        $isSessionOpen = ($sessionInfo['f_date_close'] === null);

        // --- 2. ЧТО ДОЛЖНО БЫТЬ В КАССЕ ВААААБЩЕ ---
        $sqlTotalInBox = <<<EOD
        SELECT 
            co.f_payment_type_id,
            COALESCE(MAX(NULLIF(cpt.f_name, '')), MAX(d.f_value)) as f_pay_type,
            SUM(co.f_debit - co.f_credit) as f_balance_raw
        FROM cash_operations co
        LEFT JOIN l_dictionary d ON d.f_dict_id = co.f_payment_type_id 
            AND d.f_dict='cash_payment_types' AND d.f_lang='hy'
        LEFT JOIN cash_payment_types cpt ON cpt.f_id = co.f_payment_type_id
        WHERE co.f_cashbox_id = ?
        GROUP BY co.f_payment_type_id
EOD;
        $balancesRaw = $this->select($sqlTotalInBox, "i", [$cashboxId])->fetch_all(MYSQLI_ASSOC);

        // --- 3. ИТОГИ ДНЯ (Движение за текущую смену) ---
        $sqlSummary = <<<EOD
        SELECT 
            co.f_payment_type_id,
            COALESCE(MAX(NULLIF(cpt.f_name, '')), MAX(d.f_value)) as f_pay_type,
            SUM(co.f_debit) as f_total_debit,
            SUM(co.f_credit) as f_total_credit,
            SUM(co.f_debit - co.f_credit) as f_net_diff
        FROM cash_operations co
        LEFT JOIN l_dictionary d ON d.f_dict_id = co.f_payment_type_id 
            AND d.f_dict='cash_payment_types' AND d.f_lang='hy'
        LEFT JOIN cash_payment_types cpt ON cpt.f_id = co.f_payment_type_id
        WHERE co.f_session_id = ?
        GROUP BY co.f_payment_type_id
EOD;
        $summary = $this->select($sqlSummary, "i", [$sessionId])->fetch_all(MYSQLI_ASSOC);

        // Подготавливаем массив итогов сессии для вычитания
        $sessionDiffs = [];
        foreach ($summary as $s) {
            $sessionDiffs[$s['f_payment_type_id']] = (float)$s['f_net_diff'];
        }

        // --- ФОРМИРУЕМ ВЫВОД ---
        $output = [
            ["cmd" => "fontsize", "size" => 20],
            ["cmd" => "ctext", "text" => Translator::t("CASH FLOW REPORT")],
            ["cmd" => "br"],
            ["cmd" => "fontsize", "size" => 16],
            ["cmd" => "lrtext", "left" => Translator::t("Cashbox"), "right" => $sessionInfo['f_name']],
            ["cmd" => "br"],
            ["cmd" => "lrtext", "left" => Translator::t("Shift"), "right" => "#" . $sessionId],
            ["cmd" => "br"],
            ["cmd" => "line2", "width" => 1],
            ["cmd" => "br"],
        ];

        // ПУНКТ 2: ТЕКУЩИЕ ОСТАТКИ (С КОРРЕКЦИЕЙ НА ОТКРЫТУЮ СМЕНУ)
        $output[] = ["cmd" => "ctext", "text" => Translator::t("CURRENT BALANCES")];
        $output[] = ["cmd" => "br"];

        foreach ($balancesRaw as $b) {
            $val = (float)$b['f_balance_raw'];
            $typeId = $b['f_payment_type_id'];

            // Если смена не закрыта, вычитаем её влияние на общий баланс
            if ($isSessionOpen && isset($sessionDiffs[$typeId])) {
                $val -= $sessionDiffs[$typeId];
            }

            // Выводим только если баланс не нулевой после коррекции
            if (round($val, 2) != 0) {
                $output[] = ["cmd" => "lrtext", "left" => $b['f_pay_type'], "right" => money_fmt_php($val)];
                $output[] = ["cmd" => "br"];
            }
        }

        // ПУНКТ 3: ИТОГИ ДНЯ
        $output[] = ["cmd" => "line2", "width" => 1];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "fontsize", "size" => 18];
        $output[] = ["cmd" => "ctext", "text" => Translator::t("TOTAL SUMMARY")];
        $output[] = ["cmd" => "br"];

        foreach ($summary as $s) {
            $output[] = ["cmd" => "ltext", "text" => $s['f_pay_type']];
            $output[] = ["cmd" => "br"];
            $output[] = ["cmd" => "lrtext", "left" => "  " . Translator::t("Total Income"), "right" => "+" . money_fmt_php($s['f_total_debit'])];
            $output[] = ["cmd" => "br"];
            $output[] = ["cmd" => "lrtext", "left" => "  " . Translator::t("Total Outcome"), "right" => "-" . money_fmt_php($s['f_total_credit'])];
            $output[] = ["cmd" => "br"];

            $net = (float)$s['f_net_diff'];
            $output[] = ["cmd" => "lrtext", "left" => "  " . Translator::t("Period Balance"), "right" => ($net > 0 ? "+" : "") . money_fmt_php($net)];
            $output[] = ["cmd" => "br"];
            $output[] = ["cmd" => "line", "width" => 1];
            $output[] = ["cmd" => "br"];
        }

        // --- 4. ДЕТАЛЬНОЕ ДВИЖЕНИЕ ---
        $sqlOps = <<<EOD
        SELECT 
            co.f_datetime as f_datetime_raw,
            datetime_fmt(co.f_datetime, 0) as f_datetime,
            co.f_comment,
            co.f_operation_type,
            COALESCE(NULLIF(cpt.f_name, ''), d.f_value) as f_pay_type,
            co.f_debit,
            co.f_credit,
            COALESCE(NULLIF(TRIM(p.f_taxname), ''), NULLIF(TRIM(p.f_name), ''), '') AS partner_name,
            JSON_UNQUOTE(JSON_EXTRACT(sd.f_data, '$.create_date')) AS store_create_date
        FROM cash_operations co
        LEFT JOIN l_dictionary d ON d.f_dict_id = co.f_payment_type_id 
            AND d.f_dict='cash_payment_types' AND d.f_lang='hy'
        LEFT JOIN cash_payment_types cpt ON cpt.f_id = co.f_payment_type_id
        LEFT JOIN store_document sd ON sd.f_id = co.f_order_id AND sd.f_doc_type = 1
        LEFT JOIN c_partners p ON p.f_id = sd.f_partner
        WHERE co.f_session_id = ?
        ORDER BY co.f_id ASC
EOD;
        $operations = $this->select($sqlOps, "i", [$sessionId])->fetch_all(MYSQLI_ASSOC);

        $output[] = ["cmd" => "ctext", "text" => Translator::t("DETAILED LOG")];
        $output[] = ["cmd" => "br"];

        foreach ($operations as $op) {
            $dtRaw = (string)($op['f_datetime_raw'] ?? '');
            $dtShow = (string)($op['f_datetime'] ?? '');
            if (strlen($dtRaw) >= 19 && substr($dtRaw, 11, 8) === '00:00:00') {
                $create = trim((string)($op['store_create_date'] ?? ''));
                $parsed = $create !== ''
                    ? (\DateTime::createFromFormat('d/m/Y H:i:s', $create)
                        ?: \DateTime::createFromFormat('Y-m-d H:i:s', $create))
                    : false;
                if ($parsed) {
                    $dtShow = substr($dtRaw, 0, 10) . ' ' . $parsed->format('H:i:s');
                }
            }
            $output[] = ["cmd" => "ltext", "text" => $dtShow];
            $output[] = ["cmd" => "br"];

            $label = $op['f_comment'] . ($op['f_pay_type'] ? " ({$op['f_pay_type']})" : "");
            $partner = trim((string)($op['partner_name'] ?? ''));
            if ($partner !== '' && ((int)$op['f_operation_type'] === 3 || (float)$op['f_credit'] > 0)) {
                $label .= " — " . $partner;
            }
            $valStr = ((float)$op['f_debit'] > 0) ? "+" . money_fmt_php($op['f_debit']) : "-" . money_fmt_php($op['f_credit']);

            $output[] = ["cmd" => "lrtext", "left" => $label, "right" => $valStr];
            $output[] = ["cmd" => "br"];
            $output[] = ["cmd" => "line", "width" => 1];
            $output[] = ["cmd" => "br"];
        }

        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "rtext", "text" => date("Y-m-d H:i:s")];
        $output[] = ["cmd" => "br"];
        $output[] = ["cmd" => "ctext", "text" => "---"];

        return $output;
    }
}
