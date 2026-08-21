<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-31 14:32:35
# Last Modified: 2026-05-04
# Kitchen queue: orders with unfinished kitchen lines (o_goods_process.f_status < 4).
# Include closed/paid headers (f_state=2) so dishes stay visible until served on kitchen workflow.
# Payment breakdown from oh.f_data (amounts > 0 → show method names on kitchen client).

require_once __DIR__ . "/index.php";

class InProgress extends Auth
{
    /** @var array<int,string> */
    private const STATUS_LABELS = [
        1 => 'accepted',
        2 => 'cooking',
        3 => 'ready',
        4 => 'served',
    ];

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
                    'f_table' => (int)($r['f_table'] ?? 0),
                    'f_hall' => (int)($r['f_hall'] ?? 0),
                    'f_table_name' => $r['f_table_name'],
                    'f_hall_name' => $r['f_hall_name'],
                    'f_date_open' => $r['f_date_open'],
                    'f_time_open' => $r['f_time_open'],
                    'f_amounttotal' => $r['f_amounttotal'],
                    'f_amount_cash' => $r['f_amount_cash'],
                    'f_amount_card' => $r['f_amount_card'],
                    'f_amount_bank' => $r['f_amount_bank'],
                    'f_amount_idram' => $r['f_amount_idram'],
                    'f_amount_complimentary' => $r['f_amount_complimentary'],
                    'f_amount_other' => $r['f_amount_other'],
                    'f_amount_telcell' => $r['f_amount_telcell'],
                    'f_amount_debt' => $r['f_amount_debt'],
                    'f_amount_prepaid' => $r['f_amount_prepaid'],
                    'f_guest_name' => $r['f_guest_name'],
                    'f_guest_phone' => $r['f_guest_phone'],
                    'f_guest_address' => $r['f_guest_address'],
                    'f_started_at' => '',
                    'f_estimated_end' => '',
                    'lines' => [],
                ];
            }

            $map[$hid]['lines'][] = [
                'f_goods_row_id' => $r['f_goods_row_id'],
                'f_qty' => $r['f_qty'],
                'f_goods_name' => $r['f_goods_name'],
                'f_status' => (int) $r['f_status'],
                'f_comment' => $r['f_comment'],
                'f_ready_at' => $r['f_ready_at'] ?? '',
                'f_cooking_time' => (int)($r['f_cooking_time'] ?? 0),
                'f_line_started_at' => $r['f_line_started_at'] ?? '',
            ];
        }

        foreach ($map as &$order) {
            $start = $this->resolveOrderStart($order);
            $order['f_started_at'] = $start;
            $order['f_estimated_end'] = $this->resolveOrderEstimatedEnd($order, $start);
        }
        unset($order);

        return array_values($map);
    }

    /** @param array<string,mixed> $order */
    private function resolveOrderStart(array $order): string
    {
        $date = trim((string)($order['f_date_open'] ?? ''));
        $time = trim((string)($order['f_time_open'] ?? ''));
        if ($date !== '' && $time !== '') {
            return trim($date . ' ' . $time);
        }
        if ($date !== '') {
            return $date;
        }
        $earliest = '';
        foreach ($order['lines'] as $line) {
            $ls = trim((string)($line['f_line_started_at'] ?? ''));
            if ($ls === '') {
                continue;
            }
            if ($earliest === '' || strcmp($ls, $earliest) < 0) {
                $earliest = $ls;
            }
        }
        return $earliest;
    }

    /** @param array<string,mixed> $order */
    private function resolveOrderEstimatedEnd(array $order, string $start): string
    {
        $maxMin = 0;
        foreach ($order['lines'] as $line) {
            $maxMin = max($maxMin, (int)($line['f_cooking_time'] ?? 0));
        }
        if ($start === '' || $maxMin <= 0) {
            return '';
        }
        $ts = strtotime($start);
        if ($ts === false) {
            return '';
        }
        return date('Y-m-d H:i:s', $ts + ($maxMin * 60));
    }

    public function get($params)
    {
        $sql = <<<EOD
        SELECT 
            oh.f_id AS f_header_id,
            oh.f_table AS f_table,
            t.f_hall AS f_hall,
            oh.f_prefix AS f_order_prefix,
            og.f_id AS f_goods_row_id,
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
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_address'), '') AS f_guest_address,
            COALESCE(
                CAST(JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_cooking_time')) AS UNSIGNED),
                CAST(JSON_UNQUOTE(JSON_EXTRACT(cg.f_data, '$.f_cooking_time')) AS UNSIGNED),
                0
            ) AS f_cooking_time,
            COALESCE(
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_1_1_time')), ''),
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(og.f_data, '$.f_append_time')), ''),
                ''
            ) AS f_line_started_at,
            COALESCE(
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_1_time')), ''),
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_2_time')), ''),
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_3_time')), ''),
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_4_time')), '')
            ) AS f_ready_at
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id = ogp.f_header AND oh.f_state IN (1, 2)
        INNER JOIN h_tables t ON t.f_id = oh.f_table
        LEFT JOIN h_halls hh ON hh.f_id = t.f_hall
        INNER JOIN o_goods og ON og.f_id = ogp.f_id AND og.f_state = 1
        INNER JOIN c_goods cg ON cg.f_id = og.f_goods
        WHERE ogp.f_status < 4
        ORDER BY oh.f_prefix, og.f_row, cg.f_name
        EOD;

        $rows = $this->select($sql)->fetch_all(MYSQLI_ASSOC);

        /* Filters — client-side (Waiter). */

        $this->result['data'] = $this->aggregateKitchenOrders($rows);
        $this->result['status_labels'] = self::STATUS_LABELS;
        $this->echoResult();
    }

    /**
     * Архив кухни на дату: строки со статусом «подано» (>= 4),
     * календарный день — по дате перевода в статус 4 (o_goods_process.f_data f_status_4_*_time).
     * Заголовки как у get(): открытые и закрытые столы (f_state IN 1,2).
     *
     * @param object $params date "Y-m-d"
     */
    public function getHistoryForDate($params)
    {
        $date = trim((string)($params->date ?? ''));
        if (!preg_match('/^\d{4}-\d{2}-\d{2}$/', $date)) {
            dieWithCode(Translator::t('Invalid date'));
        }

        $sql = <<<EOD
        SELECT 
            oh.f_id AS f_header_id,
            oh.f_table AS f_table,
            t.f_hall AS f_hall,
            oh.f_prefix AS f_order_prefix,
            og.f_id AS f_goods_row_id,
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
            COALESCE(json_value(oh.f_data, '$.f_guest.f_guest_address'), '') AS f_guest_address,
            COALESCE(
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_1_time')), ''),
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_2_time')), ''),
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_3_time')), ''),
                NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_3_4_time')), '')
            ) AS f_ready_at
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id = ogp.f_header AND oh.f_state IN (1, 2)
        INNER JOIN h_tables t ON t.f_id = oh.f_table
        LEFT JOIN h_halls hh ON hh.f_id = t.f_hall
        INNER JOIN o_goods og ON og.f_id = ogp.f_id AND og.f_state = 1
        INNER JOIN c_goods cg ON cg.f_id = og.f_goods
        WHERE ogp.f_status >= 4
          AND DATE(COALESCE(
            NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_4_1_time')), ''),
            NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_4_2_time')), ''),
            NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_4_3_time')), ''),
            NULLIF(JSON_UNQUOTE(JSON_EXTRACT(ogp.f_data, '$.f_status_4_4_time')), '')
          )) = ?
        ORDER BY oh.f_prefix,
          og.f_row,
          cg.f_name
        EOD;

        $rows = $this->select($sql, 's', [$date])->fetch_all(MYSQLI_ASSOC);

        $this->result['data'] = $this->aggregateKitchenOrders($rows);
        $this->result['status_labels'] = self::STATUS_LABELS;
        $this->result['history_date'] = $date;
        $this->echoResult();
    }

    /**
     * Kitchen line = o_goods_process.f_id = o_goods.f_id.
     * params: id (uuid), status (1..4), substatus (int, default 1).
     */
    public function UpdateStatus($params)
    {
        $id = (string)($params->id ?? '');

        if ($id === '') {
            dieWithCode('id required');
        }

        $st = (int)($params->status ?? 0);
        $ss = (int)($params->substatus ?? 1);

        if ($st < 1 || $st > 4) {
            dieWithCode('status must be 1..4');
        }

        if ($ss < 1) {
            dieWithCode('substatus must be >= 1');
        }

        $check = <<<EOD
        SELECT ogp.f_id
        FROM o_goods_process ogp
        INNER JOIN o_header oh ON oh.f_id = ogp.f_header AND oh.f_state IN (1, 2)
        INNER JOIN o_goods og ON og.f_id = ogp.f_id AND og.f_state = 1
        WHERE ogp.f_id = ?
        EOD;
        $ok = $this->select($check, 's', [$id])->fetch_assoc();

        if (!$ok) {
            dieWithCode('process row not found or line voided');
        }

        $jsonTimePath = sprintf('$.f_status_%d_%d_time', $st, $ss);
        $sql = <<<EOD
        UPDATE o_goods_process 
        SET f_status = ?,
        f_data = JSON_REMOVE(
            JSON_SET(
                COALESCE(f_data, '{}'),
                '{$jsonTimePath}',
                COALESCE(
                    NULLIF(JSON_UNQUOTE(JSON_EXTRACT(f_data, '{$jsonTimePath}')), ''),
                    NULLIF(JSON_UNQUOTE(JSON_EXTRACT(f_data, '\$."$jsonTimePath"')), ''),
                    NULLIF(JSON_UNQUOTE(JSON_EXTRACT(f_data, '$.jsonTimePath')), ''),
                    DATE_FORMAT(NOW(), '%Y-%m-%d %H:%i:%s')
                ),
                '$.f_substatus', ?
            ),
            '\$."$jsonTimePath"',
            '$.jsonTimePath',
            '\$.$jsonTimePath'
        )
        WHERE f_id = ?
        EOD;

        $this->select($sql, 'iis', [$st, $ss, $id], true);
        $this->get($params);
    }
}
