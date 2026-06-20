<?php
# © 2026 , Kudryashov Vasili

class WorkstationConfigShop extends WorkstationConfigBase
{
    public function type(): int
    {
        return 4;
    }

    protected function allowedKeys(): array
    {
        return [
            'f_cashbox_id',
            'f_default_hall_id',
            'f_fiscal_machine_id',
            'f_default_store_id',
            'receipt_phone',
            'receipt_no_table',
            'receipt_no_service_hint',
            'receipt_no_discount_hint',
        ];
    }

    protected function defaultConfig(): array
    {
        return [
            'f_cashbox_id' => 1,
            'f_default_hall_id' => 1,
            'f_fiscal_machine_id' => 1,
            'f_default_store_id' => 1,
            'receipt_phone' => '',
            'receipt_no_table' => true,
            'receipt_no_service_hint' => true,
            'receipt_no_discount_hint' => true,
        ];
    }
}
