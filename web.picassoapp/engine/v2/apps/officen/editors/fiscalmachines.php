<?php
# © 2026 , Kudryashov Vasili

class FiscalMachines
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql = <<<SQL
        SELECT f_id,
               COALESCE(f_name, '') AS f_name,
               COALESCE(f_ip, '') AS f_ip,
               COALESCE(f_port, 0) AS f_port,
               COALESCE(f_default_dept, 1) AS f_default_dept,
               COALESCE(f_external_pos, 0) AS f_external_pos
        FROM fiscal_machine
        ORDER BY f_name, f_id
        SQL;

        return [
            'rows' => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            'toolbar' => ['new' => true, 'delete' => true, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Name'),
                Translator::t('IP'),
                Translator::t('Port'),
                Translator::t('Default department'),
                Translator::t('External POS'),
            ],
        ];
    }

    public function GetItem($params)
    {
        $id = (int)($params->id ?? $params->f_id ?? 0);
        if ($id <= 0) {
            return [
                'title' => Translator::t('Fiscal machine'),
                'row' => $this->emptyRow(),
            ];
        }

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        return [
            'title' => Translator::t('Fiscal machine'),
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

        $ip = trim((string)($params->f_ip ?? ''));
        if (mb_strlen($ip) > 24) {
            dieWithCode(Translator::t('IP is too long'));
        }

        $password = trim((string)($params->f_password ?? ''));
        $opPin = trim((string)($params->f_op_pin ?? ''));
        $opPass = trim((string)($params->f_op_pass ?? ''));
        if (mb_strlen($password) > 16 || mb_strlen($opPin) > 16 || mb_strlen($opPass) > 16) {
            dieWithCode(Translator::t('Password is too long'));
        }

        $record = [
            'f_name' => $name,
            'f_ip' => $ip,
            'f_port' => (int)($params->f_port ?? 0),
            'f_password' => $password,
            'f_op_pin' => $opPin,
            'f_op_pass' => $opPass,
            'f_external_pos' => ((int)($params->f_external_pos ?? 0)) === 1 ? 1 : 0,
            'f_default_dept' => (int)($params->f_default_dept ?? 1),
        ];

        if ($id <= 0) {
            $id = (int)$this->db->insert('fiscal_machine', $record);
        } else {
            if ($this->rowById($id) === null) {
                dieWithCode(Translator::t('Record not found'), 404);
            }
            $this->db->update('fiscal_machine', $record, $id);
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

        $used = $this->db->select(
            "SELECT f_id, f_name FROM workstations
             WHERE CAST(JSON_VALUE(f_config, '$.f_fiscal_machine_id') AS UNSIGNED) = ?
             ORDER BY f_name, f_id",
            'i',
            [$id]
        )->fetch_all(MYSQLI_ASSOC);

        if (!empty($used)) {
            $names = array_map(static fn($row) => (string)($row['f_name'] ?? $row['f_id']), $used);
            dieWithCode(Translator::t('Fiscal machine is used by workstation') . ': ' . implode(', ', $names));
        }

        $this->db->delete('fiscal_machine', $id);

        return [
            'message' => Translator::t('Deleted'),
        ];
    }

    private function emptyRow(): array
    {
        return [
            'f_id' => 0,
            'f_name' => '',
            'f_ip' => '',
            'f_port' => 0,
            'f_password' => '',
            'f_op_pin' => '',
            'f_op_pass' => '',
            'f_external_pos' => 0,
            'f_default_dept' => 1,
        ];
    }

    private function rowById(int $id): ?array
    {
        $row = $this->db->select(
            'SELECT f_id, f_name, f_ip, f_port, f_password, f_op_pin, f_op_pass, f_external_pos, f_default_dept
             FROM fiscal_machine WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        if ($row === null) {
            return null;
        }

        $row['f_id'] = (int)$row['f_id'];
        $row['f_port'] = (int)$row['f_port'];
        $row['f_external_pos'] = (int)$row['f_external_pos'];
        $row['f_default_dept'] = (int)$row['f_default_dept'];
        $row['f_name'] = (string)($row['f_name'] ?? '');
        $row['f_ip'] = (string)($row['f_ip'] ?? '');
        $row['f_password'] = (string)($row['f_password'] ?? '');
        $row['f_op_pin'] = (string)($row['f_op_pin'] ?? '');
        $row['f_op_pass'] = (string)($row['f_op_pass'] ?? '');

        return $row;
    }
}
