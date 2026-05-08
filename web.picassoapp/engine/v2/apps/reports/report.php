<?php
# (C) 28/01/2025 Kudryshov Vasili
# Last modified 2025-11-26 12:07:47
require_once __DIR__ . "/../../worker/auth.php";

abstract class Report extends Auth
{

    protected $params;


    protected $debugi = "";

    public function __construct()
    {
        parent::__construct();
    }

    abstract protected function widget();

    abstract protected function columns();

    protected function childColumns() {
        return [];
    }

    abstract protected function sumColumns();

    protected function sumColumnsSpecial()
    {
        return [];
    }

    protected function modeValues()
    {
        return [];
    }

    abstract protected function filter();

    abstract protected function rows();

    protected function rowsToTree($rows)
    {
        $out = [];
        foreach ($rows as $row) {
            $out[] = [
                "data" => array_values($row),
                "children" => []
            ];
        }
        return $out;
    }


    protected function rowColors()
    {
        return [];
    }

    protected function handler()
    {
        return [];
    }

    protected function hiddenCols()
    {
        return [];
    }

    protected function where()
    {
        return "";
    }

    public function get($params)
    {
        $this->params = $params;
        echo json_encode(
            [
                "status" => 1,
                "widget" => $this->widget(),
                "cols" => $this->columns(),
                "childcols"  => $this->childColumns(),
                "rows" => $this->rows(),
                "handler" => $this->handler(),
                "sum" => $this->sumColumns(),
                "sumspecial" => $this->sumColumnsSpecial(),
                "filter" => $this->filter(),
                "hiddencols" => $this->hiddenCols(),
                "rowcolors" => $this->rowColors(),
                "debug" => empty($params->debug) ? "" : $this->debugi
            ],
            JSON_UNESCAPED_UNICODE
        );
    }
}
