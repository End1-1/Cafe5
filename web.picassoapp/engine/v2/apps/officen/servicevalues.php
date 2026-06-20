<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . '/index.php';

class Servicevalues extends Auth
{
    private const SAVE_RULES = [
        'f_id' => 'integer|nullable',
        'f_value' => 'numeric|required',
        'f_comment' => 'nullable|string|max:64',
    ];

    private function rowById(int $id): ?array
    {
        $row = $this->select(
            'SELECT f_id, f_value, COALESCE(f_comment, \'\') AS f_comment FROM o_service_values WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        return $row ?: null;
    }

    public function Get($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($id <= 0) {
            $this->result['row'] = [
                'f_id' => 0,
                'f_value' => 0,
                'f_comment' => '',
            ];
            $this->echoResult();
            return;
        }

        $row = $this->rowById($id);
        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $this->result['row'] = $row;
        $this->echoResult();
    }

    public function Save($params)
    {
        $data = $this->ValidateParams($params, self::SAVE_RULES);

        $record = (object)[
            'f_value' => round((float)$data->f_value, 3),
            'f_comment' => trim((string)($data->f_comment ?? '')),
        ];

        $id = (int)($data->f_id ?? 0);
        if ($id <= 0) {
            $id = (int)$this->insert('o_service_values', $record);
        } else {
            if ($this->rowById($id) === null) {
                dieWithCode(Translator::t('Record not found'), 404);
            }
            $this->update('o_service_values', $record, $id);
        }

        $this->result['row'] = $this->rowById($id);
        $this->result['f_id'] = $id;
        $this->echoResult();
    }

    public function Remove($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($id <= 0) {
            dieWithCode(Translator::t('Id is required'));
        }

        if ($this->rowById($id) === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $this->delete('o_service_values', $id);
        $this->echoResult();
    }
}
