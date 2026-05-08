<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-06 04:02:07
# Last Modified:2026-03-06 04:02:10

class StoreDocuments
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
            foreach ((array)$item as $k => $v) {
                if (!empty($v)) {
                    $filter[$k] = $v;
                }
            }
        }

        $date1 = date('Y-m-d', strtotime($filter["date1"] ?? 'now'));
        $date2 = date('Y-m-d', strtotime($filter["date2"] ?? 'now'));
        if ($date1 === '1970-01-01') $date1 = date('Y-m-d');
        if ($date2 === '1970-01-01') $date2 = date('Y-m-d');

        $sql_where = " WHERE cast(sd.f_doc_date as date) BETWEEN '$date1' AND '$date2' ";

        // 1. Инициализируем типы и значения ПЕРВЫМИ двумя параметрами (для JOIN l_dictionary)
        $bindtypes = "ss";
        $bindvalues = [Translator::$locale, Translator::$locale];

        $map = [
            'status_id'    => 'sd.f_status',
            'type_id'      => 'sd.f_doc_type',
            'store_in_id'  => 'sd.f_store_in',
            'store_out_id' => 'sd.f_store_out',
            'patner_id'    => 'sd.f_partner'
        ];

        // 2. Динамически добавляем фильтры
        foreach ($map as $key => $column) {
            if (isset($filter[$key])) {
                $sql_where .= " AND $column = ? ";
                $bindtypes .= "s";
                $bindvalues[] = $filter[$key];
            }
        }

        // 3. Формируем SQL. ВАЖНО: плейсхолдеры ? для локали стоят в JOIN-ах,
        // а фильтры добавятся в $sql_where в конец.
        $sql = <<<EOD
    SELECT sd.f_id, 
    ld1.f_value, ld2.f_value, 
    date_fmt(sd.f_doc_date), sd.f_user_id, si.f_name, so.f_name, sd.f_sum, p.f_taxname,
    json_value(f_data, '$.comment'), JSON_VALUE(f_data, '$.create_user'), JSON_VALUE(f_data, '$.create_date')
    FROM store_document sd 
    LEFT JOIN c_storages si ON si.f_id=sd.f_store_in
    LEFT JOIN c_storages so ON so.f_id=sd.f_store_out
    LEFT JOIN l_dictionary ld1 ON ld1.f_dict_id=sd.f_status AND ld1.f_dict='store_statuses' AND ld1.f_lang=?
    LEFT JOIN l_dictionary ld2 ON ld2.f_dict_id=sd.f_doc_type AND ld2.f_dict='store_types' AND ld2.f_lang=?
    LEFT JOIN c_partners p ON p.f_id=sd.f_partner
    $sql_where
    EOD;

        // УДАЛИЛИ ПОВТОРНЫЙ bindtypes .= "ss" ОТСЮДА

        $headers = [
            Translator::t("Id"),
            Translator::t("Status"),
            Translator::t("Type"),
            Translator::t("Date"),
            Translator::t("Number"),
            Translator::t("Input store"),
            Translator::t("Output store"),
            Translator::t("Sum"),
            Translator::t("Partner"),
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
