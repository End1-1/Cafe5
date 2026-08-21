<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-11-23 11:20:20
# Last Modified: 2026-06-06

require_once "report.php";

class MenuReview extends Report
{
    private const GOODS_HANDLER = "78dd7b1e-12d5-11f1-9245-8a884be02f31";

    /** @var array<int, array{id:int, name:string}>|null */
    private ?array $menuColumns = null;

    protected function widget()
    {
        return [
            "title" => Translator::t("Review of menu"),
            "icon" => "template.png",
            "version" => 2
        ];
    }

    protected function hiddenCols()
    {
        return [$this->colorColumnIndex()];
    }

    protected function sumColumns() {}

    protected function rowColors()
    {
        return [$this->colorColumnIndex()];
    }

    protected function columns()
    {
        return match ($this->params->mode) {
            1 => $this->columns_v1(),
            2 => $this->columns_v1(),
            default => throw new Exception("Invalid mode"),
        };
    }

    protected function childColumns()
    {
        return match ($this->params->mode) {
            1 => [],
            2 => [
                Translator::t("Goods name"),
                Translator::t("Qty"),
                Translator::t("Price"),
                Translator::t("Total"),
            ]
        };
    }

    public function handler()
    {
        return [self::GOODS_HANDLER, self::GOODS_HANDLER];
    }

    private function fixedColumnCount(): int
    {
        return 6;
    }

    private function tailColumnCount(): int
    {
        return 6;
    }

    private function colorColumnIndex(): int
    {
        $menus = $this->menuColumnsForReport();
        return $this->fixedColumnCount() + count($menus) * 2 + $this->tailColumnCount() - 1;
    }

    /**
     * @return array<int, array{id:int, name:string}>
     */
    private function menuColumnsForReport(): array
    {
        if ($this->menuColumns !== null) {
            return $this->menuColumns;
        }

        if (!empty($this->params->menu)) {
            $sql = <<<EOD
            SELECT f_id, f_name
            FROM c_menu_names
            WHERE f_id IN ({$this->params->menu})
            ORDER BY f_name
            EOD;
        } else {
            $where = $this->buildWhere();
            $sql = <<<EOD
            SELECT DISTINCT mn.f_id, mn.f_name
            FROM c_menu mm
            INNER JOIN c_goods g ON g.f_id = mm.f_dish
            LEFT JOIN c_menu_names mn ON mn.f_id = mm.f_menu
            $where
            ORDER BY mn.f_name
            EOD;
        }

        $this->menuColumns = [];
        foreach ($this->select($sql)->fetch_all(MYSQLI_ASSOC) as $row) {
            $this->menuColumns[] = [
                "id" => (int)$row["f_id"],
                "name" => $row["f_name"],
            ];
        }

        return $this->menuColumns;
    }

    private function columns_v1()
    {
        $cols = [
            Translator::t("Code"),
            Translator::t("Status"),
            Translator::t("Department"),
            Translator::t("Type"),
            Translator::t("Dish name"),
            Translator::t("Weight"),
        ];

        foreach ($this->menuColumnsForReport() as $menu) {
            $cols[] = Translator::t("Price") . " ({$menu['name']})";
            $cols[] = Translator::t("Cost Multiplier") . " ({$menu['name']})";
        }

        $cols[] = Translator::t("Self cost");
        $cols[] = Translator::t("Daily");
        $cols[] = Translator::t("Storage");
        $cols[] = Translator::t("Printer 1");
        $cols[] = Translator::t("Printer 2");
        $cols[] = Translator::t("Color");

        return $cols;
    }

    protected function filter()
    {
        $modevalues = [
            ["key" => Translator::t("Only dishes"), "value" => 1],
            ["key" => Translator::t("Dishes and recipes"), "value" => 2],
        ];
        return [
            ["type" => "combo", "title" => Translator::t("Report mode"), "field" => "mode", "default" => 2, "values" => $modevalues],
            ["type" => "keyvalue", "title" => Translator::t("Menu"), "field" => "menu", "filter" => "menunames"],
            ["type" => "keyvalue", "title" =>  Translator::t("Status"), "field" => "status", "filter" => "menustatus"],
            ["type" => "keyvalue", "title" =>  Translator::t("Dish group"), "field" => "dishgroup", "filter" => "dishgroup"],
            ["type" => "keyvalue", "title" =>  Translator::t("Dish"), "field" => "dish", "filter" => "dish"],
            ["type" => "keyvalue", "title" =>  Translator::t("Goods"), "field" => "goods", "filter" => "goods"],
        ];
    }

    protected function rows()
    {
        return match ($this->params->mode) {
            1 => $this->rows_v1(),
            2 => $this->rows_v2(),
            default => throw new Exception("Invalid mode"),
        };
    }

    private function buildWhere(): string
    {
        $where = "WHERE g.f_type IN (1, 2, 5) ";

        if (strlen($this->params->status ?? "") > 0) {
            $where .= " AND mm.f_state IN ({$this->params->status}) ";
        }
        if (!empty($this->params->menu)) {
            $where .= " AND mm.f_menu IN ({$this->params->menu}) ";
        }
        if (!empty($this->params->dish)) {
            $where .= " AND mm.f_dish IN ({$this->params->dish}) ";
        }
        if (!empty($this->params->dishgroup)) {
            $where .= " AND g.f_group IN ({$this->params->dishgroup}) ";
        }
        if (!empty($this->params->goods)) {
            $where .= " AND g.f_id IN (
                SELECT c.f_base
                FROM c_goods_complectation c
                WHERE c.f_goods IN ({$this->params->goods})
            ) ";
        }

        return $where;
    }

    private function menuRowsSql(string $where): string
    {
        return <<<EOD
        SELECT
            g.f_id,
            mm.f_menu,
            mm.f_id AS f_mid,
            CASE mm.f_state
                WHEN 1 THEN 'Active'
                ELSE 'Inactive'
            END AS f_statename,
            mn.f_name AS f_menuname,
            gr_parent.f_name AS f_part1,
            gr.f_name AS f_part2,
            g.f_name AS f_dishname,
            g.f_weight,
            mm.f_price,
            ROUND(g.f_lastinputprice, 2) AS f_cost,
            CASE
                WHEN g.f_lastinputprice > 0
                THEN ROUND(mm.f_price / g.f_lastinputprice, 2)
                ELSE NULL
            END AS f_multiplier,
            CAST(NULLIF(JSON_UNQUOTE(JSON_EXTRACT(g.f_data, '$.f_dailyqty')), '') AS DECIMAL(14, 4)) AS f_dailyqty,
            s.f_name AS f_storename,
            mm.f_print1,
            mm.f_print2,
            g.f_color
        FROM c_menu mm
        INNER JOIN c_goods g ON g.f_id = mm.f_dish
        LEFT JOIN c_groups gr ON gr.f_id = g.f_group
        LEFT JOIN c_groups gr_parent ON gr_parent.f_id = gr.f_parent
        LEFT JOIN c_menu_names mn ON mn.f_id = mm.f_menu
        LEFT JOIN c_storages s ON s.f_id = mm.f_store
        $where
        ORDER BY gr_parent.f_name, gr.f_name, g.f_name, mn.f_name
        EOD;
    }

    /**
     * @param array<int, array<string, mixed>> $rows
     * @return array<int, array<int, mixed>>
     */
    private function pivotMenuRows(array $rows): array
    {
        $menus = $this->menuColumnsForReport();
        $columnCount = $this->colorColumnIndex() + 1;
        $dishes = [];

        foreach ($rows as $row) {
            $dishId = (int)$row["f_id"];
            $menuId = (int)$row["f_menu"];

            if (!isset($dishes[$dishId])) {
                $dishes[$dishId] = [
                    "sort_dept" => $row["f_part1"] ?? "",
                    "sort_type" => $row["f_part2"] ?? "",
                    "sort_name" => $row["f_dishname"] ?? "",
                    "base" => [
                        "code" => $dishId,
                        "status" => $row["f_statename"],
                        "dept" => $row["f_part1"],
                        "type" => $row["f_part2"],
                        "dish" => $row["f_dishname"],
                        "weight" => $row["f_weight"],
                        "cost" => $row["f_cost"],
                        "daily" => $row["f_dailyqty"],
                        "store" => $row["f_storename"],
                        "print1" => $row["f_print1"],
                        "print2" => $row["f_print2"],
                        "color" => $row["f_color"],
                    ],
                    "menus" => [],
                ];
            }

            $dishes[$dishId]["menus"][$menuId] = [
                "price" => $row["f_price"],
                "multiplier" => $row["f_multiplier"],
            ];
        }

        $result = [];
        foreach ($dishes as $dish) {
            $line = array_fill(0, $columnCount, "");
            $line[0] = $dish["base"]["code"];
            $line[1] = $dish["base"]["status"];
            $line[2] = $dish["base"]["dept"];
            $line[3] = $dish["base"]["type"];
            $line[4] = $dish["base"]["dish"];
            $line[5] = $dish["base"]["weight"];

            $col = $this->fixedColumnCount();
            foreach ($menus as $menu) {
                $menuId = $menu["id"];
                if (isset($dish["menus"][$menuId])) {
                    $line[$col] = $dish["menus"][$menuId]["price"];
                    $line[$col + 1] = $dish["menus"][$menuId]["multiplier"];
                }
                $col += 2;
            }

            $line[$col++] = $dish["base"]["cost"];
            $line[$col++] = $dish["base"]["daily"];
            $line[$col++] = $dish["base"]["store"];
            $line[$col++] = $dish["base"]["print1"];
            $line[$col++] = $dish["base"]["print2"];
            $line[$col] = $dish["base"]["color"];

            $result[] = [
                "sort" => $dish["sort_dept"] . "\0" . $dish["sort_type"] . "\0" . $dish["sort_name"],
                "data" => $line,
            ];
        }

        usort($result, static fn(array $a, array $b): int => strcmp($a["sort"], $b["sort"]));

        return array_map(static fn(array $item): array => $item["data"], $result);
    }

    /**
     * @return array<int, array<string, mixed>>
     */
    private function fetchMenuRows(): array
    {
        $where = $this->buildWhere();
        $sql = $this->menuRowsSql($where);
        $this->debugi = $sql;
        return $this->select($sql)->fetch_all(MYSQLI_ASSOC);
    }

    /**
     * @param array<string, mixed> $recipe
     * @return array<int, mixed>
     */
    private function recipeRowData(array $recipe): array
    {
        $columnCount = $this->colorColumnIndex() + 1;
        $line = array_fill(0, $columnCount, "");
        $selfCostCol = $this->fixedColumnCount() + count($this->menuColumnsForReport()) * 2;

        $line[1] = $recipe["f_unit_name"] ?? "";
        $line[4] = $recipe["f_name"];
        $line[5] = $recipe["f_qty"];
        $line[$selfCostCol] = $recipe["f_price"];
        $line[$selfCostCol + 1] = $recipe["total"];

        return $line;
    }

    private function rows_v1()
    {
        $rows = $this->pivotMenuRows($this->fetchMenuRows());
        return $this->rowsToTree($rows);
    }

    private function rows_v2()
    {
        $rows = $this->fetchMenuRows();
        $pivoted = $this->pivotMenuRows($rows);

        $sql = <<<EOD
        SELECT
            c.f_base AS f_dish,
            ig.f_name AS f_name,
            c.f_qty,
            COALESCE(u.f_name, '') AS f_unit_name,
            COALESCE(NULLIF(c.f_price, 0), ig.f_lastinputprice) AS f_price,
            ROUND(c.f_qty * COALESCE(NULLIF(c.f_price, 0), ig.f_lastinputprice), 2) AS total
        FROM c_goods_complectation c
        INNER JOIN c_goods ig ON ig.f_id = c.f_goods
        LEFT JOIN c_units u ON u.f_id = ig.f_unit
        EOD;
        $recipes = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
        $mapRecipes = [];

        foreach ($recipes as $recipe) {
            $dishId = (int)$recipe["f_dish"];
            if (!isset($mapRecipes[$dishId])) {
                $mapRecipes[$dishId] = [];
            }

            $mapRecipes[$dishId][] = [
                "data" => $this->recipeRowData($recipe),
                "children" => []
            ];
        }

        $treeRows = [];
        foreach ($pivoted as $row) {
            $dishId = (int)$row[0];
            $treeRows[] = [
                "data" => $row,
                "children" => $mapRecipes[$dishId] ?? []
            ];
        }

        return $treeRows;
    }

    public function get($params)
    {
        if (empty($params->mode)) {
            $params->mode = 2;
        }
        parent::get($params);
    }
}
