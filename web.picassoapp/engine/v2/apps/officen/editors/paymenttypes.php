<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . '/../../worker/cash-ops-helper.php';

/**
 * Payment types: the code (f_id) is fixed, only the display name (cash_payment_types.f_name) is editable.
 * Empty f_name falls back to the translated default name.
 * f_waiter: when 1, the type is shown/usable in Waiter; when 0 — hidden and blocked on the server.
 */
class PaymentTypes
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $payment = require __DIR__ . '/../../worker/dict-payment.php';
        $meta = $this->metaById();

        $rows = [];
        $waiterById = [];
        foreach ($payment['editable'] as $pt) {
            $pt = (int)$pt;
            $default = Translator::t($payment['names'][$pt] ?? (string)$pt);
            $name = $meta[$pt]['f_name'] ?? '';
            $waiter = (int)($meta[$pt]['f_waiter'] ?? 1) !== 0 ? 1 : 0;
            $waiterById[(string)$pt] = $waiter;
            $rows[] = [
                $pt,
                $default,
                $name,
                $name !== '' ? $name : $default,
                $waiter ? Translator::t('Yes') : Translator::t('No'),
            ];
        }

        return [
            'rows' => $rows,
            'waiter_by_id' => $waiterById,
            'toolbar' => ['new' => false, 'delete' => false, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Payment type'),
                Translator::t('Custom name'),
                Translator::t('Display name'),
                Translator::t('In Waiter'),
            ],
        ];
    }

    public function GetItem($params)
    {
        $id = (int)($params->id ?? $params->f_id ?? 0);
        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        return [
            'title' => Translator::t('Payment type'),
            'row' => $row,
        ];
    }

    public function save($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($this->rowById($id) === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $name = trim((string)($params->f_name ?? ''));
        if (mb_strlen($name) > 64) {
            dieWithCode(Translator::t('Name is too long'));
        }

        $waiter = !empty($params->f_waiter) ? 1 : 0;
        $this->saveRow($id, $name === '' ? null : $name, $waiter);

        return [
            'f_id' => $id,
            'row' => $this->rowById($id),
        ];
    }

    /**
     * @return array<int, array{f_name: string, f_waiter: int}>
     */
    private function metaById(): array
    {
        $this->ensureWaiterColumn();
        $out = [];
        $hasWaiter = false;
        try {
            $hasWaiter = $this->db->select("SHOW COLUMNS FROM cash_payment_types LIKE 'f_waiter'")->num_rows > 0;
        } catch (Exception $e) {
            $hasWaiter = false;
        }
        $sql = $hasWaiter
            ? "SELECT f_id, COALESCE(f_name, '') AS f_name, COALESCE(f_waiter, 1) AS f_waiter FROM cash_payment_types"
            : "SELECT f_id, COALESCE(f_name, '') AS f_name, 1 AS f_waiter FROM cash_payment_types";
        $rows = $this->db->select($sql)->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as $r) {
            $out[(int)$r['f_id']] = [
                'f_name' => trim((string)$r['f_name']),
                'f_waiter' => (int)$r['f_waiter'] !== 0 ? 1 : 0,
            ];
        }
        return $out;
    }

    private function saveRow(int $id, ?string $name, int $waiter): void
    {
        $this->ensureWaiterColumn();
        $hasWaiter = false;
        try {
            $hasWaiter = $this->db->select("SHOW COLUMNS FROM cash_payment_types LIKE 'f_waiter'")->num_rows > 0;
        } catch (Exception $e) {
            $hasWaiter = false;
        }
        if ($hasWaiter) {
            $this->db->select(
                "INSERT INTO cash_payment_types (f_id, f_name, f_waiter) VALUES (?, ?, ?)
                 ON DUPLICATE KEY UPDATE f_name = VALUES(f_name), f_waiter = VALUES(f_waiter)",
                'isi',
                [$id, $name, $waiter],
                true
            );
            return;
        }
        $this->db->select(
            "INSERT INTO cash_payment_types (f_id, f_name) VALUES (?, ?)
             ON DUPLICATE KEY UPDATE f_name = VALUES(f_name)",
            'is',
            [$id, $name],
            true
        );
    }

    private function rowById(int $id): ?array
    {
        $payment = require __DIR__ . '/../../worker/dict-payment.php';
        if (!in_array($id, array_map('intval', $payment['editable']), true)) {
            return null;
        }
        $meta = $this->metaById()[$id] ?? ['f_name' => '', 'f_waiter' => 1];
        return [
            'f_id' => $id,
            'f_default_name' => Translator::t($payment['names'][$id] ?? (string)$id),
            'f_name' => $meta['f_name'],
            'f_waiter' => (int)$meta['f_waiter'],
        ];
    }

    private function ensureWaiterColumn(): void
    {
        static $done = false;
        if ($done) {
            return;
        }
        try {
            if ($this->db->select("SHOW COLUMNS FROM cash_payment_types LIKE 'f_waiter'")->num_rows == 0) {
                $this->db->select(
                    "ALTER TABLE cash_payment_types ADD COLUMN f_waiter TINYINT NOT NULL DEFAULT 1",
                    '',
                    [],
                    true
                );
            }
        } catch (Exception $e) {
            // Column may already exist or lack ALTER rights; SELECT below will still try COALESCE.
        }
        $done = true;
    }
}
