<?php
# © 2026 , Kudryashov Vasili

class CashBoxes
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql = <<<SQL
        SELECT c.f_id,
               COALESCE(c.f_name, '') AS f_name,
               COALESCE(cur.f_name, cur.f_short, '') AS f_currency_name
        FROM cash_box c
        LEFT JOIN e_currency cur ON cur.f_id = COALESCE(c.f_currency_id, 1)
        ORDER BY c.f_name, c.f_id
        SQL;

        return [
            'rows' => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            'toolbar' => ['new' => true, 'delete' => true, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Name'),
                Translator::t('Currency'),
            ],
        ];
    }

    public function GetItem($params)
    {
        $id = (int)($params->id ?? $params->f_id ?? 0);
        if ($id <= 0) {
            return [
                'title' => Translator::t('Cashbox'),
                'row' => $this->emptyRow(),
            ];
        }

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        return [
            'title' => Translator::t('Cashbox'),
            'row' => $row,
        ];
    }

    public function save($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        $name = trim((string)($params->f_name ?? ''));
        if ($name === '') {
            dieWithCode(Translator::t('Name is required'));
        }
        if (mb_strlen($name) > 255) {
            dieWithCode(Translator::t('Name is too long'));
        }

        $currencyId = (int)($params->f_currency_id ?? 0);
        if ($currencyId <= 0) {
            dieWithCode(Translator::t('Currency is required'));
        }
        $currency = $this->db->select(
            'SELECT f_id FROM e_currency WHERE f_id = ?',
            'i',
            [$currencyId]
        )->fetch_assoc();
        if (!$currency) {
            dieWithCode(Translator::t('Currency not found'));
        }

        $record = [
            'f_name' => $name,
            'f_currency_id' => $currencyId,
        ];

        if ($id <= 0) {
            $id = (int)$this->db->insert('cash_box', $record);
        } else {
            if ($this->rowById($id) === null) {
                dieWithCode(Translator::t('Record not found'), 404);
            }
            $this->db->update('cash_box', $record, $id);
        }

        return [
            'f_id' => $id,
            'row' => $this->rowById($id),
        ];
    }

    public function delete($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($id <= 0) {
            dieWithCode(Translator::t('Id is required'));
        }

        if ($this->rowById($id) === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $usedWs = $this->db->select(
            "SELECT f_id, f_name FROM workstations
             WHERE CAST(JSON_VALUE(f_config, '$.f_cashbox_id') AS UNSIGNED) = ?
             ORDER BY f_name, f_id",
            'i',
            [$id]
        )->fetch_all(MYSQLI_ASSOC);

        if (!empty($usedWs)) {
            $names = array_map(static fn($row) => (string)($row['f_name'] ?? $row['f_id']), $usedWs);
            dieWithCode(Translator::t('Cashbox is used by workstation') . ': ' . implode(', ', $names));
        }

        $openSession = $this->db->select(
            'SELECT f_id FROM cash_session WHERE f_cashbox_id = ? AND f_state = 1 LIMIT 1',
            'i',
            [$id]
        )->fetch_assoc();
        if ($openSession) {
            dieWithCode(Translator::t('Cashbox has an open session'));
        }

        $ops = $this->db->select(
            'SELECT f_id FROM cash_operations WHERE f_cashbox_id = ? LIMIT 1',
            'i',
            [$id]
        )->fetch_assoc();
        if ($ops) {
            dieWithCode(Translator::t('Cashbox has operations and cannot be deleted'));
        }

        $this->db->delete('cash_box', $id);

        return [
            'message' => Translator::t('Deleted'),
        ];
    }

    private function emptyRow(): array
    {
        $currency = $this->db->select(
            'SELECT f_id, COALESCE(f_name, f_short, \'\') AS f_name FROM e_currency ORDER BY f_id LIMIT 1'
        )->fetch_assoc();

        return [
            'f_id' => 0,
            'f_name' => '',
            'f_currency_id' => (int)($currency['f_id'] ?? 1),
            'f_currency_name' => (string)($currency['f_name'] ?? ''),
        ];
    }

    private function rowById(int $id): ?array
    {
        $row = $this->db->select(
            'SELECT c.f_id, c.f_name, COALESCE(c.f_currency_id, 1) AS f_currency_id,
                    COALESCE(cur.f_name, cur.f_short, \'\') AS f_currency_name
             FROM cash_box c
             LEFT JOIN e_currency cur ON cur.f_id = COALESCE(c.f_currency_id, 1)
             WHERE c.f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        if ($row === null) {
            return null;
        }

        $row['f_id'] = (int)$row['f_id'];
        $row['f_name'] = (string)($row['f_name'] ?? '');
        $row['f_currency_id'] = (int)$row['f_currency_id'];
        $row['f_currency_name'] = (string)($row['f_currency_name'] ?? '');

        return $row;
    }
}
