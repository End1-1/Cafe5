<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-28 21:28:31
# Last Modified: 2026-05-25

class SoldItems
{
    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $filterRaw = $params->filter ?? [];
        $filter = [];
        foreach ($filterRaw as $item) {
            $curr = (array)$item;
            foreach ($curr as $k => $v) {
                $filter[$k] = $v;
            }
        }

        $summarize = (int)($filter["summarize"] ?? 1);

        // Форматируем и защищаем даты от инъекций
        $date1 = isset($filter["date1"]) ? date('Y-m-d', strtotime($filter["date1"])) : date('Y-m-d');
        $date2 = isset($filter["date2"]) ? date('Y-m-d', strtotime($filter["date2"])) : date('Y-m-d');

        if (method_exists($this->db, 'escape')) {
            $date1 = $this->db->escape($date1);
            $date2 = $this->db->escape($date2);
        }

        $date_filter = "and oh.f_datecash BETWEEN '$date1' AND '$date2' ";

        $group_id = (int)($filter["group_id"] ?? 0);
        $group_filter = ($group_id > 0) ? " and g.f_group = $group_id " : "";

        $guest_fields = "
            COALESCE(JSON_VALUE(oh.f_data, '$.f_guest.f_guest_name'), '') AS f_guest_name,
            COALESCE(JSON_VALUE(oh.f_data, '$.f_guest.f_guest_phone'), '') AS f_guest_phone,
            COALESCE(JSON_VALUE(oh.f_data, '$.f_guest.f_guest_address'), '') AS f_guest_address";

        if ($summarize === 1) {
            // РЕЖИМ СГРУППИРОВАННОГО ОТЧЕТА (по товарам)
            $select_fields = "
                ' ' as f_id, 
                ' ' as f_cash_session_id,
                ' ' as f_prefix,
                ' ' as f_hall_name,
                gr.f_name as f_group_name,
                g.f_name as f_item_name,
                money_fmt(sum(og.f_qty)) as f_qty,
                ' ' as f_price,
                money_fmt(sum(og.f_total)) as f_total,
                money_fmt(sum(sq.f_cogs_row)) as f_cosg,
                money_fmt(sum(og.f_total) - sum(COALESCE(sq.f_cogs_row, 0))) as f_profit,
                ' ' as f_guest_name,
                ' ' as f_guest_phone,
                ' ' as f_guest_address
            ";
            $group_by = "GROUP BY gr.f_name, g.f_name";
            $order_by = "ORDER BY gr.f_name, g.f_name";
        } else {
            // ДЕТАЛЬНЫЙ РЕЖИМ (по чекам)
            // sq считает себестоимость изолированно, строки o_goods не дублируются
            $select_fields = "
                oh.f_id, 
                oh.f_cash_session_id,
                oh.f_prefix,
                h.f_name as f_hall_name,
                gr.f_name as f_group_name,
                g.f_name as f_item_name,
                money_fmt(og.f_qty) as f_qty,
                money_fmt(og.f_price) as f_price,
                money_fmt(og.f_total) as f_total,
                money_fmt(COALESCE(sq.f_cogs_row, 0)) as f_cosg,
                money_fmt(og.f_total - COALESCE(sq.f_cogs_row, 0)) as f_profit,
                $guest_fields
            ";
            $group_by = ""; // В детальном режиме группировка больше не нужна благодаря подзапросу
            $order_by = "ORDER BY oh.f_id, gr.f_name, g.f_name";
        }

        $sql = <<<EOD
    SELECT $select_fields
    FROM o_goods og 
    LEFT JOIN c_goods g ON g.f_id=og.f_goods
    LEFT JOIN c_groups gr ON gr.f_id=g.f_group
    LEFT JOIN o_header oh ON oh.f_id=og.f_header
    LEFT JOIN h_tables t ON t.f_id=oh.f_table
    LEFT JOIN h_halls h ON h.f_id=t.f_hall
    LEFT JOIN (
        SELECT f_row_sale_id, SUM(f_qty * f_price) as f_cogs_row 
        FROM store_calc_queue 
        GROUP BY f_row_sale_id
    ) sq ON sq.f_row_sale_id = og.f_id
    WHERE oh.f_state=2 AND og.f_state=1 
    $date_filter 
    $group_filter
    $group_by
    $order_by
    EOD;

        $headers = [
            Translator::t("Id"),
            Translator::t("Cash session"),
            Translator::t("Order"),
            Translator::t("Hall"),
            Translator::t("Group"),
            Translator::t("Item name"),
            Translator::t("Qty"),
            Translator::t("Price"),
            Translator::t("Total"),
            Translator::t("Total COGS"),
            Translator::t("Profit"),
            Translator::t("Guest"),
            Translator::t("Phone"),
            Translator::t("Address"),
        ];

        return [
            "sql" => !(empty($params->debug)) ? $sql : "",
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => true, "reload" => true, "filter" => true],
            "headers" => $headers,
            "hidden_columns" => $summarize == 0 ? [0] : [0, 1, 2, 3, 7, 11, 12, 13],
            "sum" => [6, 8, 9, 10],
            "filter" => [
                ["type" => "combobox", "name" => "summarize", "label" => Translator::t("Summarize"), "default" => 1, "values" => [
                    ["label" => Translator::t("Yes"), "value" => 1],
                    ["label" => Translator::t("No"), "value" => 0]
                ]],
                ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
                ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
                ["type" => "number", "name" => "session_id", "label" => Translator::t("Session id")],
                ["type" => "keyvalue", "name" => "group_id", "label" => Translator::t("Group"), "function" => "goods_group"],
            ]
        ];
    }

    public function GetItem($params)
    {
        require_once __DIR__ . "/../../waiter/order.php";
        $ord = new Order();
        return $ord->GetOrder($params->id);
    }
}
