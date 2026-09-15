<?php
# (C) 2025–2026 Kudryashov Vasili
#
# Очистка БД перед продом / из FrontDesk → Базы данных → Reset.
# POST /engine/worker/cleardb.php  JSON: {"mode":"storeandsale"} | {"mode":"all"}
#
# store          — только склад (остатки, документы, резервы)
# storeandsale   — операционные данные: склад + продажи + касса + логи (каталог, пользователи, настройки — остаются)
# all            — storeandsale + каталог/меню/залы + сиды по умолчанию (полный сброс контента)

require_once __DIR__ . "/worker.php";

class ClearDB extends DB
{
    public function __construct()
    {
        parent::__construct();
    }

    /** @param list<string> $statements */
    private function runBatch(array $statements): void
    {
        foreach ($statements as $sql) {
            $sql = trim($sql);
            if ($sql === '') {
                continue;
            }
            $this->stmtall($sql);
        }
    }

    private function tableExists(string $table): bool
    {
        $table = $this->db->real_escape_string($table);
        $res = $this->db->query("SHOW TABLES LIKE '{$table}'");
        return $res && $res->num_rows > 0;
    }

    private function deleteTable(string $table): void
    {
        if (!$this->tableExists($table)) {
            return;
        }
        $this->stmtall("DELETE FROM `{$table}`");
    }

    /** @param list<string> $tables */
    private function deleteTables(array $tables): void
    {
        foreach ($tables as $table) {
            $this->deleteTable($table);
        }
    }

    private function dropTriggerIfExists(string $name): void
    {
        $name = $this->db->real_escape_string($name);
        $this->db->query("DROP TRIGGER IF EXISTS `{$name}`");
    }

    /** Склад v2 + legacy a_store. */
    private function clearStore(): void
    {
        $this->deleteTables([
            'store_calc_queue',
            'store_moves',
            'store_user',
            'store_stock',
            'store_inventory_user',
            'store_inventory_document',
            'store_document',
            'a_store_reserve',
            'a_complectation_additions',
            'a_store_dish_waste',
            'a_store',
            'a_store_draft',
            'a_store_inventory',
            'a_store_sale',
            'a_store_current',
            'a_store_temp',
            'a_header_store',
            'op_body',
            'op_header',
            's_log_store_price',
        ]);
        if ($this->tableExists('a_header')) {
            $this->stmtall('DELETE FROM a_header WHERE f_type IN (1, 2, 3, 4, 6, 7)');
        }
    }

    /** Продажи, заказы, касса, фискал, бонусы. */
    private function clearSales(): void
    {
        $this->deleteTables([
            'cash_debts',
            'cash_operations',
            'cash_session',
            'o_daily_counter',
            'o_goods_process',
            'o_goods_loading',
            'o_draft_sale_body',
            'o_draft_sale',
            'o_draft_sound',
            'a_sale_temp',
            'e_cash',
            'a_header_cash',
            'a_header_shop2partneraccept',
            'a_header_shop2partner',
            'a_header',
            'a_calc_price',
            'a_dc',
            'b_discount_ops',
            'b_accumulate_ops',
            'b_gift_card_ops',
            'b_gift_card_history',
            'b_gift_card_sale_options',
            'b_gift_card',
            'b_history',
            'b_clients_debts',
            'b_car_orders',
            'o_tax_log',
            'o_tax_debug',
            'o_tax',
            'o_payment',
            'o_package',
            'o_goods',
            'o_body',
            'o_preorder',
            'o_header_hotel_date',
            'o_header_hotel',
            'o_header_options',
            'o_header_flags',
            'o_header',
            'o_pay_cl',
            'o_pay_room',
            'o_additional',
            'o_waiterserver_debug',
            's_working_sessions',
            's_web_log',
            's_login_session',
            's_attendance',
            's_salary',
            's_salary_attendance',
            's_salary_body',
            's_salary_options',
            'a_result',
        ]);
        if ($this->tableExists('o_goods')) {
            $this->stmtall('UPDATE o_goods SET f_returnfrom = NULL');
        }
    }

    /**
     * Очистка только склада.
     */
    public function store(): void
    {
        $this->db->query('SET FOREIGN_KEY_CHECKS = 0');
        $this->clearStore();
        $this->db->query('SET FOREIGN_KEY_CHECKS = 1');
        $this->echoResult();
    }

    /**
     * Склад + продажи + касса (каталог и настройки сохраняются).
     */
    public function storeandsale(): void
    {
        $this->dropTriggerIfExists('prevent_multi_delete');
        $this->db->query('SET FOREIGN_KEY_CHECKS = 0');
        $this->clearStore();
        $this->clearSales();
        $this->db->query('SET FOREIGN_KEY_CHECKS = 1');
        $this->echoResult();
    }

    /**
     * Полный сброс контента + сиды по умолчанию.
     */
    public function all(): void
    {
        $this->dropTriggerIfExists('prevent_multi_delete');
        $this->db->query('SET FOREIGN_KEY_CHECKS = 0');
        $this->clearStore();
        $this->clearSales();

        $catalogTables = [
            's_settings_values',
            's_settings_names',
            's_user_photo',
            's_user_access',
            's_user_fingerprint',
            's_user_config',
            'c_partners',
            'b_car',
            'b_cards_discount',
            'b_discount_cards',
            'b_accumulate_cards',
            'd_menu',
            'd_menu_names',
            'd_recipes',
            'd_dish',
            'd_part2',
            'c_menu',
            's_images',
            'c_goods_option',
            'c_goods_multiscancode',
            'c_goods_comment',
            'd_part1',
            'd_printers',
            'c_goods_images',
            'c_goods_special_prices',
            'c_goods_complectation',
            'c_goods_classes',
            'c_goods_prices',
            'c_goods',
            'c_groups',
            'c_stoplist',
            's_waiter_reports',
            's_cache',
            'droid_message',
            'd_image',
            'd_print_aliases',
            'o_dish_remove_reason',
            'd_dish_comment',
            'h_tables',
            'h_halls',
            'c_units',
            'd_special',
            'd_package',
            'd_package_list',
            's_syncronize',
            's_syncronize_in',
            'e_cash_names',
            'o_service_values',
            's_custom_reports',
            's_report_template_access',
            's_report_template',
            's_report_template_params',
            'mf_daily_workers',
            'mf_daily_process',
            'mf_process',
            'mf_actions',
            'mf_actions_group',
            'mf_materials',
            'mf_materials_in_actions',
            'mf_tasks',
            'mf_task_stage',
            'mf_task_workshop',
            'mf_stage',
            'm_goal_product_material',
            'm_goal_product',
            'm_goal_product_status',
            's_activation',
            'files',
            'ararix_restaurants',
            'c_storages',
        ];

        $this->deleteTables($catalogTables);

        // Системные строки, которые не удаляем целиком
        if ($this->tableExists('s_user')) {
            $this->stmtall('DELETE FROM s_user WHERE f_id > 1');
        }
        if ($this->tableExists('s_user_group')) {
            $this->stmtall('DELETE FROM s_user_group WHERE f_id > 1');
        }
        if ($this->tableExists('s_db_access')) {
            $this->stmtall('DELETE FROM s_db_access WHERE f_id > 1');
        }
        if ($this->tableExists('s_db')) {
            $this->stmtall('DELETE FROM s_db');
        }
        if ($this->tableExists('c_partners_group')) {
            $this->stmtall('DELETE FROM c_partners_group WHERE f_id > 1');
            $this->stmtall('UPDATE c_partners_group SET f_name = ? WHERE f_id = 1', 's', ['Հիմնական']);
        }
        if ($this->tableExists('c_partners_category')) {
            $this->stmtall('DELETE FROM c_partners_category WHERE f_id > 1');
            $this->stmtall('UPDATE c_partners_category SET f_name = ? WHERE f_id = 1', 's', ['Հիմնական']);
        }
        if ($this->tableExists('c_partners_state')) {
            $this->stmtall('DELETE FROM c_partners_state WHERE f_id > 2');
            $this->stmtall('UPDATE c_partners_state SET f_name = ? WHERE f_id = 1', 's', ['Գործող']);
            $this->stmtall('UPDATE c_partners_state SET f_name = ? WHERE f_id = 2', 's', ['Չգործող']);
        }
        if ($this->tableExists('a_reason')) {
            $this->stmtall('DELETE FROM a_reason WHERE f_id > 10');
        }

        $this->seedDefaults();
        $this->db->query('SET FOREIGN_KEY_CHECKS = 1');
        $this->echoResult();
    }

    private function seedDefaults(): void
    {
        $autoIncrement = [
            'a_store_sale' => 0,
            'c_partners_category' => 1,
            'c_partners_group' => 1,
            'c_partners_state' => 2,
            'o_header_hotel_date' => 0,
            'a_store_reserve' => 0,
            'c_goods_classes' => 0,
            's_login_session' => 0,
            'c_goods_special_prices' => 0,
            'd_part1' => 0,
            'd_part2' => 0,
            'c_goods_option' => 0,
            'o_service_values' => 0,
            's_custom_reports' => 0,
            's_log_store_price' => 0,
            's_user_group' => 1,
            's_user' => 1,
            's_report_template' => 0,
            's_report_template_access' => 0,
            'b_gift_card' => 0,
            'b_gift_card_history' => 0,
            'b_gift_card_sale_options' => 0,
            's_syncronize' => 0,
            'd_package' => 0,
            'd_print_aliases' => 0,
            'c_goods_images' => 0,
            'd_package_list' => 0,
            'd_dish' => 0,
            'e_cash_names' => 0,
            's_settings_names' => 0,
            's_settings_values' => 0,
            'c_goods' => 0,
            'o_package' => 0,
            's_user_photo' => 0,
            'c_groups' => 0,
            'c_goods_complectation' => 0,
            'c_units' => 3,
            'd_menu' => 0,
            'd_image' => 0,
            'd_menu_names' => 0,
            'd_printers' => 0,
            'c_partners' => 0,
            'd_dish_comment' => 0,
            'b_cards_discount' => 0,
            'b_history' => 0,
            'a_reason' => 7,
            'o_dish_remove_reason' => 0,
            's_db' => 1,
            's_db_access' => 1,
            'h_halls' => 0,
            'h_tables' => 0,
            'mf_daily_workers' => 0,
            'mf_daily_process' => 0,
            'mf_process' => 0,
            'mf_actions' => 0,
            'mf_actions_group' => 0,
            'd_special' => 0,
            'c_storages' => 0,
        ];

        foreach ($autoIncrement as $table => $value) {
            if ($this->tableExists($table)) {
                $this->db->query("ALTER TABLE `{$table}` AUTO_INCREMENT = {$value}");
            }
        }

        if ($this->tableExists('d_printers')) {
            $this->stmtall('INSERT INTO d_printers (f_name) VALUES (?)', 's', ['']);
        }
        if ($this->tableExists('s_db')) {
            $this->stmtall(
                'INSERT INTO s_db (f_id, f_name, f_description, f_host, f_db, f_user, f_password) VALUES (1, ?, ?, ?, ?, ?, ?)',
                'ssssss',
                ['DB Name', 'Db Description', '127.0.0.1', 'cafe5', 'root', 'root5']
            );
        }
        if ($this->tableExists('s_db_access')) {
            $this->stmtall('INSERT INTO s_db_access (f_db, f_user, f_permit) VALUES (1, 1, 1)');
        }
        if ($this->tableExists('c_units')) {
            $this->stmtall(
                'INSERT INTO c_units (f_id, f_name, f_fullname) VALUES (1, ?, ?), (2, ?, ?), (3, ?, ?)',
                'ssssss',
                ['Հատ', 'Հատ', 'Կգ', 'Կիլոգրամ', 'Լ', 'Լիտր']
            );
        }
        if ($this->tableExists('s_settings_names')) {
            $this->stmtall('INSERT INTO s_settings_names (f_id, f_name) VALUES (1, ?), (2, ?)', 'ss', ['Main', 'Sale']);
        }
        if ($this->tableExists('s_settings_values')) {
            $this->runBatch([
                "INSERT INTO s_settings_values (f_settings, f_key, f_value) VALUES (1, 97, 'Arial LatArm Unicode'), (1, 28, '12'), (1, 106, ',')",
                "INSERT INTO s_settings_values (f_settings, f_key, f_value) VALUES (2, 97, 'Arial LatArm Unicode'), (2, 28, '12'), (2, 10, 'Ս'), (2, 11, '1'), (2, 12, '1'), (2, 19, '1'), (2, 26, '0'), (2, 31, '1'), (2, 32, '1'), (2, 35, '2'), (2, 36, 'Կ'), (2, 37, 'Ա'), (2, 38, '1'), (2, 52, '1'), (2, 56, 'Շնարհակալություն այցելության համար'), (2, 61, '650'), (2, 64, '1'), (2, 106, ',')",
            ]);
        }
        if ($this->tableExists('e_cash_names')) {
            $this->stmtall('INSERT INTO e_cash_names (f_id, f_name) VALUES (1, ?), (2, ?)', 'ss', ['Կանխիկ', 'Անկանխիկ']);
        }
        if ($this->tableExists('d_menu_names')) {
            $this->stmtall(
                'INSERT INTO d_menu_names (f_id, f_name, f_datestart, f_dateend, f_comment, f_enabled) VALUES (1, ?, CURRENT_DATE(), DATE_ADD(CURRENT_DATE(), INTERVAL 10 YEAR), ?, 1)',
                'ss',
                ['Ճաշացանկ', '']
            );
        }
        if ($this->tableExists('d_part1')) {
            $this->stmtall(
                'INSERT INTO d_part1 (f_id, f_name) VALUES (1, ?), (2, ?), (3, ?)',
                'sss',
                ['Բար', 'Խոհ․', 'Այլ']
            );
        }
        if ($this->tableExists('h_halls')) {
            $this->stmtall(
                'INSERT INTO h_halls (f_id, f_counter, f_name, f_prefix, f_settings, f_counterhall) VALUES (1, 1, ?, ?, 2, 1)',
                'ss',
                ['Սրահ', 'Ս']
            );
        }
        if ($this->tableExists('c_storages')) {
            $this->stmtall('INSERT INTO c_storages (f_id, f_name) VALUES (1, ?)', 's', ['Պահեստ']);
        }
    }
}

if (!empty($params->mode)) {
    $mode = (string) $params->mode;
    $cd = new ClearDB();
    if (!method_exists($cd, $mode)) {
        dieWithCode('Unknown cleardb mode: ' . $mode);
    }
    $cd->{$mode}();
}
