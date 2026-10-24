<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . '/../../../worker/helper.php';
require_once __DIR__ . '/../../worker/cash-ops-helper.php';

/**
 * Shortage / overage log from shift open and close (cash_discrepancy).
 * Listed as the "Cash discrepancies" view mode of the revenue report; each row can be marked resolved with a comment.
 */
class CashDiscrepancies
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $filter = [];
        foreach ($params->filter ?? [] as $item) {
            foreach ((array)$item as $k => $v) {
                $filter[$k] = $v;
            }
        }
        $currencyId = (int)($filter['currency'] ?? 1);
        return $this->listRows($filter, $currencyId > 0 ? $currencyId : 1);
    }

    /**
     * @param array<string,mixed> $filter date1, date2, cashbox, discrepancy_state (0 all, 1 unresolved, 2 resolved)
     */
    public function listRows(array $filter, int $currencyId): array
    {
        $dateStart = !empty($filter['date1']) ? $filter['date1'] : '2000-01-01';
        $dateEnd = !empty($filter['date2']) ? $filter['date2'] : date('Y-m-d');
        $cashboxId = (int)($filter['cashbox'] ?? 0);
        $state = (int)($filter['discrepancy_state'] ?? 0);

        $sql = <<<SQL
        SELECT d.f_id, d.f_date, cb.f_name AS cashbox_name, d.f_stage, d.f_type, d.f_amount, d.f_session_id,
            TRIM(CONCAT(COALESCE(us.f_last, ''), ' ', COALESCE(us.f_first, ''))) AS shift_user,
            TRIM(CONCAT(COALESCE(uc.f_last, ''), ' ', COALESCE(uc.f_first, ''))) AS counted_user,
            d.f_resolved,
            TRIM(CONCAT(COALESCE(ur.f_last, ''), ' ', COALESCE(ur.f_first, ''))) AS resolved_user,
            d.f_resolved_date, d.f_comment
        FROM cash_discrepancy d
        LEFT JOIN cash_box cb ON cb.f_id = d.f_cashbox_id
        LEFT JOIN s_user us ON us.f_id = d.f_user
        LEFT JOIN s_user uc ON uc.f_id = d.f_user_counted
        LEFT JOIN s_user ur ON ur.f_id = d.f_resolved_user
        WHERE d.f_date BETWEEN ? AND ?
          AND d.f_currency_id = ?
        SQL;
        $types = 'ssi';
        $values = [$dateStart . ' 00:00:00', $dateEnd . ' 23:59:59', $currencyId];
        if ($cashboxId > 0) {
            $sql .= ' AND d.f_cashbox_id = ? ';
            $types .= 'i';
            $values[] = $cashboxId;
        }
        if ($state === 1) {
            $sql .= ' AND d.f_resolved = 0 ';
        } elseif ($state === 2) {
            $sql .= ' AND d.f_resolved = 1 ';
        }
        $sql .= ' ORDER BY d.f_date DESC, d.f_id DESC';

        $rows = [];
        foreach ($this->db->select($sql, $types, $values)->fetch_all(MYSQLI_ASSOC) as $r) {
            $amount = (float)$r['f_amount'];
            $isShortage = (int)$r['f_type'] === CASH_DISCREPANCY_SHORTAGE;
            $rows[] = [
                (int)$r['f_id'],
                (string)$r['f_date'],
                (string)$r['cashbox_name'],
                $this->stageName((int)$r['f_stage']),
                $isShortage ? money_fmt_php($amount) : '',
                $isShortage ? '' : money_fmt_php($amount),
                (int)$r['f_session_id'] > 0 ? '#' . (int)$r['f_session_id'] : '',
                (string)$r['shift_user'],
                (string)$r['counted_user'],
                (int)$r['f_resolved'] === 1 ? Translator::t('Resolved') : Translator::t('Unresolved'),
                (string)$r['resolved_user'],
                (string)($r['f_resolved_date'] ?? ''),
                (string)($r['f_comment'] ?? ''),
            ];
        }

        return [
            'rows' => $rows,
            'hidden_columns' => [0],
            'headers' => [
                'ID',
                Translator::t('DateTime'),
                Translator::t('Cashbox'),
                Translator::t('Event'),
                Translator::t('Shortage'),
                Translator::t('Overage'),
                Translator::t('Shift'),
                Translator::t('Shift staff'),
                Translator::t('Counted by'),
                Translator::t('Status'),
                Translator::t('Resolved by'),
                Translator::t('Resolved at'),
                Translator::t('Comment'),
            ],
            'toolbar' => ['reload' => true, 'filter' => true],
            'sum' => [4, 5],
        ];
    }

    public function GetItem($params)
    {
        $row = $this->rowById((int)($params->id ?? $params->f_id ?? 0));
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }
        return [
            'title' => Translator::t('Cash discrepancy'),
            'row' => $row,
        ];
    }

    public function save($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        $current = $this->rowById($id);
        if ($current === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $comment = trim((string)($params->f_comment ?? ''));
        if (mb_strlen($comment) > 1024) {
            dieWithCode(Translator::t('Comment is too long'));
        }
        $resolved = !empty($params->f_resolved) ? 1 : 0;

        if ($resolved === 1 && (int)$current['f_resolved'] === 1) {
            $this->db->select(
                'UPDATE cash_discrepancy SET f_comment = ? WHERE f_id = ?',
                'si',
                [$comment === '' ? null : $comment, $id],
                true
            );
        } elseif ($resolved === 1) {
            $this->db->select(
                'UPDATE cash_discrepancy SET f_resolved = 1, f_resolved_user = ?, f_resolved_date = NOW(), f_comment = ?
                 WHERE f_id = ?',
                'isi',
                [$this->db->currentUserId(), $comment === '' ? null : $comment, $id],
                true
            );
        } else {
            $this->db->select(
                'UPDATE cash_discrepancy SET f_resolved = 0, f_resolved_user = NULL, f_resolved_date = NULL, f_comment = ?
                 WHERE f_id = ?',
                'si',
                [$comment === '' ? null : $comment, $id],
                true
            );
        }

        return [
            'f_id' => $id,
            'row' => $this->rowById($id),
        ];
    }

    private function stageName(int $stage): string
    {
        return $stage === CASH_DISCREPANCY_STAGE_OPEN ? Translator::t('Shift open') : Translator::t('Shift close');
    }

    private function rowById(int $id): ?array
    {
        if ($id <= 0) {
            return null;
        }
        $r = $this->db->select(
            "SELECT d.*, cb.f_name AS cashbox_name,
                TRIM(CONCAT(COALESCE(us.f_last, ''), ' ', COALESCE(us.f_first, ''))) AS shift_user,
                TRIM(CONCAT(COALESCE(uc.f_last, ''), ' ', COALESCE(uc.f_first, ''))) AS counted_user,
                TRIM(CONCAT(COALESCE(ur.f_last, ''), ' ', COALESCE(ur.f_first, ''))) AS resolved_user
             FROM cash_discrepancy d
             LEFT JOIN cash_box cb ON cb.f_id = d.f_cashbox_id
             LEFT JOIN s_user us ON us.f_id = d.f_user
             LEFT JOIN s_user uc ON uc.f_id = d.f_user_counted
             LEFT JOIN s_user ur ON ur.f_id = d.f_resolved_user
             WHERE d.f_id = ?",
            'i',
            [$id]
        )->fetch_assoc();
        if (!$r) {
            return null;
        }
        $isShortage = (int)$r['f_type'] === CASH_DISCREPANCY_SHORTAGE;
        return [
            'f_id' => (int)$r['f_id'],
            'f_date' => (string)$r['f_date'],
            'f_cashbox_name' => (string)($r['cashbox_name'] ?? ''),
            'f_stage_name' => $this->stageName((int)$r['f_stage']),
            'f_type' => (int)$r['f_type'],
            'f_type_name' => $isShortage ? Translator::t('Shortage') : Translator::t('Overage'),
            'f_amount' => money_fmt_php((float)$r['f_amount']),
            'f_session_id' => (int)$r['f_session_id'],
            'f_shift_user' => (string)$r['shift_user'],
            'f_counted_user' => (string)$r['counted_user'],
            'f_resolved' => (int)$r['f_resolved'],
            'f_resolved_user' => (string)$r['resolved_user'],
            'f_resolved_date' => (string)($r['f_resolved_date'] ?? ''),
            'f_comment' => (string)($r['f_comment'] ?? ''),
        ];
    }
}
