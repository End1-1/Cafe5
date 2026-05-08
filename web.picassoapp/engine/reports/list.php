<?php
# (C) 2024-2025 Kudryashov Vasili
# Last modified - 2025-03-18 09:00:30
require_once __DIR__ . "/reports.php";

class ReportList extends PClass
{
    public function get()
    {
        $this->result = [
            "reports" => [
                ["title" => "Համառոտ հասույթ", "route" => "/engine/reports/sale1.php", "image" => "goods.png"],
                ["title" => "Քանակի հիշացում", "route" => "/engine/goods/qty-reminder.php", "image" => "goods.png"],
                ["title" => $this->tr("Effectivness"), "route" => "/engine/reports/effectivness.php", "image" => "effectiveness.png"],
                ["title" => "Պահեստի մնացորդ", "route" => "/engine/reports/out-of-stock.php", "image" => "goods.png"],
                ["title" => "Ապրանքների առկայություն", "route" => "/engine/goods/store.php", "image" => "goods.png"],
                ["title" => "Վաճառք-ինքնառժեք", "route" => "/engine/reports/suro-sale-selfcost.php", "image" => "cash.png"],
                ["title" => "Վաճառք-ինքնառժեք բաղադրիչներով", "route" => "/engine/reports/suro-sale-selfcost-recipe.php", "image" => "cash.png"],
                ["title" => "Կարճ պարտքեր", "route" => "/engine/reports/short-debts.php", "image" => "cash.png"],
                ["title" => "Պարտքերի պատմություն", "route" => "/engine/reports/debt-start-move-final.php", "image" => "cash.png"],
                ["title" => "Վաճառք ըստ օրերի", "route" => "/engine/reports/suro-sale-dish-date-column.php", "image" => "cash.png"],
                ["title" => "Գնումներ ըստ օրերի", "route" => "/engine/reports/suro-store-dish-date-column.php", "image" => "cash.png"],
                ["title" => "Ապրանքի շարժ վաճառք/պահեստ", "route" => "/engine/reports/sale-store-relation.php", "image" => "goods.png"],
                ["title" => $this->tr("Draft sales"), "route" => "/engine/reports/draft-sales.php", "image" => "goods.png"],
                ["title" => $this->tr("Dish sales by payment type"), "route" => "/engine/reports/arturcardsale.php", "image" => "goods.png"],
                ["title" => $this->tr("Daily shop report"), "route" => "/engine/reports/elina_daily_rep.php", "image" => "goods.png"],
                ["title" => $this->tr("Complectation additions"), "route" => "/engine/reports/complectation_additions.php", "image" => "goods.png"],
            ]
        ];
        $this->echoResult();
    }
}

$rl = new ReportList();
$rl->get();