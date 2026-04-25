<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-11 08:59:14
# Last Modified: 2026-03-11 08:59:18

class StoreMoves
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
        $date_filter = "where cast(sd.f_doc_date as date) BETWEEN  '$date1' AND '$date2' ";


        $bindtypes = "";
        $bindvalues = [];
        $bindtypes .= "s";
        $bindvalues[] = Translator::$locale;

        if (($filter["type_id"] ?? 0) > 0) {
            $bindtypes .= "i";
            $bindvalues[] = $filter["type_id"];
            $date_filter .= " and sd.f_doc_type=? ";
        }

        if (($filter["store_in_id"] ?? 0) > 0) {
            $bindtypes .= "i";
            $bindvalues[] = $filter["store_in_id"];
            $date_filter .= " and sd.f_store_in=? ";
        }

        if (($filter["store_out_id"] ?? 0) > 0) {
            $bindtypes .= "i";
            $bindvalues[] = $filter["store_out_id"];
            $date_filter .= " and sd.f_store_out=? ";
        }

        $sql = <<<EOD
        SELECT sd.f_id, ld2.f_value, 
        date_fmt(sd.f_doc_date), sd.f_user_id, si.f_name as f_store_in_name, so.f_name as f_store_out_name, 
        g.f_name as f_goods_name, money_fmt(su.f_qty), money_fmt(su.f_price), money_fmt(su.f_total), p.f_taxname,
        json_value(sd.f_data, '$.comment'), JSON_VALUE(sd.f_data, '$.create_user'), JSON_VALUE(sd.f_data, '$.create_date')
        from store_user su
        left join store_document sd on sd.f_id=su.f_doc
        left join c_goods g on g.f_id=su.f_item_id
        LEFT JOIN c_storages si ON si.f_id=sd.f_store_in
        LEFT JOIN c_storages so ON so.f_id=sd.f_store_out
        LEFT JOIN l_dictionary ld2 ON ld2.f_dict_id=sd.f_doc_type AND ld2.f_dict='store_types' AND ld2.f_lang=?
        left join c_partners p on p.f_id=sd.f_partner
        $date_filter
        EOD;


        $headers = [
            Translator::t("Id"),
            Translator::t("Type"),
            Translator::t("Date"),
            Translator::t("Number"),
            Translator::t("Input store"),
            Translator::t("Output store"),
            Translator::t("Goods"),
            Translator::t("Qty"),
            Translator::t("Price"),
            Translator::t("Sum"),
            Translator::t("Partner"),
            Translator::t("Comment"),
            Translator::t("Created by"),
            Translator::t("Created date"),
        ];


        return [
            "debug" => $sql,
            "rows" => $this->db->select($sql, $bindtypes, $bindvalues)->fetch_all(MYSQLI_NUM),
            "toolbar" => ["delete" => true, "reload" => true, "filter" => true],
            "headers" => $headers,
            "hidden_columns" => [0],
            "sum" => [7, 9],
            "filter" => [
                ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
                ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
                ["type" => "keyvalue", "name" => "type_id", "label" => Translator::t("Type"), "function" => "store_doc_type"],
                ["type" => "keyvalue", "name" => "store_in_id", "label" => Translator::t("Input store"), "function" => "store"],
                ["type" => "keyvalue", "name" => "store_out_id", "label" => Translator::t("Output store"), "function" => "store"],
                ["type" => "keyvalue", "name" => "patner_id", "label" => Translator::t("Partner"), "function" => "partner"],
            ]
        ];
    }

    public function GetItem($params) {}
}
