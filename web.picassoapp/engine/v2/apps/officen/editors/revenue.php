<?php
# © 2026 , Kudryashov Vasili

require_once __DIR__ . "/../../../worker/helper.php";

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
                return $this->getTotalCash($filter, $currencyId);
            case 2:
                return $this->getDetailedCash($filter, $currencyId);
        }

        return ["rows" => [], "headers" => [], "filter" => $this->getFilterConfig()];
    }

    protected function getTotalCash($filter, $currencyId)
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

        $sql = <<<EOD
    SELECT 
        c.f_id,
        c.f_name AS cashbox_name,
        cur.f_name AS currency_name,
        d.f_value as payment_type,
        money_fmt(IFNULL(SUM(op.f_debit), 0) - IFNULL(SUM(op.f_credit), 0)) AS balance
    FROM cash_box c
    CROSS JOIN e_currency cur ON cur.f_id = ?
    LEFT JOIN cash_operations op ON op.f_cashbox_id = c.f_id AND op.f_currency_id = cur.f_id
    left join l_dictionary d on d.f_dict_id=op.f_payment_type_id and d.f_dict='cash_payment_types'and d.f_lang='hy'
    WHERE 1=1 $cashFilter
    GROUP BY c.f_id, c.f_name, cur.f_name, d.f_value
    EOD;

        return [
            "rows" => $this->db->select($sql, $bindTypes, $bindValues)->fetch_all(MYSQLI_NUM),
            "headers" => ["ID", Translator::t("Cashbox"), Translator::t("Currency"), Translator::t("Payment type"), Translator::t("Balance")],
            "toolbar" => ["reload" => true, "filter" => true, "new" => true],
            "sum" => [3],
            "filter" => $this->getFilterConfig()
        ];
    }

    public function getDetailedCash($filter, $currencyId)
    {
        $cashboxId = (int)($filter["cashbox"] ?? 0);
        $dateStart = (!empty($filter["date1"])) ? $filter["date1"] : '2000-01-01';
        $dateEnd = (!empty($filter["date2"])) ? $filter["date2"] : date('Y-m-d');

        $baseWhere = " op.f_currency_id = ? ";
        $baseParams = [$currencyId];
        $baseTypes = "i";

        if ($cashboxId > 0) {
            $baseWhere .= " AND op.f_cashbox_id = ? ";
            $baseParams[] = $cashboxId;
            $baseTypes .= "i";
        }

        // 1. Входящий остаток
        $sqlStart = "SELECT SUM(op.f_debit) - SUM(op.f_credit) FROM cash_operations op WHERE $baseWhere AND op.f_datetime < ?";
        $paramsStart = $baseParams;
        $paramsStart[] = $dateStart . " 00:00:00";
        $typesStart = $baseTypes . "s";

        $resStart = $this->db->select($sqlStart, $typesStart, $paramsStart)->fetch_row();
        $openingBalance = (float)($resStart[0] ?? 0);

        // 2. Список операций
        $sqlOps = <<<EOD
    SELECT 
        op.f_operation_type,         -- index 0
        op.f_order_id,               -- index 1
        op.f_datetime,               -- index 2
        lp.f_value as payment_type,  -- index 3 (из словаря)
        money_fmt(op.f_debit) as d_f, -- index 4
        money_fmt(op.f_credit) as c_f, -- index 5
        op.f_comment,                -- index 6
        op.f_debit,                  -- index 7 (raw)
        op.f_credit                  -- index 8 (raw)
    FROM cash_operations op
    LEFT JOIN cash_box cb ON cb.f_id = op.f_cashbox_id
    LEFT JOIN l_dictionary lp ON lp.f_dict_id = op.f_payment_type_id AND lp.f_dict = 'cash_payment_types'
    WHERE $baseWhere AND op.f_datetime BETWEEN ? AND ?
    ORDER BY op.f_datetime ASC, op.f_id ASC
    EOD;

        $paramsOps = $baseParams;
        $paramsOps[] = $dateStart . " 00:00:00";
        $paramsOps[] = $dateEnd . " 23:59:59";
        $typesOps = $baseTypes . "ss";

        $dbRows = $this->db->select($sqlOps, $typesOps, $paramsOps)->fetch_all(MYSQLI_NUM);

        $rows = [];
        $currentBalance = $openingBalance;

        // Строка начального сальдо
        $rows[] = [
            "", // Type
            "", // Order ID
            $dateStart . " 00:00:00",
            Translator::t("OPENING BALANCE"),
            "", // Payment Type
            "", // Приход
            "", // Расход
            number_format($openingBalance, 2, '.', ' ')
        ];

        foreach ($dbRows as $dbRow) {
            $paymentType = $dbRow[3];
            $debitFmt    = $dbRow[4];
            $creditFmt   = $dbRow[5];
            $comment     = $dbRow[6];
            $debitRaw    = (float)$dbRow[7];
            $creditRaw   = (float)$dbRow[8];

            $currentBalance += ($debitRaw - $creditRaw);

            $rows[] = [
                $dbRow[0], // f_operation_type
                $dbRow[1], // f_order_id (будет скрыт)
                $dbRow[2], // f_datetime
                $comment,
                $paymentType,
                ($debitRaw > 0) ? $debitFmt : "",
                ($creditRaw > 0) ? $creditFmt : "-", // Расход со знаком "-" для визуальности (по желанию)
                money_fmt_php($currentBalance, 2, '.', ' ')
            ];
        }

        return [
            "rows" => $rows,
            "hidden_columns" => [0, 1],
            "headers" => [
                Translator::t("Type"),
                "UUID",
                Translator::t("DateTime"),
                Translator::t("Comment"),
                Translator::t("Payment type"),
                Translator::t("Income"),
                Translator::t("Outcome"),
                Translator::t("Balance")
            ],
            "toolbar" => ["reload" => true, "filter" => true, "new" => true],
            "sum" => [5, 6, 7], // Суммируем приход, расход и баланс
            "filter" => $this->getFilterConfig()
        ];
    }

    protected function getFilterConfig()
    {
        return [
            ["type" => "combobox", "name" => "viewmode", "label" => Translator::t("View mode"), "default" => 1, "values" => [
                ["label" => Translator::t("Total by cashboxes"), "value" => 1],
                ["label" => Translator::t("Detailed movement"), "value" => 2],
            ]],
            ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
            ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
            ["type" => "keyvalue", "name" => "currency", "label" => Translator::t("Currency"), "default" => 1, "function" => "currency"],
            ["type" => "keyvalue", "name" => "cashbox", "label" => Translator::t("Cashbox"), "function" => "cashbox"]
        ];
    }
}
