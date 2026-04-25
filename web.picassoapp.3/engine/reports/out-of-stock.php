<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modified - 2025-09-17 14:15:00
require_once __DIR__ . "/reports.php";

class OutOfStockReport extends Report
{
    public function __construct()
    {
        parent::__construct();
        if (empty($this->params->mode)) {
            $this->params->mode = 0;
        }
    }

    public function modeValues()
    {
        return [
            ["key" => $this->tr("Out of stock"), "value" => 0],
            ["key" => $this->tr("Overflow"), "value" => 1],
        ];
    }

    private function parseStores()
    {
        if (empty($this->params->store)) {
            return [];
        }

        $parts = [];
        if (is_string($this->params->store)) {
            $parts = preg_split('/\s*,\s*/', trim($this->params->store));
        } elseif (is_array($this->params->store)) {
            $parts = $this->params->store;
        } else {
            return [];
        }

        $stores = [];
        foreach ($parts as $p) {
            $p = trim($p);
            if ($p === '') continue;
            $id = (int)$p;
            if ($id > 0) $stores[] = $id;
        }

        return array_values(array_unique($stores));
    }

    protected function columns()
    {
        $cols = [
            $this->tr("Group"),
            $this->tr("Goods"),
            $this->tr("Scancode"),
            $this->tr("Out of stock"),
            $this->tr("Remains"),
            $this->tr("Qty of group"),
        ];

        $stores = $this->parseStores();
        if (!empty($stores)) {
            foreach ($stores as $storeId) {
                $cols[] = $this->tr("Store") . " " . $storeId;
                $cols[] = $this->tr("Store") . " " . $storeId . "B"; // дубликат
            }
        }

        return $cols;
    }

    protected function filter()
    {
        return [
            ["type" => "combo", "title" => $this->tr("Report mode"), "field" => "mode", "default" => 0, "values" => $this->modeValues()],
            ["type" => "keyvalue", "title" => $this->tr("Out of stock"), "field" => "outofstock", "filter" => "storages"],
            ["type" => "keyvalue", "title" => $this->tr("Main storage"), "field" => "mainstorage", "filter" => "storages"],
            ["type" => "value", "vartype" => "int", "title" => $this->tr("Greater than"), "field" => "greater"],
            ["type" => "keyvalue", "title" => $this->tr("Stores"), "field" => "store", "filter" => "storages"],
        ];
    }

    protected function rows()
    {
        switch ($this->params->mode) {
            case 0:
                return $this->outOfStock();
            case 1:
                return $this->overflow();
        }
        exitError("not in variants");
    }

    public function outOfStock()
    {
        if (empty($this->params->outofstock)) {
            return [];
        }
        if (empty($this->params->mainstorage)) {
            $this->params->mainstorage = 0;
        }

        $params = [
            $this->params->mainstorage,
            $this->params->outofstock
        ];
        $types = "ii";

        $sqlStores = "";
        $selectStores = "";
        $stores = $this->parseStores();
        if (!empty($stores)) {
            foreach ($stores as $idx => $storeId) {
                $alias = "s" . $idx;
                $aliasB = "s" . $idx . "b";

                $sqlStores .= " LEFT JOIN (SELECT f_goods, SUM(f_qty*f_type) AS f_qty 
                                FROM a_store WHERE f_store=? GROUP BY 1) {$alias} 
                                ON {$alias}.f_goods=og.f_goods ";

                $sqlStores .= " LEFT JOIN (
                                SELECT gc.f_goods, a.f_goods AS f_bases, SUM(a.f_qty*a.f_type) AS f_qty 
                                FROM a_store a
                                LEFT JOIN c_goods g ON g.f_id=a.f_goods
                                LEFT JOIN c_goods_complectation gc on gc.f_base=g.f_id
                                WHERE f_store=? AND g.f_unit=10
                                GROUP BY 1
                                HAVING SUM(a.f_qty*a.f_type)>0) {$aliasB} 
                                ON {$aliasB}.f_goods=og.f_goods ";

                $selectStores .= ", COALESCE({$alias}.f_qty, 0) AS store_{$storeId}";
                $selectStores .= ", COALESCE({$aliasB}.f_qty, 0) AS store_{$storeId}B";

                $params[] = $storeId;
                $params[] = $storeId;
                $types .= "ii";
            }
        }

        $filter = "";
        if (!empty($this->params->outofstock)) {
            $filter = " AND og.f_store=?";
            $params[] = $this->params->outofstock;
            $types .= "i";
        }

        $sql = <<<EOD
        SELECT gr.f_name, g.f_name, g.f_scancode, 
               SUM(og.f_qty*og.f_type) AS total_qty, COALESCE(ms.f_qty, 0) AS main_qty, COALESCE(mg.f_gqty, 0) AS group_qty
               {$selectStores}
        FROM a_store og 
        LEFT JOIN c_goods g  ON g.f_id=og.f_goods 
        LEFT JOIN c_groups gr ON gr.f_id=g.f_group 
        LEFT JOIN (SELECT f_goods, SUM(f_qty*f_type) AS f_qty 
                   FROM a_store WHERE f_store=? GROUP BY 1) ms ON ms.f_goods=og.f_goods 
        INNER JOIN (SELECT g1.f_group, SUM(f_qty*f_type) AS f_gqty 
                    FROM a_store a1 
                    LEFT JOIN c_goods g1 ON g1.f_id=a1.f_goods 
                    WHERE a1.f_store=? 
                    GROUP BY g1.f_group 
                    HAVING SUM(f_qty*f_type)>0) mg ON mg.f_group=g.f_group 
        {$sqlStores}
        WHERE g.f_enabled=1 {$filter} 
        and ( COALESCE({$alias}.f_qty, 0) > 0 or COALESCE({$aliasB}.f_qty, 0)>0)
        GROUP BY g.f_scancode 
        HAVING SUM(og.f_qty*og.f_type)=0 
        ORDER BY gr.f_name
        EOD;

        return $this->db->stmtall(
            $sql,
            $types,
            $params
        )->fetch_all();
    }

    public function overflow()
    {
        if (empty($this->params->greater)) {
            $this->params->greater = 0;
        }
        if (empty($this->params->outofstock)) {
            return [];
        }
        if (empty($this->params->mainstorage)) {
            $this->params->mainstorage = 0;
        }

        $filter = "";
        if (!empty($this->params->outofstock)) {
            $filter .= " and og.f_store={$this->params->outofstock}";
        }

        $sql = <<<EOD
        select gr.f_name, g.f_name, g.f_scancode, 
               sum(og.f_qty*og.f_type), coalesce(ms.f_qty, 0), COALESCE(mg.f_gqty, 0) 
        from a_store og 
        left join c_goods g  on g.f_id=og.f_goods 
        left join c_groups gr on gr.f_id=g.f_group 
        left join (select f_goods, sum(f_qty*f_type) as f_qty 
                   from a_store where f_store=? group by 1) ms on ms.f_goods=og.f_goods 
        inner join (select g1.f_group, sum(f_qty*f_type) as f_gqty 
                    from a_store a1 
                    left join c_goods g1 on g1.f_id=a1.f_goods 
                    where a1.f_store=? 
                    group by g1.f_group 
                    having sum(f_qty*f_type)>0) mg on mg.f_group=g.f_group 
        where g.f_enabled=1 {$filter} 
        group by g.f_scancode 
        having sum(og.f_qty*og.f_type) > ? 
        order by gr.f_name
        EOD;

        return $this->db->stmtall(
            $sql,
            "iii",
            [$this->params->mainstorage, $this->params->outofstock, $this->params->greater]
        )->fetch_all();
    }

    protected function sumColumns() {}

    protected function widget()
    {
        return ["icon" => "cash.png", "title" => $this->tr("Stock remains")];
    }
}

$out = new OutOfStockReport();
$out->echoResult();
