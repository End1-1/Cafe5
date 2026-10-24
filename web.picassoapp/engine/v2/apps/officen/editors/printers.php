<?php
# © 2026 , Kudryashov Vasili

class Printers
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql = <<<SQL
        SELECT f_id, COALESCE(f_name, '') AS f_name
        FROM d_printers
        ORDER BY f_name, f_id
        SQL;

        return [
            'rows' => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            'toolbar' => ['new' => true, 'delete' => true, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Name'),
            ],
        ];
    }

    public function GetItem($params)
    {
        $id = (int)($params->id ?? $params->f_id ?? 0);
        if ($id <= 0) {
            return [
                'title' => Translator::t('Printer'),
                'row' => $this->emptyRow(),
            ];
        }

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        return [
            'title' => Translator::t('Printer'),
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

        $dup = $this->db->select(
            'SELECT f_id FROM d_printers WHERE f_name = ? AND f_id <> ? LIMIT 1',
            'si',
            [$name, $id]
        )->fetch_assoc();
        if ($dup) {
            dieWithCode(Translator::t('Name already exists'));
        }

        $record = ['f_name' => $name];

        if ($id <= 0) {
            $id = (int)$this->db->insert('d_printers', $record);
        } else {
            if ($this->rowById($id) === null) {
                dieWithCode(Translator::t('Record not found'), 404);
            }
            $this->db->update('d_printers', $record, $id);
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

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $this->db->delete('d_printers', $id);

        return [
            'message' => Translator::t('Deleted'),
        ];
    }

    private function emptyRow(): array
    {
        return [
            'f_id' => 0,
            'f_name' => '',
        ];
    }

    private function rowById(int $id): ?array
    {
        $row = $this->db->select(
            'SELECT f_id, COALESCE(f_name, \'\') AS f_name FROM d_printers WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        if ($row === null) {
            return null;
        }

        return [
            'f_id' => (int)$row['f_id'],
            'f_name' => (string)($row['f_name'] ?? ''),
        ];
    }
}
