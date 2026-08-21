<?php
# © 2026 , Kudryashov Vasili

class WorkstationConfigWaiter extends WorkstationConfigBase
{
    public function type(): int
    {
        return 1;
    }

    /** @return array<string,bool> */
    public static function defaultSetupButtons(): array
    {
        return [
            'dlgface_disable_btnguests' => false,
            'dlgface_disable_btninprogress' => false,
            'dlgface_disable_btnmyorders' => false,
            'dlgface_disable_btnnormalview' => false,
            'dlgface_disable_preorders' => false,
            'dglorder_disable_btntransfertable' => false,
            'dlgorder_disable_btnactivate_preorder' => false,
            'dlgorder_disable_btnanyqty' => false,
            'dlgorder_disable_btnbackgroup' => false,
            'dlgorder_disable_btnchangemenu' => false,
            'dlgorder_disable_btnchangestaff' => false,
            'dlgorder_disable_btndeliveryamount' => false,
            'dlgorder_disable_btndiscount' => false,
            'dlgorder_disable_btngroupselect' => false,
            'dlgorder_disable_btnpackage' => false,
            'dlgorder_disable_btnpart1' => false,
            'dlgorder_disable_btnpart2' => false,
            'dlgorder_disable_btnpart3' => false,
            'dlgorder_disable_btnpartfavorite' => false,
            'dlgorder_disable_btnpreorder_datetime' => false,
            'dlgorder_disable_btnprepaid' => false,
            'dlgorder_disable_btnreceiptlanguage' => false,
            'dlgorder_disable_btnshowcustomerdisplay' => false,
            'dlgorder_disable_btnshowhideremoved' => false,
            'dlgorder_disable_btnservice' => false,
            'dlgorder_disable_btnsit' => false,
            'dlgorder_disable_btnstoplistmode' => false,
            'dlgorder_disable_btntransferdishes' => false,
            'payment_button_1' => false,
            'payment_button_2' => false,
            'payment_button_3' => false,
            'payment_button_4' => false,
            'payment_button_5' => false,
            'payment_button_6' => false,
            'payment_button_7' => false,
            'payment_button_8' => false,
            'payment_button_9' => false,
        ];
    }

    protected function allowedKeys(): array
    {
        return [
            'f_cashbox_id',
            'f_default_hall_id',
            'f_fiscal_machine_id',
            'f_default_store_id',
            'receipt_phone',
            'precheck_printer',
            'receipt_no_table',
            'receipt_no_service_hint',
            'receipt_policy',
            'receipt_no_discount_hint',
            'dlgsearchmenu_hsection_size',
            'dlgsearchmenu_vsection_size',
            'cost_depend_on_service_and_discount',
            'do_not_print_customer_on_receipt',
            'customer_notification',
            'recent_dishes_minutes',
            'setup_buttons',
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
            'precheck_printer' => '',
            'receipt_no_table' => false,
            'receipt_no_service_hint' => false,
            'receipt_policy' => '',
            'receipt_no_discount_hint' => false,
            'dlgsearchmenu_hsection_size' => 240,
            'dlgsearchmenu_vsection_size' => 100,
            'cost_depend_on_service_and_discount' => false,
            'do_not_print_customer_on_receipt' => false,
            'customer_notification' => false,
            'recent_dishes_minutes' => 40,
            'setup_buttons' => self::defaultSetupButtons(),
        ];
    }
}
