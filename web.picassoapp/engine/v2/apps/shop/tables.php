<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/index.php";

class Tables extends Auth
{
    /**
     * Tables of one hall (workstation f_default_hall_id).
     * Each cashbox uses only its hall — first free tab gets first f_id in that hall
     * (e.g. hall1 → 1, hall2 → 50), not a global 1..N sequence.
     */
    public function get($params)
    {
        $hall = (int)($params->hall ?? 0);
        if ($hall <= 0) {
            dieWithCode(Translator::t("Default hall is not set for this workstation (f_default_hall_id)"));
        }

        $sql = "SELECT f_id, f_name FROM h_tables WHERE f_hall=? ORDER BY f_id";
        $rows = $this->select($sql, "i", [$hall])->fetch_all(MYSQLI_ASSOC);

        $ids = [];
        foreach ($rows as $row) {
            $ids[] = (int)$row["f_id"];
        }

        $this->result["hall"] = $hall;
        $this->result["tables"] = $ids;
        $this->echoResult();
    }
}
