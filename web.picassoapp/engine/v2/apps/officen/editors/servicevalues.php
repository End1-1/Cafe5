<?php
# © 2026 , Kudryashov Vasili

class ServiceValues
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql = <<<SQL
        SELECT f_id, f_value, COALESCE(f_comment, '') AS f_comment
        FROM o_service_values
        ORDER BY f_value, f_id
        SQL;

        return [
            'rows' => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            'toolbar' => ['new' => true, 'delete' => true, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Value'),
                Translator::t('Comment'),
            ],
        ];
    }
}
