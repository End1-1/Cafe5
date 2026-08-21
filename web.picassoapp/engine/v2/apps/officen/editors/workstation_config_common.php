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
            'print_server',
            'print_paper_width',
            'bistro',
            'f_auto_fiscal',
            'dont_allow_negative_remains',
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
            'print_server' => '',
            'print_paper_width' => 0,
            'bistro' => false,
            'f_auto_fiscal' => false,
            'dont_allow_negative_remains' => false,
        ];
    }

    /** @param array<string,mixed> $stored */
    protected function mergeDefaults(array $stored): array
    {
        if (!isset($stored['print_server']) && isset($stored['print_sssserver'])) {
            $stored['print_server'] = $stored['print_sssserver'];
        }
        return parent::mergeDefaults($stored);
    }
}
