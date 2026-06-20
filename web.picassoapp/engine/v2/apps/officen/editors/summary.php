<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . '/../../worker/dict-cash-operation-type.php';

class Summary
{
    private $db;

    /** @var array<string,mixed>|null */
    private static $paymentConfig = null;

    public function __construct($db)
    {
        $this->db = $db;
    }

    public function money_fmt($value)
    {
        $val = (float)$value;
        return number_format($val, 2, '.', ',');
    }

    /**
     * @return array<string,mixed>
     */
    private function paymentConfig(): array
    {
        if (self::$paymentConfig !== null) {
            return self::$paymentConfig;
        }

        $path = __DIR__ . '/../../worker/dict-payment.php';
        if (!is_file($path)) {
            self::$paymentConfig = ['types' => [], 'fields' => [], 'names' => []];
            return self::$paymentConfig;
        }

        if (!defined('PAYMENT_TYPE_CASH')) {
            self::$paymentConfig = require $path;
        } else {
            self::$paymentConfig = [
                'types' => [1, 2, 3, 4, 5, 6, 7, 8, 9],
                'fields' => [
                    1 => 'f_amount_cash', 2 => 'f_amount_card', 3 => 'f_amount_bank', 4 => 'f_amount_idram',
                    5 => 'f_amount_complimentary', 6 => 'f_amount_other', 7 => 'f_amount_telcell',
                    8 => 'f_amount_debt', 9 => 'f_amount_prepaid',
                ],
                'names' => [
                    1 => 'Cash', 2 => 'Card', 3 => 'Bank', 4 => 'Idram', 5 => 'Complimentary',
                    6 => 'Other', 7 => 'Telcell', 8 => 'Debt', 9 => 'Prepaid',
                ],
            ];
        }

        return self::$paymentConfig;
    }

    private function normalizeReportDate($date): string
    {
        if ($date instanceof \DateTimeInterface) {
            return $date->format('Y-m-d');
        }
        $s = trim((string)$date);
        return strlen($s) >= 10 ? substr($s, 0, 10) : $s;
    }

    private function parseFilter($params)
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
        return $filter;
    }

    /**
     * @param array<string> $paymentFieldKeys
     * @return array<string,mixed>
     */
    private function initRow($date, array $paymentFieldKeys = [])
    {
        $payments = [];
        foreach ($paymentFieldKeys as $fk) {
            $payments[$fk] = 0.0;
        }

        return [
            'date' => $date,
            'revenue' => 0,
            'payments' => $payments,
            'cost_price' => 0,
            'salary' => 0,
            'procurement' => 0,
            'other_expenses' => 0,
            'profit' => 0,
        ];
    }

    /**
     * @param array<string,array<string,mixed>> $data
     * @param array<string,mixed> $payConfig
     */
    private function formatForClient(array $data, bool $showPayments, array $payConfig)
    {
        ksort($data);

        $headers = [
            Translator::t('Date'),
            Translator::t('Revenue'),
        ];

        if ($showPayments) {
            foreach ($payConfig['types'] as $pt) {
                $headers[] = Translator::t($payConfig['names'][$pt] ?? (string)$pt);
            }
        }

        $headers = array_merge($headers, [
            Translator::t('Cost Price'),
            Translator::t('Salary'),
            Translator::t('Procurement'),
            Translator::t('Other Expenses'),
            Translator::t('Profit'),
        ]);

        $sumCols = [1];
        $col = 2;
        if ($showPayments) {
            foreach ($payConfig['types'] as $pt) {
                $sumCols[] = $col++;
            }
        }
        $sumCols = array_merge($sumCols, [$col, $col + 1, $col + 2, $col + 3, $col + 4]);

        $rows = [];
        foreach ($data as $d => $v) {
            $row = [
                $d,
                $this->money_fmt($v['revenue']),
            ];

            if ($showPayments) {
                foreach ($payConfig['types'] as $pt) {
                    $fk = $payConfig['fields'][$pt] ?? '';
                    $row[] = $this->money_fmt($v['payments'][$fk] ?? 0);
                }
            }

            $row[] = $this->money_fmt($v['cost_price']);
            $row[] = $this->money_fmt($v['salary']);
            $row[] = $this->money_fmt($v['procurement']);
            $row[] = $this->money_fmt($v['other_expenses']);
            $row[] = $this->money_fmt($v['profit']);
            $rows[] = $row;
        }

        return [
            'rows' => $rows,
            'toolbar' => ['reload' => true, 'filter' => true],
            'headers' => $headers,
            'sum' => $sumCols,
            'filter' => [
                ['type' => 'date', 'name' => 'date1', 'label' => Translator::t('Date start')],
                ['type' => 'date', 'name' => 'date2', 'label' => Translator::t('Date end')],
                ['type' => 'combobox', 'name' => 'show_payments', 'label' => Translator::t('Show payment methods'), 'default' => 0, 'values' => [
                    ['label' => Translator::t('No'), 'value' => 0],
                    ['label' => Translator::t('Yes'), 'value' => 1],
                ]],
            ],
        ];
    }

    public function Get($params)
    {
        $filter = $this->parseFilter($params);
        $date1 = $filter['date1'] ?? date('Y-m-01');
        $date2 = $filter['date2'] ?? date('Y-m-t');
        $date1 = date('Y-m-d', strtotime($date1));
        $date2 = date('Y-m-d', strtotime($date2));
        if ($date1 === '1970-01-01') {
            $date1 = date('Y-m-01');
        }
        if ($date2 === '1970-01-01') {
            $date2 = date('Y-m-t');
        }
        $showPayments = (int)($filter['show_payments'] ?? 0) === 1;

        $payConfig = $this->paymentConfig();
        $paymentFieldKeys = [];
        if ($showPayments) {
            foreach ($payConfig['types'] as $pt) {
                $fk = $payConfig['fields'][$pt] ?? '';
                if ($fk !== '') {
                    $paymentFieldKeys[] = $fk;
                }
            }
        }

        $dailyReport = [];

        // 1. ВЫРУЧКА (Берем из заголовков заказов)
        $sqlRevenue = "SELECT CAST(f_datecash AS DATE) as f_date, SUM(f_amounttotal) as revenue 
                       FROM o_header 
                       WHERE f_datecash BETWEEN ? AND ? AND f_state=2 
                       GROUP BY f_date";
        $revData = $this->db->select($sqlRevenue, 'ss', [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($revData as $row) {
            $date = $this->normalizeReportDate($row['f_date']);
            $dailyReport[$date] = $this->initRow($date, $paymentFieldKeys);
            $dailyReport[$date]['revenue'] = (float)$row['revenue'];
        }

        if ($showPayments && !empty($paymentFieldKeys)) {
            $sumParts = [];
            foreach ($paymentFieldKeys as $field) {
                $sumParts[] = "SUM(COALESCE(CAST(JSON_VALUE(f_data, '\$.$field') AS DECIMAL(14,2)), 0)) AS `$field`";
            }
            $sqlPayments = 'SELECT CAST(f_datecash AS DATE) AS f_date, ' . implode(', ', $sumParts) . '
                       FROM o_header
                       WHERE f_datecash BETWEEN ? AND ? AND f_state=2
                       GROUP BY f_date';
            $payData = $this->db->select($sqlPayments, 'ss', [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
            foreach ($payData as $row) {
                $date = $this->normalizeReportDate($row['f_date']);
                if (!isset($dailyReport[$date])) {
                    $dailyReport[$date] = $this->initRow($date, $paymentFieldKeys);
                }
                foreach ($paymentFieldKeys as $field) {
                    $dailyReport[$date]['payments'][$field] = (float)($row[$field] ?? 0);
                }
            }
        }

        // 2. СЕБЕСТОИМОСТЬ (Расчет по складской очереди)
        $sqlCost = "SELECT CAST(o.f_datecash AS DATE) as f_date, SUM(st.f_qty * st.f_price) as cost_price 
                    FROM store_calc_queue st
                    LEFT JOIN o_header o ON o.f_id = st.f_doc_sale_id
                    WHERE o.f_datecash BETWEEN ? AND ? AND o.f_state=2 
                    GROUP BY f_date";
        $costData = $this->db->select($sqlCost, 'ss', [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($costData as $row) {
            $date = $this->normalizeReportDate($row['f_date']);
            if (!isset($dailyReport[$date])) {
                $dailyReport[$date] = $this->initRow($date, $paymentFieldKeys);
            }
            $dailyReport[$date]['cost_price'] = (float)$row['cost_price'];
        }

        // 3. ЗАКУП (Накладные от поставщиков)
        $sqlProcurement = "SELECT CAST(f_doc_date AS DATE) as f_date, SUM(f_sum) as procurement 
                            FROM store_document 
                            WHERE f_doc_date BETWEEN ? AND ? AND f_doc_type=1 
                            GROUP BY f_date";
        $procData = $this->db->select($sqlProcurement, 'ss', [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($procData as $row) {
            $date = $this->normalizeReportDate($row['f_date']);
            if (!isset($dailyReport[$date])) {
                $dailyReport[$date] = $this->initRow($date, $paymentFieldKeys);
            }
            $dailyReport[$date]['procurement'] = (float)$row['procurement'];
        }

        // 4. ЗАРПЛАТА
        $sqlSalary = "SELECT CAST(f_date AS DATE) as f_date, SUM(f_amount_credit) as salary 
                      FROM s_salary 
                      WHERE f_date BETWEEN ? AND ? and f_type=1
                      GROUP BY f_date";
        $salData = $this->db->select($sqlSalary, 'ss', [$date1, $date2])->fetch_all(MYSQLI_ASSOC);
        foreach ($salData as $row) {
            $date = $this->normalizeReportDate($row['f_date']);
            if (!isset($dailyReport[$date])) {
                $dailyReport[$date] = $this->initRow($date, $paymentFieldKeys);
            }
            $dailyReport[$date]['salary'] = (float)$row['salary'];
        }

        // 5. ПРОЧИЕ ТРАТЫ — cash_operations без заказа, только типы «прочий расход»:
        // 2 Total Expenses, 6 Debt Repayment, 7 Utilities, 8 Cash Shortage.
        // Не включаем: 1 выручка, 3 закуп (store_document), 4 зарплата (s_salary),
        // 5 взыскание долга, 9 излишек, 10 доставка, 11/12 переводы между кассами.
        $otherExpenseOpTypes = [
            CASH_OP_TOTAL_EXPENSES,
            CASH_OP_DEBT_REPAYMENT,
            CASH_OP_UTILITIES,
            CASH_OP_CASH_SHORTAGE,
        ];
        $opPlaceholders = implode(',', array_fill(0, count($otherExpenseOpTypes), '?'));
        // f_order_id: ручные операции из FrontDesk/Waiter сохраняются как '' (не NULL).
        $sqlOther = "SELECT CAST(f_datetime AS DATE) as f_date,
                            SUM(GREATEST(f_credit, f_debit)) as other_expenses
                     FROM cash_operations 
                     WHERE CAST(f_datetime AS DATE) BETWEEN ? AND ? 
                     AND (f_order_id IS NULL OR TRIM(f_order_id) = '')
                     AND f_operation_type IN ($opPlaceholders)
                     AND (f_credit > 0 OR f_debit > 0)
                     GROUP BY f_date";
        $otherBindTypes = 'ss' . str_repeat('i', count($otherExpenseOpTypes));
        $otherBindValues = array_merge([$date1, $date2], $otherExpenseOpTypes);
        $otherData = $this->db->select($sqlOther, $otherBindTypes, $otherBindValues)->fetch_all(MYSQLI_ASSOC);
        foreach ($otherData as $row) {
            $date = $this->normalizeReportDate($row['f_date']);
            if (!isset($dailyReport[$date])) {
                $dailyReport[$date] = $this->initRow($date, $paymentFieldKeys);
            }
            $dailyReport[$date]['other_expenses'] = (float)$row['other_expenses'];
        }

        // 6. РАСЧЕТ ИТОГА
        foreach ($dailyReport as &$day) {
            $day['profit'] = $day['revenue'] - $day['cost_price'] - $day['salary'] - $day['other_expenses'];
        }
        unset($day);

        return $this->formatForClient($dailyReport, $showPayments, $payConfig);
    }
}
