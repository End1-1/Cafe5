<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-17 13:33:09
# Last Modified: 2026-01-17 13:33:14

class Workstations
{

    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $sql =<<<EOD
        SELECT t.f_id, t.f_type,  d.f_value, t.f_station_account, t.f_name
        from workstations t
        LEFT JOIN l_dictionary d ON d.f_dict_id=t.f_type AND d.f_dict='workstations_type' AND d.f_lang=?
        ORDER BY t.f_type, t.f_name
        EOD;

        return [
            "rows" => $this->db->select($sql, "s", [Translator::$locale])->fetch_all(MYSQLI_NUM),
            "toolbar" => ["new" => false, "delete" => true, "reload" => true],
            "headers" => [Translator::t("Id"), Translator::t("Id of type"), Translator::t("Name of type"), Translator::t("Station account"), Translator::t("Workstation")],
            "hidden_columns" => [1]
        ];
    }

    public function GetItem($params){ 
        return [
            "title" => Translator::t("Workstation")
        ];
    }
}
