<?php
# © 2026 , Kudryashov Vasili

class WorkstationConfigCommon extends WorkstationConfigBase
{
    public function type(): int
    {
        return 5;
    }

    protected function allowedKeys(): array
    {
        return [
            'f_present_card_pattern',
            'f_discount_card_pattern',
            'f_accumulate_card_pattern',
            'f_scale_pattern',
            'f_quick_debt_partner_id',
            'input_cashbox_amount_before_close',
            'print_sssserver',
            'bistro',
        ];
    }

    protected function defaultConfig(): array
    {
        return [
            'f_present_card_pattern' => '2211',
            'f_discount_card_pattern' => '0000',
            'f_accumulate_card_pattern' => '1111',
            'f_scale_pattern' => '22',
            'f_quick_debt_partner_id' => 0,
            'input_cashbox_amount_before_close' => true,
            'print_sssserver' => 'http://127.0.0.1:8181/print',
            'bistro' => false,
        ];
    }
}
