<?php
defined('APP') or die('Die, vampire!');
require_once (__DIR__ . "/db.php");

class DBTranslator extends DB
{
    private $hy = [];
    private $ru = [];
    private $no = [];
    public function __construct()
    {
        parent::__construct();
        $tr = $this->stmtall("select f_en, f_am, f_ru from s_translator")->fetch_all(MYSQLI_ASSOC);
        foreach ($tr as $t) {
            $this->hy[$t["f_en"]] = $t["f_am"];
            $this->ru[$t["f_en"]] = $t["f_ru"];
        }
    }

    public function __destruct()
    {
        foreach ($this->no as $n) {
            $e = $this->stmtall("select f_en from s_translator where f_en=?", "s", [$n])->fetch_row();
            if (empty($e)) {
                $v["f_en"] = $n;
                $v["f_am"] = "am___" . $n;
                $v["f_ru"] = "ru___" . $n;
                try {
                    $this->sinsert("s_translator", $v);
                } catch (Exception $e) {

                }
            }
        }
    }

    public function tr($text, $lang = "hy")
    {
        if ($lang = "hy") {
            if (array_key_exists($text, $this->hy)) {
                return $this->hy[$text];
            }
            array_push($this->no, $text);
        } else if ($lang = "ru") {
            if (array_key_exists($text, $this->ru)) {
                return $this->ru[$text];
            }
            array_push($this->no, $text);
        }
        return $text;
    }
}