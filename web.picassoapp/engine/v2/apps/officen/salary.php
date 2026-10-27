<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-03-13 10:51:39

require_once __DIR__ . "/index.php";
require_once __DIR__ . "/../../worker/helper.php";
require_once __DIR__ . "/../../worker/uuid.php";
require_once __DIR__ . "/../worker/cash-ops-helper.php";

class Salary extends Auth
{
    public function StaffPosition($params)
    {
        $staff = (int)($params->staff ?? $params->f_staff ?? 0);
        if ($staff <= 0) {
            dieWithCode("Employee is required");
        }

        $sql = <<<SQL
        SELECT
            u.f_group AS f_position,
            g.f_name AS f_position_name,
            (SELECT COALESCE(SUM(ss.f_amount_credit) - SUM(ss.f_amount_debit), 0)
             FROM s_salary ss WHERE ss.f_staff = u.f_id) AS f_debt
        FROM s_user u
        LEFT JOIN s_user_group g ON g.f_id = u.f_group
        WHERE u.f_id = ?
        SQL;

        $row = $this->select($sql, "i", [$staff])->fetch_assoc();
        $this->result["f_position"] = (int)($row["f_position"] ?? 0);
        $this->result["f_position_name"] = (string)($row["f_position_name"] ?? "");
        $this->result["f_debt"] = (float)($row["f_debt"] ?? 0);
        $this->echoResult();
    }

    /**
     * Staff who checked out on the given calendar day (DATE(f_out) = day).
     * Still-open shifts (f_out IS NULL) are not included.
     */
    public function AutofillAccrual($params)
    {
        $date = trim((string)($params->date ?? ''));
        if ($date === '') {
            dieWithCode("Salary date is required");
        }

        if (strtotime($date . ' 00:00:00') === false) {
            dieWithCode("Invalid salary date");
        }

        $sql = <<<SQL
        SELECT
            u.f_id AS f_staff,
            TRIM(CONCAT(u.f_last, ' ', u.f_first)) AS f_staff_name,
            COALESCE(NULLIF(a.f_position, 0), u.f_group) AS f_position,
            gr.f_name AS f_position_name
        FROM s_attendance a
        INNER JOIN s_user u ON u.f_id = a.f_worker
        LEFT JOIN s_user_group gr ON gr.f_id = COALESCE(NULLIF(a.f_position, 0), u.f_group)
        WHERE a.f_out IS NOT NULL
          AND DATE(a.f_out) = ?
        ORDER BY a.f_worker, a.f_out DESC
        SQL;

        $rows = $this->select($sql, 's', [$date])->fetch_all(MYSQLI_ASSOC);

        $seen = [];
        $items = [];
        foreach ($rows as $row) {
            $staffId = (int)($row['f_staff'] ?? 0);
            if ($staffId <= 0 || isset($seen[$staffId])) {
                continue;
            }
            $seen[$staffId] = true;
            $times = $this->attendanceTimesForDay($staffId, $date);
            $items[] = [
                'f_staff' => $staffId,
                'f_staff_name' => (string)($row['f_staff_name'] ?? ''),
                'f_position' => (int)($row['f_position'] ?? 0),
                'f_position_name' => (string)($row['f_position_name'] ?? ''),
                'f_time_in' => $times['in'],
                'f_time_out' => $times['out'],
            ];
        }

        $this->result['items'] = $items;
        $this->echoResult();
    }

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

        $sql = <<<SQL
        SELECT
            s.f_id,
            s.f_staff,
            COALESCE(NULLIF(s.f_position, 0), u.f_group) AS f_position,
            CONCAT(u.f_last, ' ', u.f_first) AS f_staff_name,
            gr.f_name AS f_position_name,
            COALESCE(s.f_fixed, 0) AS f_fixed,
            COALESCE(s.f_calculated, 0) AS f_calculated,
            COALESCE(s.f_bonus, 0) AS f_bonus,
            COALESCE(s.f_fine, 0) AS f_fine,
            COALESCE(s.f_amount_credit, 0) AS f_amount_credit,
            COALESCE(s.f_amount_debit, 0) AS f_amount_debit,
            (SELECT COALESCE(SUM(ss.f_amount_credit) - SUM(ss.f_amount_debit), 0)
             FROM s_salary ss WHERE ss.f_staff = s.f_staff) AS f_debt
        FROM s_salary s
        LEFT JOIN s_user u ON u.f_id = s.f_staff
        LEFT JOIN s_user_group gr ON gr.f_id = COALESCE(NULLIF(s.f_position, 0), u.f_group)
        WHERE s.f_date = ?
          AND s.f_type = ?
        ORDER BY u.f_last, u.f_first, gr.f_name
        SQL;

        $items = $this->select($sql, "si", [$date, $f_type])->fetch_all(MYSQLI_ASSOC);
        foreach ($items as &$item) {
            $times = $this->attendanceTimesForDay((int)($item["f_staff"] ?? 0), (string)$date);
            $item["f_time_in"] = $times["in"];
            $item["f_time_out"] = $times["out"];
        }
        unset($item);
        $this->result["items"] = $items;
        if ($f_type === 2) {
            $this->result["payment"] = $this->paymentSourceForDate((string)$date);
        }
        $this->echoResult();
    }

    /**
     * Payment statement source (cashbox + payment type) for the date; f_cashbox_id 0 = paid outside the cashboxes.
     *
     * @return array<string,mixed>
     */
    private function paymentSourceForDate(string $date): array
    {
        $row = $this->paymentHeader($date);
        if (!$row) {
            return ["exists" => false, "f_cashbox_id" => 0, "f_cashbox_name" => "", "f_payment_type_id" => PAYMENT_TYPE_CASH];
        }
        $cb = $this->select("SELECT f_name FROM cash_box WHERE f_id=?", "i", [(int)$row["f_cashbox_id"]])->fetch_assoc();
        return [
            "exists" => true,
            "f_cashbox_id" => (int)$row["f_cashbox_id"],
            "f_cashbox_name" => (string)($cb["f_name"] ?? ""),
            "f_payment_type_id" => (int)$row["f_payment_type_id"],
        ];
    }

    private function paymentHeader(string $date): ?array
    {
        $exists = $this->select("SHOW TABLES LIKE 's_salary_payment_doc'")->num_rows > 0;
        if (!$exists) {
            return null;
        }
        $row = $this->select("SELECT * FROM s_salary_payment_doc WHERE f_date=?", "s", [$date])->fetch_assoc();
        return $row ?: null;
    }

    /**
     * Remove cash operations of a payment statement. Float is restored only on still-open shifts;
     * closed shifts keep their stored totals.
     */
    private function unpostPaymentCash(string $uuid): void
    {
        if ($uuid === "") {
            return;
        }
        $rows = $this->select(
            "SELECT op.f_session_id,
                    COALESCE(SUM(CASE WHEN op.f_affects_float = 1 THEN op.f_credit - op.f_debit ELSE 0 END), 0) AS f_delta
             FROM cash_operations op
             INNER JOIN cash_session cs ON cs.f_id = op.f_session_id AND cs.f_state = 1
             WHERE op.f_order_id = ?
             GROUP BY op.f_session_id",
            "s",
            [$uuid]
        )->fetch_all(MYSQLI_ASSOC);
        foreach ($rows as $r) {
            cash_ops_adjust_session_float($this, (int)$r["f_session_id"], (float)$r["f_delta"]);
        }
        $this->select("DELETE FROM cash_operations WHERE f_order_id = ?", "s", [$uuid], true);
    }

    /**
     * Balance of the cashbox for one payment type, without the operations of this statement.
     */
    private function cashboxBalance(int $cashboxId, int $paymentTypeId, int $currencyId, string $excludeUuid): float
    {
        $row = $this->select(
            "SELECT COALESCE(SUM(f_debit), 0) - COALESCE(SUM(f_credit), 0) AS f_balance
             FROM cash_operations
             WHERE f_cashbox_id = ? AND COALESCE(f_payment_type_id, 1) = ? AND f_currency_id = ?
               AND COALESCE(f_order_id, '') <> ?",
            "iiis",
            [$cashboxId, $paymentTypeId, $currencyId, $excludeUuid]
        )->fetch_assoc();
        return (float)($row["f_balance"] ?? 0);
    }

    /**
     * One salary cash operation per employee from the chosen cashbox; attached to the open shift if any.
     *
     * @param list<array{staff:int,amount:float}> $payments
     */
    private function postPaymentCash(string $uuid, string $date, int $cashboxId, int $paymentTypeId, int $currencyId, array $payments): void
    {
        $session = $this->select(
            "SELECT f_id FROM cash_session WHERE f_cashbox_id = ? AND f_state = 1 ORDER BY f_id DESC LIMIT 1",
            "i",
            [$cashboxId]
        )->fetch_assoc();
        $sessionId = (int)($session["f_id"] ?? 0);
        $now = date("Y-m-d H:i:s");
        $dateLabel = date("d.m.Y", strtotime($date));
        $floatDelta = 0.0;

        foreach ($payments as $p) {
            $u = $this->select("SELECT TRIM(CONCAT(COALESCE(f_last, ''), ' ', COALESCE(f_first, ''))) AS f_name FROM s_user WHERE f_id=?",
                "i", [$p["staff"]])->fetch_assoc();
            // Store English template text; translate_cash_operation_comment localizes on display.
            $comment = "Salary payment " . $dateLabel . ": " . (string)($u["f_name"] ?? $p["staff"]);
            cash_ops_insert($this, [
                "f_cashbox_id" => $cashboxId,
                "f_session_id" => $sessionId,
                "f_order_id" => $uuid,
                "f_user" => (int)$this->userid,
                "f_operation_type" => CASH_OP_SALARIES,
                "f_payment_type_id" => $paymentTypeId,
                "f_datetime" => $now,
                "f_debit" => 0,
                "f_credit" => $p["amount"],
                "f_currency_id" => $currencyId,
                "f_comment" => mb_substr($comment, 0, 255),
            ]);
            if (cash_ops_affects_float($paymentTypeId)) {
                $floatDelta -= $p["amount"];
            }
        }
        if ($sessionId > 0) {
            cash_ops_adjust_session_float($this, $sessionId, $floatDelta);
        }
    }

    /**
     * Delete a salary document for the date; a payment statement also removes its cash operations.
     */
    public function RemoveDocument($params)
    {
        $date = (string)($params->date ?? "");
        if ($date === "") {
            dieWithCode("Salary date is required");
        }
        $f_type = (int)($params->f_type ?? $params->type ?? 1);
        if ($f_type !== 1 && $f_type !== 2) {
            dieWithCode("Invalid salary type");
        }

        $this->beginTransaction();
        try {
            if ($f_type === 2) {
                $header = $this->paymentHeader($date);
                if ($header) {
                    $this->unpostPaymentCash((string)$header["f_uuid"]);
                    $this->select("DELETE FROM s_salary_payment_doc WHERE f_date = ?", "s", [$date], true);
                }
            }
            $this->select("DELETE FROM s_salary WHERE f_date = ? AND f_type = ?", "si", [$date, $f_type], true);
            $this->commit();
        } catch (Throwable $e) {
            $this->rollback();
            dieWithCode("Salary remove failed: " . $e->getMessage());
        }

        $this->result["removed"] = true;
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

        $cashboxId = 0;
        $paymentTypeId = PAYMENT_TYPE_CASH;
        $currencyId = 1;
        $header = null;
        if ($f_type === 2) {
            $cashboxId = max(0, (int)($params->cashbox_id ?? 0));
            $paymentTypeId = (int)($params->payment_type_id ?? PAYMENT_TYPE_CASH);
            if ($cashboxId > 0) {
                $cb = $this->select("SELECT f_currency_id FROM cash_box WHERE f_id=?", "i", [$cashboxId])->fetch_assoc();
                if (!$cb) {
                    dieWithCode(Translator::t("Cashbox not found"));
                }
                $currencyId = max(1, (int)($cb["f_currency_id"] ?? 1));
                if (!cash_ops_records_payment($paymentTypeId)) {
                    dieWithCode(Translator::t("Invalid payment type"));
                }
            }
            $header = $this->paymentHeader((string)$date);

            if ($cashboxId > 0 && empty($params->confirm_low_balance)) {
                $total = 0.0;
                foreach ($itemsRaw as $it) {
                    $total += max(0.0, (float)($it->f_amount_debit ?? $it->amount_debit ?? $it->debit ?? 0));
                }
                $balance = $this->cashboxBalance($cashboxId, $paymentTypeId, $currencyId, (string)($header["f_uuid"] ?? ""));
                if ($total - $balance > 0.009) {
                    $this->result["need_confirm"] = true;
                    $this->result["balance"] = round($balance, 2);
                    $this->result["total"] = round($total, 2);
                    $this->echoResult();
                    return;
                }
            }
        }

        $payments = [];
        $this->beginTransaction();
        try {
            $this->select("delete from s_salary where f_date=? and f_type=?", "si", [$date, $f_type], true);

            foreach ($itemsRaw as $it) {
                if (empty($it)) {
                    continue;
                }

                $staff = (int)($it->f_staff ?? $it->staff ?? 0);
                $position = (int)($it->f_position ?? $it->position ?? 0);

                if ($staff <= 0) {
                    continue;
                }

                if ($position <= 0) {
                    $ur = $this->select("select f_group from s_user where f_id=?", "i", [$staff])->fetch_assoc();
                    $position = (int)($ur["f_group"] ?? 0);
                }

                if ($position <= 0) {
                    continue;
                }

                if ($f_type === 1) {
                    $fixed = (float)($it->f_fixed ?? 0);
                    $calculated = (float)($it->f_calculated ?? 0);
                    $bonus = (float)($it->f_bonus ?? 0);
                    $fine = max(0.0, (float)($it->f_fine ?? 0));
                    $credit = max(0.0, $fixed + $calculated + $bonus - $fine);
                    $debit = 0;

                    if ($fixed + $calculated + $bonus == 0) {
                        continue;
                    }

                    $v = [
                        "f_date" => $date,
                        "f_type" => $f_type,
                        "f_staff" => $staff,
                        "f_position" => $position,
                        "f_fixed" => $fixed,
                        "f_calculated" => $calculated,
                        "f_bonus" => $bonus,
                        "f_fine" => $fine,
                        "f_amount_credit" => $credit,
                        "f_amount_debit" => $debit,
                    ];
                } else {
                    $debit = (float)($it->f_amount_debit ?? $it->amount_debit ?? $it->debit ?? 0);
                    $credit = 0;

                    if ($debit == 0) {
                        continue;
                    }

                    $v = [
                        "f_date" => $date,
                        "f_type" => $f_type,
                        "f_staff" => $staff,
                        "f_position" => $position,
                        "f_fixed" => 0,
                        "f_calculated" => 0,
                        "f_bonus" => 0,
                        "f_fine" => 0,
                        "f_amount_credit" => $credit,
                        "f_amount_debit" => $debit,
                    ];
                    $payments[] = ["staff" => $staff, "amount" => round(abs($debit), 2)];
                }

                $this->insert("s_salary", $v);
            }

            if ($f_type === 2 && $this->select("SHOW TABLES LIKE 's_salary_payment_doc'")->num_rows > 0) {
                $uuid = (string)($header["f_uuid"] ?? "");
                $this->unpostPaymentCash($uuid);
                if ($uuid === "") {
                    $uuid = uuid_v4();
                }
                $this->select(
                    "INSERT INTO s_salary_payment_doc (f_date, f_uuid, f_cashbox_id, f_payment_type_id, f_currency_id, f_user, f_updated)
                     VALUES (?, ?, ?, ?, ?, ?, NOW())
                     ON DUPLICATE KEY UPDATE f_cashbox_id = VALUES(f_cashbox_id), f_payment_type_id = VALUES(f_payment_type_id),
                        f_currency_id = VALUES(f_currency_id), f_user = VALUES(f_user), f_updated = NOW()",
                    "ssiiii",
                    [$date, $uuid, $cashboxId, $paymentTypeId, $currencyId, (int)$this->userid],
                    true
                );
                if ($cashboxId > 0 && $payments) {
                    $this->postPaymentCash($uuid, (string)$date, $cashboxId, $paymentTypeId, $currencyId, $payments);
                }
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

    /**
     * Salary rules from s_user_group.f_data (see CE5UserGroup).
     * Optional positions[] — unique group ids from the document; if empty, all groups.
     */
    public function GetFormulas($params)
    {
        $formulas = $this->loadGroupFormulas($params->positions ?? $params->f_positions ?? null);
        $this->result["formulas"] = $formulas;
        $this->echoResult();
    }

    /**
     * Accrual calculation for document rows (f_type = 1).
     * rows[]: f_staff, f_position; f_bonus is preserved from the client when sent.
     */
    public function CalculateAccrual($params)
    {
        $date = $params->date ?? null;
        if (empty($date)) {
            dieWithCode("Salary date is required");
        }

        $rowsRaw = $params->rows ?? [];
        if (!is_array($rowsRaw) || count($rowsRaw) === 0) {
            dieWithCode("Rows are required");
        }

        [$date1, $date2] = $this->salaryDayRange($date);

        $positionIds = [];
        foreach ($rowsRaw as $row) {
            if (empty($row)) {
                continue;
            }
            $position = (int)($row->f_position ?? $row->position ?? 0);
            if ($position > 0) {
                $positionIds[$position] = true;
            }
        }

        $formulas = $this->loadGroupFormulas(array_keys($positionIds));

        $positionCounts = [];
        foreach ($rowsRaw as $row) {
            if (empty($row)) {
                continue;
            }
            $position = (int)($row->f_position ?? $row->position ?? 0);
            if ($position > 0) {
                $positionCounts[$position] = ($positionCounts[$position] ?? 0) + 1;
            }
        }

        $items = [];
        foreach ($rowsRaw as $row) {
            if (empty($row)) {
                continue;
            }

            $staff = (int)($row->f_staff ?? $row->staff ?? 0);
            $position = (int)($row->f_position ?? $row->position ?? 0);
            if ($staff <= 0 || $position <= 0) {
                $items[] = [
                    "f_staff" => $staff,
                    "f_position" => $position,
                    "f_fixed" => 0,
                    "f_calculated" => 0,
                    "f_bonus" => (float)($row->f_bonus ?? 0),
                    "f_total" => (float)($row->f_bonus ?? 0),
                ];
                continue;
            }

            $formula = $formulas[(string)$position] ?? $this->defaultFormula();
            $posCount = max(1, (int)($positionCounts[$position] ?? 1));

            $fixed = (float)($formula["f_fixed"] ?? 0);
            $varPart = $this->variableSalaryPart($staff, $position, $date1, $formula, $posCount);
            $calculated = (float)($varPart["variable"] ?? 0);
            $dishTaxableBase = (float)($varPart["dish_taxable_base"] ?? 0);
            [$fixed, $calculated] = $this->clampSalaryBase($fixed, $calculated, $formula);

            $bonus = (float)($row->f_bonus ?? 0);
            $late = $this->lateFineForStaffDay($staff, $date1, $formula);
            $lateMinutes = (int)($late["late_minutes"] ?? 0);
            $fine = (float)($late["fine"] ?? 0);
            $total = max(0.0, $fixed + $calculated + $bonus - $fine);
            $times = $this->attendanceTimesForDay($staff, $date1);
            $items[] = [
                "f_staff" => $staff,
                "f_position" => $position,
                "f_fixed" => round($fixed, 2),
                "f_dish_taxable_base" => round($dishTaxableBase, 2),
                "f_calculated" => round($calculated, 2),
                "f_bonus" => round($bonus, 2),
                "f_late_minutes" => $lateMinutes,
                "f_fine" => round($fine, 2),
                "f_total" => round($total, 2),
                "f_time_in" => $times["in"],
                "f_time_out" => $times["out"],
            ];
        }

        $this->result["date_from"] = $date1;
        $this->result["date_to"] = $date2;
        $this->result["items"] = $items;
        $this->echoResult();
    }

    /**
     * Breakdown of dish sales base (percent dishes only) for salary accrual row.
     */
    public function DishSalesDetail($params)
    {
        $date = trim((string)($params->date ?? ''));
        if ($date === '') {
            dieWithCode("Salary date is required");
        }

        $staff = (int)($params->f_staff ?? $params->staff ?? 0);
        $position = (int)($params->f_position ?? $params->position ?? 0);
        $posCount = max(1, (int)($params->f_position_count ?? $params->position_count ?? 1));

        if ($staff <= 0) {
            dieWithCode("Employee is required");
        }
        if ($position <= 0) {
            dieWithCode("Position is required");
        }

        [$date1] = $this->salaryDayRange($date);
        $formulas = $this->loadGroupFormulas([$position]);
        $formula = $formulas[(string)$position] ?? $this->defaultFormula();
        $depCode = (int)($formula["f_dep"] ?? 0);
        $skipAmount = (float)($formula["f_skip_amount"] ?? -1);
        $byWorkingTime = !empty($formula["f_count_working_time"]);

        $staffRow = $this->select(
            "SELECT TRIM(CONCAT(u.f_last, ' ', u.f_first)) AS f_staff_name FROM s_user u WHERE u.f_id = ?",
            "i",
            [$staff]
        )->fetch_assoc();

        $detail = $this->buildDishPercentSalesDetail(
            $staff,
            $depCode,
            $date1,
            $byWorkingTime,
            $posCount,
            $skipAmount
        );

        $this->result["date"] = $date1;
        $this->result["f_staff"] = $staff;
        $this->result["f_staff_name"] = trim((string)($staffRow["f_staff_name"] ?? ""));
        $this->result["f_position"] = $position;
        $this->result["f_department"] = $depCode;
        $this->result["items"] = $detail["items"];
        $this->result["f_sales_total"] = $detail["f_sales_total"];
        $this->result["f_skip_amount"] = $detail["f_skip_amount"];
        $this->result["f_taxable_sales"] = $detail["f_taxable_sales"];
        $this->result["f_dish_taxable_base"] = $detail["f_dish_taxable_base"];
        $this->result["f_divide_by"] = $detail["f_divide_by"];
        $this->echoResult();
    }

    private function defaultFormula(): array
    {
        return [
            "f_fixed" => 0,
            "f_min" => 0,
            "f_max" => 0,
            "f_dep" => 0,
            "f_total" => 0,
            "f_owntotal" => 0,
            "f_skip_amount" => -1,
            "f_count_working_time" => false,
            "f_work_start_time" => "09:00",
            "f_work_end_time" => "18:00",
            "f_fine_per_minute" => 0,
            "f_maximum_fine_minutes" => 0,
        ];
    }

    private function parseGroupFormulaJson(mixed $json): array
    {
        if (is_array($json)) {
            $data = $json;
        } else {
            $data = json_decode(is_string($json) ? $json : '{}', true);
        }
        if (!is_array($data)) {
            $data = [];
        }

        return [
            "f_fixed" => (float)($data["f_fixed"] ?? 0),
            "f_min" => (float)($data["f_min"] ?? 0),
            "f_max" => (float)($data["f_max"] ?? 0),
            "f_dep" => (float)($data["f_dep"] ?? 0),
            "f_total" => (float)($data["f_total"] ?? 0),
            "f_owntotal" => (float)($data["f_owntotal"] ?? 0),
            "f_skip_amount" => (float)($data["f_skip_amount"] ?? -1),
            "f_count_working_time" => !empty($data["f_count_working_time"]),
            "f_work_start_time" => $this->normalizeHm((string)($data["f_work_start_time"] ?? "09:00"), "09:00"),
            "f_work_end_time" => $this->normalizeHm((string)($data["f_work_end_time"] ?? "18:00"), "18:00"),
            "f_fine_per_minute" => (float)($data["f_fine_per_minute"] ?? 0),
            "f_maximum_fine_minutes" => (int)($data["f_maximum_fine_minutes"] ?? 0),
        ];
    }

    private function normalizeHm(string $hm, string $fallback): string
    {
        $hm = trim($hm);
        if (preg_match('/^\d{1,2}:\d{2}/', $hm, $m)) {
            $parts = explode(':', $m[0]);
            $h = max(0, min(23, (int)$parts[0]));
            $min = max(0, min(59, (int)$parts[1]));
            return sprintf('%02d:%02d', $h, $min);
        }
        return $fallback;
    }

    /**
     * First check-in and last check-out of the day (HH:mm); empty when absent or shift still open.
     * @return array{in: string, out: string}
     */
    private function attendanceTimesForDay(int $staff, string $day): array
    {
        $empty = ["in" => "", "out" => ""];
        if ($staff <= 0 || $day === '') {
            return $empty;
        }

        $row = $this->select(
            "SELECT MIN(f_in) AS f_in, MAX(f_out) AS f_out,
                    SUM(f_out IS NULL) AS f_open
             FROM s_attendance
             WHERE f_worker = ? AND DATE(f_in) = ?",
            "is",
            [$staff, $day]
        )->fetch_assoc();

        $fmt = static function ($raw): string {
            $ts = strtotime((string)($raw ?? ''));
            return $ts === false || $raw === null || $raw === '' ? '' : date('H:i', $ts);
        };

        return [
            "in" => $fmt($row["f_in"] ?? null),
            "out" => (int)($row["f_open"] ?? 0) > 0 ? "" : $fmt($row["f_out"] ?? null),
        ];
    }

    /**
     * Late fine vs group work start time using first s_attendance.f_in of the day.
     * @return array{late_minutes: int, fine: float}
     */
    private function lateFineForStaffDay(int $staff, string $day, array $formula): array
    {
        $empty = ["late_minutes" => 0, "fine" => 0.0];
        if ($staff <= 0) {
            return $empty;
        }

        $finePerMinute = (float)($formula["f_fine_per_minute"] ?? 0);
        $maxMinutes = (int)($formula["f_maximum_fine_minutes"] ?? 0);
        if ($finePerMinute <= 0.0001 || $maxMinutes <= 0) {
            return $empty;
        }

        $startHm = $this->normalizeHm((string)($formula["f_work_start_time"] ?? "09:00"), "09:00");
        $scheduledTs = strtotime($day . ' ' . $startHm . ':00');
        if ($scheduledTs === false) {
            return $empty;
        }

        $row = $this->select(
            "SELECT MIN(f_in) AS f_in FROM s_attendance WHERE f_worker = ? AND DATE(f_in) = ?",
            "is",
            [$staff, $day]
        )->fetch_assoc();

        $inRaw = (string)($row["f_in"] ?? '');
        if ($inRaw === '') {
            return $empty;
        }
        $inTs = strtotime($inRaw);
        if ($inTs === false) {
            return $empty;
        }

        $lateMinutes = (int)max(0, floor(($inTs - $scheduledTs) / 60));
        $capped = min($lateMinutes, $maxMinutes);
        return [
            "late_minutes" => $lateMinutes,
            "fine" => $capped * $finePerMinute,
        ];
    }

    /**
     * Percent of revenue; if skipAmount >= 0, subtract it from revenue first.
     * Negative skipAmount — full revenue is used (skip not applied).
     */
    private function revenuePercentAmount(float $revenueSum, float $percent, float $skipAmount): float
    {
        if ($percent <= 0.0001) {
            return 0.0;
        }
        $base = $revenueSum;
        if ($skipAmount >= 0) {
            $base = max(0.0, $revenueSum - $skipAmount);
        }
        return $base * ($percent / 100.0);
    }

    /**
     * @param array<int>|null $positionIds
     * @return array<string, array<string, float>>
     */
    private function loadGroupFormulas($positionIds): array
    {
        $ids = [];
        if (is_array($positionIds)) {
            foreach ($positionIds as $id) {
                $id = (int)$id;
                if ($id > 0) {
                    $ids[$id] = true;
                }
            }
        }
        $ids = array_keys($ids);

        if (count($ids) > 0) {
            $placeholders = implode(",", array_fill(0, count($ids), "?"));
            $sql = "SELECT f_id, f_data FROM s_user_group WHERE f_id IN ($placeholders)";
            $types = str_repeat("i", count($ids));
            $rows = $this->select($sql, $types, $ids)->fetch_all(MYSQLI_ASSOC);
        } else {
            $rows = $this->select("SELECT f_id, f_data FROM s_user_group")->fetch_all(MYSQLI_ASSOC);
        }

        $formulas = [];
        foreach ($rows as $row) {
            $gid = (int)($row["f_id"] ?? 0);
            if ($gid <= 0) {
                continue;
            }
            $formulas[(string)$gid] = $this->parseGroupFormulaJson($row["f_data"] ?? null);
        }

        return $formulas;
    }

    /** @return array{0: string, 1: string} */
    private function salaryDayRange(string $date): array
    {
        $ts = strtotime($date);
        if ($ts === false) {
            $ts = time();
        }
        $day = date("Y-m-d", $ts);
        return [$day, $day];
    }

    /** @return array{variable: float, dish_taxable_base: float} */
    private function variableSalaryPart(int $staff, int $position, string $day, array $formula, int $posCount): array
    {
        $variable = 0.0;
        $dishTaxableBase = 0.0;
        $depCode = (int)($formula["f_dep"] ?? 0);
        $totalPct = (float)($formula["f_total"] ?? 0);
        $ownPct = (float)($formula["f_owntotal"] ?? 0);
        $skipAmount = (float)($formula["f_skip_amount"] ?? -1);
        $byWorkingTime = !empty($formula["f_count_working_time"]);

        if ($depCode > 0) {
            $dishPart = $this->dishOutputSalaryPart(
                $staff,
                $depCode,
                $day,
                $byWorkingTime,
                $byWorkingTime ? 1 : max(1, $posCount),
                $skipAmount
            );
            $variable += (float)($dishPart["amount"] ?? 0);
            $dishTaxableBase = (float)($dishPart["taxable_base"] ?? 0);
        }

        if ($totalPct > 0.0001) {
            $sql = <<<SQL
            SELECT COALESCE(SUM(oh.f_amounttotal), 0) AS revenue_sum
            FROM o_header oh
            WHERE oh.f_state = 2
              AND oh.f_datecash = ?
            SQL;
            $row = $this->select($sql, "s", [$day])->fetch_assoc();
            $revenueSum = (float)($row["revenue_sum"] ?? 0);
            $variable += $this->revenuePercentAmount($revenueSum, $totalPct, $skipAmount) / max(1, $posCount);
        }

        if ($ownPct > 0.0001) {
            $sql = <<<SQL
            SELECT COALESCE(SUM(oh.f_amounttotal), 0) AS revenue_sum
            FROM o_header oh
            WHERE oh.f_state = 2
              AND oh.f_datecash = ?
              AND oh.f_staff = ?
            SQL;
            $row = $this->select($sql, "si", [$day, $staff])->fetch_assoc();
            $revenueSum = (float)($row["revenue_sum"] ?? 0);
            $variable += $this->revenuePercentAmount($revenueSum, $ownPct, $skipAmount);
        }

        return [
            "variable" => $variable,
            "dish_taxable_base" => $dishTaxableBase,
        ];
    }

    /**
     * Salary from sold dishes — department, fixed and percent from c_goods.f_data.
     * Percent is taken from qty * price, without the service surcharge baked into f_total.
     * Only lines with service check printed (o_goods.f_data.f_printed = true).
     * Cash day: o_header.f_datecash; open and closed checks (f_state 1, 2).
     * Working-time clip uses o_goods.f_data.f_append_time.
     */
    /**
     * @return array{amount: float, taxable_base: float}
     */
    private function dishOutputSalaryPart(
        int $staff,
        int $depCode,
        string $day,
        bool $byWorkingTime,
        int $divideBy,
        float $skipAmount = -1
    ): array {
        $rows = $this->queryDishSalaryGoodsRows($depCode, $day);
        $windows = $byWorkingTime ? $this->attendanceWindowsOnDay($staff, $day) : [];

        $fixedSum = 0.0;
        $percentPay = 0.0;
        $percentSales = 0.0;

        foreach ($rows as $row) {
            $line = $this->parseDishSalaryGoodsRow($row);
            if ($line === null) {
                continue;
            }

            if ($byWorkingTime) {
                $appendTs = strtotime((string)($line["append_time"] ?? ''));
                if ($appendTs === false || !$this->isTimestampInWindows($appendTs, $windows)) {
                    continue;
                }
            }

            if ($line["sal_fixed"] > 0.0001) {
                $fixedSum += $line["f_qty"] * $line["sal_fixed"];
            } elseif ($line["sal_pct"] > 0.0001) {
                $salesBase = $line["f_qty"] * $line["f_price"];
                $percentSales += $salesBase;
                $percentPay += $salesBase * ($line["sal_pct"] / 100.0);
            }
        }

        if ($percentPay > 0.0001 && $percentSales > 0.0001 && $skipAmount >= 0) {
            $taxableSales = max(0.0, $percentSales - $skipAmount);
            $percentPay *= ($taxableSales / $percentSales);
        } else {
            $taxableSales = $percentSales;
        }

        $divide = max(1, $divideBy);
        return [
            "amount" => ($fixedSum + $percentPay) / $divide,
            "taxable_base" => ($percentSales > 0.0001 ? $taxableSales : 0.0) / $divide,
        ];
    }

    /**
     * Percent-dish breakdown for dish sales base column.
     *
     * @return array{
     *   items: array<int, array<string, float|int|string>>,
     *   f_sales_total: float,
     *   f_skip_amount: float,
     *   f_taxable_sales: float,
     *   f_dish_taxable_base: float,
     *   f_divide_by: int
     * }
     */
    private function buildDishPercentSalesDetail(
        int $staff,
        int $depCode,
        string $day,
        bool $byWorkingTime,
        int $divideBy,
        float $skipAmount = -1
    ): array {
        $empty = [
            "items" => [],
            "f_sales_total" => 0.0,
            "f_skip_amount" => $skipAmount,
            "f_taxable_sales" => 0.0,
            "f_dish_taxable_base" => 0.0,
            "f_divide_by" => max(1, $divideBy),
        ];

        if ($depCode <= 0) {
            return $empty;
        }

        $rows = $this->queryDishSalaryGoodsRows($depCode, $day);
        $windows = $byWorkingTime ? $this->attendanceWindowsOnDay($staff, $day) : [];
        $agg = [];

        foreach ($rows as $row) {
            $line = $this->parseDishSalaryGoodsRow($row);
            if ($line === null || $line["sal_pct"] <= 0.0001 || $line["sal_fixed"] > 0.0001) {
                continue;
            }

            if ($byWorkingTime) {
                $appendTs = strtotime((string)($line["append_time"] ?? ''));
                if ($appendTs === false || !$this->isTimestampInWindows($appendTs, $windows)) {
                    continue;
                }
            }

            $goodsId = (int)$line["goods_id"];
            if (!isset($agg[$goodsId])) {
                $agg[$goodsId] = [
                    "f_goods" => $goodsId,
                    "f_goods_name" => (string)$line["goods_name"],
                    "f_qty" => 0.0,
                    "f_sales" => 0.0,
                    "f_percent" => (float)$line["sal_pct"],
                ];
            }

            $agg[$goodsId]["f_qty"] += (float)$line["f_qty"];
            $agg[$goodsId]["f_sales"] += (float)$line["f_qty"] * (float)$line["f_price"];
        }

        $items = [];
        $percentSales = 0.0;
        foreach ($agg as $item) {
            $sales = round((float)$item["f_sales"], 2);
            $percent = (float)$item["f_percent"];
            $items[] = [
                "f_goods" => (int)$item["f_goods"],
                "f_goods_name" => (string)$item["f_goods_name"],
                "f_qty" => round((float)$item["f_qty"], 4),
                "f_sales" => $sales,
                "f_percent" => round($percent, 4),
                "f_pay" => round($sales * ($percent / 100.0), 2),
            ];
            $percentSales += $sales;
        }

        usort($items, static function (array $a, array $b): int {
            return strcmp((string)$a["f_goods_name"], (string)$b["f_goods_name"]);
        });

        if ($percentSales > 0.0001 && $skipAmount >= 0) {
            $taxableSales = max(0.0, $percentSales - $skipAmount);
        } else {
            $taxableSales = $percentSales;
        }

        $divide = max(1, $divideBy);
        return [
            "items" => $items,
            "f_sales_total" => round($percentSales, 2),
            "f_skip_amount" => $skipAmount,
            "f_taxable_sales" => round($taxableSales, 2),
            "f_dish_taxable_base" => round(($percentSales > 0.0001 ? $taxableSales : 0.0) / $divide, 2),
            "f_divide_by" => $divide,
        ];
    }

    /** @return array<int, array<string, mixed>> */
    private function queryDishSalaryGoodsRows(int $depCode, string $day): array
    {
        $sql = <<<SQL
        SELECT
            cg.f_id AS goods_id,
            COALESCE(cg.f_name, '') AS goods_name,
            og.f_qty,
            og.f_price,
            CAST(JSON_UNQUOTE(JSON_VALUE(cg.f_data, '$.f_salary_fixed_value')) AS DECIMAL(18, 4)) AS sal_fixed,
            CAST(JSON_UNQUOTE(JSON_VALUE(cg.f_data, '$.f_salary_percent_value')) AS DECIMAL(18, 4)) AS sal_pct,
            JSON_UNQUOTE(JSON_VALUE(og.f_data, '$.f_append_time')) AS append_time
        FROM o_goods og
        INNER JOIN o_header oh ON oh.f_id = og.f_header
        INNER JOIN c_goods cg ON cg.f_id = og.f_goods
        WHERE oh.f_state IN (1, 2)
          AND og.f_state = 1
          AND oh.f_datecash = ?
          AND CAST(JSON_UNQUOTE(JSON_VALUE(cg.f_data, '$.f_salary_department')) AS SIGNED) = ?
          AND LOWER(COALESCE(JSON_UNQUOTE(JSON_VALUE(og.f_data, '$.f_printed')), 'false')) IN ('true', '1')
        SQL;

        return $this->select($sql, "si", [$day, $depCode])->fetch_all(MYSQLI_ASSOC);
    }

    /**
     * @return array<string, mixed>|null
     */
    private function parseDishSalaryGoodsRow(array $row): ?array
    {
        $fixed = (float)($row["sal_fixed"] ?? 0);
        $pct = (float)($row["sal_pct"] ?? 0);
        $name = trim((string)($row["goods_name"] ?? ""));
        $goodsId = (int)($row["goods_id"] ?? 0);
        if ($name === '' && $goodsId > 0) {
            $name = '#' . $goodsId;
        }

        if ($fixed > 0.0001 && $pct > 0.0001) {
            dieWithCode(
                'Goods "' . $name . '": both salary fixed amount and salary percent are set in product card. '
                . 'Leave only one of f_salary_fixed_value or f_salary_percent_value in goods f_data.'
            );
        }

        if ($fixed <= 0.0001 && $pct <= 0.0001) {
            return null;
        }

        return [
            "goods_id" => $goodsId,
            "goods_name" => $name,
            "f_qty" => (float)($row["f_qty"] ?? 0),
            "f_price" => (float)($row["f_price"] ?? 0),
            "sal_fixed" => $fixed,
            "sal_pct" => $pct,
            "append_time" => (string)($row["append_time"] ?? ''),
        ];
    }

    /**
     * Attendance intervals clipped to calendar day [00:00:00 .. 23:59:59].
     *
     * @return array<int, array{0: int, 1: int}>
     */
    private function attendanceWindowsOnDay(int $staff, string $day): array
    {
        $dayStart = strtotime($day . ' 00:00:00');
        $dayEnd = strtotime($day . ' 23:59:59');
        if ($dayStart === false || $dayEnd === false) {
            return [];
        }

        $sql = <<<SQL
        SELECT f_in, f_out
        FROM s_attendance
        WHERE f_worker = ?
          AND f_out IS NOT NULL
          AND f_in <= ?
          AND f_out >= ?
        ORDER BY f_in
        SQL;

        $rows = $this->select(
            $sql,
            'iss',
            [$staff, date('Y-m-d H:i:s', $dayEnd), date('Y-m-d H:i:s', $dayStart)]
        )->fetch_all(MYSQLI_ASSOC);

        $windows = [];
        foreach ($rows as $row) {
            $inTs = strtotime((string)($row['f_in'] ?? ''));
            $outTs = strtotime((string)($row['f_out'] ?? ''));
            if ($inTs === false || $outTs === false || $outTs < $inTs) {
                continue;
            }
            $start = max($inTs, $dayStart);
            $end = min($outTs, $dayEnd);
            if ($end >= $start) {
                $windows[] = [$start, $end];
            }
        }

        return $windows;
    }

    /**
     * @param array<int, array{0: int, 1: int}> $windows
     */
    private function isTimestampInWindows(int $ts, array $windows): bool
    {
        foreach ($windows as [$start, $end]) {
            if ($ts >= $start && $ts <= $end) {
                return true;
            }
        }

        return false;
    }

    /**
     * Clamp fixed + calculated (without bonus): trim calculated first, then fixed.
     *
     * @return array{0: float, 1: float}
     */
    private function clampSalaryBase(float $fixed, float $calculated, array $formula): array
    {
        $minVal = (float)($formula["f_min"] ?? 0);
        $maxVal = (float)($formula["f_max"] ?? 0);
        $sum = $fixed + $calculated;

        if ($minVal > 0.0001 && $sum < $minVal - 0.0001) {
            $calculated += $minVal - $sum;
            $sum = $fixed + $calculated;
        }

        if ($maxVal > 0.0001 && $sum > $maxVal + 0.0001) {
            $excess = $sum - $maxVal;
            $fromCalc = min($calculated, $excess);
            $calculated -= $fromCalc;
            $excess -= $fromCalc;
            if ($excess > 0.0001) {
                $fixed = max(0.0, $fixed - $excess);
            }
        }

        return [$fixed, $calculated];
    }
}
