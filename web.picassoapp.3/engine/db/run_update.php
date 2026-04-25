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



echo json_encode([
    "status" => "ok",
    "database" => $dbname,
    "database_version_updated" => "$update_verision => $v",
    "skipped_files" => $skippedFiles,
    "executed_files" => $executedFiles,
    "error_files" => $errorFiles,
    "message" => "Updates completed successfully."
]);
