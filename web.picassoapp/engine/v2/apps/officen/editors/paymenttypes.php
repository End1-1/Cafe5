<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . '/../../worker/cash-ops-helper.php';

/**
 * Payment types: the code (f_id) is fixed, only the display name (cash_payment_types.f_name) is editable.
 * Empty f_name falls back to the translated default name.
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
        $custom = $this->customNames();

        $rows = [];
        foreach ($payment['editable'] as $pt) {
            $pt = (int)$pt;
            $default = Translator::t($payment['names'][$pt] ?? (string)$pt);
            $name = $custom[$pt] ?? '';
            $rows[] = [$pt, $default, $name, $name !== '' ? $name : $default];
        }

        return [
            'rows' => $rows,
            'toolbar' => ['new' => false, 'delete' => false, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Payment type'),
                Translator::t('Custom name'),
                Translator::t('Display name'),
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

        $this->db->select(
            "INSERT INTO cash_payment_types (f_id, f_name) VALUES (?, ?)
             ON DUPLICATE KEY UPDATE f_name = VALUES(f_name)",
            'is',
            [$id, $name === '' ? null : $name],
            true
        );

        return [
            'f_id' => $id,
            'row' => $this->rowById($id),
        ];
    }

    /** @return array<int,string> */
    private function customNames(): array
    {
        $out = [];
        $rows = $this->db->select(
            "SELECT f_id, f_name FROM cash_payment_types WHERE COALESCE(f_name, '') <> ''"
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as $r) {
            $out[(int)$r['f_id']] = trim((string)$r['f_name']);
        }
        return $out;
    }

    private function rowById(int $id): ?array
    {
        $payment = require __DIR__ . '/../../worker/dict-payment.php';
        if (!in_array($id, array_map('intval', $payment['editable']), true)) {
            return null;
        }
        $name = $this->customNames()[$id] ?? '';
        return [
            'f_id' => $id,
            'f_default_name' => Translator::t($payment['names'][$id] ?? (string)$id),
            'f_name' => $name,
        ];
    }
}
