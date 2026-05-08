<?php
#(C) 2025 Kudryashov Vasili
# Last modified - 2025-05-09 14:00:10
require_once __DIR__ . "/worker.php";

class ClearDB extends DB
{
    public function __construct()
    {
        parent::__construct();
    }

    public function storeandsale()
    {
        $sql = <<<EOD
        delete from store_moves;   
        delete from  store_user;
        delete from store_stock;
        delete from store_document;
        
        delete from a_store_sale;
        delete from op_body where f_id='1';
        delete from op_header;
        delete from a_complectation_additions;
        delete from a_header_shop2partner;
        delete from a_header_shop2partneraccept;
        delete from a_store;
        delete from a_store_inventory;
        delete from a_store_draft;
        delete from o_draft_sale_body;
        delete from o_draft_sale;
        delete from o_draft_sound;
        delete from e_cash;
        delete from a_header_store;
        delete from a_header_cash;
        delete from a_header;
        delete from b_history;
        delete from b_clients_debts;
        delete from o_tax_log;
        delete from o_tax_debug;
        delete from o_tax;
        delete from o_payment;
        delete from o_package;
        update o_goods set f_returnfrom = null;

        delete from b_gift_card;
        delete from b_gift_card_history;
        delete from b_gift_card_sale_options;

        delete from o_goods;
        delete from o_body;
        delete from o_preorder;
        delete from o_header_hotel;
        delete from o_header_hotel_date;
        delete from o_header_options;
        delete from o_header_flags;
        delete from o_header;
        delete from o_pay_cl;
        delete from o_pay_room;
        delete from o_additional;
        DELETE FROM s_working_sessions;
        delete from s_web_log;
        DELETE FROM a_result;
        EOD;
        $sqllist = explode(";", $sql);
        foreach ($sqllist as $s) {
            if (empty($s)) {
                continue;
            }
            $this->stmtall($s);
        }
        $this->echoResult();
    }

    public function all()
    {
        $this->storeandsale();
        $sql =  <<<EOD
        
        delete from s_settings_values;
        delete from s_settings_names;
        delete from s_user_photo;
        delete from s_user_access;
        delete from c_partners;
        delete from s_user where f_id>1;
        delete from s_user_group where f_id>1;
        delete from s_db_access where f_id>1;
        delete from s_db;
        delete from b_car;
        delete from b_cards_discount;
        delete from b_history;
        delete from d_menu;
        delete from d_menu_names;
        delete from d_recipes;
        delete from d_dish;
        delete from d_part2;
        delete from c_menu;
        delete from c_goods_option;
        delete from c_goods_multiscancode;
        delete from d_part1;
        delete from d_printers;
        delete from c_goods_images;
        delete from c_goods_special_prices;
        delete from c_goods_complectation;
        delete from c_goods_classes;
        delete from c_goods_prices;
        delete from c_goods;
        delete from c_groups;
        delete from s_waiter_reports;
        delete from s_cache;
        delete from droid_message;
        delete from d_image;
        delete from d_print_aliases;
        delete from o_dish_remove_reason;
        delete from d_dish_comment;
        delete from a_reason where f_id>10;
        delete from h_tables;
        delete from h_halls;
        
        delete from c_partners_group where f_id>1;
        delete from c_partners_category where f_id>1;
        delete from c_partners_state where f_id>2;
        update c_partners_group set f_name='Հիմնական';
        update c_partners_category set f_name ='Հիմնական';
        update c_partners_state set f_name='Գործող' where f_id=1;
        update c_partners_state set f_name='Չգործող' where f_id=2;
        delete from c_units;
        delete from d_special;
        delete from d_package;
        delete from d_package_list;
        delete from d_stoplist;
        delete from s_syncronize;
        delete from e_cash_names;
        delete from a_store_sale;
        delete from a_store_dish_waste;
        delete from o_service_values;
        delete from s_custom_reports;
        delete from s_report_template where f_id>0;
        delete from s_report_template_access where f_report>0;

        delete from mf_daily_workers;
        delete from mf_daily_process;
        delete from mf_process;
        delete from mf_actions;
        delete from mf_actions_group;

        delete from s_log_store_price;

        delete from c_storages;
        alter table a_store_sale auto_increment=0;
        alter table c_partners_category auto_increment=1;
        alter table c_partners_group auto_increment=1;
        alter table c_partners_state auto_increment=2;
        alter table o_header_hotel_date auto_increment=0;
        alter table a_store_reserve auto_increment=0;
        alter table c_goods_classes auto_increment = 0;
        alter table s_login_session auto_increment=0;
        alter table c_goods_special_prices auto_increment=0;
        ALTER TABLE d_part1 AUTO_INCREMENT = 0 ;
        ALTER TABLE d_part2 AUTO_INCREMENT = 0 ;
        alter table c_goods_option auto_increment = 0;
        alter table o_service_values auto_increment = 0;
        alter table s_custom_reports auto_increment = 0;
        alter table s_log_store_price auto_increment = 0;
        alter table s_user_group auto_increment = 1;
        alter table s_user auto_increment = 1;
        alter table s_report_template auto_increment = 0;
        alter table s_report_template_access auto_increment = 0;
        alter table b_gift_card auto_increment = 0;
        alter table b_gift_card_history auto_increment = 0;
        alter table b_gift_card_sale_options auto_increment = 0;

        ALTER TABLE s_syncronize AUTO_INCREMENT = 0 ;
        ALTER TABLE d_package AUTO_INCREMENT = 0 ;
        alter table d_print_aliases auto_increment=0;
        alter table c_goods_images auto_increment=0;
        ALTER TABLE d_package_list AUTO_INCREMENT = 0 ;
        ALTER TABLE d_dish AUTO_INCREMENT = 0 ;
        alter table e_cash_names auto_increment=0;
        ALTER TABLE s_settings_names AUTO_INCREMENT = 0 ;
        ALTER TABLE s_settings_values AUTO_INCREMENT = 0 ;
        ALTER TABLE c_goods AUTO_INCREMENT = 0 ;
        ALTER TABLE o_package AUTO_INCREMENT = 0 ;
        alter table s_user_photo auto_increment=0;
        ALTER TABLE c_groups AUTO_INCREMENT = 0 ;
        ALTER TABLE c_goods_complectation AUTO_INCREMENT = 0 ;
        ALTER TABLE c_units AUTO_INCREMENT = 3 ;
        ALTER TABLE d_menu AUTO_INCREMENT = 0 ;
        ALTER TABLE d_image AUTO_INCREMENT = 0 ;
        ALTER TABLE d_menu_names AUTO_INCREMENT = 0 ;
        ALTER TABLE d_printers AUTO_INCREMENT = 0 ;
        ALTER TABLE c_partners AUTO_INCREMENT = 0 ;
        ALTER TABLE d_dish_comment AUTO_INCREMENT = 0 ;
        ALTER TABLE b_cards_discount AUTO_INCREMENT = 0 ;
        ALTER TABLE b_history AUTO_INCREMENT = 0 ;
        ALTER TABLE a_reason AUTO_INCREMENT = 7 ;
        ALTER TABLE o_dish_remove_reason AUTO_INCREMENT = 0 ;
        ALTER TABLE s_db AUTO_INCREMENT = 1 ;
        ALTER TABLE s_db_access AUTO_INCREMENT = 1 ;
        ALTER TABLE h_halls AUTO_INCREMENT = 0 ;
        ALTER TABLE h_tables AUTO_INCREMENT = 0 ;

        ALTER TABLE mf_daily_workers AUTO_INCREMENT = 0 ;
        ALTER TABLE mf_daily_process AUTO_INCREMENT = 0 ;
        ALTER TABLE mf_process AUTO_INCREMENT = 0 ;
        ALTER TABLE mf_actions AUTO_INCREMENT = 0 ;
        ALTER TABLE mf_actions_group AUTO_INCREMENT = 0 ;

        ALTER TABLE a_store_sale AUTO_INCREMENT = 0 ;
        alter table d_special auto_increment=0;
        ALTER TABLE c_storages AUTO_INCREMENT = 0 ;
        insert into d_printers (f_name) values ('');
        insert into s_db (f_id, f_name, f_description, f_host, f_db, f_user, f_password) values (1, 'DB Name', 'Db Description', '127.0.0.1', 'cafe5', 'root', 'root5');
        insert into s_db_access (f_db, f_user, f_permit) values (1, 1, 1);
        insert into c_units (f_id, f_name, f_fullname) values (1, 'Հատ', 'Հատ'), (2, 'Կգ', 'Կիլոգրամ'), (3, 'Լ', 'Լիտր');
        insert into s_settings_names (f_id, f_name) values (1, 'Main');
        insert into s_settings_names (f_id, f_name) values (2, 'Sale');
        insert into s_settings_values (f_settings, f_key, f_value) values (1, 97, 'Arial LatArm Unicode'), (1, 28, '12'), (1, 106, ',');
        insert into s_settings_values (f_settings, f_key, f_value) values (2, 97, 'Arial LatArm Unicode'), (2, 28, '12'),(2,10,'Ս'),(2,11,'1'),(2,12,'1'),(2,19,'1'),(2,26,'0'),(2,31,'1'),(2,32,'1'),(2,35,'2'),(2,36,'Կ'),(2,37,'Ա'),(2,38,'1'),(2,52,'1'),(2,56,'Շնարհակալություն այցելության համար'),(2,61,'650'),(2,64,'1'),(2, 106, ',');
        insert into e_cash_names (f_id, f_name) values (1, 'Կանխիկ'), (2, 'Անկանխիկ');
        insert into d_menu_names (f_id, f_name, f_datestart, f_dateend, f_comment, f_enabled) values (1, 'Ճաշացանկ', current_date(), date_add(current_date(), interval 10 YEAR), '', 1);
        insert into d_part1 (f_id, f_name) values (1, 'Բար'), (2, 'Խոհ․'), (3, 'Այլ');
        insert into h_halls (f_id, f_counter, f_name, f_prefix, f_settings, f_counterhall) values (1, 1, 'Սրահ', 'Ս', 2, 1);
        insert into c_storages (f_id, f_name) values (1, 'Պահեստ');
        EOD;
        $sqllist = explode(";", $sql);
        foreach ($sqllist as $s) {
            if (empty($s)) {
                continue;
            }
            $this->stmtall($s);
        }
        $this->echoResult();
    }
}

if (!empty($params->mode)) {
    $cd = new ClearDB();
    $cd->{$params->mode}();
}
