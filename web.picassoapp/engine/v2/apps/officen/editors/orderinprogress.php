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

    /**
     * @param array<string,mixed> $order
     */
    private function paymentMultiline(array $order): string
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

        $orderTotal = (float)($order['f_amounttotal'] ?? 0);
        $orderTotalLine = money_fmt_php($orderTotal);
        $typeLine = empty($types) ? Translator::t('No') : implode(', ', $types);
        $lines = [$typeLine];

        if ($paidTotal > 1e-9) {
            if (abs($orderTotal - $paidTotal) > 0.01) {
                $lines[] = $orderTotalLine;
            }
            $lines[] = money_fmt_php($paidTotal);
        } else {
            $lines[] = $orderTotalLine;
        }

        return implode("\n", $lines);
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
     */
    private function statusMultiline(array $order): string
    {
        $lines = [];

        foreach ($order['lines'] as $line) {
            $st = (int)($line['f_status'] ?? 1);
            $lines[] = Translator::t(self::STATUS_LABELS[$st] ?? (string)$st);
        }

        return implode("\n", $lines);
    }

    /**
     * @param array<int,array<string,mixed>> $orders
     * @param int $kitchenStatus 0 = all
     */
    private function filterOrdersByKitchenStatus(array $orders, int $kitchenStatus): array
    {
        if ($kitchenStatus === 0) {
            return $orders;
        }

        $out = [];

        foreach ($orders as $order) {
            foreach ($order['lines'] as $line) {
                if ((int)($line['f_status'] ?? 0) === $kitchenStatus) {
                    $out[] = $order;
                    break;
                }
            }
        }

        return $out;
    }

    /**
     * @param array<int,array<string,mixed>> $lines
     */
    private function minStatusInLines(array $lines): int
    {
        $mn = 99;

        foreach ($lines as $line) {
            $st = (int)($line['f_status'] ?? 1);
            if ($st < $mn) {
                $mn = $st;
            }
        }

        return $mn >= 99 ? 1 : $mn;
    }

    /**
     * @param array<int,array<string,mixed>> $orders
     * @return array<int,array<int,mixed>>
     */
    private function ordersToReportRows(array $orders): array
    {
        $rows = [];

        foreach ($orders as $order) {
            if (empty($order['lines'])) {
                continue;
            }

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
                $this->paymentMultiline($order),
                $this->statusMultiline($order),
                $this->minStatusInLines($order['lines']),
            ];
        }

        return $rows;
    }

    private function fetchLiveRows(): array
    {
        $sql = <<<EOD
        SELECT 
            oh.f_id AS f_header_id,
            oh.f_prefix AS f_order_prefix,
            og.f_qty AS f_qty,
            ogp.f_status,
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
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_address'), '') AS f_guest_address
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id = ogp.f_header AND oh.f_state IN (1, 2)
        INNER JOIN h_tables t ON t.f_id = oh.f_table
        LEFT JOIN h_halls hh ON hh.f_id = t.f_hall
        INNER JOIN o_goods og ON og.f_id = ogp.f_id AND og.f_state = 1
        INNER JOIN c_goods cg ON cg.f_id = og.f_goods
        WHERE ogp.f_status < 4
        ORDER BY oh.f_prefix, og.f_row, cg.f_name
        EOD;

        return $this->db->select($sql)->fetch_all(MYSQLI_ASSOC);
    }

    private function fetchHistoryRows(string $dateYmd): array
    {
        $sql = <<<EOD
        SELECT 
            oh.f_id AS f_header_id,
            oh.f_prefix AS f_order_prefix,
            og.f_qty AS f_qty,
            ogp.f_status,
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
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_address'), '') AS f_guest_address
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id = ogp.f_header AND oh.f_state IN (1, 2)
        INNER JOIN h_tables t ON t.f_id = oh.f_table
        LEFT JOIN h_halls hh ON hh.f_id = t.f_hall
        INNER JOIN o_goods og ON og.f_id = ogp.f_id AND og.f_state = 1
        INNER JOIN c_goods cg ON cg.f_id = og.f_goods
        WHERE ogp.f_status > 3
          AND DATE(oh.f_datecash) = ?
        ORDER BY oh.f_prefix, og.f_row, cg.f_name
        EOD;

        return $this->db->select($sql, 's', [$dateYmd])->fetch_all(MYSQLI_ASSOC);
    }

    public function get($params)
    {
        $filter = $this->parseFilter($params);
        $mode = (int)($filter['mode'] ?? 0);
        $kitchenStatus = (int)($filter['kitchen_status'] ?? 0);

        if ($mode === 1) {
            $date = trim((string)($filter['history_date'] ?? ''));
            if (!preg_match('/^\d{4}-\d{2}-\d{2}$/', $date)) {
                $date = date('Y-m-d');
            }
            $raw = $this->fetchHistoryRows($date);
        } else {
            $raw = $this->fetchLiveRows();
        }

        $orders = $this->aggregateKitchenOrders($raw);
        $orders = $this->filterOrdersByKitchenStatus($orders, $kitchenStatus);
        $rows = $this->ordersToReportRows($orders);

        $headers = [
            Translator::t('Id'),
            Translator::t('Order'),
            Translator::t('Time'),
            Translator::t('Table'),
            Translator::t('Hall'),
            Translator::t('Dishes'),
            Translator::t('Guest'),
            Translator::t('Payment'),
            Translator::t('Status'),
            '',
        ];

        return [
            'rows' => $rows,
            'toolbar' => ['delete' => false, 'new' => false, 'reload' => true, 'filter' => true],
            'headers' => $headers,
            'hidden_columns' => [0, 9],
            'sum' => [],
            'filter' => [
                ['type' => 'combobox', 'name' => 'mode', 'label' => Translator::t('Mode'), 'default' => 0, 'values' => [
                    ['label' => Translator::t('Live queue'), 'value' => 0],
                    ['label' => Translator::t('History (served)'), 'value' => 1],
                ]],
                ['type' => 'combobox', 'name' => 'kitchen_status', 'label' => Translator::t('Kitchen status'), 'default' => 0, 'values' => [
                    ['label' => Translator::t('All'), 'value' => 0],
                    ['label' => Translator::t('Accepted'), 'value' => 1],
                    ['label' => Translator::t('Cooking'), 'value' => 2],
                    ['label' => Translator::t('Ready'), 'value' => 3],
                ]],
                ['type' => 'date', 'name' => 'history_date', 'label' => Translator::t('Archive date')],
            ],
        ];
    }

    public function GetItem($params)
    {
        require_once __DIR__ . '/../../waiter/order.php';
        $ord = new Order();
        return ['order' => $ord->GetOrder($params->id)];
    }
}
