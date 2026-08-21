<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-04-21 11:51:45
# Modified: 2026-04-21 11:51:45

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

        // Проверка обязательной валюты
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
     * Итоговый отчет (Total)
     */
    protected function getTotalDebts($filter, $docType, $currencyId)
    {
        $filterExtra = "";
        $filterBindtypes = "ii";
        $filterBindvalues = [(int)$docType, $currencyId];

        if (($filter["partner"] ?? 0) > 0) {
            $filterExtra .= " and d.f_partner=? ";
            $filterBindtypes .= "i";
            $filterBindvalues[] = (int) $filter["partner"];
        }

        $sql = <<<EOD
    SELECT 
        d.f_partner, -- Добавляем ID партнера в начало (index 0)
        p.f_name AS partner_name, p.f_phone, p.f_taxcode, p.f_taxname, c.f_name AS currency_name,
        money_fmt(SUM(d.f_credit) - SUM(d.f_debit)) AS balance
    FROM cash_debts d
    LEFT JOIN c_partners p ON p.f_id = d.f_partner
    LEFT JOIN e_currency c ON c.f_id = d.f_currency_id
    WHERE d.f_doc_type=? AND d.f_currency_id=? $filterExtra
    GROUP BY d.f_partner, d.f_currency_id, p.f_name, p.f_taxcode, p.f_taxname, c.f_name
    HAVING (SUM(d.f_credit) - SUM(d.f_debit)) <> 0
    EOD;

        return [
            "rows" => $this->db->select($sql, $filterBindtypes, $filterBindvalues)->fetch_all(MYSQLI_NUM),
            "headers" => ["ID", Translator::t("Partner"), Translator::t("Phone"), Translator::t("TIN"), Translator::t("Firm name"), Translator::t("Currency"), Translator::t("Balance")],
            "toolbar" => ["reload" => true, "filter" => true, "redeem_debt" => true],
            "sum" => [5], // Смещаем индекс суммы, так как добавили колонку
            "filter" => $this->getFilterConfig()
        ];
    }

    /**
     * Детальный отчет (Detailed)
     */
    protected function getDetailedDebts($filter, $docType, $currencyId)
    {
        $partnerId = (int)($filter["partner"] ?? 0);
        $dateStart = $filter["date1"] ?? '2000-01-01';
        $dateEnd = $filter["date2"] ?? '2099-12-31';

        $partnerSql = ($partnerId > 0) ? " AND f_partner=? " : "";
        $partnerSqlD = ($partnerId > 0) ? " AND d.f_partner=? " : "";

        // 1. Входящее сальдо
        $sqlStart = "SELECT SUM(f_credit) - SUM(f_debit) FROM cash_debts WHERE f_doc_type=? $partnerSql AND f_currency_id=? AND f_date < ?";
        $bindStart = [$docType];
        $typesStart = "i";
        if ($partnerId > 0) {
            $typesStart .= "i";
            $bindStart[] = $partnerId;
        }
        $typesStart .= "is";
        $bindStart[] = $currencyId;
        $bindStart[] = $dateStart;
        $resStart = $this->db->select($sqlStart, $typesStart, $bindStart)->fetch_row();
        $openingRaw = (float)($resStart[0] ?? 0);

        // 2. Список операций + Добавляем f_doc_uuid
        $sqlOps = <<<EOD
    SELECT 
        d.f_doc_uuid, -- Индекс 0: UUID для открытия заказа
        date_fmt(d.f_date), 
        concat(coalesce(p.f_taxname, ''), ' ', coalesce(p.f_name, '')), 
        p.f_phone,
        money_fmt(d.f_credit), 
        money_fmt(d.f_debit), 
        c.f_name,
        d.f_credit, 
        d.f_debit   
    FROM cash_debts d
    LEFT JOIN c_partners p ON p.f_id = d.f_partner
    LEFT JOIN e_currency c ON c.f_id = d.f_currency_id
    WHERE d.f_doc_type=? $partnerSqlD AND d.f_currency_id=? AND d.f_date BETWEEN ? AND ?
    ORDER BY d.f_date ASC, d.f_id ASC
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
        $dbRows = $this->db->select($sqlOps, $typesOps, $bindOps)->fetch_all(MYSQLI_NUM);

        $rows = [];
        $currentBalance = $openingRaw;

        // Первая строка (Opening Balance) — UUID ставим пустой или зарезервированный
        $rows[] = [
            "", // Нет конкретного документа для начального сальдо
            $dateStart,
            Translator::t("OPENING BALANCE"),
            Translator::t(""),
            $openingRaw > 0 ? number_format($openingRaw, 2, '.', '') : "0.00",
            $openingRaw < 0 ? number_format(abs($openingRaw), 2, '.', '') : "0.00",
            "",
            number_format($openingRaw, 2, '.', '')
        ];

        foreach ($dbRows as $dbRow) {
            $currentBalance += ((float)$dbRow[7] - (float)$dbRow[8]);
            $rows[] = [
                $dbRow[0], // f_doc_uuid (теперь тут лежит UUID заказа)
                $dbRow[1], // Дата
                $dbRow[2], // Партнер/Описание
                $dbRow[3], // телефон
                $dbRow[4], // Кредит
                $dbRow[5], // Дебет
                $dbRow[6], // Валюта
                number_format($currentBalance, 2, '.', '') // Текущий баланс
            ];
        }

        return [
            "rows" => $rows,
            "hidden_columns" => [0],
            "headers" => [
                "UUID", // Скрытый столбец в Qt
                Translator::t("Date"),
                Translator::t("Description"),
                Translator::t("Phone"),
                Translator::t("Credit"),
                Translator::t("Debit"),
                Translator::t("Currency"),
                Translator::t("Balance")
            ],
            "toolbar" => ["reload" => true, "filter" => true, "redeem_debt" => true],
            "sum" => [4, 5], // Индексы Кредит и Дебет сместились на +1
            "filter" => $this->getFilterConfig()
        ];
    }

    protected function getFilterConfig()
    {
        return [
            ["type" => "viewmode", "name" => "viewmode", "label" => Translator::t("View mode"), "default" => 1, "values" => [
                ["label" => Translator::t("Total partners debts"), "value" => 1],
                ["label" => Translator::t("Detailed partners debts"), "value" => 2],
                ["label" => Translator::t("Total customers debts"), "value" => 3],
                ["label" => Translator::t("Detailed customers debts"), "value" => 4]
            ]],
            ["type" => "date", "name" => "date1", "label" => Translator::t("Date start")],
            ["type" => "date", "name" => "date2", "label" => Translator::t("Date end")],
            ["type" => "keyvalue", "name" => "currency", "label" => Translator::t("Currency"), "default" => 1, "function" => "currency"],
            ["type" => "keyvalue", "name" => "partner", "label" => Translator::t("Partner"), "function" => "partner"]
        ];
    }
}
