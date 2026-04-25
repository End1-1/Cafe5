<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-11-10 2025-11-10 11:12:30
# Last Modified: 2025-11-10 11:12:33
require_once __DIR__ . "/index.php";

class Materials extends Auth {
    public function list($params) {
        $where = "";
        if (!empty($params->filter)) {
            $where = "and f_name like '{$params->filter}%'";
        }
        $sql = <<<EOD
        select f_id, f_name as f_materialname
        from c_goods 
        where f_production=1 and f_enabled=1
        $where
        EOD;
        $this->result["data"] = $this->select($sql)->fetch_all(MYSQLI_ASSOC);
       
        $this->echoResult();
    }

    public function get($params) {
        $this->result["data"] = $this->select("select *, f_name as f_materialname from c_goods where f_id=?", "i", [$params->f_id])->fetch_assoc();
        $this->echoResult();
    }
   
}