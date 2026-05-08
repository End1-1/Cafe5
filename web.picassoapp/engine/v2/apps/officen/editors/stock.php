<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-11 08:59:14
# Last Modified: 2026-04-21 12:10:00

class Stock
{
    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    /**
     * Основной метод получения остатков
     */
    public function get($params)
    {
        $filterRaw = $params->filter ?? [];
        $filter = [];
        foreach ($filterRaw as $item) {
            foreach ((array)$item as $k => $v) {
                $filter[$k] = $v;
            }
        }

        $viewMode = (int) ($filter["summarize"] ?? 1);

        // Переключаем логику в зависимости от режима отображения
        if ($viewMode === 2) {
            // Исторические остатки (расчет по движениям)
            $rows = $this->getHistorical($filter);
        } else {
            // Текущие остатки (по таблице stock)
            $rows = $this->getCurrentStock($filter, $viewMode === 1);
        }

        return [
            "rows" => $rows,
            "toolbar" => ["delete" => true, "reload" => true, "filter" => true],
            "headers" => [
                Translator::t("Id"),
                Translator::t("Storage"),
                Translator::t("Group"),
                Translator::t("Name"),
                Translator::t("Qty"),
                Translator::t("Unit"),
                Translator::t("Price"),
                Translator::t("Sum"),
                Translator::t("Expiry"),
            ],
            "hidden_columns" => [0],
            "col_widths" => [
                ["col" => 4, "width" => 100],
                ["col" => 7, "width" => 100]
            ],
            "sum" => [4, 7],
            "filter" => $this->getFilterConfig()
        ];
    }

    /**
     * Логика текущих остатков (summarize 0 и 1)
     */
    protected function getCurrentStock($filter, $summarize)
    {
        $filterExtra = "";
        $filterBindtypes = "";
        $filterBindvalues = [];

        if (($filter["store"] ?? 0) > 0) {
            $filterExtra .= " AND st.f_store_id=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int) $filter["store"];
        }
        if (($filter["goods_group"] ?? 0) > 0) {
            $filterExtra .= " AND g.f_group=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int) $filter["goods_group"];
        }
        if (($filter["goods"] ?? 0) > 0) {
            $filterExtra .= " AND st.f_item_id=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int) $filter["goods"];
        }

        $joinsAndFrom = " FROM store_stock st
            LEFT JOIN c_goods g ON g.f_id = st.f_item_id
            LEFT JOIN c_groups gr ON gr.f_id = g.f_group
            LEFT JOIN c_units u ON u.f_id = g.f_unit
            LEFT JOIN c_storages s ON s.f_id = st.f_store_id
            WHERE 1=1 " . $filterExtra;

        if ($summarize) {
            $sql = "SELECT MIN(st.f_doc), MAX(s.f_name), MAX(gr.f_name), MAX(g.f_name),
                    money_fmt(SUM(st.f_qty_left)), MAX(u.f_name),
                    money_fmt(SUM(st.f_qty_left * st.f_price) / NULLIF(SUM(st.f_qty_left), 0)),
                    money_fmt(SUM(st.f_qty_left * st.f_price)),
                    '' " . $joinsAndFrom . " GROUP BY st.f_store_id, st.f_item_id";
        } else {
            $sql = "SELECT st.f_doc, s.f_name, gr.f_name, g.f_name,
                    money_fmt(st.f_qty_left), u.f_name, money_fmt(st.f_price), 
                    money_fmt(st.f_qty_left * st.f_price),
                    date_fmt(st.f_expiry_date) " . $joinsAndFrom;
        }

        return $this->db->select($sql, $filterBindtypes, $filterBindvalues)->fetch_all(MYSQLI_NUM);
    }

    /**
     * Расчет исторических остатков на дату (summarize 2)
     */
    protected function getHistorical($filter)
    {
        $dateEnd = $filter["date1"] ?? date('Y-m-d');
        $dateEnd .= " 23:59:59"; // Захватываем весь день

        $filterExtra = "";
        $filterBindtypes = "s";
        $filterBindvalues = [$dateEnd];

        if (($filter["store"] ?? 0) > 0) {
            $filterExtra .= " AND m.f_store_id=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int) $filter["store"];
        }
        if (($filter["goods_group"] ?? 0) > 0) {
            $filterExtra .= " AND g.f_group=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int) $filter["goods_group"];
        }
        if (($filter["goods"] ?? 0) > 0) {
            $filterExtra .= " AND m.f_item_id=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int) $filter["goods"];
        }

        $sql = "SELECT 
                    MIN(m.f_id), 
                    s.f_name, 
                    gr.f_name, 
                    g.f_name,
                    money_fmt(SUM(m.f_qty_in - m.f_qty_out)), 
                    u.f_name, 
                    money_fmt(SUM(m.f_total) / NULLIF(SUM(m.f_qty_in - m.f_qty_out), 0)),
                    money_fmt(SUM(m.f_total)),
                    ''
                FROM store_moves m
                INNER JOIN store_document sd ON sd.f_id = m.f_doc
                LEFT JOIN c_goods g ON g.f_id = m.f_item_id
                LEFT JOIN c_groups gr ON gr.f_id = g.f_group
                LEFT JOIN c_units u ON u.f_id = g.f_unit
                LEFT JOIN c_storages s ON s.f_id = m.f_store_id
                WHERE sd.f_doc_date <= ? $filterExtra
                GROUP BY m.f_store_id, m.f_item_id
                HAVING SUM(m.f_qty_in - m.f_qty_out) <> 0";

        return $this->db->select($sql, $filterBindtypes, $filterBindvalues)->fetch_all(MYSQLI_NUM);
    }

    private function getFilterConfig()
    {
        return [
            ["type" => "date", "name" => "date1", "label" => Translator::t("Date")],
            ["type" => "combobox", "name" => "summarize", "label" => Translator::t("View mode"), "default" => 1, "values" => [
                ["label" => Translator::t("Current total"), "value" => 1],
                ["label" => Translator::t("Current detailed"), "value" => 0],
                ["label" => Translator::t("Historical on date"), "value" => 2]
            ]],
            ["type" => "keyvalue", "name" => "store", "label" => Translator::t("Storage"), "function" => "store"],
            ["type" => "keyvalue", "name" => "goods_group", "label" => Translator::t("Goods group"), "function" => "goods_group"],
            ["type" => "keyvalue", "name" => "goods", "label" => Translator::t("Goods"), "function" => "goods"],
        ];
    }
}
