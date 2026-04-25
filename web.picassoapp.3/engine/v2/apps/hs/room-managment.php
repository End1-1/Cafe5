<?php
# © 2025 , Kudryashov Vasili
# Created: 2025-12-01 21:37:13
# Last Modified: 2025-12-01 21:37:18

require_once __DIR__ . "/index.php";

class RoomManagment extends Auth {
    public function NewBuilding($params) {
        $this->result["buildingid"] = $this->insert("hs_room_building", ["f_name" => $params->name]);
        $this->result["buildingname"] = $params->name;
        $this->echoResult();
    }
    public function Get($params) {
        $buildings = $this->select("select * from hs_room_building")->fetch_all(MYSQLI_ASSOC);
        $data = [];
        foreach ($buildings as $b) {
            $bb = ["building" => $b];
            $data[]= $bb;
        }
        $this->result["buildings"] = $data;
        $this->echoResult();
    }
}