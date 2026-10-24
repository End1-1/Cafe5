<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-13 10:51:39
# Last Modified: 2026-03-13 10:51:41

require_once __DIR__ . "/index.php";

class Menu extends Auth
{
    public function List($params)
    {
        $this->result["menu"] = $this->select("select f_id, f_name from c_menu_names where f_enabled=1")->fetch_all(MYSQLI_ASSOC);
        $storages = $this->select("select f_id, f_name from c_storages where f_state=1")->fetch_all(MYSQLI_ASSOC);
        $this->result["storages"] = $storages;
        $printers = $this->select("select f_name from d_printers")->fetch_all(MYSQLI_ASSOC);
        // Aliases are logical print stations (separate checks); include them in dish print combos.
        try {
            $aliases = $this->select(
                "SELECT DISTINCT TRIM(f_alias) AS f_name FROM d_print_aliases
                 WHERE LENGTH(TRIM(IFNULL(f_alias, ''))) > 0"
            )->fetch_all(MYSQLI_ASSOC);
            $seen = [];
            foreach ($printers as $p) {
                $n = trim((string)($p["f_name"] ?? ""));
                if ($n !== "") {
                    $seen[mb_strtolower($n)] = true;
                }
            }
            foreach ($aliases as $a) {
                $n = trim((string)($a["f_name"] ?? ""));
                if ($n === "" || isset($seen[mb_strtolower($n)])) {
                    continue;
                }
                $seen[mb_strtolower($n)] = true;
                $printers[] = ["f_name" => $n];
            }
        } catch (Exception $e) {
            // table may be missing on old DBs
        }
        $this->result["printers"] = $printers;

        $this->echoResult();
    }
}
