<?php
# © 2026 , Kudryashov Vasili
# Kitchen queue report for OfficeN (same data as waiter in-progress).

require_once __DIR__ . '/../../../worker/helper.php';

class OrderInProgress
{
    private $db;

    /** @var array<string,mixed>|null */
    private static $paymentConfig = null;

    /** @var array<int,string> */
    private const STATUS_LABELS = [
        1 => 'Accepted',
        2 => 'Cooking',
        3 => 'Ready',
        4 => 'Served',
        6 => 'Closed',
    ];

    public function __construct($db)
    {
        $this->db = $db;
    }

    /**
     * @param array<int,array<string,mixed>> $rows
     * @return array<int,array<string,mixed>>
     */
    private function aggregateKitchenOrders(array $rows): array
    {
        $map = [];

        foreach ($rows as $r) {
            $hid = $r['f_header_id'];

            if (!isset($map[$hid])) {
                $map[$hid] = [
                    'f_header_id' => $hid,
                    'f_order_prefix' => $r['f_order_prefix'],
                    'f_table_name' => $r['f_table_name'],
                    'f_hall_name' => $r['f_hall_name'],
                    'f_date_open' => $r['f_date_open'],
                    'f_time_open' => $r['f_time_open'],
                    'f_amounttotal' => (float)($r['f_amounttotal'] ?? 0),
                    'f_amount_cash' => (float)($r['f_amount_cash'] ?? 0),
                    'f_amount_card' => (float)($r['f_amount_card'] ?? 0),
                    'f_amount_bank' => (float)($r['f_amount_bank'] ?? 0),
                    'f_amount_idram' => (float)($r['f_amount_idram'] ?? 0),
                    'f_amount_complimentary' => (float)($r['f_amount_complimentary'] ?? 0),
                    'f_amount_other' => (float)($r['f_amount_other'] ?? 0),
                    'f_amount_telcell' => (float)($r['f_amount_telcell'] ?? 0),
                    'f_amount_debt' => (float)($r['f_amount_debt'] ?? 0),
                    'f_amount_prepaid' => (float)($r['f_amount_prepaid'] ?? 0),
                    'f_cost' => (float)($r['f_cost'] ?? 0),
                    'f_service_comment' => (string)($r['f_service_comment'] ?? ''),
                    'f_discount_comment' => (string)($r['f_discount_comment'] ?? ''),
                    'f_service_factor' => (float)($r['f_service_factor'] ?? 0),
                    'f_discount_factor' => (float)($r['f_discount_factor'] ?? 0),
                    'f_guest_name' => $r['f_guest_name'],
                    'f_guest_phone' => $r['f_guest_phone'],
                    'f_guest_address' => $r['f_guest_address'],
                    'lines' => [],
                ];
            }

            $map[$hid]['lines'][] = [
                'f_qty' => (float)$r['f_qty'],
                'f_goods_name' => $r['f_goods_name'],
                'f_status' => (int)$r['f_status'],
                'f_comment' => $r['f_comment'],
                'f_kitchen_time' => trim((string)($r['f_kitchen_time'] ?? '')),
            ];
        }

        return array_values($map);
    }

    /**
     * @param array<string,mixed> $filter
     */
    private function parseFilter($params): array
    {
        $filter = [];
        $filterRaw = $params->filter ?? [];

        foreach ($filterRaw as $item) {
            foreach ((array)$item as $k => $v) {
                if ($v !== '' && $v !== null) {
                    $filter[$k] = $v;
                }
            }
        }

        return $filter;
    }

    private function formatOpenedCell(string $date, string $time): string
    {
        $datePart = $date;
        if ($date !== '') {
            $ts = strtotime($date);
            if ($ts !== false) {
                $datePart = date('d.m.Y', $ts);
            }
        }
        $timePart = $time;
        if ($time !== '') {
            $ts = strtotime($time);
            if ($ts !== false) {
                $timePart = date('H:i', $ts);
            }
        }
        if ($datePart === '') {
            return $timePart;
        }
        if ($timePart === '') {
            return $datePart;
        }

        return $datePart . "\n" . $timePart;
    }

    private function formatQtyLine(float $qty): string
    {
        $s = rtrim(rtrim(number_format($qty, 3, '.', ''), '0'), '.');
        if (strlen($s) > 5) {
            $s = substr($s, 0, 5);
        }

        return str_pad($s, 5, ' ', STR_PAD_LEFT);
    }

    /**
     * @param array<string,mixed> $order
     */
    private function guestMultiline(array $order): string
    {
        $lines = [];
        $name = trim((string)($order['f_guest_name'] ?? ''));
        $phone = trim((string)($order['f_guest_phone'] ?? ''));
        $addr = trim((string)($order['f_guest_address'] ?? ''));

        if ($name !== '') {
            $lines[] = $name;
        }
        if ($phone !== '') {
            $lines[] = $phone;
        }
        if ($addr !== '') {
            $lines[] = $addr;
        }

        return implode("\n", $lines);
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

        /* Load once; if dict-payment was already included in this request, do not require again. */
        if (!defined('PAYMENT_TYPE_CASH')) {
            self::$paymentConfig = require $path;
        } else {
            self::$paymentConfig = [
                'types' => [1, 2, 3, 4, 5, 6, 7, 8, 9],
                'fields' => [
                    1 => 'f_amount_cash',
                    2 => 'f_amount_card',
                    3 => 'f_amount_bank',
                    4 => 'f_amount_idram',
                    5 => 'f_amount_complimentary',
                    6 => 'f_amount_other',
                    7 => 'f_amount_telcell',
                    8 => 'f_amount_debt',
                    9 => 'f_amount_prepaid',
                ],
                'names' => [
                    1 => 'Cash',
                    2 => 'Card',
                    3 => 'Bank',
                    4 => 'Idram',
                    5 => 'Complimentary',
                    6 => 'Other',
                    7 => 'Telcell',
                    8 => 'Debt',
                    9 => 'Prepaid',
                ],
            ];
        }

        return self::$paymentConfig;
    }

    /**
     * @param array<string,mixed> $order
     * @return array{0: string, 1: float} payment type names (one per line), paid total
     */
    private function paymentBreakdown(array $order): array
    {
        $payment = $this->paymentConfig();
        $types = [];
        $paidTotal = 0.0;

        foreach ($payment['types'] as $pt) {
            $field = $payment['fields'][$pt] ?? '';
            if ($field === '') {
                continue;
            }
            $amt = (float)($order[$field] ?? 0);
            if ($amt > 1e-9) {
                $paidTotal += $amt;
                $types[] = Translator::t($payment['names'][$pt] ?? (string)$pt);
            }
        }

        $typeLine = empty($types) ? Translator::t('No') : implode("\n", $types);

        return [$typeLine, $paidTotal];
    }

    /**
     * Payment method column (type names only).
     *
     * @param array<string,mixed> $order
     */
    private function paymentMethodMultiline(array $order): string
    {
        [$typeLine] = $this->paymentBreakdown($order);

        return $typeLine;
    }

    /**
     * Amount column: order total, plus paid total when it differs.
     *
     * @param array<string,mixed> $order
     */
    private function amountMultiline(array $order): string
    {
        [, $paidTotal] = $this->paymentBreakdown($order);

        $orderTotal = (float)($order['f_amounttotal'] ?? 0);
        $lines = [money_fmt_php($orderTotal)];

        if ($paidTotal > 1e-9 && abs($orderTotal - $paidTotal) > 0.01) {
            $lines[] = money_fmt_php($paidTotal);
        }

        return implode("\n", $lines);
    }

    /**
     * Cost (COGS) column, from store_calc_queue.
     *
     * @param array<string,mixed> $order
     */
    private function costCell(array $order): string
    {
        return money_fmt_php((float)($order['f_cost'] ?? 0));
    }

    /**
     * Revenue minus COGS (f_amounttotal - store_calc_queue cost).
     *
     * @param array<string,mixed> $order
     */
    private function marginCell(array $order): string
    {
        $revenue = (float)($order['f_amounttotal'] ?? 0);
        $cost = (float)($order['f_cost'] ?? 0);

        return money_fmt_php($revenue - $cost);
    }

    /**
     * Service surcharge label from o_header.f_data (f_service_comment).
     *
     * @param array<string,mixed> $order
     */
    private function serviceNameCell(array $order): string
    {
        $factor = (float)($order['f_service_factor'] ?? 0);
        if ($factor <= 1e-9) {
            return '';
        }

        $comment = trim((string)($order['f_service_comment'] ?? ''));

        return $comment !== '' ? $comment : Translator::t('Service');
    }

    /**
     * Discount label from o_header.f_data (f_discount_comment).
     *
     * @param array<string,mixed> $order
     */
    private function discountNameCell(array $order): string
    {
        $factor = abs((float)($order['f_discount_factor'] ?? 0));
        if ($factor <= 1e-9) {
            return '';
        }

        $comment = trim((string)($order['f_discount_comment'] ?? ''));

        return $comment !== '' ? $comment : Translator::t('Discount');
    }

    /**
     * @param array<string,mixed> $order
     */
    private function dishesMultiline(array $order): string
    {
        $lines = [];

        foreach ($order['lines'] as $line) {
            $qty = $this->formatQtyLine((float)($line['f_qty'] ?? 0));
            $name = (string)($line['f_goods_name'] ?? '');
            $comment = trim((string)($line['f_comment'] ?? ''));
            $row = $qty . $name;
            if ($comment !== '') {
                $row .= ' (' . $comment . ')';
            }
            $lines[] = $row;
        }

        return implode("\n", $lines);
    }

    /**
     * @param array<string,mixed> $order
     * @param int $historyFilterStatus 0 = use line f_status; 1..4 = history filter (label + time for that status)
     */
    private function statusMultiline(array $order, int $historyFilterStatus = 0): string
    {
        $lines = [];

        foreach ($order['lines'] as $line) {
            $st = (int)($line['f_status'] ?? 1);
            if ($historyFilterStatus >= 1 && $historyFilterStatus <= 4) {
                $st = $historyFilterStatus;
            }
            $label = Translator::t(self::STATUS_LABELS[$st] ?? (string)$st);
            $time = trim((string)($line['f_kitchen_time'] ?? ''));
            $lines[] = $time !== '' ? ($label . "\n" . $time) : $label;
        }

        return implode("\n", $lines);
    }

    /**
     * @param array<int,array<string,mixed>> $lines
     */
    private function minStatusInLines(array $lines): int
    {
        $mn = 99;

        foreach ($lines as $line) {
            $st = (int)($line['f_status'] ?? 1);
            if ($st >= 1 && $st <= 4 && $st < $mn) {
                $mn = $st;
            }
        }

        if ($mn >= 99) {
            foreach ($lines as $line) {
                if ((int)($line['f_status'] ?? 0) === 6) {
                    return 4;
                }
            }

            return 1;
        }

        return $mn;
    }

    /**
     * @param array<int,array<string,mixed>> $orders
     * @return array<int,array<int,mixed>>
     */
    private function ordersToReportRows(array $orders, bool $historyMode = false, int $kitchenStatus = 0): array
    {
        $rows = [];
        $historyFilterStatus = ($historyMode && $kitchenStatus >= 1 && $kitchenStatus <= 4) ? $kitchenStatus : 0;

        foreach ($orders as $order) {
            if (empty($order['lines'])) {
                continue;
            }

            $rowKitchenStatus = $historyFilterStatus > 0
                ? $historyFilterStatus
                : $this->minStatusInLines($order['lines']);

            $rows[] = [
                $order['f_header_id'],
                $order['f_order_prefix'],
                $this->formatOpenedCell(
                    (string)($order['f_date_open'] ?? ''),
                    (string)($order['f_time_open'] ?? '')
                ),
                $order['f_table_name'],
                $order['f_hall_name'],
                $this->dishesMultiline($order),
                $this->guestMultiline($order),
                $this->paymentMethodMultiline($order),
                $this->amountMultiline($order),
                $this->costCell($order),
                $this->marginCell($order),
                $this->discountNameCell($order),
                $this->serviceNameCell($order),
                $this->statusMultiline($order, $historyFilterStatus),
                $rowKitchenStatus,
            ];
        }

        return $rows;
    }

    private function fetchLiveRows(int $kitchenStatus = 0): array
    {
        $statusWhere = 'ogp.f_status >= 1 AND ogp.f_status <= 4';
        if ($kitchenStatus >= 1 && $kitchenStatus <= 4) {
            $statusWhere = 'ogp.f_status = ' . (int)$kitchenStatus;
        }

        $kitchenTimeExpr = $this->kitchenTimeSql();
        $sql = <<<EOD
        SELECT 
            oh.f_id AS f_header_id,
            oh.f_prefix AS f_order_prefix,
            og.f_qty AS f_qty,
            ogp.f_status,
            {$kitchenTimeExpr} AS f_kitchen_time,
            cg.f_name AS f_goods_name,
            t.f_name AS f_table_name,
            hh.f_name AS f_hall_name,
            COALESCE(json_value(og.f_data, '$.f_comment'), '') AS f_comment,
            COALESCE(json_value(oh.f_data, '$.f_date_open'), '') AS f_date_open,
            COALESCE(json_value(oh.f_data, '$.f_time_open'), '') AS f_time_open,
            COALESCE(oh.f_amounttotal, 0) AS f_amounttotal,
            COALESCE(json_value(oh.f_data, '$.f_amount_cash'), '0') AS f_amount_cash,
            COALESCE(json_value(oh.f_data, '$.f_amount_card'), '0') AS f_amount_card,
            COALESCE(json_value(oh.f_data, '$.f_amount_bank'), '0') AS f_amount_bank,
            COALESCE(json_value(oh.f_data, '$.f_amount_idram'), '0') AS f_amount_idram,
            COALESCE(json_value(oh.f_data, '$.f_amount_complimentary'), '0') AS f_amount_complimentary,
            COALESCE(json_value(oh.f_data, '$.f_amount_other'), '0') AS f_amount_other,
            COALESCE(json_value(oh.f_data, '$.f_amount_telcell'), '0') AS f_amount_telcell,
            COALESCE(json_value(oh.f_data, '$.f_amount_debt'), '0') AS f_amount_debt,
            COALESCE(json_value(oh.f_data, '$.f_amount_prepaid'), '0') AS f_amount_prepaid,
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_name'), '') AS f_guest_name,
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_phone'), '') AS f_guest_phone,
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_address'), '') AS f_guest_address,
            COALESCE(json_value(oh.f_data, '$.f_service_comment'), '') AS f_service_comment,
            COALESCE(json_value(oh.f_data, '$.f_discount_comment'), '') AS f_discount_comment,
            COALESCE(json_value(oh.f_data, '$.f_service_factor'), '0') AS f_service_factor,
            COALESCE(json_value(oh.f_data, '$.f_discount_factor'), '0') AS f_discount_factor,
            COALESCE(scq.f_cost, 0) AS f_cost
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id = ogp.f_header AND oh.f_state IN (1, 2)
        INNER JOIN h_tables t ON t.f_id = oh.f_table
        LEFT JOIN h_halls hh ON hh.f_id = t.f_hall
        INNER JOIN o_goods og ON og.f_id = ogp.f_id AND og.f_state = 1
        INNER JOIN c_goods cg ON cg.f_id = og.f_goods
        LEFT JOIN (
            SELECT f_doc_sale_id, SUM(f_qty * f_price) AS f_cost
            FROM store_calc_queue
            GROUP BY f_doc_sale_id
        ) scq ON scq.f_doc_sale_id = oh.f_id
        WHERE {$statusWhere}
        ORDER BY oh.f_prefix, og.f_row, cg.f_name
        EOD;

        return $this->db->select($sql)->fetch_all(MYSQLI_ASSOC);
    }

    /**
     * Best kitchen timestamp for report row (o_goods_process.f_data, same keys as kitchen screen).
     */
    private function kitchenTimeSql(): string
    {
        $current = 'CASE ';
        for ($st = 1; $st <= 4; ++$st) {
            $at = $this->statusTransitionTimeSql($st);
            $current .= "WHEN ogp.f_status = {$st} THEN {$at} ";
        }
        $current .= 'ELSE NULL END';

        return <<<EOD
COALESCE(
    {$current},
    NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_1_1_time'))), ''),
    NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$."$jsonTimePath"'))), ''),
    NULLIF(TRIM(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.jsonTimePath'))), '')
)
EOD;
    }

    /**
     * When the line entered kitchen status $status (waiter UpdateStatus → f_status_{st}_{ss}_time).
     */
    private function statusTransitionTimeSql(int $status): string
    {
        $status = max(1, min(4, $status));
        $parts = [];
        for ($ss = 1; $ss <= 4; ++$ss) {
            $parts[] = "NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_{$status}_{$ss}_time')), '')";
        }

        return 'COALESCE(' . implode(",\n    ", $parts) . ')';
    }

    /**
     * History: filter by transition time of the kitchen status selected in the filter (not order open date).
     *
     * @param int $kitchenStatus 0 = any status 1..4 transitioned in date range; 1..4 = only that status time
     */
    private function historyStatusDateWhere(int $kitchenStatus): string
    {
        if ($kitchenStatus >= 1 && $kitchenStatus <= 4) {
            $at = $this->statusTransitionTimeSql($kitchenStatus);

            return "({$at}) IS NOT NULL AND DATE({$at}) BETWEEN ? AND ?";
        }

        $ors = [];
        for ($st = 1; $st <= 4; ++$st) {
            $at = $this->statusTransitionTimeSql($st);
            $ors[] = "(({$at}) IS NOT NULL AND DATE({$at}) BETWEEN ? AND ?)";
        }

        return '(' . implode(' OR ', $ors) . ')';
    }

    /**
     * Timestamp shown in Status column for history rows (time of the filtered status transition).
     */
    private function historyKitchenTimeExpr(int $kitchenStatus): string
    {
        if ($kitchenStatus >= 1 && $kitchenStatus <= 4) {
            return $this->statusTransitionTimeSql($kitchenStatus);
        }

        return $this->kitchenTimeSql();
    }

    /**
     * @return array{0: string, 1: array<int, string>} bind types, values
     */
    private function historyStatusDateBind(string $dateFromYmd, string $dateToYmd, int $kitchenStatus): array
    {
        if ($kitchenStatus >= 1 && $kitchenStatus <= 4) {
            return ['ss', [$dateFromYmd, $dateToYmd]];
        }

        $types = '';
        $values = [];
        for ($st = 1; $st <= 4; ++$st) {
            $types .= 'ss';
            $values[] = $dateFromYmd;
            $values[] = $dateToYmd;
        }

        return [$types, $values];
    }

    /**
     * @return array{0: string, 1: string} Y-m-d from, to (inclusive)
     */
    private function parseHistoryDateRange(array $filter): array
    {
        $today = date('Y-m-d');
        $from = trim((string)($filter['date1'] ?? ''));
        $to = trim((string)($filter['date2'] ?? ''));

        if (!preg_match('/^\d{4}-\d{2}-\d{2}$/', $from)) {
            $from = $today;
        }
        if (!preg_match('/^\d{4}-\d{2}-\d{2}$/', $to)) {
            $to = $today;
        }
        if ($from > $to) {
            [$from, $to] = [$to, $from];
        }

        return [$from, $to];
    }

    private function fetchHistoryRows(string $dateFromYmd, string $dateToYmd, int $kitchenStatus): array
    {
        $statusDateWhere = $this->historyStatusDateWhere($kitchenStatus);
        $kitchenTimeExpr = $this->historyKitchenTimeExpr($kitchenStatus);
        $sql = <<<EOD
        SELECT 
            oh.f_id AS f_header_id,
            oh.f_prefix AS f_order_prefix,
            og.f_qty AS f_qty,
            ogp.f_status,
            {$kitchenTimeExpr} AS f_kitchen_time,
            cg.f_name AS f_goods_name,
            t.f_name AS f_table_name,
            hh.f_name AS f_hall_name,
            COALESCE(json_value(og.f_data, '$.f_comment'), '') AS f_comment,
            COALESCE(json_value(oh.f_data, '$.f_date_open'), '') AS f_date_open,
            COALESCE(json_value(oh.f_data, '$.f_time_open'), '') AS f_time_open,
            COALESCE(oh.f_amounttotal, 0) AS f_amounttotal,
            COALESCE(json_value(oh.f_data, '$.f_amount_cash'), '0') AS f_amount_cash,
            COALESCE(json_value(oh.f_data, '$.f_amount_card'), '0') AS f_amount_card,
            COALESCE(json_value(oh.f_data, '$.f_amount_bank'), '0') AS f_amount_bank,
            COALESCE(json_value(oh.f_data, '$.f_amount_idram'), '0') AS f_amount_idram,
            COALESCE(json_value(oh.f_data, '$.f_amount_complimentary'), '0') AS f_amount_complimentary,
            COALESCE(json_value(oh.f_data, '$.f_amount_other'), '0') AS f_amount_other,
            COALESCE(json_value(oh.f_data, '$.f_amount_telcell'), '0') AS f_amount_telcell,
            COALESCE(json_value(oh.f_data, '$.f_amount_debt'), '0') AS f_amount_debt,
            COALESCE(json_value(oh.f_data, '$.f_amount_prepaid'), '0') AS f_amount_prepaid,
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_name'), '') AS f_guest_name,
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_phone'), '') AS f_guest_phone,
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_address'), '') AS f_guest_address,
            COALESCE(json_value(oh.f_data, '$.f_service_comment'), '') AS f_service_comment,
            COALESCE(json_value(oh.f_data, '$.f_discount_comment'), '') AS f_discount_comment,
            COALESCE(json_value(oh.f_data, '$.f_service_factor'), '0') AS f_service_factor,
            COALESCE(json_value(oh.f_data, '$.f_discount_factor'), '0') AS f_discount_factor,
            COALESCE(scq.f_cost, 0) AS f_cost
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id = ogp.f_header AND oh.f_state IN (1, 2)
        INNER JOIN h_tables t ON t.f_id = oh.f_table
        LEFT JOIN h_halls hh ON hh.f_id = t.f_hall
        INNER JOIN o_goods og ON og.f_id = ogp.f_id AND og.f_state = 1
        INNER JOIN c_goods cg ON cg.f_id = og.f_goods
        LEFT JOIN (
            SELECT f_doc_sale_id, SUM(f_qty * f_price) AS f_cost
            FROM store_calc_queue
            GROUP BY f_doc_sale_id
        ) scq ON scq.f_doc_sale_id = oh.f_id
        WHERE {$statusDateWhere}
        ORDER BY oh.f_prefix, og.f_row, cg.f_name
        EOD;

        [$bindTypes, $bindValues] = $this->historyStatusDateBind($dateFromYmd, $dateToYmd, $kitchenStatus);

        return $this->db->select($sql, $bindTypes, $bindValues)->fetch_all(MYSQLI_ASSOC);
    }

    public function get($params)
    {
        $filter = $this->parseFilter($params);
        $mode = (int)($filter['mode'] ?? 0);
        $kitchenStatus = (int)($filter['kitchen_status'] ?? 0);

        if ($mode === 1) {
            [$dateFrom, $dateTo] = $this->parseHistoryDateRange($filter);
            $raw = $this->fetchHistoryRows($dateFrom, $dateTo, $kitchenStatus);
        } else {
            $raw = $this->fetchLiveRows($kitchenStatus);
        }

        $orders = $this->aggregateKitchenOrders($raw);
        $rows = $this->ordersToReportRows($orders, $mode === 1, $kitchenStatus);

        $headers = [
            Translator::t('Id'),
            Translator::t('Order'),
            Translator::t('Time'),
            Translator::t('Table'),
            Translator::t('Hall'),
            Translator::t('Dishes'),
            Translator::t('Guest'),
            Translator::t('Payment'),
            Translator::t('Amount'),
            Translator::t('Cost'),
            Translator::t('Margin'),
            Translator::t('Discount'),
            Translator::t('Service'),
            Translator::t('Status'),
            '',
        ];

        return [
            'rows' => $rows,
            'toolbar' => ['delete' => false, 'new' => false, 'reload' => true, 'filter' => true],
            'headers' => $headers,
            'hidden_columns' => [0, 14],
            'sum' => [8, 9, 10],
            'filter' => [
                ['type' => 'combobox', 'name' => 'mode', 'label' => Translator::t('Mode'), 'default' => 0, 'values' => [
                    ['label' => Translator::t('Live queue'), 'value' => 0],
                    ['label' => Translator::t('History by status date'), 'value' => 1],
                ]],
                ['type' => 'combobox', 'name' => 'kitchen_status', 'label' => Translator::t('Kitchen status'), 'default' => 1, 'values' => [
                    ['label' => Translator::t('All statuses'), 'value' => 0],
                    ['label' => Translator::t('Accepted'), 'value' => 1],
                    ['label' => Translator::t('Cooking'), 'value' => 2],
                    ['label' => Translator::t('Ready'), 'value' => 3],
                    ['label' => Translator::t('Served'), 'value' => 4],
                ]],
                ['type' => 'date', 'name' => 'date1', 'label' => Translator::t('Status date from')],
                ['type' => 'date', 'name' => 'date2', 'label' => Translator::t('Status date to')],
            ],
        ];
    }

    public function GetItem($params)
    {
        require_once __DIR__ . '/../../waiter/order.php';
        $ord = new Order();
        $order = $ord->GetOrder($params->id);
        $order['calc_queue'] = $ord->GetCalcQueue($params->id);

        return ['order' => $order];
    }
}
