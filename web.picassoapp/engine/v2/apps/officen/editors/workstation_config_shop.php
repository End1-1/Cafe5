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
            'f_default_table_id',
            'f_fiscal_machine_id',
            'f_default_store_id',
            'receipt_phone',
            'receipt_printer',
            'receipt_no_table',
            'receipt_no_service_hint',
            'receipt_no_discount_hint',
            'assign_sale_to_associate',
            'arcus_port',
            'arcus_address',
            'arcus_key',
            'f_precheck_font_size',
            'f_precheck_margins',
        ];
    }

    protected function defaultConfig(): array
    {
        return [
            'f_cashbox_id' => 1,
            'f_default_hall_id' => 1,
            'f_default_table_id' => 1,
            'f_fiscal_machine_id' => 1,
            'f_default_store_id' => 1,
            'receipt_phone' => '',
            'receipt_printer' => '',
            'receipt_no_table' => false,
            'receipt_no_service_hint' => false,
            'receipt_no_discount_hint' => false,
            'assign_sale_to_associate' => false,
            'arcus_port' => 0,
            'arcus_address' => '',
            'arcus_key' => '',
            'f_precheck_font_size' => 16,
            'f_precheck_margins' => 0,
        ];
    }
}
