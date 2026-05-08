<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-04-29 10:14:36

class Salary
{
    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    /**
     * Основной метод получения остатков
     */
    public function get($params)
    {
        $filterRaw = $params->filter ?? [];
        $filter = [];
        foreach ($filterRaw as $item) {
            foreach ((array)$item as $k => $v) {
                $filter[$k] = $v;
            }
        }

        $viewMode = (int) ($filter["viewmode"] ?? 1);

        // Переключаем логику в зависимости от режима отображения
        if ($viewMode === 1) {
            return $this->getTotal($filter);
        } else if ($viewMode === 2) {
            return $this->getAccrualPayments($filter);
        } else if ($viewMode === 3) {
            return $this->getHistoricalDetailed($filter);
        } else if ($viewMode === 4) {
            return $this->getAttendanceTotal($filter);
        } else if ($viewMode === 5) {
            return $this->getAttendanceDetailed($filter);
        }
    }


    protected function getTotal($filter)
    {

        $sql = <<<SQL
        SELECT CONCAT(u.f_last, ' ', u.f_first) AS f_worker_name, 
        SUM(s.f_amount_credit)-SUM(s.f_amount_debit)
        FROM s_salary s 
        LEFT JOIN s_user u ON u.f_id=s.f_staff
        group by 1
        SQL;
        $headers = [Translator::t("Worker"), Translator::t("Amount")];
        return [
            "sql" => !(empty($params->debug)) ? $sql : "",
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => true, "reload" => true, "filter" => true, "new" => true],
            "headers" => $headers,
            "hidden_columns" => [],
            "sum" => [1],
            "filter" => $this->getFilterConfig()
        ];
    }

    protected function getAccrualPayments($filter)
    {
        // Собираем условия фильтрации (по датам, если они переданы)
        $where = ["1=1"];
        if (!empty($filter['date1'])) {
            $where[] = "s.f_date >= '" . $filter['date1'] . "'";
        }
        if (!empty($filter['date2'])) {
            $where[] = "s.f_date <= '" . $filter['date2'] . "'";
        }

        $sql = "
        SELECT 
            s.f_date, 
            SUM(COALESCE(s.f_amount_credit, 0)) AS total_accrued, 
            SUM(COALESCE(s.f_amount_debit, 0)) AS total_paid
        FROM s_salary s 
        WHERE " . implode(" AND ", $where) . "
        GROUP BY s.f_date
        ORDER BY s.f_date DESC
    ";

        $headers = [
            Translator::t("Date"),
            Translator::t("Accrual"),
            Translator::t("Payments")
        ];

        return [
            "sql" => $sql, // Для отладки
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => false, "reload" => true, "filter" => true, "new" => true],
            "headers" => $headers,
            "sum" => [1, 2], // Итоговые суммы по колонкам Начислено и Выплачено
            "filter" => $this->getFilterConfig()
        ];
    }


    protected function getHistoricalDetailed($filter)
    {
        $where = ["1=1"];

        // Убираем real_escape_string, используем переменные напрямую, как в твоих примерах выше
        if (!empty($filter['date1'])) {
            $where[] = "s.f_date >= '" . $filter['date1'] . "'";
        }
        if (!empty($filter['date2'])) {
            $where[] = "s.f_date <= '" . $filter['date2'] . "'";
        }
        if (!empty($filter['user'])) {
            $where[] = "s.f_staff = " . (int)$filter['user'];
        }
        if (!empty($filter['group'])) {
            $where[] = "s.f_position = " . (int)$filter['group'];
        }

        $sql = "
            SELECT 
                s.f_date, 
                CONCAT(u.f_last, ' ', u.f_first) AS f_worker_name,
                SUM(COALESCE(s.f_amount_credit, 0)) AS total_accrued, 
                SUM(COALESCE(s.f_amount_debit, 0)) AS total_paid
            FROM s_salary s 
            LEFT JOIN s_user u ON u.f_id = s.f_staff
            WHERE " . implode(" AND ", $where) . "
            GROUP BY s.f_date, s.f_staff
            ORDER BY s.f_date DESC, f_worker_name ASC
        ";

        $headers = [
            Translator::t("Date"),
            Translator::t("Worker"),
            Translator::t("Accrual"),
            Translator::t("Payments")
        ];

        return [
            "sql" => $sql,
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => false, "reload" => true, "filter" => true, "new" => true],
            "headers" => $headers,
            "hidden_columns" => [],
            "sum" => [2, 3],
            "filter" => $this->getFilterConfig()
        ];
    }

    protected function getAttendanceTotal($filter)
    {
        $where = ["1=1"];
        if (!empty($filter['date1'])) {
            $where[] = "f_date >= '" . $filter['date1'] . "'";
        }
        if (!empty($filter['date2'])) {
            $where[] = "f_date <= '" . $filter['date2'] . "'";
        }
        if (!empty($filter['user'])) {
            $where[] = "f_worker = " . (int)$filter['user'];
        }

        $sql = "
        SELECT 
            CONCAT(u.f_last, ' ', u.f_first) AS f_worker_name,
            -- Считаем разницу в минутах, вычитаем перерывы и переводим в формат ЧЧ:ММ
            SEC_TO_TIME(SUM(TIMESTAMPDIFF(SECOND, f_in, f_out) - (f_break_minutes * 60))) AS total_time_formatted,
            -- Для удобства расчетов в PHP оставляем сумму в чистых минутах
            ROUND(SUM(TIMESTAMPDIFF(MINUTE, f_in, f_out) - f_break_minutes), 2) AS total_minutes
        FROM s_attendance a
        LEFT JOIN s_user u ON u.f_id = a.f_worker
        WHERE " . implode(" AND ", $where) . " AND f_out IS NOT NULL
        GROUP BY f_worker
        ORDER BY f_worker_name ASC
    ";

        return [
            "sql" => $sql,
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["reload" => true, "filter" => true],
            "headers" => [Translator::t("Worker"), Translator::t("Total Time (HH:MM)"), Translator::t("Total Minutes")],
            "sum" => [2], // Суммируем минуты в подвале
            "filter" => $this->getFilterConfig()
        ];
    }

    protected function getAttendanceDetailed($filter)
    {
        $where = ["1=1"];
        if (!empty($filter['date1'])) {
            $where[] = "f_date >= '" . $filter['date1'] . "'";
        }
        if (!empty($filter['date2'])) {
            $where[] = "f_date <= '" . $filter['date2'] . "'";
        }
        if (!empty($filter['user'])) {
            $where[] = "f_worker = " . (int)$filter['user'];
        }

        // ВОТ ЭТОЙ СТРОЧКИ НЕ ХВАТАЛО:
        $whereClause = implode(" AND ", $where);

        $sql = <<<SQL
        SELECT 
            f_date,
            CONCAT(u.f_last, ' ', u.f_first) AS f_worker_name,
            DATE_FORMAT(f_in, '%d.%m %H:%i') as time_in,
            DATE_FORMAT(f_out, '%d.%m %H:%i') as time_out,
            f_break_minutes,
            TIMESTAMPDIFF(MINUTE, f_in, f_out) - f_break_minutes AS net_minutes,
            f_comment
        FROM s_attendance a
        LEFT JOIN s_user u ON u.f_id = a.f_worker
        WHERE {$whereClause}
        ORDER BY f_in DESC
SQL;

        return [
            "sql" => $sql,
            "rows" => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => true, "reload" => true, "filter" => true],
            "headers" => [
                Translator::t("Date"),
                Translator::t("Worker"),
                Translator::t("In"),
                Translator::t("Out"),
                Translator::t("Break"),
                Translator::t("Net"),
                Translator::t("Comment")
            ],
            "sum" => [5],
            "filter" => $this->getFilterConfig()
        ];
    }

    private function getFilterConfig()
    {
        return [
            ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
            ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
            ["type" => "combobox", "name" => "viewmode", "label" => Translator::t("View mode"), "default" => 1, "values" => [
                ["label" => Translator::t("Current total"), "value" => 1],
                ["label" => Translator::t("Historical accyral / payments"), "value" => 2],
                ["label" => Translator::t("Historical detailed"), "value" => 3],
                ["label" => Translator::t("Attendance total"), "value" => 4],
                ["label" => Translator::t("Attendance detailed"), "value" => 5],
            ]],
            ["type" => "keyvalue", "name" => "user", "label" => Translator::t("Worker"), "function" => "employee"],
            ["type" => "keyvalue", "name" => "group", "label" => Translator::t("Position"), "function" => "employee_group"],
        ];
    }
}
