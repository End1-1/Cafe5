<?php
# © 2026 , Kudryashov Vasili
# Simple Ararix dictionaries for FrontDesk CE5Goods selectors.

require_once __DIR__ . "/index.php";

class ArarixDict extends Auth
{
    public function ListGroups($params)
    {
        $rows = $this->select(
            "SELECT f_id AS id, f_name AS name FROM ararix_goods_groups ORDER BY f_sort, f_name"
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$r) {
            $r["id"] = (int)$r["id"];
        }
        $this->result["items"] = $rows;
        $this->echoResult();
    }

    public function ListCountries($params)
    {
        $rows = $this->select(
            "SELECT f_id AS id, f_name AS name FROM ararix_goods_country ORDER BY f_sort, f_name"
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$r) {
            $r["id"] = (int)$r["id"];
        }
        $this->result["items"] = $rows;
        $this->echoResult();
    }

    public function ListNationalities($params)
    {
        $rows = $this->select(
            "SELECT f_id AS id, f_name AS name FROM ararix_restaurant_nationality ORDER BY f_sort, f_name"
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as &$r) {
            $r["id"] = (int)$r["id"];
        }
        $this->result["items"] = $rows;
        $this->echoResult();
    }
}
