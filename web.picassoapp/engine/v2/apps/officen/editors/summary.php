<?php
# © 2026 , Kudryashov Vasili

class Summary
{
    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    public function money_fmt($value)
    {
        $val = (float)$value;
        return number_format($val, 2, '.', ',');
    }

    private function parseFilter($params)
    {
        $filterRaw = $params->filter ?? [];
        $filter = [];
        foreach ($filterRaw as $item) {
            if (is_object($item) || is_array($item)) {
                foreach ($item as $k => $v) $filter[$k] = $v;
            }
        }
        return $filter;
    }

    private function formatForClient($data)
    {
        ksort($data);
        $rows = [];
        foreach ($data as $d => $v) {
            $rows[] = [
                $d,
                $this->money_fmt($v['revenue']),
                $this->money_fmt($v['cost_price']),
                $this->money_fmt($v['salary']),
                $this->money_fmt($v['procurement']),
                $this->money_fmt($v['other_expenses']),
                $this->money_fmt($v['profit'])
            ];
        }

        return [
            "rows" => $rows,
            "toolbar" => ["reload" => true, "filter" => true],
            "headers" => [
                Translator::t("Date"),
                Translator::t("Revenue"),
                Translator::t("Cost Price"),
                Translator::t("Salary"),
                Translator::t("Procurement"),
                Translator::t("Other Expenses"),
                Translator::t("Profit"),
            ],
            "sum" => [1, 2, 3, 4, 5, 6],
            "filter" => [
                ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
                ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
            ]
        ];
    }

    private function initRow($date)
    {
        return [
            'date' => $date,
            'revenue' => 0,
            'cost_price' => 0,
            'salary' => 0,
            'procurement' => 0,
            'other_expenses' => 0,
            'profit' => 0
        ];
    }

    public function Get($params)
    {
        $filter = $this->parseFilter($params);
        $date1 = $filter["date1"] ?? date('Y-m-01');
        $date2 = $filter["date2"] ?? date('Y-m-t');

        $dailyReport = [];

        // 1. ВЫРУЧКА (Берем из заголовков заказов)
        $sqlRevenue = "SELECT CAST(f_datecash AS DATE) as f_date, SUM(f_amounttotal) as revenue 
                       FROM o_header 
                       WHERE f_datecash BETWEEN ? AND ? AND f_state=2 
                       GROUP BY f_date";
        $revData = $this->db->select($sqlRevenue, "ss", [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($revData as $row) {
            $date = $row['f_date'];
            $dailyReport[$date] = $this->initRow($date);
            $dailyReport[$date]['revenue'] = (float)$row['revenue'];
        }

        // 2. СЕБЕСТОИМОСТЬ (Расчет по складской очереди)
        $sqlCost = "SELECT CAST(o.f_datecash AS DATE) as f_date, SUM(st.f_qty * st.f_price) as cost_price 
                    FROM store_calc_queue st
                    LEFT JOIN o_header o ON o.f_id = st.f_doc_sale_id
                    WHERE o.f_datecash BETWEEN ? AND ? AND o.f_state=2 
                    GROUP BY f_date";
        $costData = $this->db->select($sqlCost, "ss", [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($costData as $row) {
            $date = $row['f_date'];
            if (!isset($dailyReport[$date])) $dailyReport[$date] = $this->initRow($date);
            $dailyReport[$date]['cost_price'] = (float)$row['cost_price'];
        }

        // 3. ЗАКУП (Накладные от поставщиков)
        $sqlProcurement = "SELECT CAST(f_doc_date AS DATE) as f_date, SUM(f_sum) as procurement 
                            FROM store_document 
                            WHERE f_doc_date BETWEEN ? AND ? AND f_doc_type=1 
                            GROUP BY f_date";
        $procData = $this->db->select($sqlProcurement, "ss", [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($procData as $row) {
            $date = $row['f_date'];
            if (!isset($dailyReport[$date])) $dailyReport[$date] = $this->initRow($date);
            $dailyReport[$date]['procurement'] = (float)$row['procurement'];
        }

        // 4. ЗАРПЛАТА
        $sqlSalary = "SELECT CAST(f_date AS DATE) as f_date, SUM(f_amount_credit) as salary 
                      FROM s_salary 
                      WHERE f_date BETWEEN ? AND ? and f_type=1
                      GROUP BY f_date";
        $salData = $this->db->select($sqlSalary, "ss", [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($salData as $row) {
            $date = $row['f_date'];
            if (!isset($dailyReport[$date])) $dailyReport[$date] = $this->initRow($date);
            $dailyReport[$date]['salary'] = (float)$row['salary'];
        }

        // 5. ПРОЧИЕ ТРАТЫ (Теперь берем строго КРЕДИТ из кассовых операций)
        // Важно: CAST(f_datetime AS DATE) обязателен, иначе GROUP BY будет по секундам
        $sqlOther = "SELECT CAST(f_datetime AS DATE) as f_date, SUM(f_credit) as other_expenses 
                     FROM cash_operations 
                     WHERE f_datetime BETWEEN ? AND ? 
                     AND f_order_id IS NULL 
                     GROUP BY f_date";
        $otherData = $this->db->select($sqlOther, "ss", [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($otherData as $row) {
            $date = $row['f_date'];
            if (!isset($dailyReport[$date])) $dailyReport[$date] = $this->initRow($date);
            $dailyReport[$date]['other_expenses'] = (float)$row['other_expenses'];
        }

        // 6. РАСЧЕТ ИТОГА
        foreach ($dailyReport as &$day) {
            // Прибыль = Выручка - Себестоимость - ЗП - Прочие расходы.
            // Закуп (procurement) обычно не вычитают из прибыли напрямую, 
            // так как это "замороженные" деньги в товаре, но если у тебя такая логика - ок.
            $day['profit'] = $day['revenue'] - $day['cost_price'] - $day['salary'] - $day['other_expenses'];
        }

        return $this->formatForClient($dailyReport);
    }
}
