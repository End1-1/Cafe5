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
        $this->result["printers"] = $printers;

        $this->echoResult();
    }
}
