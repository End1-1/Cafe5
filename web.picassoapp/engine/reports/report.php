<?php
# (C) 28/01/2025 Kudryshov Vasili
require_once __DIR__ . "/../functions/db.php";
require_once __DIR__ . "/../functions/dbtranslator.php";

abstract class Report
{

    protected $db;

    protected $params;

    private $translator;

    protected $userid;

    protected $debugi = "";

    public function __construct()
    {
        global $params;
        global $userid;
        $this->params = $params;
        $this->userid = $userid;
        $this->db = new DB();

        $this->translator = new DBTranslator();
    }

    protected function tr($src, $lang = "hy")
    {
        return $this->translator->tr($src, $lang);
    }

    abstract protected function widget();

    abstract protected function columns();

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

    public function echoResult()
    {

        echo json_encode(
            [
                "status" => 1,
                "widget" => $this->widget(),
                "cols" => $this->columns(),
                "rows" => $this->rows(),
                "handler" => $this->handler(),
                "sum" => $this->sumColumns(),
                "sumspecial" => $this->sumColumnsSpecial(),
                "filter" => $this->filter(),
                "hiddencols" => $this->hiddenCols(),
                "rowcolors" => $this->rowColors(),
                "debug" => $this->debugi
            ],
            JSON_UNESCAPED_UNICODE
        );
    }

}


