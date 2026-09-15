<?php
# @2025 Kudryashov Vasili 
# Created 2025-06-07 23:18:15
# Last modified 2025-12-12 14:13:54

define('APP', true);

require_once __DIR__ . '/../config.php';
require_once __DIR__ . '/../functions/db.php';

#update stored procedures
$folder = __DIR__ . '/sp';
$pdo = new PDO("mysql:host=127.0.0.1;dbname={$dbname};charset=utf8", $dbuser, $dbpass);

$skippedFiles = [];
$executedFiles = [];
$errorFiles = [];
foreach (glob("$folder/*.sql") as $filePath) {
    $lines = file($filePath, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
    if (!$lines || strpos(trim($lines[0]), '-- draft') === 0) {
        $skippedFiles[] = $filePath;
        continue;
    }

    $s = file_get_contents($filePath);
    $s = preg_replace('/^delimiter\s+\$\$/mi', '', $s);
    $s = preg_replace('/^delimiter\s+;/mi', '', $s);
    $s = str_replace("DELIMITER $$", "", $s);
    $statements = explode('$$', $s);

    foreach ($statements as $stmt) {
        $stmt = trim($stmt);
        if (!$stmt) continue;

        try {
            $pdo->exec($stmt);
        } catch (PDOException $e) {
            $errorFiles[] = "$filePath: " . $e->getMessage() . "<br>";
        }
    }
    $executedFiles[] = "$filePath ";
}

$db->begin_transaction();

$v = 199;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table h_halls add column f_shift int default 0;
EOD;
$v = 200;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    create table o_goods_loading (f_id int primary key auto_increment, f_date date, f_store int, f_goods int, f_qty float(12,2), f_emarks tinytext);
    alter table o_goods_loading add column f_time time after f_date;
    alter table o_goods_loading add column f_state int after f_time, ADD column f_removed timestamp default current_timestamp on update current_timestamp after f_state;
EOD;

$v = 201;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE `o_body` CHANGE COLUMN `f_emarks` `f_emarks` VARCHAR(128) NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci' AFTER `f_reprint`;
EOD;

$v = 202;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table o_goods add f_returnedqty float default 0;
EOD;

$v = 203;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table b_history add column f_date date;
    insert into b_card_types values (9, 'Կանխիկացում');
EOD;

$v = 204;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table s_activation add column f_user int;
EOD;

$v = 205;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table s_user add column f_phone varchar(16);
    create table m_goal_product (f_id integer primary key auto_increment, f_date date, f_status int, f_name tinytext, f_code tinytext, f_width float, f_height float, f_34 int, f_36 int, f_38 int, f_40 int, f_42 int, f_44 int, f_46 int);
    create table m_goal_product_status (f_id integer primary key auto_increment, f_name tinytext);
    INSERT INTO `m_goal_product_status` (`f_id`, `f_name`) VALUES (1, 'Նոր');
    ALTER TABLE `o_body` ADD INDEX `fk_header_state` (`f_state`, `f_header`);
    ALTER TABLE `o_header` ADD INDEX `idx_table_state` (`f_table`, `f_state`);
    ALTER TABLE `o_tax_log` ADD INDEX `idx_taxlog_header` (`f_order`);
    ALTER TABLE `o_tax_log` ADD INDEX `idx_taxlog_header_state` (`f_order`, `f_state`);
    ALTER TABLE `d_recipes` CHANGE COLUMN `f_price` `f_price` FLOAT NULL DEFAULT NULL AFTER `f_qty`;
    INSERT INTO `o_flags` (`f_id`) VALUES (3);
    INSERT INTO `o_flags` (`f_id`) VALUES (4);
    INSERT INTO `o_flags` (`f_id`) VALUES (5);
    UPDATE `o_flags` SET `f_name`='-', `f_enabled`=0, `f_field`='f_3' WHERE  `f_id`=3;
    UPDATE `o_flags` SET `f_name`='-', `f_enabled`=0, `f_field`='f_4' WHERE  `f_id`=4;
    UPDATE `o_flags` SET `f_name`='Կարճ պարտք', `f_enabled`=0, `f_field`='f_5' WHERE  `f_id`=5;
    ALTER TABLE `o_flags` CHANGE COLUMN `f_id` `f_id` INT NOT NULL DEFAULT 0 FIRST, CHANGE COLUMN `f_enabled` `f_enabled` INT NULL DEFAULT NULL AFTER `f_name`;
EOD;

$v = 206;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE `b_gift_card_history` ADD COLUMN `f_date` DATE NULL AFTER `f_amount`;
    ALTER TABLE `b_history` CHANGE COLUMN `f_value` `f_value` FLOAT NULL DEFAULT NULL AFTER `f_card`, CHANGE COLUMN `f_data` `f_data` FLOAT NULL DEFAULT NULL AFTER `f_value`;
    ALTER TABLE `b_gift_card_history` ADD COLUMN `f_type` INT NULL AFTER `f_date`;
    create table mf_materials (f_id integer primary key auto_increment, f_name tinytext);
    create table mf_materials_in_actions (f_id integer primary key auto_increment, f_action int, f_material int, f_code char(8), f_qty float, f_comment tinytext);
    alter table m_goal_product drop column f_name, drop column f_code, add column f_product int after f_date;
    create table m_goal_product_material (f_id integer primary key auto_increment, f_product int, f_material int, f_code char(16), f_color char(16), f_qty1 float, f_qty2 float, f_totalqty float, f_qtyperone float, f_colorqty float);
EOD;

$v = 207;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table m_goal_product_material add column f_row int;
    alter table m_goal_product_material add column f_reason int;
    create  table m_goal_product_reason (f_id integer primary key auto_increment, f_name tinytext);
    insert into m_goal_product_reason (f_id, f_name) values (1, 'Գործող'), (2, 'Բռակ'), (3, 'Ուսադկա'), (4, 'Տոչկա'), (5, 'Պակաս');
    insert m_goal_product_status (f_id, f_name) values (2, 'Ավարտած');
    insert m_goal_product_status (f_id, f_name) VALUES (3, 'Չեղարկված');
    ALTER TABLE `m_goal_product` ADD COLUMN `f_image_url` TINYTEXT NULL AFTER `f_46`;
EOD;

$v = 208;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table c_goods add column f_production int default  0;
    ALTER TABLE `c_goods` ADD INDEX `idx_uk_production` (`f_production`);
    drop table mf_materials;
EOD;

$v = 209;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table d_dish add column f_dailyqty float;
    alter table c_goods add column f_autodiscount varchar(16);
    alter table b_cards_discount add column f_number varchar(16);
EOD;


$v = 210;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table o_draft_sale_body add f_emarks varchar(36) collate latin1_general_ci;
    ALTER TABLE `o_draft_sale_body` ADD UNIQUE INDEX `uq_state_emarks` (`f_state`, `f_emarks`);
EOD;


$v = 211;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE `s_user_state` CHANGE COLUMN `f_id` `f_id` INT NOT NULL DEFAULT 0 FIRST;
    alter table mf_actions_group add column f_goods_group int ;
    ALTER TABLE `mf_actions_group` CHANGE COLUMN `f_goods_group` `f_goods_group` VARCHAR(8) NULL DEFAULT NULL AFTER `f_data`;
EOD;

$v = 212;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE b_card_types ADD COLUMN f_pattern VARCHAR(16);
    INSERT INTO `b_card_types` (`f_id`, `f_name`) VALUES (10, 'Նվեր քարտ');
EOD;

$v = 213;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    delete from m_goal_product_status;
    insert m_goal_product_status (f_id, f_name) VALUES (1, 'Նոր'), (2, 'Ձևարան'), (3, 'Արտադրամաս'), (4, 'Գիծ 1'), (5, 'Գիծ 2'), (6, 'Ավարտաց');
    alter table m_goal_product_material add f_parent int after f_product;
    alter table b_history add column f_comment tinytext;
EOD;


$v = 214;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE `c_goods` CHANGE COLUMN `f_model` `f_type` INT(11) NULL DEFAULT NULL AFTER `f_id`;
    ALTER TABLE `c_goods` CHANGE COLUMN `f_group` `f_group` INT(11) NULL DEFAULT NULL AFTER `f_type`;
    alter table c_groups add column f_color int;
    alter table c_groups add column f_parent int after f_id;
    CREATE TABLE `c_menu_names` (
	`f_id` INT(11) NOT NULL AUTO_INCREMENT,
	`f_name` VARCHAR(32) NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci',
	`f_dateStart` DATE NULL DEFAULT NULL,
	`f_dateEnd` DATE NULL DEFAULT NULL,
	`f_comment` VARCHAR(128) NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci',
	`f_enabled` INT(11) NULL DEFAULT NULL,
	PRIMARY KEY (`f_id`) USING BTREE
)
COLLATE='utf8mb3_general_ci'
ENGINE=InnoDB
AUTO_INCREMENT=3
;

CREATE TABLE `c_menu` (
	`f_id` INT(11) NOT NULL AUTO_INCREMENT,
	`f_menu` INT(11) NULL DEFAULT NULL,
	`f_dish` INT(11) NULL DEFAULT NULL,
	`f_price` FLOAT NULL DEFAULT NULL,
	`f_store` INT(11) NULL DEFAULT NULL,
	`f_print1` TINYTEXT NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci',
	`f_print2` TINYTEXT NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci',
	`f_state` INT(11) NULL DEFAULT NULL,
	`f_recent` TINYINT(4) NULL DEFAULT '0',
	PRIMARY KEY (`f_id`) USING BTREE,
	INDEX `fk_cmenu_menu_idx` (`f_menu`) USING BTREE,
	INDEX `fk_cmenu_dish_idx` (`f_dish`) USING BTREE,
	INDEX `fk_cmenu_store_idx` (`f_store`) USING BTREE,
	CONSTRAINT `fk_cmenu_dish` FOREIGN KEY (`f_dish`) REFERENCES `c_goods` (`f_id`) ON UPDATE RESTRICT ON DELETE RESTRICT,
	CONSTRAINT `fk_cmenu_menu` FOREIGN KEY (`f_menu`) REFERENCES `c_menu_names` (`f_id`) ON UPDATE RESTRICT ON DELETE RESTRICT,
	CONSTRAINT `fk_cmenu_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`) ON UPDATE RESTRICT ON DELETE RESTRICT
)
COLLATE='utf8mb3_general_ci'
ENGINE=InnoDB
AUTO_INCREMENT=2036
;
alter table c_goods add f_color int;
alter table c_goods add f_emark_required int;
INSERT INTO `a_type` (`f_name`, `f_counter`) VALUES ('Վաճառք', 0);
CREATE TABLE `c_stoplist` (
	`f_dish` INT(11) NOT NULL,
	`f_qty` FLOAT NULL DEFAULT NULL,
	PRIMARY KEY (`f_dish`) USING BTREE
)
COLLATE='utf8mb3_general_ci'
ENGINE=InnoDB
;
alter table o_goods add column f_type int after f_header, ADD column f_guest int;
create table o_goods_type (f_id integer primary key auto_increment, f_name tinytext);
alter table o_goods add column f_data json;
alter table o_header add column f_data json;
alter table o_goods add column f_parent int after f_type;
alter table o_goods add column f_state int after f_header;
create table c_goods_type (f_id integer primary key);
create table l_languages (f_id integer primary key, f_name tinytext, f_short char(2));
create table l_dictionary (f_id integer primary key auto_increment, f_dict varchar(16), f_dict_id int, f_lang char(2), f_value char(64));
update c_goods set f_type=1;
create table c_goods_comment (f_id integer primary key auto_increment, f_name tinytext);
alter table c_goods add column f_data json;
ALTER TABLE `o_service_values` CHANGE COLUMN `f_value` `f_value` FLOAT (5,3) NULL DEFAULT NULL AFTER `f_id` ;
EOD;

$v = 215;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE `l_dictionary` CHANGE COLUMN `f_dict` `f_dict` VARCHAR(32) NULL DEFAULT NULL COLLATE 'utf8mb4_general_ci' AFTER `f_id`;
    CREATE TABLE workstations (f_id INTEGER PRIMARY KEY AUTO_INCREMENT,f_type INT NULL COMMENT '1 - waiter\n2 - smart\n3 - office\n4 - shop',f_station_account TINYTEXT NULL,f_name TINYTEXT,f_config JSON);
    create table workstations_type (f_id integer primary key);
    insert into workstations_type(f_id) values (1),(2),(3),(4);
    insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values ('workstations_type', 1, 'hy', 'Ռեստորան'), ('workstations_type', 1, 'hy', 'Արագ սնունդ'), ('workstations_type', 1, 'hy', 'Մենեջեր'), ('workstations_type', 1, 'hy', 'Խանութ');
    create table cash_box  (f_id integer primary key auto_increment, f_name tinytext);
    create table cash_session (f_id integer primary key auto_increment, f_state int, f_cashbox_id int, f_user_open int, f_user_close int, f_date_open datetime, f_date_close datetime, f_amount_open decimal(12,2), f_amount_fact decimal(12,2), f_amount_expected decimal(12,2), f_amount_difference decimal(12,2));
    create table cash_operations (f_id integer primary key auto_increment, f_session_id int, f_order_id char(36) collate latin1_general_ci, f_user int, f_operation_type int, f_payment_type_id int, f_datetime datetime, f_amount decimal(12,2));
    ALTER TABLE `o_header` CHANGE COLUMN `f_prefix` `f_prefix` VARCHAR(16) NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci' AFTER `f_hallid`;
    alter table o_header add column f_cash_session_id int after f_state;
    DELIMITER $$

    CREATE FUNCTION money_fmt(val DECIMAL(12,2))
    RETURNS VARCHAR(32)
    DETERMINISTIC
    BEGIN
        RETURN TRIM(TRAILING '.00' FROM FORMAT(val, 2));
    END$$


    CREATE FUNCTION date_fmt(d DATETIME)
    RETURNS VARCHAR(10)
    DETERMINISTIC
    BEGIN
        RETURN DATE_FORMAT(d, '%d/%m/%Y');
    END$$


    CREATE FUNCTION datetime_fmt(d DATETIME, mode INT)
    RETURNS VARCHAR(20)
    DETERMINISTIC
    BEGIN
        RETURN CASE
            WHEN mode = 1 THEN DATE_FORMAT(d, '%d/%m/%Y')
            WHEN mode = 2 THEN DATE_FORMAT(d, '%H:%i')
            ELSE DATE_FORMAT(d, '%d/%m/%Y %H:%i')
        END;
    END$$

    DELIMITER ;
EOD;

$v = 216;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE `s_salary_payment` ADD COLUMN `f_comment` TINYTEXT NULL AFTER `f_amount`;
    ALTER TABLE `s_salary_attendance` ADD COLUMN `f_comment` TINYTEXT NULL ;
    insert into c_partners_category (f_id) values (2), (3);
    alter table c_partners change column f_legaladdress f_legal_address varchar(255);
    insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values ('c_partners_category', 1, 'hy', 'Ընդհանուր'), ('c_partners_category', 2, 'hy', 'Գնորդ'),('c_partners_category', 3, 'hy', 'Մատակարար');
    insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values ('c_partners_state', 1, 'hy', 'Գործող'), ('c_partners_state', 2, 'hy', 'Չգործող');
    alter table b_gift_card add column f_valid_until date;
    create table fiscal_machine (f_id integer primary key auto_increment, f_name varchar(255), f_ip varchar(24), f_port int, f_password varchar(16), f_op_pin varchar(16), f_op_pass varchar(16), f_external_pos int, f_default_dept int default 1);
EOD;


$v = 217;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    create table store_document (f_id char(36) primary key collate latin1_general_ci, f_user_id char(8), f_status integer, f_doc_type int, f_doc_date datetime, 
        f_store_in int, f_store_out int, f_sum decimal(14,2), f_partner int, f_version int, f_data json);
    create table store_user (f_id char(36) collate latin1_general_ci not null, f_doc char(36) collate latin1_general_ci, f_item_id integer, f_qty decimal(14,4), f_price decimal(14,2), f_total decimal(14,2), 
        f_comment varchar(255),
        primary key (f_id),
        index idx_doc (f_doc));
    create table store_stock (f_id char(36) collate latin1_general_ci not null, f_doc char(36) collate latin1_general_ci, f_doc_row_id char(36) collate latin1_general_ci, f_batch_date datetime, f_expiry_date datetime,
        f_store_id integer, f_item_id integer, f_qty_in decimal(14,4) not null, f_qty_left decimal(14, 4) not null, f_price decimal(14,2),   
        primary key (f_id),
        index idx_fifo (f_store_id, f_item_id, f_qty_left, f_batch_date));
    create table store_moves (f_id char(36) collate latin1_general_ci not null, f_doc char(36) collate latin1_general_ci, f_doc_row_id char(36) collate latin1_general_ci not null, f_batch_id char(36) collate latin1_general_ci not null, 
        f_store_id integer, f_item_id integer, f_qty_in decimal(14,4) not null default 0, f_qty_out decimal(14,4) not null default 0, f_price decimal(14,2), f_total decimal(14,2),
        primary key (f_id),
        index idx_move(f_store_id, f_item_id),
        index idx_batch (f_batch_id),
        index idx_doc_row (f_doc_row_id));
    create table store_statuses (f_id integer primary key);
    insert into store_statuses (f_id) values (0), (1);
    insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values ('store_statuses', 0, 'hy', 'Սևագիր'),  ('store_statuses', 1, 'hy', 'Գրանցված');
    create table store_types (f_id integer primary key);
    insert into store_types (f_id) values (1), (2), (3), (4), (5);
    insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values 
        ('store_types', 1, 'hy', 'Պահեստի մուտք'),  
        ('store_types', 2, 'hy', 'Պահեստի ելք'), 
        ('store_types', 3, 'hy', 'Պահեստի տեղաշարժ'),
        ('store_types', 4, 'hy', 'Կոմպլեկտավորում'),
        ('store_types', 5, 'hy', 'Ապակոմպլեկտավորում');
    create table store_calc_queue (
        f_id char(36) primary key collate latin1_general_ci,
        f_doc_store_out_id char(36),
        f_doc_sale_id char(36),
        f_row_sale_id char(36),           
        f_store_id integer,               
        f_item_id integer,                
        f_qty decimal(14,4),             
        f_status integer default 0,      
        f_create_date datetime default now(),
        index calc_idx_item_store (f_item_id, f_store_id, f_status), index calc_idx_store_out(f_doc_store_out_id), index calc_idx_sale (f_doc_sale_id)
    );
    alter table store_user add column f_row int;
    alter table store_calc_queue add f_price decimal(14,2);
EOD;


$v = 218;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    create table store_inventory_document (f_id char(36) collate latin1_general_ci, f_date datetime, f_store int, f_data json, f_create_user int, f_create_date datetime);
    create table store_inventory_user (f_id char(36) collate latin1_general_ci, f_doc char(36) collate latin1_general_ci, f_row int, f_item int, f_qty_sys decimal(14,3), f_qty_user decimal(14,3), f_price decimal(14,2));
EOD;

$v = 219;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    insert into workstations_type (f_id) values (5), (6);
    insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values  ('workstations_type', 6, 'hy', 'Ավտոլվացում'),('workstations_type', 5, 'hy', 'Ընդհանուր');
    ALTER TABLE `l_dictionary` ADD UNIQUE INDEX `uq_dict_and_id` (`f_dict`, `f_dict_id`, `f_lang`);
EOD;

$v = 220;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table c_groups add f_image char(36) collate latin1_general_ci;
    update h_halls set f_prefix=left(f_prefix, 1);
    alter table h_halls change column f_prefix f_prefix char(1);
    create table o_daily_counter (f_id integer primary key auto_increment, f_cash_session_id int not null, f_department char(1) not null, f_counter int default 0, UNIQUE KEY `idx_date_dept` (`f_cash_session_id`, `f_department`));
    create table o_goods_process (f_id char(36) primary key collate latin1_general_ci, f_header char(36) collate latin1_general_ci, f_status int, f_daily_number char(8), f_data json, 
    index idx_goods_process_status (f_status),  index idx_goods_process_header (f_header));

EOD;

$v = 221;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
   ALTER TABLE `c_goods_multiscancode` ADD CONSTRAINT `fk_multi_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`) ON UPDATE NO ACTION ON DELETE NO ACTION;
   create table s_salary (f_id integer primary key auto_increment, f_date date, f_staff int, f_position int, f_amount_credit decimal(14,2), f_amount_debit decimal(14,2));
   alter table b_clients_debts add column f_credit decimal(24,2) not null default '0.00' after f_amount, add column f_debit decimal(24,2) not null default '0.00' after f_credit;
   alter table cash_operations add column f_cashbox_id int after f_id;
   create table cash_payment_types (f_id int primary key);
   insert into cash_payment_types (f_id) values (1),(2),(3),(4),(7);
   insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values
   ('cash_payment_types', 1, 'hy', 'Կանխիկ'), 
   ('cash_payment_types', 2, 'hy', 'Քարտ'),
   ('cash_payment_types', 3, 'hy', 'Փոխանցում'),
   ('cash_payment_types', 4, 'hy', 'Իդրամ'),
   ('cash_payment_types', 7, 'hy', 'ԹելՍել');
   create table cash_debts (f_id integer primary key auto_increment, f_date date, f_partner int, f_doc_type int, f_doc_uuid char(36) collate latin1_general_ci, f_credit decimal(14,2), f_debit decimal(14,2), f_currency_id int);

   insert into c_goods_type (f_id) values (1), (2), (3), (4), (5), (6);
    insert into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values
   ('c_goods_type', 1, 'hy', 'Ապրանք'), 
   ('c_goods_type', 2, 'hy', 'Ուտեստ'),
   ('c_goods_type', 3, 'hy', 'Ծառայություն'),
   ('c_goods_type', 4, 'hy', 'Մոդիֆիկատոր'),
   ('c_goods_type', 5, 'hy', 'Փաթեթ');
   alter table cash_operations add column f_currency_id int default '1', add column f_comment varchar(255);

   ALTER TABLE `cash_operations` 
    DROP COLUMN `f_amount`,
    ADD COLUMN `f_debit` DECIMAL(12,2) NOT NULL DEFAULT '0.00',
    ADD COLUMN `f_credit` DECIMAL(12,2) NOT NULL DEFAULT '0.00';
EOD;

$v = 222;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE `o_goods` CHANGE COLUMN `f_parent` `f_parent` CHAR(36) NULL DEFAULT NULL COLLATE 'latin1_general_ci' AFTER `f_type`;
    ALTER TABLE `s_salary` ADD COLUMN `f_type` INT NULL AFTER `f_id`;
    CREATE TABLE `s_attendance` (
    `f_id` INT(11) NOT NULL AUTO_INCREMENT,
    `f_date` DATE NOT NULL,
    `f_worker` INT(11) NOT NULL,
    `f_position` INT(11) NOT NULL,
    `f_in` DATETIME NOT NULL,
    `f_out` DATETIME NULL DEFAULT NULL,
    `f_break_minutes` INT(11) NOT NULL DEFAULT '0', -- Сумма всех перерывов
    `f_state` TINYINT(1) NOT NULL DEFAULT '0',
    `f_comment` VARCHAR(500) NULL DEFAULT NULL,
    PRIMARY KEY (`f_id`),
    INDEX `idx_worker_date` (`f_worker`, `f_date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
EOD;

$v = 223;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table o_service_values add column f_comment varchar(64);
    alter table s_salary add column f_fixed decimal(14,2) after f_position, add column f_calculated decimal(14,2) after f_fixed, add column f_bonus decimal(14,2) after f_calculated;
EOD;

$v = 224;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    alter table s_user_group add column f_data json, drop column f_starttime, drop column f_duration;
    CREATE table cash_operations_types (f_id integer primary key auto_increment);
    ALTER TABLE cash_operations_types ADD COLUMN f_comment VARCHAR(32);
EOD;

$v = 225;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    INSERT INTO cash_operations_types (f_id, f_comment) VALUES
    (1, 'Sales Revenue'),
    (2, 'Total Expenses'),
    (3, 'Purchasing Costs'),
    (4, 'Salaries and Wages'),
    (5, 'Debt Recovery'),
    (6, 'Debt Repayment'),
    (7, 'Utilities'),
    (8, 'Cash Shortage'),
    (9, 'Cash Overage')
    ON DUPLICATE KEY UPDATE f_comment = VALUES(f_comment);
    UPDATE cash_operations SET f_operation_type = 5 WHERE f_operation_type = 101;
    UPDATE cash_operations SET f_operation_type = 2 WHERE f_operation_type = 102;
EOD;

$v = 226;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
INSERT INTO `l_dictionary` (`f_dict`, `f_dict_id`, `f_lang`, `f_value`) VALUES ('c_goods_type', 6, 'hy', 'Փաթեթի անդամ');
EOD;

$v = 227;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    INSERT INTO cash_operations_types (f_id, f_comment) VALUES
    (10, 'Delivery Fee')
    ON DUPLICATE KEY UPDATE f_comment = VALUES(f_comment);
EOD;

$v = 228;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    INSERT INTO cash_operations_types (f_id, f_comment) VALUES
    (11, 'Transfer Out'),
    (12, 'Transfer In')
    ON DUPLICATE KEY UPDATE f_comment = VALUES(f_comment);
EOD;

$v = 229;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    UPDATE cash_operations
    SET f_credit = f_debit, f_debit = 0
    WHERE (f_order_id IS NULL OR TRIM(f_order_id) = '')
      AND f_operation_type IN (2, 6, 7, 8)
      AND f_debit > 0
      AND f_credit <= 0;
EOD;

$v = 230;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    CREATE TABLE s_user_fingerprint (
  f_user      INT PRIMARY KEY,
  f_template  MEDIUMBLOB NOT NULL,
  f_size      INT NOT NULL,
  f_updated   DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  FOREIGN KEY (f_user) REFERENCES s_user(f_id)
);
EOD;

$v = 231;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    create table b_discount_cards (f_id integer primary key auto_increment, f_code varchar(64), f_client int, f_value float, f_mode int, f_datestart date, f_dateend date, f_number varchar(16), f_active int default 1, index idx_discount_cards_code (f_code));
    create table b_accumulate_cards (f_id integer primary key auto_increment, f_code varchar(64), f_client int, f_value float, f_datestart date, f_dateend date, f_number varchar(16), f_active int default 1, index idx_accumulate_cards_code (f_code));
    create table b_discount_ops (f_id char(36) primary key collate latin1_general_ci, f_order_id char(36) collate latin1_general_ci, f_card_id int null, f_partner_id int null, f_type int, f_factor float, f_amount decimal(14,2), f_date datetime default current_timestamp, f_comment varchar(255), index idx_discount_ops_order (f_order_id), index idx_discount_ops_card (f_card_id));
    create table b_gift_card_ops (f_id char(36) primary key collate latin1_general_ci, f_card_id int, f_order_id char(36) collate latin1_general_ci null, f_amount decimal(14,2), f_op_type varchar(16), f_date datetime default current_timestamp, f_comment varchar(255), index idx_gift_card_ops_card (f_card_id), index idx_gift_card_ops_order (f_order_id));
    create table b_accumulate_ops (f_id char(36) primary key collate latin1_general_ci, f_card_id int, f_order_id char(36) collate latin1_general_ci null, f_amount decimal(14,2), f_op_type varchar(16), f_percent float, f_date datetime default current_timestamp, f_comment varchar(255), index idx_accumulate_ops_card (f_card_id), index idx_accumulate_ops_order (f_order_id));
    insert into b_discount_cards (f_id, f_code, f_client, f_value, f_mode, f_datestart, f_dateend, f_number, f_active) select f_id, f_code, f_client, f_value, f_mode, f_datestart, f_dateend, f_number, f_active from b_cards_discount where coalesce(f_mode, 0) <> 4;
    insert into b_accumulate_cards (f_id, f_code, f_client, f_value, f_datestart, f_dateend, f_number, f_active) select f_id, f_code, f_client, f_value, f_datestart, f_dateend, f_number, f_active from b_cards_discount where f_mode = 4;
    insert into b_gift_card_ops (f_id, f_card_id, f_order_id, f_amount, f_op_type, f_date, f_comment) select uuid(), b.f_id, null, coalesce(bh.f_sum, 0), 'migrate', current_timestamp, 'opening balance' from b_gift_card b inner join (select f_card, sum(f_amount) as f_sum from b_gift_card_history group by f_card) bh on bh.f_card = b.f_id where coalesce(bh.f_sum, 0) <> 0;
    insert into b_accumulate_ops (f_id, f_card_id, f_order_id, f_amount, f_op_type, f_percent, f_date, f_comment) select uuid(), c.f_id, null, coalesce(bh.f_sum, 0), 'migrate', c.f_value, current_timestamp, 'opening balance' from b_accumulate_cards c inner join (select f_card, sum(f_amount) as f_sum from b_gift_card_history group by f_card) bh on bh.f_card = c.f_id where coalesce(bh.f_sum, 0) <> 0;
EOD;

$v = 232;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    insert ignore into c_partners_category (f_id, f_name) values (1, 'General'), (2, 'Buyer'), (3, 'Supplier');
    insert ignore into c_partners_group (f_id, f_name) values (1, 'Main'), (2, 'Retail'), (3, 'Shop');
    insert ignore into l_dictionary (f_dict, f_dict_id, f_lang, f_value) values
        ('c_partners_category', 1, 'hy', 'Ընդհանուր'),
        ('c_partners_category', 2, 'hy', 'Գնորդ'),
        ('c_partners_category', 3, 'hy', 'Մատակարար');
EOD;

$v = 233;
#dont forget update store2_input.sql
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
   create table sys_data (f_id int primary key auto_increment, f_type int, f_data json,KEY idx_sys_data_type (f_type));
   alter table store_document change column f_user_id f_user_id char(16);
   ALTER TABLE store_calc_queue
  ADD INDEX calc_idx_row_sale (f_row_sale_id);
EOD;

$v = 234;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
   ALTER TABLE store_calc_queue ADD INDEX calc_idx_row_sale (f_row_sale_id);
EOD;

$v = 235;
$sql[$v] = <<<EOD
    update s_app set f_version = '$v' where lower(f_app)='db';
    ALTER TABLE o_tax_log ADD COLUMN f_fiscal_machine_id INT NULL;
EOD;

$v = 236;
$sql[$v] = <<<EOD
update s_app set f_version = '$v' where lower(f_app)='db';
INSERT INTO `l_dictionary` (`f_dict`, `f_dict_id`, `f_lang`, `f_value`) VALUES
('workstations_type', 1, 'ru', 'Ресторан'),
('workstations_type', 2, 'ru', 'Фастфуд'),
('workstations_type', 3, 'ru', 'Менеджер'),
('workstations_type', 4, 'ru', 'Магазин'),
('workstations_type', 5, 'ru', 'Общий'),
('workstations_type', 6, 'ru', 'Автомойка'),

('store_types', 1, 'ru', 'Приход на склад'),
('store_types', 2, 'ru', 'Расход со склада'),
('store_types', 3, 'ru', 'Перемещение'),
('store_types', 4, 'ru', 'Комплектация'),
('store_types', 5, 'ru', 'Разукомплектация'),

('store_statuses', 0, 'ru', 'Черновик'),
('store_statuses', 1, 'ru', 'Проведён'),

('c_partners_state', 1, 'ru', 'Действующий'),
('c_partners_state', 2, 'ru', 'Недействующий'),

('c_partners_category', 1, 'ru', 'Общий'),
('c_partners_category', 2, 'ru', 'Покупатель'),
('c_partners_category', 3, 'ru', 'Поставщик'),

('c_goods_type', 1, 'ru', 'Товар'),
('c_goods_type', 2, 'ru', 'Блюдо'),
('c_goods_type', 3, 'ru', 'Услуга'),
('c_goods_type', 4, 'ru', 'Модификатор'),
('c_goods_type', 5, 'ru', 'Пакет'),
('c_goods_type', 6, 'ru', 'Член пакета'),

('cash_payment_types', 1, 'ru', 'Наличные'),
('cash_payment_types', 2, 'ru', 'Карта'),
('cash_payment_types', 3, 'ru', 'Перевод'),
('cash_payment_types', 4, 'ru', 'Идрам'),
('cash_payment_types', 7, 'ru', 'ТелСелл'),

('cash_operations_types', 1, 'ru', 'Приход от продаж'),
('cash_operations_types', 2, 'ru', 'Общие расходы'),
('cash_operations_types', 3, 'ru', 'Закупки'),
('cash_operations_types', 4, 'ru', 'Зарплата'),
('cash_operations_types', 5, 'ru', 'Погашение долга'),
('cash_operations_types', 6, 'ru', 'Погашение долга клиента'),
('cash_operations_types', 7, 'ru', 'Коммунальные'),
('cash_operations_types', 8, 'ru', 'Недостача'),
('cash_operations_types', 9, 'ru', 'Излишек'),
('cash_operations_types', 10, 'ru', 'Доставка'),
('cash_operations_types', 11, 'ru', 'Расход, перемещение'),
('cash_operations_types', 12, 'ru', 'Приход, перемещение');
EOD;


$v = 237;
$sql[$v] = <<<EOD
update s_app set f_version = '$v' where lower(f_app)='db';
create table if not exists c_goods_country(f_id integer primary key);

EOD;

$v = 238;
$sql[$v] = <<<EOD
update s_app set f_version = '$v' where lower(f_app)='db';

create table if not exists ararix_restaurants (
    f_id int primary key auto_increment,
    f_name varchar(128) not null,
    f_score int not null default 0,
    f_location point null,
    f_image_url varchar(255) null,
    f_logo_url varchar(255) null,
    f_category varchar(128) null,
    f_eta_min int not null default 55,
    f_db varchar(64) null
);

create table if not exists ararix_goods_groups (
    f_id int primary key auto_increment,
    f_name varchar(128) not null,
    f_image_url varchar(255) null,
    f_sort int not null default 0
);

create table if not exists ararix_goods_country (
    f_id int primary key auto_increment,
    f_name varchar(128) not null,
    f_sort int not null default 0
);

insert ignore into ararix_goods_groups (f_id, f_name, f_sort) values
(1, 'Burger', 1),
(2, 'Pizza', 2);

insert ignore into ararix_goods_country (f_id, f_name, f_sort) values
(1, 'Asian', 1),
(2, 'Caucasus', 2),
(3, 'Europian', 3),
(4, 'Mexican', 4),
(5, 'Japan', 5);

insert into ararix_restaurants (f_name, f_score, f_location, f_category)
select 'McDonald''s', 100, ST_GeomFromText('POINT(44.5121 40.1872)'), 'Fast food'
from dual where not exists (select 1 from ararix_restaurants where f_name = 'McDonald''s');

insert into ararix_restaurants (f_name, f_score, f_location, f_category)
select 'KFC', 90, ST_GeomFromText('POINT(44.5140 40.1850)'), 'Fast food'
from dual where not exists (select 1 from ararix_restaurants where f_name = 'KFC');

insert into ararix_restaurants (f_name, f_score, f_location, f_category)
select 'Pizza Lab', 95, ST_GeomFromText('POINT(44.5100 40.1890)'), 'Pizzeria / Mexican'
from dual where not exists (select 1 from ararix_restaurants where f_name = 'Pizza Lab');

EOD;

$v = 239;
$sql[$v] = <<<EOD
update s_app set f_version = '$v' where lower(f_app)='db';
alter table cash_debts add column if not exists f_comment varchar(255) not null default '';

EOD;

$v = 240;
$sql[$v] = <<<EOD
update s_app set f_version = '$v' where lower(f_app)='db';
alter table ararix_restaurants add column if not exists f_logo_url varchar(255) null after f_image_url;
alter table ararix_restaurants add column if not exists f_eta_min int not null default 55 after f_category;
alter table ararix_restaurants add column if not exists f_db varchar(64) null after f_eta_min;

EOD;

$v = 241;
$sql[$v] = <<<EOD
update s_app set f_version = '$v' where lower(f_app)='db';
CREATE TABLE IF NOT EXISTS ararix_restaurant_nationality (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_name VARCHAR(128) NOT NULL,
    f_sort INT NOT NULL DEFAULT 0
);
INSERT IGNORE INTO ararix_restaurant_nationality (f_id, f_name, f_sort) VALUES
    (1, 'Չինական', 1),
    (2, 'Ճապոնական', 2),
    (3, 'Հնդկական', 3),
    (4, 'Եվրոպական', 4),
    (5, 'Իտալական', 5),
    (6, 'Հայկական', 6),
    (7, 'Վրացական', 7),
    (8, 'Լիբանանյան', 8),
    (9, 'Արաբական', 9),
    (10, 'Թուրքական', 10),
    (11, 'Պարսկական', 11),
    (12, 'Միջերկրածովյան', 12),
    (13, 'Ամերիկյան', 13),
    (14, 'Մեքսիկական', 14);
ALTER TABLE ararix_restaurants ADD COLUMN IF NOT EXISTS f_nationality_id INT NULL AFTER f_category;
CREATE INDEX IF NOT EXISTS idx_ararix_restaurants_nationality ON ararix_restaurants (f_nationality_id);

CREATE TABLE IF NOT EXISTS ararix_restaurant_groups (
    f_restaurant_id INT NOT NULL,
    f_group_id INT NOT NULL,
    f_sort INT NOT NULL DEFAULT 0,
    PRIMARY KEY (f_restaurant_id, f_group_id),
    INDEX idx_ararix_rg_group (f_group_id)
);

CREATE TABLE IF NOT EXISTS ararix_menu (
    f_id INT PRIMARY KEY AUTO_INCREMENT,
    f_restaurant_id INT NOT NULL,
    f_group_id INT NOT NULL,
    f_name VARCHAR(128) NOT NULL,
    f_description TEXT NULL,
    f_price DECIMAL(12,2) NOT NULL DEFAULT 0,
    f_image_url VARCHAR(255) NULL,
    f_state TINYINT NOT NULL DEFAULT 1,
    f_sort INT NOT NULL DEFAULT 0,
    f_source_goods_id INT NULL,
    f_source_db VARCHAR(64) NULL,
    f_data JSON NULL,
    INDEX idx_ararix_menu_rest_state (f_restaurant_id, f_state),
    INDEX idx_ararix_menu_group (f_group_id)
);

INSERT INTO ararix_goods_groups (f_id, f_name, f_sort) VALUES
    (1, 'Բուրգեր', 1),
    (2, 'Պիցցա', 2),
    (3, 'Սուշի', 3),
    (4, 'Սթեյք', 4),
    (5, 'BBQ', 5),
    (6, 'Ծովամթերք', 6),
    (7, 'Ֆասթֆուդ', 7),
    (8, 'Street Food', 8),
    (9, 'Վեգան', 9),
    (10, 'Վեգետարիանական', 10),
    (11, 'Առողջ սնունդ', 11),
    (12, 'Դեսերտներ / Հացաբուլկեղեն', 12),
    (13, 'Սրճարան / Coffee & Dessert', 13)
ON DUPLICATE KEY UPDATE f_name = VALUES(f_name), f_sort = VALUES(f_sort);

EOD;

$update_verision = intval(stmtall("select * from s_app where lower(f_app)='db'")->fetch_assoc()["f_version"]);
for ($i = $update_verision + 1; $i <= $v; $i++) {
    if (isset($sql[$i])) {
        $sqls = explode(';', $sql[$i]);
        foreach ($sqls as $sss) {
            try {
                stmtall($sss);
                $executedFiles[] = "Update version $i executed successfully.<br>";
            } catch (Exception $e) {
                $errorFiles[] = "Error executing update version $i: " . $e->getMessage() . "<br>";
            }
        }
    } else {
        $skippedFiles[] = "Update version $i skipped, no SQL found.<br>";
    }
}

// Idempotent menu seed (safe if v241 already applied earlier).
$seedFile = __DIR__ . '/seed_ararix_menu.sql';
if (is_file($seedFile)) {
    $seedSql = file_get_contents($seedFile);
    foreach (explode(';', $seedSql) as $sss) {
        $sss = preg_replace('/^\s*--.*$/m', '', $sss);
        $sss = trim($sss);
        if ($sss === '') {
            continue;
        }
        try {
            stmtall($sss);
        } catch (Exception $e) {
            $errorFiles[] = "Error in seed_ararix_menu.sql: " . $e->getMessage() . "<br>";
        }
    }
    $executedFiles[] = "seed_ararix_menu.sql checked.<br>";
}

echo json_encode([
    "status" => "ok",
    "database" => $dbname,
    "database_version_updated" => "$update_verision => $v",
    "skipped_files" => $skippedFiles,
    "executed_files" => $executedFiles,
    "error_files" => $errorFiles,
    "message" => "Updates completed successfully."
]);
