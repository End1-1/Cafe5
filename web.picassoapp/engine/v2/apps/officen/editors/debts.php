<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-04-21 11:51:45
# Modified: 2026-09-07

class Debts
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

        $currencyId = (int)($filter["currency"] ?? 0);
        if ($currencyId <= 0) {
            $currencyId = 1;
        }

        $viewMode = (int)($filter["viewmode"] ?? 1);

        switch ($viewMode) {
            case 1:
                return $this->getTotalDebts($filter, 1, $currencyId);
            case 3:
                return $this->getTotalDebts($filter, 2, $currencyId);
            case 2:
                return $this->getDetailedDebts($filter, 1, $currencyId);
            case 4:
                return $this->getDetailedDebts($filter, 2, $currencyId);
        }

        return ["rows" => [], "headers" => [], "filter" => $this->getFilterConfig()];
    }

    /**
     * Open debt amount: suppliers credit-debit (we owe); customers debit-credit (they owe us).
     */
    private function openDebtExpr(int $docType): string
    {
        if ($docType === 2) {
            return "SUM(d.f_debit) - SUM(d.f_credit)";
        }
        return "SUM(d.f_credit) - SUM(d.f_debit)";
    }

    private function rowDelta(int $docType, float $credit, float $debit): float
    {
        return ($docType === 2) ? ($debit - $credit) : ($credit - $debit);
    }

    protected function getTotalDebts($filter, $docType, $currencyId)
    {
        $filterExtra = "";
        $filterBindtypes = "ii";
        $filterBindvalues = [(int)$docType, $currencyId];

        if (($filter["partner"] ?? 0) > 0) {
            $filterExtra .= " and d.f_partner=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int)$filter["partner"];
        }

        $balExpr = $this->openDebtExpr((int)$docType);

        $sql = <<<EOD
    SELECT
        d.f_partner,
        p.f_name AS partner_name, p.f_phone, p.f_taxcode, p.f_taxname, c.f_name AS currency_name,
        money_fmt($balExpr) AS balance
    FROM cash_debts d
    LEFT JOIN c_partners p ON p.f_id = d.f_partner
    LEFT JOIN e_currency c ON c.f_id = d.f_currency_id
    WHERE d.f_doc_type=? AND d.f_currency_id=? $filterExtra
    GROUP BY d.f_partner, d.f_currency_id, p.f_name, p.f_phone, p.f_taxcode, p.f_taxname, c.f_name
    HAVING ($balExpr) <> 0
    ORDER BY p.f_name
    EOD;

        return [
            "rows" => $this->db->select($sql, $filterBindtypes, $filterBindvalues)->fetch_all(MYSQLI_NUM),
            "headers" => [
                "ID",
                Translator::t("Partner"),
                Translator::t("Phone"),
                Translator::t("TIN"),
                Translator::t("Firm name"),
                Translator::t("Currency"),
                Translator::t("Balance"),
            ],
            "toolbar" => ["reload" => true, "filter" => true, "redeem_debt" => true],
            "sum" => [6],
            "filter" => $this->getFilterConfig(),
        ];
    }

    protected function getDetailedDebts($filter, $docType, $currencyId)
    {
        $docType = (int)$docType;
        $partnerId = (int)($filter["partner"] ?? 0);
        $dateStart = $filter["date1"] ?? "2000-01-01";
        $dateEnd = $filter["date2"] ?? "2099-12-31";

        $partnerSql = ($partnerId > 0) ? " AND d.f_partner=? " : "";

        $balExpr = $this->openDebtExpr($docType);
        $sqlStart = <<<EOD
        SELECT d.f_partner, COALESCE($balExpr, 0) AS bal
        FROM cash_debts d
        WHERE d.f_doc_type=? $partnerSql AND d.f_currency_id=? AND d.f_date < ?
        GROUP BY d.f_partner
        EOD;
        $bindStart = [$docType];
        $typesStart = "i";
        if ($partnerId > 0) {
            $typesStart .= "i";
            $bindStart[] = $partnerId;
        }
        $typesStart .= "is";
        $bindStart[] = $currencyId;
        $bindStart[] = $dateStart;
        $openingRows = $this->db->select($sqlStart, $typesStart, $bindStart)->fetch_all(MYSQLI_ASSOC);
        $openingByPartner = [];
        foreach ($openingRows as $or) {
            $pid = (int)($or["f_partner"] ?? 0);
            if ($pid > 0) {
                $openingByPartner[$pid] = (float)$or["bal"];
            }
        }

        $sqlOps = <<<EOD
    SELECT
        d.f_id,
        d.f_doc_uuid,
        d.f_partner,
        date_fmt(d.f_date) AS f_date_fmt,
        TRIM(CONCAT(COALESCE(p.f_taxname, ''), ' ', COALESCE(p.f_name, ''))) AS partner_name,
        p.f_phone,
        money_fmt(d.f_credit) AS credit_fmt,
        money_fmt(d.f_debit) AS debit_fmt,
        c.f_name AS currency_name,
        d.f_credit,
        d.f_debit,
        COALESCE(d.f_comment, '') AS f_comment,
        sd.f_id AS store_doc_id,
        sd.f_user_id AS store_doc_number,
        oh.f_id AS order_id,
        oh.f_prefix AS order_prefix,
        (
            SELECT co.f_id
            FROM cash_operations co
            WHERE co.f_order_id = d.f_doc_uuid
              AND co.f_operation_type IN (5, 6)
            ORDER BY co.f_id
            LIMIT 1
        ) AS cash_op_id,
        (
            SELECT co.f_comment
            FROM cash_operations co
            WHERE co.f_order_id = d.f_doc_uuid
              AND co.f_operation_type IN (5, 6)
            ORDER BY co.f_id
            LIMIT 1
        ) AS cash_op_comment
    FROM cash_debts d
    LEFT JOIN c_partners p ON p.f_id = d.f_partner
    LEFT JOIN e_currency c ON c.f_id = d.f_currency_id
    LEFT JOIN store_document sd ON sd.f_id = d.f_doc_uuid
    LEFT JOIN o_header oh ON oh.f_id = d.f_doc_uuid
    WHERE d.f_doc_type=? $partnerSql AND d.f_currency_id=? AND d.f_date BETWEEN ? AND ?
    ORDER BY partner_name ASC, d.f_partner ASC, d.f_date ASC, d.f_id ASC
    EOD;

        $bindOps = [$docType];
        $typesOps = "i";
        if ($partnerId > 0) {
            $typesOps .= "i";
            $bindOps[] = $partnerId;
        }
        $typesOps .= "iss";
        $bindOps[] = $currencyId;
        $bindOps[] = $dateStart;
        $bindOps[] = $dateEnd;
        $dbRows = $this->db->select($sqlOps, $typesOps, $bindOps)->fetch_all(MYSQLI_ASSOC);

        $opsByPartner = [];
        $metaByPartner = [];
        foreach ($dbRows as $dbRow) {
            $pid = (int)($dbRow["f_partner"] ?? 0);
            if ($pid <= 0) {
                continue;
            }
            if (!isset($metaByPartner[$pid])) {
                $metaByPartner[$pid] = [
                    "name" => trim((string)($dbRow["partner_name"] ?? "")),
                    "phone" => (string)($dbRow["f_phone"] ?? ""),
                    "currency" => (string)($dbRow["currency_name"] ?? ""),
                ];
            }
            $opsByPartner[$pid][] = $dbRow;
        }

        foreach ($openingByPartner as $pid => $bal) {
            if (abs($bal) < 0.0001 || isset($metaByPartner[$pid])) {
                continue;
            }
            $p = $this->db->select(
                "SELECT TRIM(CONCAT(COALESCE(f_taxname,''), ' ', COALESCE(f_name,''))) AS n, f_phone FROM c_partners WHERE f_id=?",
                "i",
                [$pid]
            )->fetch_assoc();
            $cur = $this->db->select("SELECT f_name FROM e_currency WHERE f_id=?", "i", [$currencyId])->fetch_assoc();
            $metaByPartner[$pid] = [
                "name" => trim((string)($p["n"] ?? ("#" . $pid))),
                "phone" => (string)($p["f_phone"] ?? ""),
                "currency" => (string)($cur["f_name"] ?? ""),
            ];
            $opsByPartner[$pid] = [];
        }

        $partnerIds = array_keys($metaByPartner);
        usort($partnerIds, function ($a, $b) use ($metaByPartner) {
            return strcasecmp($metaByPartner[$a]["name"], $metaByPartner[$b]["name"]);
        });

        $rows = [];
        $closingTotal = 0.0;
        foreach ($partnerIds as $pid) {
            $meta = $metaByPartner[$pid];
            $ops = $opsByPartner[$pid] ?? [];
            $openingRaw = (float)($openingByPartner[$pid] ?? 0);
            if (abs($openingRaw) < 0.0001 && count($ops) === 0) {
                continue;
            }

            $currentBalance = $openingRaw;
            $openCredit = "0.00";
            $openDebit = "0.00";
            if ($openingRaw > 0.00001) {
                if ($docType === 2) {
                    $openDebit = number_format($openingRaw, 2, ".", "");
                } else {
                    $openCredit = number_format($openingRaw, 2, ".", "");
                }
            } elseif ($openingRaw < -0.00001) {
                if ($docType === 2) {
                    $openCredit = number_format(abs($openingRaw), 2, ".", "");
                } else {
                    $openDebit = number_format(abs($openingRaw), 2, ".", "");
                }
            }
            $rows[] = [
                "", // uuid
                "0", // cash_debts id
                "opening", // row kind
                $dateStart,
                $meta["name"],
                $meta["phone"],
                Translator::t("OPENING BALANCE"),
                $openCredit,
                $openDebit,
                $meta["currency"],
                number_format($currentBalance, 2, ".", ""),
            ];

            foreach ($ops as $dbRow) {
                $credit = (float)$dbRow["f_credit"];
                $debit = (float)$dbRow["f_debit"];
                $currentBalance += $this->rowDelta($docType, $credit, $debit);
                $opInfo = $this->describeDebtRow($docType, $dbRow);
                $rows[] = [
                    (string)($dbRow["f_doc_uuid"] ?? ""),
                    (string)(int)($dbRow["f_id"] ?? 0),
                    $opInfo["kind"],
                    (string)$dbRow["f_date_fmt"],
                    $meta["name"],
                    $meta["phone"],
                    $opInfo["label"],
                    (string)$dbRow["credit_fmt"],
                    (string)$dbRow["debit_fmt"],
                    (string)$dbRow["currency_name"],
                    number_format($currentBalance, 2, ".", ""),
                ];
            }
            $closingTotal += $currentBalance;
        }

        return [
            "rows" => $rows,
            "hidden_columns" => [0, 1, 2],
            "headers" => [
                "UUID",
                "DebtId",
                "Kind",
                Translator::t("Date"),
                Translator::t("Partner"),
                Translator::t("Phone"),
                Translator::t("Operation"),
                Translator::t("Credit"),
                Translator::t("Debit"),
                Translator::t("Currency"),
                Translator::t("Balance"),
            ],
            "toolbar" => ["reload" => true, "filter" => true, "redeem_debt" => true],
            "sum" => [7, 8],
            "footer_values" => [
                "10" => number_format($closingTotal, 2, ".", ","),
            ],
            "filter" => $this->getFilterConfig(),
        ];
    }

    /**
     * @param array<string,mixed> $dbRow
     * @return array{kind:string,label:string}
     */
    private function describeDebtRow(int $docType, array $dbRow): array
    {
        $credit = (float)($dbRow["f_credit"] ?? 0);
        $debit = (float)($dbRow["f_debit"] ?? 0);
        $comment = trim((string)($dbRow["f_comment"] ?? ""));
        if ($comment === "") {
            $comment = trim((string)($dbRow["cash_op_comment"] ?? ""));
        }

        $storeId = (string)($dbRow["store_doc_id"] ?? "");
        $orderId = (string)($dbRow["order_id"] ?? "");
        $cashOpId = (int)($dbRow["cash_op_id"] ?? 0);
        $prefix = trim((string)($dbRow["order_prefix"] ?? ""));
        $storeNum = trim((string)($dbRow["store_doc_number"] ?? ""));

        if ($storeId !== "") {
            $label = Translator::t("Purchase on credit");
            if ($storeNum !== "") {
                $label .= " №" . $storeNum;
            }
            return ["kind" => "store", "label" => $label];
        }

        if ($orderId !== "") {
            if ($docType === 2 && $debit > 0.00001 && $credit < 0.00001) {
                $label = Translator::t("Sale on credit");
            } elseif ($docType === 2 && $credit > 0.00001) {
                $label = Translator::t("Return / debt write-off");
            } else {
                $label = Translator::t("Order");
            }
            if ($prefix !== "") {
                $label .= " " . $prefix;
            }
            return ["kind" => "order", "label" => $label];
        }

        // Redeem / manual payment (linked cash_operations or inferred by signs)
        if ($cashOpId > 0 || $comment !== "" || ($docType === 1 && $debit > 0.00001) || ($docType === 2 && $credit > 0.00001)) {
            if ($docType === 1) {
                $label = Translator::t("Payment to supplier");
            } else {
                $label = Translator::t("Payment from customer");
            }
            if ($comment !== "") {
                $label .= ": " . $comment;
            }
            return ["kind" => "redeem", "label" => $label];
        }

        if ($credit > 0.00001) {
            $label = $docType === 1
                ? Translator::t("Purchase on credit")
                : Translator::t("Payment from customer");
            $kind = $docType === 1 ? "other" : "redeem";
        } else {
            $label = $docType === 1
                ? Translator::t("Payment to supplier")
                : Translator::t("Sale on credit");
            $kind = $docType === 1 ? "redeem" : "other";
        }
        return ["kind" => $kind, "label" => $label];
    }

    protected function getFilterConfig()
    {
        return [
            ["type" => "viewmode", "name" => "viewmode", "label" => Translator::t("View mode"), "default" => 1, "values" => [
                ["label" => Translator::t("Total partners debts"), "value" => 1],
                ["label" => Translator::t("Detailed partners debts"), "value" => 2],
                ["label" => Translator::t("Total customers debts"), "value" => 3],
                ["label" => Translator::t("Detailed customers debts"), "value" => 4],
            ]],
            ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
            ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
            ["type" => "keyvalue", "name" => "currency", "label" => Translator::t("Currency"), "default" => 1, "function" => "currency"],
            ["type" => "keyvalue", "name" => "partner", "label" => Translator::t("Partner"), "function" => "partner"],
        ];
    }
}
