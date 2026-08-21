<?php
# © 2026 , Kudryashov Vasili
# Created: 2026-01-17 13:29:36
# Last Modified: 2026-01-17 13:29:39
require_once __DIR__ . "/index.php";

class Editors extends Auth
{
    private array $allowedEditors = [
        "form_workstations" => "Workstations",
        "form_cashsessions" => "CashSessions",
        "form_store_documents" => "StoreDocuments",
        "form_store_moves" => "StoreMoves",
        "form_stock" => "Stock",
        "form_inventory_documents" => "StoreInventory",
        "form_sold_items" => "SoldItems",
        "form_consumed_goods" => "ConsumedGoods",
        "form_summary" => "Summary",
        "form_debts" => "Debts",
        "form_revenue" => "Revenue",
        "form_salary" => "Salary",
        "form_order_in_progress" => "OrderInProgress",
        "form_groups_of_goods" => "GoodsGroup",
        "form_service_values" => "ServiceValues",
        "form_tables" => "Tables",
    ];

    private function validate($params)
    {
        if (empty($params->editor)) {
            dieWithCode("Editor not specified");
        }

        if (!array_key_exists($params->editor, $this->allowedEditors)) {
            dieWithCode("Access denied for editor");
        }

        $className = $this->allowedEditors[$params->editor];
        $fileName = __DIR__ . "/editors/" . strtolower($className) . ".php";

        if (!file_exists($fileName)) {
            dieWithCode("Editor handler not found");
        }

        require_once $fileName;
    }

    public function GetAll($params)
    {
        $this->validate($params);

        $className = $this->allowedEditors[$params->editor];
        $class = new $className($this);

        $this->result = array_merge($class->get($params), $this->result);
        $this->echoResult();
    }

    public function GetItem($params)
    {
        $this->validate($params);

        $className = $this->allowedEditors[$params->editor];
        $class = new $className($this);

        $this->result = array_merge($class->GetItem($params), $this->result);
        $this->echoResult();
    }

    public function Save($params)
    {
        $this->validate($params);

        $className = $this->allowedEditors[$params->editor];
        $class = new $className($this);

        if (!method_exists($class, 'save')) {
            dieWithCode(Translator::t('Save is not supported for this editor'));
        }

        $this->result = array_merge($class->save($params), $this->result);
        $this->echoResult();
    }

    public function Remove($params)
    {
        $this->validate($params);

        $className = $this->allowedEditors[$params->editor];
        $class = new $className($this);

        if (!method_exists($class, 'delete')) {
            dieWithCode(Translator::t('Delete is not supported for this editor'));
        }

        $this->result = array_merge($class->delete($params), $this->result);
        $this->echoResult();
    }

    public function ListSaleDocs($params)
    {
        $this->validate($params);

        $className = $this->allowedEditors[$params->editor];
        $class = new $className($this);

        if (!method_exists($class, 'listSaleDocs')) {
            dieWithCode(Translator::t('List sale docs is not supported for this editor'));
        }

        $this->result = array_merge($class->listSaleDocs($params), $this->result);
        $this->echoResult();
    }

    public function RecalcSale($params)
    {
        $this->validate($params);

        $className = $this->allowedEditors[$params->editor];
        $class = new $className($this);

        if (!method_exists($class, 'recalcSale')) {
            dieWithCode(Translator::t('Recalc sale is not supported for this editor'));
        }

        $this->result = array_merge($class->recalcSale($params), $this->result);
        $this->echoResult();
    }
}
