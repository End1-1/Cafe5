<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-28 14:59:24
# Last Modified: 2026-03-14 09:46:32
require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/locale.php";
require_once __DIR__ . "/../../worker/selfboard-menu.php";

class Menu extends Auth
{
    public function get($params)
    {
        $payload = selfboard_menu_payload($this, $params);
        $this->result["groups"] = $payload["groups"];
        $this->result["parts"] = $payload["parts"];
        $this->result["dishes"] = $payload["dishes"];
        $this->result["locale"] = $payload["locale"];
        $this->echoResult();
    }

    public function GetComments($params)
    {
        $this->result["data"] = $this->select("select f_name from c_goods_comment")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetRemoveReason($params)
    {
        $this->result["data"] = $this->select("select f_name from o_dish_remove_reason")->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }

    public function GetMenuNames($params)
    {
        $locale = resolve_menu_locale($params, 'hy');
        $sql = <<<SQL
        SELECT m.f_id,
               COALESCE(NULLIF(TRIM(tr.f_name), ''), m.f_name) AS f_name
        FROM c_menu_names m
        LEFT JOIN c_menu_names_tr tr ON tr.f_menu_id = m.f_id AND tr.f_lang = ?
        WHERE m.f_enabled = 1
        SQL;
        $this->result["names"] = $this->select($sql, "s", [$locale])->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
