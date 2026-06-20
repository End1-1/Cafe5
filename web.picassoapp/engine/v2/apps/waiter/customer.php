<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-05-31 00:52:07

require_once __DIR__ . "/index.php";

class Customer extends Auth
{
    /**
     * Search customers by name or phone. Triggers only when more than one character is provided.
     */
    public function Search($params)
    {
        $q = trim((string)($params->query ?? ""));

        if (mb_strlen($q) <= 1) {
            $this->result["customers"] = [];
            $this->echoResult();
            return;
        }

        $like = "%" . $q . "%";
        $digits = preg_replace('/[^0-9]/', '', $q);

        $where = "p.f_taxname LIKE ? OR p.f_contact LIKE ? OR p.f_name LIKE ?";
        $types = "sss";
        $values = [$like, $like, $like];

        if ($digits !== "") {
            $where .= " OR REPLACE(REPLACE(REPLACE(COALESCE(p.f_phone, ''), ' ', ''), '-', ''), '+', '') LIKE ?";
            $types .= "s";
            $values[] = "%" . $digits . "%";
        }

        $sql = <<<EOD
        SELECT p.f_id,
               COALESCE(NULLIF(TRIM(p.f_taxname), ''), NULLIF(TRIM(p.f_contact), ''), p.f_name) AS f_name,
               COALESCE(p.f_phone, '') AS f_phone,
               COALESCE(p.f_address, '') AS f_address
        FROM c_partners p
        WHERE {$where}
        ORDER BY f_name
        LIMIT 50
        EOD;

        $this->result["customers"] = $this->select($sql, $types, $values)->fetch_all(MYSQLI_ASSOC);
        $this->echoResult();
    }
}
