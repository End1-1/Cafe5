<?php
# © 2026 , Kudryashov Vasili

class AxRestaurants
{
    private Auth $db;

    public function __construct(Auth $db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql = <<<SQL
        SELECT r.f_id,
               r.f_name,
               r.f_score,
               COALESCE(n.f_name, '') AS f_nationality,
               COALESCE(r.f_category, '') AS f_category,
               COALESCE(r.f_image_url, '') AS f_image_url,
               IF(r.f_location IS NULL, NULL, ST_Y(r.f_location)) AS f_lat,
               IF(r.f_location IS NULL, NULL, ST_X(r.f_location)) AS f_lng
        FROM ararix_restaurants r
        LEFT JOIN ararix_restaurant_nationality n ON n.f_id = r.f_nationality_id
        ORDER BY r.f_score DESC, r.f_name, r.f_id
        SQL;

        return [
            'rows' => $this->db->select($sql)->fetch_all(MYSQLI_NUM),
            'toolbar' => ['new' => true, 'delete' => true, 'reload' => true],
            'headers' => [
                Translator::t('Code'),
                Translator::t('Name'),
                Translator::t('Score'),
                Translator::t('Nationality'),
                Translator::t('Category'),
                Translator::t('Image URL'),
                Translator::t('Latitude'),
                Translator::t('Longitude'),
            ],
        ];
    }

    public function GetItem($params)
    {
        return [
            'title' => Translator::t('Restaurant'),
        ];
    }

    public function delete($params)
    {
        $id = (int)($params->f_id ?? $params->id ?? 0);
        if ($id <= 0) {
            dieWithCode(Translator::t('Id is required'));
        }

        $row = $this->db->select(
            'SELECT f_id FROM ararix_restaurants WHERE f_id = ?',
            'i',
            [$id]
        )->fetch_assoc();

        if ($row === null) {
            dieWithCode(Translator::t('Record not found'), 404);
        }

        $this->db->delete('ararix_restaurants', $id);

        return [
            'message' => Translator::t('Deleted'),
        ];
    }
}
