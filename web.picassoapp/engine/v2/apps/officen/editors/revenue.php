<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/../../../worker/helper.php";
require_once __DIR__ . '/../../worker/dict-cash-operation-type.php';

class Revenue
{
    private $db;

    public function __construct($db)
    {
        $this->db = $db;
    }

    public function get($params)
    {
        $filterRaw = $params->filter ?? [];
        $filter = [];
        foreach ($filterRaw as $item) {
            foreach ((array)$item as $k => $v) {
                $filter[$k] = $v;
            }
        }

        $currencyId = (int)($filter["currency"] ?? 1);
        if ($currencyId <= 0) $currencyId = 1;

        $viewMode = (int)($filter["viewmode"] ?? 1);

        switch ($viewMode) {
            case 1:
                return $this->getTotalCash($filter, $currencyId, "cash");
            case 2:
                return $this->getDetailedCash($filter, $currencyId, "cash");
            case 3:
                return $this->getDetailedCash($filter, $currencyId, "noncash");
            case 4:
                return $this->getShiftClosings($filter, $currencyId);
            case 5:
                require_once __DIR__ . "/cashdiscrepancies.php";
                $result = (new CashDiscrepancies($this->db))->listRows($filter, $currencyId);
                $result["filter"] = $this->getFilterConfig(5);
                return $result;
        }

        return ["rows" => [], "headers" => [], "filter" => $this->getFilterConfig()];
    }

    protected function getTotalCash($filter, $currencyId, string $scope = "cash")
    {
        $cashboxId = (int)($filter["cashbox"] ?? 0);
        $bindTypes = "i";
        $bindValues = [$currencyId];
        $cashFilter = "";

        if ($cashboxId > 0) {
            $cashFilter = " AND c.f_id = ? ";
            $bindTypes .= "i";
            $bindValues[] = $cashboxId;
        }

        $paymentFilter = $this->paymentScopeSql($scope);

        $sql = <<<EOD
    SELECT 
        c.f_id,
        c.f_name AS cashbox_name,
        cur.f_name AS currency_name,
        money_fmt(IFNULL(SUM(op.f_debit), 0) - IFNULL(SUM(op.f_credit), 0)) AS balance
    FROM cash_box c
    CROSS JOIN e_currency cur ON cur.f_id = ?
    LEFT JOIN cash_operations op ON op.f_cashbox_id = c.f_id
        AND op.f_currency_id = cur.f_id
        $paymentFilter
    WHERE 1=1 $cashFilter
    GROUP BY c.f_id, c.f_name, cur.f_name
    EOD;

        return [
            "rows" => $this->db->select($sql, $bindTypes, $bindValues)->fetch_all(MYSQLI_NUM),
            "headers" => ["ID", Translator::t("Cashbox"), Translator::t("Currency"), Translator::t("Balance")],
            "toolbar" => ["reload" => true, "filter" => true, "new" => true],
            "sum" => [3],
            "filter" => $this->getFilterConfig()
        ];
    }

    public function getDetailedCash($filter, $currencyId, string $scope = "cash")
    {
        $cashboxId = (int)($filter["cashbox"] ?? 0);
        $dateStart = (!empty($filter["date1"])) ? $filter["date1"] : '2000-01-01';
        $dateEnd = (!empty($filter["date2"])) ? $filter["date2"] : date('Y-m-d');
        $otherExpensesOnly = $scope === "cash" && (int)($filter['other_expenses'] ?? 0) === 1;
        $nonCash = $scope === "noncash";

        $baseWhere = " op.f_currency_id = ? " . $this->paymentScopeSql($scope);
        $baseParams = [$currencyId];
        $baseTypes = "i";

        if ($cashboxId > 0) {
            $baseWhere .= " AND op.f_cashbox_id = ? ";
            $baseParams[] = $cashboxId;
            $baseTypes .= "i";
        }

        if ($otherExpensesOnly) {
            $baseWhere .= ' AND (op.f_order_id IS NULL OR TRIM(op.f_order_id) = \'\')';
            $baseWhere .= ' AND op.f_operation_type IN ('
                . CASH_OP_TOTAL_EXPENSES . ','
                . CASH_OP_DEBT_REPAYMENT . ','
                . CASH_OP_UTILITIES . ','
                . CASH_OP_CASH_SHORTAGE
                . ')';
            $baseWhere .= ' AND (op.f_credit > 0 OR op.f_debit > 0)';
        }

        $openingByType = [];
        $openingTypeName = [];
        if (!$otherExpensesOnly) {
            $sqlStart = "SELECT COALESCE(op.f_payment_type_id, 1),
                    COALESCE(MAX(NULLIF(cpt.f_name, '')), MAX(lp.f_value), ''),
                    SUM(op.f_debit) - SUM(op.f_credit)
                FROM cash_operations op
                LEFT JOIN l_dictionary lp ON lp.f_dict_id = op.f_payment_type_id
                    AND lp.f_dict = 'cash_payment_types' AND lp.f_lang = 'hy'
                LEFT JOIN cash_payment_types cpt ON cpt.f_id = op.f_payment_type_id
                WHERE $baseWhere AND op.f_datetime < ?
                GROUP BY COALESCE(op.f_payment_type_id, 1)";
            $paramsStart = $baseParams;
            $paramsStart[] = $dateStart . " 00:00:00";
            $typesStart = $baseTypes . "s";

            $resStart = $this->db->select($sqlStart, $typesStart, $paramsStart)->fetch_all(MYSQLI_NUM);
            foreach ($resStart as $startRow) {
                $pt = (int)$startRow[0];
                $openingByType[$pt] = (float)($startRow[2] ?? 0);
                $openingTypeName[$pt] = (string)$startRow[1];
            }
        }
        if (!$nonCash && !isset($openingByType[1])) {
            $openingByType[1] = 0.0;
        }

        // 2. Список операций
        $sqlOps = <<<EOD
    SELECT 
        op.f_id,                     -- index 0 (для редактирования)
        COALESCE(lot.f_value, CAST(op.f_operation_type AS CHAR)) AS f_operation_type_name,
        op.f_operation_type,         -- index 2 (id)
        op.f_order_id,               -- index 3 (UUID заказа; пусто = прямая кассовая операция)
        op.f_datetime,               -- index 4
        TRIM(CONCAT(u.f_last, ' ', LEFT(u.f_first, 1), '.')) AS operator_name, -- index 5
        COALESCE(NULLIF(cpt.f_name, ''), lp.f_value) as payment_type,  -- index 6
        money_fmt(op.f_debit) as d_f, -- index 7
        money_fmt(op.f_credit) as c_f, -- index 8
        op.f_comment,                -- index 9
        op.f_debit,                  -- index 10 (raw)
        op.f_credit,                 -- index 11 (raw)
        COALESCE(op.f_payment_type_id, 1) -- index 12
    FROM cash_operations op
    LEFT JOIN cash_box cb ON cb.f_id = op.f_cashbox_id
    LEFT JOIN l_dictionary lot ON lot.f_dict_id = op.f_operation_type AND lot.f_dict = 'cash_operations_types' AND lot.f_lang = 'hy'
    LEFT JOIN s_user u ON u.f_id = op.f_user
    LEFT JOIN l_dictionary lp ON lp.f_dict_id = op.f_payment_type_id AND lp.f_dict = 'cash_payment_types' AND lp.f_lang = 'hy'
    LEFT JOIN cash_payment_types cpt ON cpt.f_id = op.f_payment_type_id
    WHERE $baseWhere AND op.f_datetime BETWEEN ? AND ?
    ORDER BY op.f_datetime ASC, op.f_id ASC
    EOD;

        $paramsOps = $baseParams;
        $paramsOps[] = $dateStart . " 00:00:00";
        $paramsOps[] = $dateEnd . " 23:59:59";
        $typesOps = $baseTypes . "ss";

        $dbRows = $this->db->select($sqlOps, $typesOps, $paramsOps)->fetch_all(MYSQLI_NUM);

        $rows = [];
        $balanceByType = $openingByType;

        if (!$otherExpensesOnly && !$nonCash) {
            $rows[] = [
                "",
                "",
                "",
                "",
                $dateStart . " 00:00:00",
                "",
                Translator::t("OPENING BALANCE"),
                "",
                "",
                "",
                number_format((float)($openingByType[1] ?? 0), 2, '.', ' ')
            ];
        } elseif ($nonCash) {
            foreach ($openingByType as $pt => $opening) {
                if (abs($opening) < 0.0001) {
                    continue;
                }
                $rows[] = [
                    "",
                    "",
                    "",
                    "",
                    $dateStart . " 00:00:00",
                    "",
                    Translator::t("OPENING BALANCE"),
                    $openingTypeName[$pt] ?? "",
                    "",
                    "",
                    number_format($opening, 2, '.', ' ')
                ];
            }
        }

        foreach ($dbRows as $dbRow) {
            $operator    = $dbRow[5];
            $paymentType = $dbRow[6];
            $debitFmt    = $dbRow[7];
            $creditFmt   = $dbRow[8];
            $comment     = translate_cash_operation_comment((string)$dbRow[9]);
            $debitRaw    = (float)$dbRow[10];
            $creditRaw   = (float)$dbRow[11];

            if ($otherExpensesOnly) {
                $expenseAmt = max($creditRaw, $debitRaw);
                $rows[] = [
                    $dbRow[0],
                    $dbRow[1],
                    $dbRow[2],
                    $dbRow[3],
                    $dbRow[4],
                    $operator,
                    $comment,
                    $paymentType,
                    "",
                    $expenseAmt > 0.009 ? money_fmt_php($expenseAmt) : "-",
                    "",
                ];
                continue;
            }

            $paymentTypeId = (int)$dbRow[12];
            if ($paymentTypeId <= 0) {
                $paymentTypeId = 1;
            }
            $currentBalance = (float)($balanceByType[$paymentTypeId] ?? 0);
            $currentBalance += ($debitRaw - $creditRaw);
            $balanceByType[$paymentTypeId] = $currentBalance;

            $rows[] = [
                $dbRow[0], // f_id
                $dbRow[1], // operation type name
                $dbRow[2], // f_operation_type id
                $dbRow[3], // f_order_id
                $dbRow[4], // f_datetime
                $operator,
                $comment,
                $paymentType,
                ($debitRaw > 0) ? $debitFmt : "",
                ($creditRaw > 0) ? $creditFmt : "-",
                money_fmt_php($currentBalance, 2, '.', ' ')
            ];
        }

        return [
            "rows" => $rows,
            "hidden_columns" => $otherExpensesOnly ? [0, 2, 3, 10] : [0, 2, 3],
            "headers" => [
                Translator::t("Id"),
                Translator::t("Type"),
                "TypeId",
                "UUID",
                Translator::t("DateTime"),
                Translator::t("Operator"),
                Translator::t("Comment"),
                Translator::t("Payment type"),
                Translator::t("Income"),
                Translator::t("Outcome"),
                Translator::t("Balance")
            ],
            "toolbar" => ["reload" => true, "filter" => true, "new" => true],
            "sum" => $otherExpensesOnly ? [9] : [8, 9, 10],
            "filter" => $this->getFilterConfig()
        ];
    }

    protected function getShiftClosings($filter, $currencyId)
    {
        $cashboxId = (int)($filter["cashbox"] ?? 0);
        $dateStart = (!empty($filter["date1"])) ? $filter["date1"] : "2000-01-01";
        $dateEnd = (!empty($filter["date2"])) ? $filter["date2"] : date("Y-m-d");

        $sql = <<<SQL
        SELECT cs.f_id, cb.f_name, cs.f_date_open, cs.f_date_close,
            cs.f_amount_open, cs.f_amount_fact, cs.f_amount_expected, cs.f_amount_difference,
            COALESCE(cs.f_cash_counted, 0) AS f_cash_counted,
            TRIM(CONCAT(COALESCE(u.f_last, ''), ' ', LEFT(COALESCE(u.f_first, ''), 1), '.')) AS closer
        FROM cash_session cs
        INNER JOIN cash_box cb ON cb.f_id = cs.f_cashbox_id
        LEFT JOIN s_user u ON u.f_id = cs.f_user_close
        WHERE cs.f_state = 2
          AND CAST(cs.f_date_close AS DATE) BETWEEN ? AND ?
          AND COALESCE(cb.f_currency_id, 1) = ?
        SQL;
        $types = "ssi";
        $values = [$dateStart, $dateEnd, $currencyId];
        if ($cashboxId > 0) {
            $sql .= " AND cs.f_cashbox_id = ? ";
            $types .= "i";
            $values[] = $cashboxId;
        }
        $sql .= " ORDER BY cs.f_date_open ASC, cs.f_id ASC ";
        $sessions = $this->db->select($sql, $types, $values)->fetch_all(MYSQLI_ASSOC);

        $opsBySession = [];
        if ($sessions) {
            $ids = [];
            foreach ($sessions as $session) {
                $ids[] = (int)$session["f_id"];
            }
            $in = implode(",", $ids);
            $opSql = <<<SQL
            SELECT op.f_id, op.f_session_id, op.f_datetime, op.f_operation_type, op.f_affects_float,
                COALESCE(lot.f_value, '') AS type_name,
                op.f_comment, op.f_debit, op.f_credit,
                TRIM(CONCAT(COALESCE(u.f_last, ''), ' ', LEFT(COALESCE(u.f_first, ''), 1), '.')) AS operator_name,
                COALESCE(NULLIF(TRIM(p.f_taxname), ''), NULLIF(TRIM(p.f_name), ''), '') AS partner_name,
                JSON_UNQUOTE(JSON_EXTRACT(sd.f_data, '$.create_date')) AS store_create_date,
                sd.f_doc_date AS store_doc_date
            FROM cash_operations op
            LEFT JOIN l_dictionary lot ON lot.f_dict_id = op.f_operation_type
                AND lot.f_dict = 'cash_operations_types' AND lot.f_lang = 'hy'
            LEFT JOIN s_user u ON u.f_id = op.f_user
            LEFT JOIN store_document sd ON sd.f_id = op.f_order_id AND sd.f_doc_type = 1
            LEFT JOIN c_partners p ON p.f_id = sd.f_partner
            WHERE op.f_session_id IN ($in)
              AND COALESCE(op.f_payment_type_id, 1) <= 1
              AND NOT (op.f_operation_type IN (
            SQL;
            $opSql .= CASH_OP_CASH_SHORTAGE . "," . CASH_OP_CASH_OVERAGE . ") AND COALESCE(op.f_affects_float, 0) = 0)
            ORDER BY op.f_datetime ASC, op.f_id ASC";
            $ops = $this->db->select($opSql)->fetch_all(MYSQLI_ASSOC);
            foreach ($ops as $op) {
                $opsBySession[(int)$op["f_session_id"]][] = $op;
            }
            foreach ($opsBySession as &$sessionOps) {
                $sessionOps = self::groupShiftOps($sessionOps);
            }
            unset($sessionOps);
        }

        $rows = [];
        $first = true;
        foreach ($sessions as $session) {
            if (!$first) {
                $rows[] = ["", "", "", "", "", "", "", ""];
            }
            $first = false;
            $sid = (int)$session["f_id"];
            $cashbox = (string)$session["f_name"];
            $opened = (string)$session["f_date_open"];
            $closed = (string)$session["f_date_close"];
            $closer = trim((string)$session["closer"], " .");
            $title = Translator::t("Shift") . " #" . $sid
                . "  " . Translator::t("Date open") . " " . $opened
                . "  " . Translator::t("Date close") . " " . $closed;
            if ($closer !== "") {
                $title .= "  " . $closer;
            }
            $rows[] = ["", $cashbox, $closed, $title, "", "", "", ""];

            $opening = (float)$session["f_amount_open"];
            $balance = $opening;
            $rows[] = [
                "",
                $cashbox,
                $opened,
                Translator::t("OPENING BALANCE"),
                "",
                "",
                "",
                money_fmt_php($opening),
            ];

            $currentGroup = null;
            foreach ($opsBySession[$sid] ?? [] as $op) {
                $groupKey = self::shiftOpGroupKey((int)$op["f_operation_type"]);
                if ($groupKey !== $currentGroup) {
                    $currentGroup = $groupKey;
                    $rows[] = [
                        "",
                        $cashbox,
                        "",
                        "— " . self::shiftOpGroupTitle($groupKey) . " —",
                        "",
                        "",
                        "",
                        "",
                    ];
                }
                $debit = (float)$op["f_debit"];
                $credit = (float)$op["f_credit"];
                $balance += $debit - $credit;
                $comment = translate_cash_operation_comment((string)$op["f_comment"]);
                $typeName = trim((string)$op["type_name"]);
                $isOpenCount = (int)$op["f_affects_float"] === 1
                    && in_array((int)$op["f_operation_type"], [CASH_OP_CASH_SHORTAGE, CASH_OP_CASH_OVERAGE], true);
                if ($isOpenCount && $comment !== "") {
                    $desc = $comment;
                } else {
                    $desc = $typeName !== "" ? $typeName : $comment;
                    if ($typeName !== "" && $comment !== "" && $comment !== $typeName) {
                        $desc .= " — " . $comment;
                    }
                }
                $operator = trim((string)$op["operator_name"], " .");
                if ($operator !== "") {
                    $desc .= " (" . $operator . ")";
                }
                $partnerName = "";
                if ((int)$op["f_operation_type"] === CASH_OP_PURCHASING_COSTS
                    || (!empty($op["partner_name"]) && (float)$op["f_credit"] > 0.009)) {
                    $partnerName = trim((string)($op["partner_name"] ?? ""));
                }
                $rows[] = [
                    "",
                    $cashbox,
                    self::shiftOpDisplayDateTime($op),
                    $desc,
                    $partnerName,
                    $debit > 0.009 ? money_fmt_php($debit) : "",
                    $credit > 0.009 ? money_fmt_php($credit) : "",
                    money_fmt_php($balance),
                ];
            }

            $expected = (float)$session["f_amount_expected"];
            $rows[] = [
                "",
                $cashbox,
                $closed,
                Translator::t("Expected cash"),
                "",
                "",
                "",
                money_fmt_php($expected),
            ];

            $counted = (int)$session["f_cash_counted"] === 1;
            if (!$counted) {
                $rows[] = [
                    "",
                    $cashbox,
                    $closed,
                    Translator::t("Cash was not counted"),
                    "",
                    "",
                    "",
                    "",
                ];
                continue;
            }

            $fact = (float)$session["f_amount_fact"];
            $diff = (float)$session["f_amount_difference"];
            $rows[] = [
                "",
                $cashbox,
                $closed,
                Translator::t("Counted cash"),
                "",
                "",
                "",
                money_fmt_php($fact),
            ];
            if (abs($diff) < 0.009) {
                $result = Translator::t("Balanced");
                $resultAmount = 0.0;
            } elseif ($diff > 0) {
                $result = Translator::t("Cash Overage");
                $resultAmount = $diff;
            } else {
                $result = Translator::t("Cash Shortage");
                $resultAmount = abs($diff);
            }
            $rows[] = [
                "",
                $cashbox,
                $closed,
                $result,
                "",
                "",
                "",
                money_fmt_php($resultAmount),
            ];
        }

        return [
            "rows" => $rows,
            "hidden_columns" => [0],
            "headers" => [
                "ID",
                Translator::t("Cashbox"),
                Translator::t("DateTime"),
                Translator::t("Comment"),
                Translator::t("Supplier"),
                Translator::t("Income"),
                Translator::t("Outcome"),
                Translator::t("Balance"),
            ],
            "toolbar" => ["reload" => true, "filter" => true],
            "sum" => [5, 6],
            "filter" => $this->getFilterConfig(),
        ];
    }

    /**
     * Shift report groups: purchases → salaries → cash in/out → revenue.
     * @return int 1..4
     */
    private static function shiftOpGroupKey(int $operationType): int
    {
        return match ($operationType) {
            CASH_OP_PURCHASING_COSTS => 1,
            CASH_OP_SALARIES => 2,
            CASH_OP_SALES_REVENUE => 4,
            default => 3, // cash in/out, transfers, collection, debts, utilities, …
        };
    }

    private static function shiftOpGroupTitle(int $groupKey): string
    {
        return match ($groupKey) {
            1 => Translator::t("Purchasing Costs"),
            2 => Translator::t("Salaries and Wages"),
            4 => Translator::t("Sales Revenue"),
            default => Translator::t("Cash in / cash out"),
        };
    }

    /**
     * @param list<array<string,mixed>> $ops
     * @return list<array<string,mixed>>
     */
    private static function groupShiftOps(array $ops): array
    {
        usort($ops, static function (array $a, array $b): int {
            $ga = self::shiftOpGroupKey((int)$a["f_operation_type"]);
            $gb = self::shiftOpGroupKey((int)$b["f_operation_type"]);
            if ($ga !== $gb) {
                return $ga <=> $gb;
            }
            $byTime = strcmp(self::shiftOpDisplayDateTime($a), self::shiftOpDisplayDateTime($b));
            if ($byTime !== 0) {
                return $byTime;
            }
            return ((int)$a["f_id"]) <=> ((int)$b["f_id"]);
        });
        return $ops;
    }

    /** Prefer real clock time for purchase ops that were stored as date-only (00:00:00). */
    private static function shiftOpDisplayDateTime(array $op): string
    {
        $raw = trim((string)($op["f_datetime"] ?? ""));
        if ($raw === "") {
            return "";
        }
        $timePart = strlen($raw) >= 19 ? substr($raw, 11, 8) : "";
        if ($timePart !== "" && $timePart !== "00:00:00") {
            return $raw;
        }
        $create = trim((string)($op["store_create_date"] ?? ""));
        if ($create !== "") {
            $parsed = \DateTime::createFromFormat("d/m/Y H:i:s", $create)
                ?: \DateTime::createFromFormat("Y-m-d H:i:s", $create);
            if ($parsed) {
                $datePart = strlen($raw) >= 10 ? substr($raw, 0, 10) : $parsed->format("Y-m-d");
                return $datePart . " " . $parsed->format("H:i:s");
            }
        }
        $storeDoc = trim((string)($op["store_doc_date"] ?? ""));
        if ($storeDoc !== "" && strlen($storeDoc) >= 19 && substr($storeDoc, 11, 8) !== "00:00:00") {
            return $storeDoc;
        }
        return $raw;
    }

    private function paymentScopeSql(string $scope): string
    {
        if ($scope === "noncash") {
            return " AND COALESCE(op.f_payment_type_id, 0) > 1 ";
        }
        return " AND COALESCE(op.f_payment_type_id, 1) <= 1 ";
    }

    protected function getFilterConfig(int $viewMode = 0)
    {
        $config = [
            ["type" => "viewmode", "name" => "viewmode", "label" => Translator::t("View mode"), "default" => 1, "values" => [
                ["label" => Translator::t("Total by cashboxes"), "value" => 1],
                ["label" => Translator::t("Detailed movement"), "value" => 2],
                ["label" => Translator::t("Non-cash movement"), "value" => 3],
                ["label" => Translator::t("Shift close report"), "value" => 4],
                ["label" => Translator::t("Cash discrepancies"), "value" => 5],
            ]],
            ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
            ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
            ["type" => "keyvalue", "name" => "currency", "label" => Translator::t("Currency"), "default" => 1, "function" => "currency"],
            ["type" => "keyvalue", "name" => "cashbox", "label" => Translator::t("Cashbox"), "function" => "cashbox"]
        ];
        if ($viewMode === 5) {
            $config[] = ["type" => "combobox", "name" => "discrepancy_state", "label" => Translator::t("Status"), "default" => 0, "values" => [
                ["label" => Translator::t("All"), "value" => 0],
                ["label" => Translator::t("Unresolved"), "value" => 1],
                ["label" => Translator::t("Resolved"), "value" => 2],
            ]];
        }
        return $config;
    }
}
