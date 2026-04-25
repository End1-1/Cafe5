<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-06 04:02:07
# Last Modified:2026-03-06 04:02:10

class StoreInventory
{

    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $filterRaw = $params->filter ?? [];
        $filter = [];
        foreach ($filterRaw as $item) {
            if (is_object($item)) {
                foreach ($item as $k => $v) {
                    $filter[$k] = $v;
                }
            } elseif (is_array($item)) {
                foreach ($item as $k => $v) {
                    $filter[$k] = $v;
                }
            }
        }

        $date1 = $filter["date1"] ?? date('Y-m-d');
        $date2 = $filter["date2"] ?? date('Y-m-d');
        $date1 = date('Y-m-d', strtotime($date1));
        $date2 = date('Y-m-d', strtotime($date2));
        if ($date1 === false || $date1 === '1970-01-01') {
            $date1 = date('Y-m-d');
        }
        if ($date2 === false || $date2 === '1970-01-01') {
            $date2 = date('Y-m-d');
        }
        $date_filter = "where cast(sd.f_date as date) BETWEEN  '$date1' AND '$date2' ";

        $bindtypes = "";
        $bindvalues = [];


        $sql = <<<EOD
        SELECT sd.f_id, date_fmt(sd.f_date), '', si.f_name, 
        json_value(f_data, '$.f_comment'), JSON_VALUE(f_data, '$.f_create_user'), JSON_VALUE(f_data, '$.f_create_date')
        FROM store_inventory_document sd 
        LEFT JOIN c_storages si ON si.f_id=sd.f_store
        $date_filter
        EOD;

        $headers = [
            Translator::t("Id"),
            Translator::t("Date"),
            Translator::t("Number"),
            Translator::t("Store"),
            Translator::t("Comment"),
            Translator::t("Created by"),
            Translator::t("Created date"),
        ];


        return [
            "rows" => $this->db->select($sql, $bindtypes, $bindvalues)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => true, "reload" => true, "filter" => true],
            "headers" => $headers,
            "hidden_columns" => [0],
            "sum" => [7],
            "filter" => [
                ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
                ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
                ["type" => "keyvalue", "name" => "status_id", "label" => Translator::t("Status"), "function" => "store_doc_status"],
                ["type" => "keyvalue", "name" => "type_id", "label" => Translator::t("Type"), "function" => "store_doc_type"],
                ["type" => "keyvalue", "name" => "store_in_id", "label" => Translator::t("Input store"), "function" => "store"],
                ["type" => "keyvalue", "name" => "store_out_id", "label" => Translator::t("Output store"), "function" => "store"],
                ["type" => "keyvalue", "name" => "patner_id", "label" => Translator::t("Partner"), "function" => "partner"],
            ]
        ];
    }

    public function GetItem($params) {}
}
