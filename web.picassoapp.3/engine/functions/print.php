<?php
# (C) 2024 - 2025 Kudryashov Vasili
# Last modified - 2025-05-10 10:48:21
defined('APP') or die('Die, vampire!');
require_once __DIR__ . "/dbtranslator.php";



trait Printer
{
    protected const font_name = "Arial LatArm Unicode";
    protected const font_size = 18;
    public $data = [];

    protected $params;

    private $fontSize;
    private $fontWeight;

    public function initPrinter()
    {
        global $params;
        $this->params = $params;
        $this->data[] = ["cmd" => "scene", "width" => 650, "height" => 3000, "orientation" => 0];
    }

    public function ltext($text, $left = 0, $width = -1)
    {
        $this->data[] = ["cmd" => "ltext", "text" => $text, "x" => $left, "textwidth" => $width];
        return $this;
    }

    public function ctext($text)
    {
        $this->data[] = ["cmd" => "ctext", "text" => $text];
        $this->br();
    }

    public function rtext($text)
    {
        $this->data[] = ["cmd" => "rtext", "text" => $text];
    }

    public function lrtext($ltext, $rtext)
    {
        $this->ltext($ltext);
        $this->rtext($rtext);
        $this->br();
    }

    public function font($family, $size = 12, $bold = false)
    {
        $this->fontSize = $size;
        $this->fontWeight = $bold;
        $this->data[] = ["cmd" => "font", "family" => $family, "size" => $size, "bold" => $bold];
    }

    public function fontSize($size)
    {
        $this->data[] = ["cmd" => "fontsize", "size" => $size];
    }

    public function fontBold($bold)
    {
        $this->fontWeight = $bold;
        $this->data[] = ["cmd" => "fontbold", "bold" => $bold];
    }

    public function br($height = 0)
    {
        $this->data[] = ["cmd" => "br", "height" => $height];
    }

    public function line()
    {
        $this->data[] = ["cmd" => "line2", "width" => 2];
        $this->br();
    }

    public function printCommand($printer) {
        $this->data[] = ["cmd" =>"print", "printer" => $printer];
        return $this;
    }
}

class DBPrinter
{
    use Printer;
    protected $db;
    protected $dbtranslator;
    public function __construct()
    {
        $this->db = new DB();
        $this->dbtranslator = new DBTranslator();
        $this->initPrinter();
    }

    public function tr($src, $lang = "hy")
    {
        return $this->dbtranslator->tr($src, $lang);
    }
}