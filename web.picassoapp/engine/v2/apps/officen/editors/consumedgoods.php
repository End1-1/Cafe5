<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-28 21:28:31
# Last Modified: 2026-01-28 21:28:34

class ConsumedGoods
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
            foreach ((array)$item as $k => $v) {
                if ($v !== '' && $v !== null) {
                    $filter[$k] = $v;
                }
            }
        }

        $summarize = (int)($filter["summarize"] ?? 1);

        // Подготовка дат
        $date1 = date('Y-m-d', strtotime($filter["date1"] ?? 'now'));
        $date2 = date('Y-m-d', strtotime($filter["date2"] ?? 'now'));
        if ($date1 === '1970-01-01') $date1 = date('Y-m-d');
        if ($date2 === '1970-01-01') $date2 = date('Y-m-d');

        $bindtypes = "";
        $bindvalues = [];

        // Логика фильтров времени и сессий
        $session_id = (int)($filter["session_id"] ?? 0);
        if ($session_id > 0) {
            $date_filter = "";
            $session_id_filter = " AND oh.f_cash_session_id = ? ";
            $bindtypes .= "i";
            $bindvalues[] = $session_id;
        } else {
            $date_filter = " AND cast(cs.f_date_open as date) BETWEEN ? AND ? ";
            $session_id_filter = "";
            $bindtypes .= "ss";
            $bindvalues[] = $date1;
            $bindvalues[] = $date2;
        }

        // Фильтр по группе товаров
        $group_id = (int)($filter["group_id"] ?? 0);
        $group_filter = "";
        if ($group_id > 0) {
            $group_filter = " AND g.f_group = ? ";
            $bindtypes .= "i";
            $bindvalues[] = $group_id;
        }

        // Выбор режима: Суммарно или Детально
        if ($summarize == 1) {
            $select_fields = "
            st.f_name as f_store_name,
            gr.f_name as f_group_name,
            g.f_name as f_item_name,
            g2.f_name as f_consumed_item_name,
            money_fmt(SUM(og.f_qty)) as f_qty,
            money_fmt(SUM(og.f_total)) as f_total,
            money_fmt(SUM(ogc.f_qty)) as f_consumed_qty,
            money_fmt(SUM(ogc.f_qty * ogc.f_price)) as f_consumed_total
        ";
            $group_by = "GROUP BY st.f_name, gr.f_name, g.f_name, g2.f_name";
        } else {
            $select_fields = "
            st.f_name as f_store_name,
            gr.f_name as f_group_name,
            g.f_name as f_item_name,
            g2.f_name as f_consumed_item_name,
            money_fmt(og.f_qty) as f_qty,
            money_fmt(og.f_total) as f_total,
            money_fmt(ogc.f_qty) as f_consumed_qty,
            money_fmt(ogc.f_qty * ogc.f_price) as f_consumed_total
        ";
            // Группируем по ID записи, чтобы получить детальный список
            $group_by = "GROUP BY ogc.f_id, st.f_name, gr.f_name, g.f_name, g2.f_name";
        }

        $order_by = "ORDER BY st.f_name, gr.f_name, g.f_name";

        $sql = <<<EOD
    SELECT $select_fields
    FROM store_calc_queue ogc
    LEFT JOIN o_goods og ON og.f_id = ogc.f_row_sale_id
    LEFT JOIN c_storages st ON st.f_id = ogc.f_store_id
    LEFT JOIN c_goods g ON g.f_id = og.f_goods
    LEFT JOIN c_goods g2 ON g2.f_id = ogc.f_item_id
    LEFT JOIN c_groups gr ON gr.f_id = g.f_group
    LEFT JOIN o_header oh ON oh.f_id = og.f_header
    LEFT JOIN cash_session cs ON cs.f_id = oh.f_cash_session_id
    WHERE oh.f_state = 2 AND og.f_state = 1 
    $date_filter 
    $session_id_filter
    $group_filter
    $group_by
    $order_by
    EOD;

        $headers = [
            Translator::t("Storage"),
            Translator::t("Group"),
            Translator::t("Item name"),
            Translator::t("Consumed item"),
            Translator::t("Sold qty"),
            Translator::t("Sold sum"),
            Translator::t("Consumed qty"),
            Translator::t("Consumed amount"),
        ];

        return [
            "rows" => $this->db->select($sql, $bindtypes, $bindvalues)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => false, "reload" => true, "filter" => true],
            "headers" => $headers,
            "hidden_columns" => [0],
            "sum" => [4, 5, 6, 7],
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
