<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-13 10:51:39

require_once __DIR__ . "/index.php";

class Salary extends Auth
{
    public function Open($params)
    {
        $date = $params->date ?? null;
        if (empty($date)) {
            dieWithCode("Salary date is required");
        }

        $f_type = (int)($params->f_type ?? $params->type ?? 1);
        if ($f_type !== 1 && $f_type !== 2) {
            dieWithCode("Invalid salary type");
        }

        // Load rows for this document (date + type)
        $sql = <<<SQL
        SELECT
            s.f_id,
            s.f_staff,
            s.f_position,
            CONCAT(u.f_last, ' ', u.f_first) AS f_staff_name,
            gr.f_name AS f_position_name,
            COALESCE(s.f_amount_credit, 0) AS f_amount_credit,
            COALESCE(s.f_amount_debit, 0) AS f_amount_debit
        FROM s_salary s
        LEFT JOIN s_user u ON u.f_id = s.f_staff
        LEFT JOIN c_groups gr ON gr.f_id = s.f_position
        WHERE s.f_date = ?
          AND s.f_type = ?
        ORDER BY u.f_last, u.f_first, gr.f_name
        SQL;

        $items = $this->select($sql, "si", [$date, $f_type])->fetch_all(MYSQLI_ASSOC);
        $this->result["items"] = $items;
        $this->echoResult();
    }

    public function Save($params)
    {
        $date = $params->date ?? null;
        if (empty($date)) {
            dieWithCode("Salary date is required");
        }

        $f_type = (int)($params->f_type ?? $params->type ?? 1);
        if ($f_type !== 1 && $f_type !== 2) {
            dieWithCode("Invalid salary type");
        }

        $itemsRaw = $params->items ?? [];
        if (!is_array($itemsRaw)) {
            dieWithCode("Salary items must be an array");
        }

        $this->beginTransaction();
        try {
            // Replace rows for the same date+type (so saving accrual won't wipe payment and vice versa)
            $this->select("delete from s_salary where f_date=? and f_type=?", "si", [$date, $f_type], true);

            foreach ($itemsRaw as $it) {
                if (empty($it)) {
                    continue;
                }

                $staff = (int)($it->f_staff ?? $it->staff ?? 0);
                $position = (int)($it->f_position ?? $it->position ?? 0);

                $credit = (float)($it->f_amount_credit ?? $it->amount_credit ?? $it->credit ?? 0);
                $debit = (float)($it->f_amount_debit ?? $it->amount_debit ?? $it->debit ?? 0);

                if ($staff <= 0 || $position <= 0) {
                    continue;
                }

                if ($credit == 0 && $debit == 0) {
                    continue;
                }

                if ($f_type === 1) {
                    $debit = 0;
                } else {
                    $credit = 0;
                }

                $v = [
                    "f_date" => $date,
                    "f_type" => $f_type,
                    "f_staff" => $staff,
                    "f_position" => $position,
                    "f_amount_credit" => $credit,
                    "f_amount_debit" => $debit
                ];

                $this->insert("s_salary", $v);
            }

            $this->commit();
        } catch (Throwable $e) {
            $this->rollback();
            dieWithCode("Salary save failed: " . $e->getMessage());
        }

        $this->result["status"] = 0;
        $this->result["saved"] = true;
        $this->echoResult();
    }
}
