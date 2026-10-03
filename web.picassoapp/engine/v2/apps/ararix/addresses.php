<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Addresses extends ArarixAuth
{
    private const BUILDING_TYPES = ["house", "apartment", "office", "other"];

    public function List($params)
    {
        $rows = $this->select(
            "SELECT * FROM ararix_client_addresses
             WHERE f_client_id = ?
             ORDER BY f_is_active DESC, f_id DESC",
            "i",
            [$this->clientId]
        )->fetch_all(MYSQLI_ASSOC);

        $addresses = [];
        foreach ($rows as $row) {
            $addresses[] = $this->formatClientAddress($row);
        }

        $this->result["addresses"] = $addresses;
        $this->result["active"] = $this->clientActiveAddress();
        $this->echoResult();
    }

    public function Save($params)
    {
        $id = (int)($params->id ?? 0);
        $street = trim((string)($params->street ?? ""));
        $label = trim((string)($params->label ?? ""));
        $lat = isset($params->lat) ? (float)$params->lat : null;
        $lng = isset($params->lng) ? (float)$params->lng : null;
        $buildingType = strtolower(trim((string)($params->building_type ?? "house")));
        $floor = isset($params->floor) ? trim((string)$params->floor) : null;
        $door = isset($params->door) ? trim((string)$params->door) : null;
        $comment = isset($params->comment) ? trim((string)$params->comment) : null;
        $entranceLat = isset($params->entrance_lat) ? (float)$params->entrance_lat : null;
        $entranceLng = isset($params->entrance_lng) ? (float)$params->entrance_lng : null;
        $setActive = !isset($params->set_active) || (bool)$params->set_active;

        if ($street === "" && $label === "") {
            dieWithCode("street or label is required", 400);
        }
        if ($lat === null || $lng === null) {
            dieWithCode("lat and lng are required", 400);
        }
        if (!in_array($buildingType, self::BUILDING_TYPES, true)) {
            $buildingType = "house";
        }
        if ($label === "") {
            $label = $street;
        }
        if ($street === "") {
            $street = $label;
        }

        $data = [
            "f_label" => $label,
            "f_street" => $street,
            "f_lat" => $lat,
            "f_lng" => $lng,
            "f_building_type" => $buildingType,
        ];
        if ($entranceLat !== null && $entranceLng !== null) {
            $data["f_entrance_lat"] = $entranceLat;
            $data["f_entrance_lng"] = $entranceLng;
        }
        if ($floor !== null && $floor !== "") {
            $data["f_floor"] = $floor;
        }
        if ($door !== null && $door !== "") {
            $data["f_door"] = $door;
        }
        if ($comment !== null && $comment !== "") {
            $data["f_comment"] = $comment;
        }

        if ($id > 0) {
            $existing = $this->select(
                "SELECT f_id FROM ararix_client_addresses WHERE f_id = ? AND f_client_id = ? LIMIT 1",
                "ii",
                [$id, $this->clientId]
            )->fetch_assoc();
            if (empty($existing)) {
                dieWithCode("Address not found", 404);
            }
            // Clear optional fields when omitted on update.
            $data["f_entrance_lat"] = ($entranceLat !== null) ? $entranceLat : null;
            $data["f_entrance_lng"] = ($entranceLng !== null) ? $entranceLng : null;
            $data["f_floor"] = ($floor !== null && $floor !== "") ? $floor : null;
            $data["f_door"] = ($door !== null && $door !== "") ? $door : null;
            $data["f_comment"] = ($comment !== null && $comment !== "") ? $comment : null;
            $this->update("ararix_client_addresses", $data, $id);
        } else {
            $data["f_client_id"] = $this->clientId;
            $data["f_is_active"] = 0;
            $id = (int)$this->insert("ararix_client_addresses", $data);
        }

        if ($setActive) {
            $this->activateAddress($id);
        }

        $row = $this->select(
            "SELECT * FROM ararix_client_addresses WHERE f_id = ? LIMIT 1",
            "i",
            [$id]
        )->fetch_assoc();

        $this->result["address"] = $this->formatClientAddress($row);
        $this->result["active"] = $this->clientActiveAddress();
        $this->echoResult();
    }

    public function SetActive($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("id is required", 400);
        }

        $existing = $this->select(
            "SELECT f_id FROM ararix_client_addresses WHERE f_id = ? AND f_client_id = ? LIMIT 1",
            "ii",
            [$id, $this->clientId]
        )->fetch_assoc();
        if (empty($existing)) {
            dieWithCode("Address not found", 404);
        }

        $this->activateAddress($id);

        $this->result["active"] = $this->clientActiveAddress();
        $this->echoResult();
    }

    public function Remove($params)
    {
        $id = (int)($params->id ?? 0);
        if ($id <= 0) {
            dieWithCode("id is required", 400);
        }

        $row = $this->select(
            "SELECT * FROM ararix_client_addresses WHERE f_id = ? AND f_client_id = ? LIMIT 1",
            "ii",
            [$id, $this->clientId]
        )->fetch_assoc();
        if (empty($row)) {
            dieWithCode("Address not found", 404);
        }

        $wasActive = (int)($row["f_is_active"] ?? 0) === 1;
        $this->select(
            "DELETE FROM ararix_client_addresses WHERE f_id = ? AND f_client_id = ?",
            "ii",
            [$id, $this->clientId],
            true
        );

        if ($wasActive) {
            $next = $this->select(
                "SELECT f_id FROM ararix_client_addresses
                 WHERE f_client_id = ? ORDER BY f_id DESC LIMIT 1",
                "i",
                [$this->clientId]
            )->fetch_assoc();
            if (!empty($next)) {
                $this->activateAddress((int)$next["f_id"]);
            }
        }

        $this->result["ok"] = true;
        $this->result["active"] = $this->clientActiveAddress();
        $this->echoResult();
    }

    private function activateAddress(int $id): void
    {
        $this->select(
            "UPDATE ararix_client_addresses SET f_is_active = 0 WHERE f_client_id = ?",
            "i",
            [$this->clientId],
            true
        );
        $this->select(
            "UPDATE ararix_client_addresses SET f_is_active = 1 WHERE f_id = ? AND f_client_id = ?",
            "ii",
            [$id, $this->clientId],
            true
        );
    }
}
