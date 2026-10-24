<?php
# © 2026 , Kudryashov Vasili

class PrintAliases
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
               COALESCE(f_alias, '') AS f_alias,
               COALESCE(f_printer, '') AS f_printer
        FROM d_print_aliases
        ORDER BY f_alias, f_id
        SQL;

        return [
            'rows' => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            'toolbar' => ['new' => true, 'delete' => true, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Alias'),
                Translator::t('Printer'),
            ],
        ];
    }

    public function GetItem($params)
    {
        $id = (int)($params->id ?? $params->f_id ?? 0);
        if ($id <= 0) {
            return [
                'title' => Translator::t('Print alias'),
                'row' => $this->emptyRow(),
                'printers' => $this->printerNames(),
            ];
        }

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        return [
            'title' => Translator::t('Print alias'),
            'row' => $row,
            'printers' => $this->printerNames(),
        ];
    }

    public function save($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        $alias = trim((string)($params->f_alias ?? ''));
        $printer = trim((string)($params->f_printer ?? ''));
        if ($alias === '') {
            dieWithCode(Translator::t('Alias is required'));
        }
        if ($printer === '') {
            dieWithCode(Translator::t('Printer is required'));
        }
        if (mb_strlen($alias) > 255 || mb_strlen($printer) > 255) {
            dieWithCode(Translator::t('Name is too long'));
        }

        $dup = $this->db->select(
            'SELECT f_id FROM d_print_aliases WHERE f_alias = ? AND f_id <> ? LIMIT 1',
            'si',
            [$alias, $id]
        )->fetch_assoc();
        if ($dup) {
            dieWithCode(Translator::t('Alias already exists'));
        }

        $record = [
            'f_alias' => $alias,
            'f_printer' => $printer,
        ];

        if ($id <= 0) {
            $id = (int)$this->db->insert('d_print_aliases', $record);
        } else {
            if ($this->rowById($id) === null) {
                dieWithCode(Translator::t('Record not found'), 404);
            }
            $this->db->update('d_print_aliases', $record, $id);
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

        $this->db->delete('d_print_aliases', $id);

        return [
            'message' => Translator::t('Deleted'),
        ];
    }

    /** @return list<string> */
    private function printerNames(): array
    {
        $rows = $this->db->select(
            "SELECT TRIM(f_name) AS f_name FROM d_printers
             WHERE LENGTH(TRIM(IFNULL(f_name, ''))) > 0
             ORDER BY f_name"
        )->fetch_all(MYSQLI_ASSOC);
        $out = [];
        foreach ($rows as $r) {
            $n = trim((string)($r['f_name'] ?? ''));
            if ($n !== '') {
                $out[] = $n;
            }
        }
        return $out;
    }

    private function emptyRow(): array
    {
        return [
            'f_id' => 0,
            'f_alias' => '',
            'f_printer' => '',
        ];
    }

    private function rowById(int $id): ?array
    {
        $row = $this->db->select(
            'SELECT f_id, COALESCE(f_alias, \'\') AS f_alias, COALESCE(f_printer, \'\') AS f_printer
             FROM d_print_aliases WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        if ($row === null) {
            return null;
        }

        return [
            'f_id' => (int)$row['f_id'],
            'f_alias' => (string)($row['f_alias'] ?? ''),
            'f_printer' => (string)($row['f_printer'] ?? ''),
        ];
    }
}
