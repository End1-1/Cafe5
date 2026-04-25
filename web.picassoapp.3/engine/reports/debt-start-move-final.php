<?php
#(C) 28/01/2025 Kudryashov Vasili
require_once __DIR__ . "/reports.php";
class DebtStartMoveFinal extends Report
{

    public function widget()
    {
        return ["icon" => "cash.png", "title" => "Պարտքերի պատմություն"];
    }

    public function modeValues() {
        return  [
            ["key" => $this->tr("General form for customers"), "value" => 0],
            ["key" => $this->tr("Customer reports by deal"), "value" => 1],
            ["key" => $this->tr("General t-report"), "value" => 2],
            ["key" => $this->tr("Partner general report"), "value" => 3],
            ["key" => $this->tr("Partners deals"), "value" => 4],
            ["key" => $this->tr("Range history"), "value" => 5]
        ];
    }
    public function filter()
    {
        return [
            ["type" => "combo", "title" => $this->tr("Report mode"), "field" => "mode", "default" => 0, "values" => $this->modeValues()],
            ["type" => "date", "title" => $this->tr("Date of begin"), "field" => "date1"],
            ["type" => "date", "title" => $this->tr("Date of end"), "field" => "date2"],
            ["type" => "keyvalue", "title" => $this->tr("Currency"), "field" => "currency", "filter"=>"currency"],
            ["type" => "keyvalue", "title" => $this->tr("Partner"), "field" => "partner", "filter" => "partners"],
            ["type" => "keyvalue", "title" => $this->tr("Manager"), "field" => "manager", "filter" => "users"],
            ["type" => "keyvalue", "title" => $this->tr("Mark"), "field" => "mark", "filter" => "halls"]
        ];
    }
    public function columns()
    {
        return [
            $this->tr("Date"),
            $this->tr("Debt"),
            $this->tr("Payment"),
            $this->tr("Balance")
        ];
    }

    public function sumColumns()
    {
        return [["1"=>0], ["2"=>0]];
    }

    public function sumColumnsSpecial()
    {
        return [
            [
                "column" => 3,
                "formula" => "totaldiff",
                "diffcols" => [1, 2]
            ]
        ];
    }

    public function rows()
    {
        if (empty($this->params->date1)) {
            $this->params->date1 = date("Y-m-d");
            $this->params->date2 = date("Y-m-d");
        }
        if (empty($this->params->partner)) {
            return [
                [
                    $this->tr("Select partner"),
                    0,
                    0,
                    0
                ]
            ];
        }
        if (empty($this->params->currency)) {
            $this->params->currency = 1;
        }
        $rows = [];
        $start = $this->db->stmtall("select coalesce(cast(sum(f_amount) as float), 0) "
            . "from b_clients_debts "
            . "where f_costumer=? and f_date<? and f_currency=? ", "isi", [$this->params->partner, $this->params->date1, $this->params->currency])->fetch_row();
        array_push($rows, [
            $this->tr("Initial balance"),
            $start[0] < 0 ? $start[0] : 0,
            $start[0] < 0 ? 0 : $start[0],
            $start[0]
        ]);
        $all = $this->db->stmtall("select f_date, cast(if(f_amount<0, f_amount, 0) as float), cast(if(f_amount<0,0, f_amount) as float),0 "
            . "from b_clients_debts "
            . "where f_costumer=? and f_date between ? and ? and f_currency=? "
            . "order by f_date", 
            "issi", [$this->params->partner, $this->params->date1, $this->params->date2, $this->params->currency])->fetch_all(MYSQLI_NUM);
        $rows = array_merge($rows, $all);
        for ($i = 0; $i < count($rows); $i++) {
            if ($i == 0) {
                continue;
            }
            $rows[$i][3] = $rows[$i - 1][3] + $rows[$i][1] + $rows[$i][2];
        }
        return $rows;
    }
}