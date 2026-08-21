<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-28 21:28:31
# Last Modified: 2026-01-28 21:28:34

class ConsumedGoods
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
                if ($v !== '' && $v !== null) {
                    $filter[$k] = $v;
                }
            }
        }

        $summarize = (int)($filter["summarize"] ?? 1);

        $date1 = date('Y-m-d', strtotime($filter["date1"] ?? 'now'));
        $date2 = date('Y-m-d', strtotime($filter["date2"] ?? 'now'));
        if ($date1 === '1970-01-01') {
            $date1 = date('Y-m-d');
        }
        if ($date2 === '1970-01-01') {
            $date2 = date('Y-m-d');
        }

        $group_id = (int)($filter["group_id"] ?? 0);

        $filterSql = $this->buildSaleFilters($date1, $date2, $group_id, $summarize);

        if ($summarize === 2) {
            return $this->getOnlyConsumed($filterSql);
        }
        if ($summarize === 1) {
            return $this->getSummarizedDishes($filterSql);
        }
        return $this->getDetailedDishes($filterSql);
    }

    /**
     * @return array{date_filter:string,group_filter_dish:string,group_filter_cons:string,bindtypes:string,bindvalues:array}
     */
    private function buildSaleFilters(string $date1, string $date2, int $group_id, int $summarize): array
    {
        $bindtypes = "ss";
        $bindvalues = [$date1, $date2];
        $date_filter = " AND oh.f_datecash BETWEEN ? AND ? ";

        $group_filter_dish = "";
        $group_filter_cons = "";
        if ($group_id > 0) {
            if ($summarize === 2) {
                $group_filter_cons = " AND g2.f_group = ? ";
            } else {
                $group_filter_dish = " AND g.f_group = ? ";
            }
            $bindtypes .= "i";
            $bindvalues[] = $group_id;
        }

        return [
            "date_filter" => $date_filter,
            "group_filter_dish" => $group_filter_dish,
            "group_filter_cons" => $group_filter_cons,
            "bindtypes" => $bindtypes,
            "bindvalues" => $bindvalues,
        ];
    }

    private function reportMeta(array $headers, array $sum, array $rows): array
    {
        return [
            "rows" => $rows,
            "toolbar" => ["delete" => false, "reload" => true, "filter" => true],
            "headers" => $headers,
            "hidden_columns" => [0],
            "sum" => $sum,
            "filter" => [
                ["type" => "viewmode", "name" => "summarize", "label" => Translator::t("Summarize"), "default" => 1, "values" => [
                    ["label" => Translator::t("Summarize dishes"), "value" => 1],
                    ["label" => Translator::t("Dont summarize dishes"), "value" => 0],
                    ["label" => Translator::t("Only consumed goods"), "value" => 2],
                ]],
                ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
                ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
                ["type" => "keyvalue", "name" => "group_id", "label" => Translator::t("Group"), "function" => "goods_group"],
            ],
        ];
    }

    private function dishReportHeaders(): array
    {
        return [
            Translator::t("Storage"),
            Translator::t("Group"),
            Translator::t("Item name"),
            Translator::t("Consumed item"),
            Translator::t("Sold qty"),
            Translator::t("Sold sum"),
            Translator::t("Consumed qty"),
            Translator::t("Consumed amount"),
        ];
    }

    /** Mode 2 — only consumed goods (unchanged logic). */
    private function getOnlyConsumed(array $f): array
    {
        $sql = <<<EOD
        SELECT
            st.f_name AS f_store_name,
            gr2.f_name AS f_group_name,
            g2.f_name AS f_consumed_item_name,
            money_fmt(SUM(ogc.f_qty)) AS f_consumed_qty,
            money_fmt(SUM(ogc.f_qty * ogc.f_price)) AS f_consumed_total
        FROM store_calc_queue ogc
        LEFT JOIN c_storages st ON st.f_id = ogc.f_store_id
        LEFT JOIN c_goods g2 ON g2.f_id = ogc.f_item_id
        LEFT JOIN c_groups gr2 ON gr2.f_id = g2.f_group
        LEFT JOIN o_goods og ON og.f_id = ogc.f_row_sale_id
        LEFT JOIN o_header oh ON oh.f_id = og.f_header
        WHERE oh.f_state = 2 AND og.f_state = 1
        {$f["date_filter"]}
        {$f["group_filter_cons"]}
        GROUP BY st.f_name, gr2.f_name, g2.f_name
        ORDER BY st.f_name, gr2.f_name, g2.f_name
        EOD;

        $headers = [
            Translator::t("Storage"),
            Translator::t("Group"),
            Translator::t("Consumed item"),
            Translator::t("Consumed qty"),
            Translator::t("Consumed amount"),
        ];
        $rows = $this->db->select($sql, $f["bindtypes"], $f["bindvalues"])->fetch_all(MYSQLI_NUM);
        return $this->reportMeta($headers, [3, 4], $rows);
    }

    /**
     * Mode 1 — dish header row (sold qty/sum once) + component rows (consumption only).
     * Footer auto-sum of columns 4–7 is then correct.
     */
    private function getSummarizedDishes(array $f): array
    {
        $dishSql = <<<EOD
        SELECT
            g.f_id AS f_goods_id,
            COALESCE(gr.f_name, '') AS f_group_name,
            COALESCE(g.f_name, '') AS f_item_name,
            money_fmt(SUM(og.f_qty)) AS f_qty,
            money_fmt(SUM(og.f_total)) AS f_total
        FROM o_goods og
        INNER JOIN o_header oh ON oh.f_id = og.f_header
        INNER JOIN c_goods g ON g.f_id = og.f_goods
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        WHERE oh.f_state = 2 AND og.f_state = 1
          AND EXISTS (
              SELECT 1 FROM store_calc_queue ogc WHERE ogc.f_row_sale_id = og.f_id
          )
        {$f["date_filter"]}
        {$f["group_filter_dish"]}
        GROUP BY g.f_id, gr.f_name, g.f_name
        ORDER BY gr.f_name, g.f_name
        EOD;

        $consSql = <<<EOD
        SELECT
            g.f_id AS f_goods_id,
            COALESCE(st.f_name, '') AS f_store_name,
            COALESCE(gr.f_name, '') AS f_group_name,
            COALESCE(g.f_name, '') AS f_item_name,
            COALESCE(g2.f_name, '') AS f_consumed_item_name,
            money_fmt(SUM(ogc.f_qty)) AS f_consumed_qty,
            money_fmt(SUM(ogc.f_qty * ogc.f_price)) AS f_consumed_total
        FROM store_calc_queue ogc
        INNER JOIN o_goods og ON og.f_id = ogc.f_row_sale_id
        INNER JOIN o_header oh ON oh.f_id = og.f_header
        LEFT JOIN c_storages st ON st.f_id = ogc.f_store_id
        LEFT JOIN c_goods g ON g.f_id = og.f_goods
        LEFT JOIN c_goods g2 ON g2.f_id = ogc.f_item_id
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        WHERE oh.f_state = 2 AND og.f_state = 1
        {$f["date_filter"]}
        {$f["group_filter_dish"]}
        GROUP BY g.f_id, st.f_name, gr.f_name, g.f_name, g2.f_name
        ORDER BY gr.f_name, g.f_name, st.f_name, g2.f_name
        EOD;

        $dishes = $this->db->select($dishSql, $f["bindtypes"], $f["bindvalues"])->fetch_all(MYSQLI_ASSOC);
        $cons = $this->db->select($consSql, $f["bindtypes"], $f["bindvalues"])->fetch_all(MYSQLI_ASSOC);

        $consByGoods = [];
        foreach ($cons as $c) {
            $gid = (int)$c["f_goods_id"];
            $consByGoods[$gid][] = $c;
        }

        $rows = [];
        foreach ($dishes as $d) {
            $gid = (int)$d["f_goods_id"];
            $rows[] = [
                "",
                $d["f_group_name"],
                $d["f_item_name"],
                "",
                $d["f_qty"],
                $d["f_total"],
                "",
                "",
            ];
            foreach ($consByGoods[$gid] ?? [] as $c) {
                $rows[] = [
                    $c["f_store_name"],
                    $c["f_group_name"],
                    $c["f_item_name"],
                    $c["f_consumed_item_name"],
                    "",
                    "",
                    $c["f_consumed_qty"],
                    $c["f_consumed_total"],
                ];
            }
        }

        return $this->reportMeta($this->dishReportHeaders(), [4, 5, 6, 7], $rows);
    }

    /**
     * Mode 0 — per sale line: header with sold qty/sum, then its recipe components.
     */
    private function getDetailedDishes(array $f): array
    {
        $dishSql = <<<EOD
        SELECT
            og.f_id AS f_sale_row_id,
            COALESCE(gr.f_name, '') AS f_group_name,
            COALESCE(g.f_name, '') AS f_item_name,
            money_fmt(og.f_qty) AS f_qty,
            money_fmt(og.f_total) AS f_total
        FROM o_goods og
        INNER JOIN o_header oh ON oh.f_id = og.f_header
        INNER JOIN c_goods g ON g.f_id = og.f_goods
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        WHERE oh.f_state = 2 AND og.f_state = 1
          AND EXISTS (
              SELECT 1 FROM store_calc_queue ogc WHERE ogc.f_row_sale_id = og.f_id
          )
        {$f["date_filter"]}
        {$f["group_filter_dish"]}
        ORDER BY gr.f_name, g.f_name, og.f_id
        EOD;

        $consSql = <<<EOD
        SELECT
            ogc.f_row_sale_id AS f_sale_row_id,
            COALESCE(st.f_name, '') AS f_store_name,
            COALESCE(gr.f_name, '') AS f_group_name,
            COALESCE(g.f_name, '') AS f_item_name,
            COALESCE(g2.f_name, '') AS f_consumed_item_name,
            money_fmt(ogc.f_qty) AS f_consumed_qty,
            money_fmt(ogc.f_qty * ogc.f_price) AS f_consumed_total
        FROM store_calc_queue ogc
        INNER JOIN o_goods og ON og.f_id = ogc.f_row_sale_id
        INNER JOIN o_header oh ON oh.f_id = og.f_header
        LEFT JOIN c_storages st ON st.f_id = ogc.f_store_id
        LEFT JOIN c_goods g ON g.f_id = og.f_goods
        LEFT JOIN c_goods g2 ON g2.f_id = ogc.f_item_id
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        WHERE oh.f_state = 2 AND og.f_state = 1
        {$f["date_filter"]}
        {$f["group_filter_dish"]}
        ORDER BY gr.f_name, g.f_name, ogc.f_row_sale_id, st.f_name, g2.f_name
        EOD;

        $dishes = $this->db->select($dishSql, $f["bindtypes"], $f["bindvalues"])->fetch_all(MYSQLI_ASSOC);
        $cons = $this->db->select($consSql, $f["bindtypes"], $f["bindvalues"])->fetch_all(MYSQLI_ASSOC);

        $consBySale = [];
        foreach ($cons as $c) {
            $sid = (string)$c["f_sale_row_id"];
            $consBySale[$sid][] = $c;
        }

        $rows = [];
        foreach ($dishes as $d) {
            $sid = (string)$d["f_sale_row_id"];
            $rows[] = [
                "",
                $d["f_group_name"],
                $d["f_item_name"],
                "",
                $d["f_qty"],
                $d["f_total"],
                "",
                "",
            ];
            foreach ($consBySale[$sid] ?? [] as $c) {
                $rows[] = [
                    $c["f_store_name"],
                    $c["f_group_name"],
                    $c["f_item_name"],
                    $c["f_consumed_item_name"],
                    "",
                    "",
                    $c["f_consumed_qty"],
                    $c["f_consumed_total"],
                ];
            }
        }

        return $this->reportMeta($this->dishReportHeaders(), [4, 5, 6, 7], $rows);
    }

    public function GetItem($params)
    {
        require_once __DIR__ . "/../../waiter/order.php";
        $ord = new Order();
        return $ord->GetOrder($params->id);
    }

    /**
     * Closed sale documents for the report period (for store output recalculation).
     */
    public function listSaleDocs($params)
    {
        $filterRaw = $params->filter ?? [];
        $filter = [];
        foreach ($filterRaw as $item) {
            foreach ((array)$item as $k => $v) {
                if ($v !== '' && $v !== null) {
                    $filter[$k] = $v;
                }
            }
        }

        $date1 = date('Y-m-d', strtotime($filter["date1"] ?? 'now'));
        $date2 = date('Y-m-d', strtotime($filter["date2"] ?? 'now'));
        if ($date1 === '1970-01-01') {
            $date1 = date('Y-m-d');
        }
        if ($date2 === '1970-01-01') {
            $date2 = date('Y-m-d');
        }

        $bindtypes = "ss";
        $bindvalues = [$date1, $date2];

        $sql = <<<EOD
        SELECT
            oh.f_id,
            oh.f_prefix,
            date_fmt(oh.f_datecash) AS f_date
        FROM o_header oh
        WHERE oh.f_state = 2
          AND oh.f_datecash BETWEEN ? AND ?
        ORDER BY oh.f_datecash, oh.f_prefix
        EOD;

        $rows = $this->db->select($sql, $bindtypes, $bindvalues)->fetch_all(MYSQLI_ASSOC);
        return [
            "docs" => $rows,
            "date1" => $date1,
            "date2" => $date2,
        ];
    }

    /**
     * Rollback store write-off for one sale and post again from current recipes.
     */
    public function recalcSale($params)
    {
        $id = trim((string)($params->id ?? ""));
        if ($id === "") {
            dieWithCode(Translator::t("Order id is required"));
        }
        require_once __DIR__ . "/../../waiter/order.php";
        $ord = new Order();
        $costDepend = !empty($params->cost_depend_on_service_and_discount);
        $ord->RecalculateStoreOutput($id, $costDepend);
        return [
            "id" => $id,
            "ok" => true,
        ];
    }
}
