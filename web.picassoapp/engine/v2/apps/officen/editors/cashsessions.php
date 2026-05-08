<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-28 21:28:31
# Last Modified: 2026-01-28 21:28:34

class CashSessions
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
            if (is_object($item) || is_array($item)) {
                foreach ($item as $k => $v) {
                    $filter[$k] = $v;
                }
            }
        }

        $date1 = $filter["date1"] ?? date('Y-m-d');
        $date2 = $filter["date2"] ?? date('Y-m-d');
        $date1 = date('Y-m-d', strtotime($date1));
        $date2 = date('Y-m-d', strtotime($date2));

        // Валидация дат
        if (!$date1 || $date1 === '1970-01-01') $date1 = date('Y-m-d');
        if (!$date2 || $date2 === '1970-01-01') $date2 = date('Y-m-d');

        // ЛОГИКА ПЕРЕКЛЮЧЕНИЯ ФИЛЬТРА
        $dateMode = (int)($filter["datemode"] ?? 1);

        if ($dateMode === 1) {
            // Фильтр по дате закрытия заказа (oh.f_datecash)
            $date_filter = "and oh.f_datecash BETWEEN '$date1' AND '$date2' ";
        } else {
            // Фильтр по дате открытия смены (cs.f_date_open)
            $date_filter = "and cast(cs.f_date_open as date) BETWEEN '$date1' AND '$date2' ";
        }

        $session_id_filter = "";
        $session_id = (int) ($filter["session_id"] ?? 0);
        if ($session_id > 0) {
            $session_id_filter = " and oh.f_cash_session_id=$session_id ";
            $date_filter = "";
        }

        $payConfig = require __DIR__ . "/../../worker/dict-payment.php";
        $fieldsSql = [];
        foreach ($payConfig["fields"] as $type => $fieldName) {
            $fieldsSql[] = "money_fmt(CAST(JSON_VALUE(oh.f_data, '$.$fieldName') AS DECIMAL(14,2))) AS $fieldName";
        }
        $other_payments = implode(",\n        ", $fieldsSql);

        $sql = <<<EOD
    SELECT oh.f_id, 
    oh.f_cash_session_id,
    oh.f_prefix,
    h.f_name,
    t.f_name,
    concat(date_fmt(JSON_VALUE(oh.f_data, '$.f_date_open')), ' ', JSON_VALUE(oh.f_data, '$.f_time_open')) AS f_datetime_open,
    concat(date_fmt(JSON_VALUE(oh.f_data, '$.f_date_close')), ' ', JSON_VALUE(oh.f_data, '$.f_time_close')) AS f_datetime_close,
    CONCAT(u1.f_last, ' ', LEFT(u1.f_first, 1), '.') AS f_staff,
    CONCAT(u2.f_last, ' ', LEFT(u2.f_first, 1), '.') AS f_cashier,
    money_fmt(oh.f_amounttotal), $other_payments,
    money_fmt(CAST(JSON_VALUE(oh.f_data, '$.f_service_amount') AS DECIMAL(14,2))) AS f_service_amount
    FROM o_header oh
    LEFT JOIN s_user u1 ON u1.f_id=oh.f_staff
    LEFT JOIN s_user u2 ON u2.f_id=oh.f_cashier
    LEFT JOIN cash_session cs ON cs.f_id=oh.f_cash_session_id
    left join h_tables t on t.f_id=oh.f_table
    left join h_halls h on h.f_id=t.f_hall
    WHERE oh.f_state=2 $date_filter $session_id_filter
    ORDER BY oh.f_cash_session_id,
    STR_TO_DATE(CONCAT(JSON_VALUE(oh.f_data, '$.f_date_close'),' ', JSON_VALUE(oh.f_data, '$.f_time_close')), '%Y-%m-%d %H:%i:%s') 
    EOD;

        $headers = [
            Translator::t("Id"),
            Translator::t("Cash session"),
            Translator::t("Order"),
            Translator::t("Hall"),
            Translator::t("Table"),
            Translator::t("Open date"),
            Translator::t("Close date"),
            Translator::t("Staff"),
            Translator::t("Cashier"),
            Translator::t("Total"),
        ];

        foreach ($payConfig["names"] as $name) {
            $headers[] = Translator::t($name);
        }
        $headers[] = Translator::t("Service amount");

        return [
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => true, "reload" => true, "filter" => true],
            "headers" => $headers,
            "hidden_columns" => [0],
            "sum" => [9, 10, 11, 12, 13, 14, 15], // Поправил индексы суммы (Total начинается с 9)
            "filter" => [
                ["type" => "combobox", "name" => "datemode", "label" => Translator::t("Filter by date type"), "default" => 1, "values" => [
                    ["label" => Translator::t("Order closing date"), "value" => 1],
                    ["label" => Translator::t("Shift opening date"), "value" => 2]
                ]],
                ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
                ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
                ["type" => "number", "name" => "session_id", "label" => Translator::t("Session id")],
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
