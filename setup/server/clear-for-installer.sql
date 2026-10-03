-- Runs only against a temporary copy of picassodev during setup\stage_server.ps1.
-- Same wipe as web.picassoapp/engine/worker/cleardb.php mode "all".
-- The live picassodev database is not modified.

DROP PROCEDURE IF EXISTS clear_if_exists;
DROP PROCEDURE IF EXISTS reset_autoinc;

DELIMITER $$
CREATE PROCEDURE clear_if_exists(IN tname VARCHAR(64))
BEGIN
  IF EXISTS (
    SELECT 1 FROM information_schema.tables
    WHERE table_schema = DATABASE() AND table_name = tname
  ) THEN
    SET @q = CONCAT('DELETE FROM `', tname, '`');
    PREPARE st FROM @q;
    EXECUTE st;
    DEALLOCATE PREPARE st;
  END IF;
END$$

CREATE PROCEDURE reset_autoinc(IN tname VARCHAR(64), IN nextval INT)
BEGIN
  IF EXISTS (
    SELECT 1 FROM information_schema.tables
    WHERE table_schema = DATABASE() AND table_name = tname
  ) THEN
    SET @q = CONCAT('ALTER TABLE `', tname, '` AUTO_INCREMENT = ', nextval);
    PREPARE st FROM @q;
    EXECUTE st;
    DEALLOCATE PREPARE st;
  END IF;
END$$
DELIMITER ;

DROP TRIGGER IF EXISTS prevent_multi_delete;
SET FOREIGN_KEY_CHECKS = 0;

-- store
CALL clear_if_exists('store_calc_queue');
CALL clear_if_exists('store_moves');
CALL clear_if_exists('store_user');
CALL clear_if_exists('store_stock');
CALL clear_if_exists('store_inventory_user');
CALL clear_if_exists('store_inventory_document');
CALL clear_if_exists('store_document');
CALL clear_if_exists('a_store_reserve');
CALL clear_if_exists('a_complectation_additions');
CALL clear_if_exists('a_store_dish_waste');
CALL clear_if_exists('a_store');
CALL clear_if_exists('a_store_draft');
CALL clear_if_exists('a_store_inventory');
CALL clear_if_exists('a_store_sale');
CALL clear_if_exists('a_store_current');
CALL clear_if_exists('a_store_temp');
CALL clear_if_exists('a_header_store');
CALL clear_if_exists('op_body');
CALL clear_if_exists('op_header');
CALL clear_if_exists('s_log_store_price');
DELETE FROM a_header WHERE f_type IN (1, 2, 3, 4, 6, 7);

-- sales
CALL clear_if_exists('cash_debts');
CALL clear_if_exists('cash_operations');
CALL clear_if_exists('cash_session');
CALL clear_if_exists('o_daily_counter');
CALL clear_if_exists('o_goods_process');
CALL clear_if_exists('o_goods_loading');
CALL clear_if_exists('o_draft_sale_body');
CALL clear_if_exists('o_draft_sale');
CALL clear_if_exists('o_draft_sound');
CALL clear_if_exists('a_sale_temp');
CALL clear_if_exists('e_cash');
CALL clear_if_exists('a_header_cash');
CALL clear_if_exists('a_header_shop2partneraccept');
CALL clear_if_exists('a_header_shop2partner');
CALL clear_if_exists('a_header');
CALL clear_if_exists('a_calc_price');
CALL clear_if_exists('a_dc');
CALL clear_if_exists('b_discount_ops');
CALL clear_if_exists('b_accumulate_ops');
CALL clear_if_exists('b_gift_card_ops');
CALL clear_if_exists('b_gift_card_history');
CALL clear_if_exists('b_gift_card_sale_options');
CALL clear_if_exists('b_gift_card');
CALL clear_if_exists('b_history');
CALL clear_if_exists('b_clients_debts');
CALL clear_if_exists('b_car_orders');
CALL clear_if_exists('o_tax_log');
CALL clear_if_exists('o_tax_debug');
CALL clear_if_exists('o_tax');
CALL clear_if_exists('o_payment');
CALL clear_if_exists('o_package');
CALL clear_if_exists('o_goods');
CALL clear_if_exists('o_body');
CALL clear_if_exists('o_preorder');
CALL clear_if_exists('o_header_hotel_date');
CALL clear_if_exists('o_header_hotel');
CALL clear_if_exists('o_header_options');
CALL clear_if_exists('o_header_flags');
CALL clear_if_exists('o_header');
CALL clear_if_exists('o_pay_cl');
CALL clear_if_exists('o_pay_room');
CALL clear_if_exists('o_additional');
CALL clear_if_exists('o_waiterserver_debug');
CALL clear_if_exists('s_working_sessions');
CALL clear_if_exists('s_web_log');
CALL clear_if_exists('s_login_session');
CALL clear_if_exists('s_attendance');
CALL clear_if_exists('s_salary');
CALL clear_if_exists('s_salary_attendance');
CALL clear_if_exists('s_salary_body');
CALL clear_if_exists('s_salary_options');
CALL clear_if_exists('a_result');

-- catalog
CALL clear_if_exists('s_settings_values');
CALL clear_if_exists('s_settings_names');
CALL clear_if_exists('s_user_photo');
CALL clear_if_exists('s_user_access');
CALL clear_if_exists('s_user_fingerprint');
CALL clear_if_exists('s_user_config');
CALL clear_if_exists('c_partners');
CALL clear_if_exists('b_car');
CALL clear_if_exists('b_cards_discount');
CALL clear_if_exists('b_discount_cards');
CALL clear_if_exists('b_accumulate_cards');
CALL clear_if_exists('d_menu');
CALL clear_if_exists('d_menu_names');
CALL clear_if_exists('d_recipes');
CALL clear_if_exists('d_dish');
CALL clear_if_exists('d_part2');
CALL clear_if_exists('c_menu');
CALL clear_if_exists('s_images');
CALL clear_if_exists('c_goods_option');
CALL clear_if_exists('c_goods_multiscancode');
CALL clear_if_exists('c_goods_comment');
CALL clear_if_exists('d_part1');
CALL clear_if_exists('d_printers');
CALL clear_if_exists('c_goods_images');
CALL clear_if_exists('c_goods_special_prices');
CALL clear_if_exists('c_goods_complectation');
CALL clear_if_exists('c_goods_classes');
CALL clear_if_exists('c_goods_prices');
CALL clear_if_exists('c_goods');
CALL clear_if_exists('c_groups');
CALL clear_if_exists('c_stoplist');
CALL clear_if_exists('s_waiter_reports');
CALL clear_if_exists('s_cache');
CALL clear_if_exists('droid_message');
CALL clear_if_exists('d_image');
CALL clear_if_exists('d_print_aliases');
CALL clear_if_exists('o_dish_remove_reason');
CALL clear_if_exists('d_dish_comment');
CALL clear_if_exists('h_tables');
CALL clear_if_exists('h_halls');
CALL clear_if_exists('c_units');
CALL clear_if_exists('d_special');
CALL clear_if_exists('d_package');
CALL clear_if_exists('d_package_list');
CALL clear_if_exists('s_syncronize');
CALL clear_if_exists('s_syncronize_in');
CALL clear_if_exists('e_cash_names');
CALL clear_if_exists('o_service_values');
CALL clear_if_exists('s_custom_reports');
CALL clear_if_exists('s_report_template_access');
CALL clear_if_exists('s_report_template');
CALL clear_if_exists('s_report_template_params');
CALL clear_if_exists('mf_daily_workers');
CALL clear_if_exists('mf_daily_process');
CALL clear_if_exists('mf_process');
CALL clear_if_exists('mf_actions');
CALL clear_if_exists('mf_actions_group');
CALL clear_if_exists('mf_materials');
CALL clear_if_exists('mf_materials_in_actions');
CALL clear_if_exists('mf_tasks');
CALL clear_if_exists('mf_task_stage');
CALL clear_if_exists('mf_task_workshop');
CALL clear_if_exists('mf_stage');
CALL clear_if_exists('m_goal_product_material');
CALL clear_if_exists('m_goal_product');
CALL clear_if_exists('m_goal_product_status');
CALL clear_if_exists('s_activation');
CALL clear_if_exists('files');
CALL clear_if_exists('ararix_restaurants');
CALL clear_if_exists('c_storages');

DELETE FROM s_user WHERE f_id > 1;
DELETE FROM s_user_group WHERE f_id > 1;
DELETE FROM s_db_access WHERE f_id > 1;
DELETE FROM s_db;
DELETE FROM c_partners_group WHERE f_id > 1;
UPDATE c_partners_group SET f_name = 'Հիմնական' WHERE f_id = 1;
DELETE FROM c_partners_category WHERE f_id > 1;
UPDATE c_partners_category SET f_name = 'Հիմնական' WHERE f_id = 1;
DELETE FROM c_partners_state WHERE f_id > 2;
UPDATE c_partners_state SET f_name = 'Գործող' WHERE f_id = 1;
UPDATE c_partners_state SET f_name = 'Չգործող' WHERE f_id = 2;
DELETE FROM a_reason WHERE f_id > 10;

CALL reset_autoinc('a_store_sale', 0);
CALL reset_autoinc('c_partners_category', 1);
CALL reset_autoinc('c_partners_group', 1);
CALL reset_autoinc('c_partners_state', 2);
CALL reset_autoinc('o_header_hotel_date', 0);
CALL reset_autoinc('a_store_reserve', 0);
CALL reset_autoinc('c_goods_classes', 0);
CALL reset_autoinc('s_login_session', 0);
CALL reset_autoinc('c_goods_special_prices', 0);
CALL reset_autoinc('d_part1', 0);
CALL reset_autoinc('d_part2', 0);
CALL reset_autoinc('c_goods_option', 0);
CALL reset_autoinc('o_service_values', 0);
CALL reset_autoinc('s_custom_reports', 0);
CALL reset_autoinc('s_log_store_price', 0);
CALL reset_autoinc('s_user_group', 1);
CALL reset_autoinc('s_user', 1);
CALL reset_autoinc('s_report_template', 0);
CALL reset_autoinc('s_report_template_access', 0);
CALL reset_autoinc('b_gift_card', 0);
CALL reset_autoinc('b_gift_card_history', 0);
CALL reset_autoinc('b_gift_card_sale_options', 0);
CALL reset_autoinc('s_syncronize', 0);
CALL reset_autoinc('d_package', 0);
CALL reset_autoinc('d_print_aliases', 0);
CALL reset_autoinc('c_goods_images', 0);
CALL reset_autoinc('d_package_list', 0);
CALL reset_autoinc('d_dish', 0);
CALL reset_autoinc('e_cash_names', 0);
CALL reset_autoinc('s_settings_names', 0);
CALL reset_autoinc('s_settings_values', 0);
CALL reset_autoinc('c_goods', 0);
CALL reset_autoinc('o_package', 0);
CALL reset_autoinc('s_user_photo', 0);
CALL reset_autoinc('c_groups', 0);
CALL reset_autoinc('c_goods_complectation', 0);
CALL reset_autoinc('c_units', 3);
CALL reset_autoinc('d_menu', 0);
CALL reset_autoinc('d_image', 0);
CALL reset_autoinc('d_menu_names', 0);
CALL reset_autoinc('d_printers', 0);
CALL reset_autoinc('c_partners', 0);
CALL reset_autoinc('d_dish_comment', 0);
CALL reset_autoinc('b_cards_discount', 0);
CALL reset_autoinc('b_history', 0);
CALL reset_autoinc('a_reason', 7);
CALL reset_autoinc('o_dish_remove_reason', 0);
CALL reset_autoinc('s_db', 1);
CALL reset_autoinc('s_db_access', 1);
CALL reset_autoinc('h_halls', 0);
CALL reset_autoinc('h_tables', 0);
CALL reset_autoinc('mf_daily_workers', 0);
CALL reset_autoinc('mf_daily_process', 0);
CALL reset_autoinc('mf_process', 0);
CALL reset_autoinc('mf_actions', 0);
CALL reset_autoinc('mf_actions_group', 0);
CALL reset_autoinc('d_special', 0);
CALL reset_autoinc('c_storages', 0);

INSERT INTO d_printers (f_name) VALUES ('');
INSERT INTO s_db (f_id, f_name, f_description, f_host, f_db, f_user, f_password)
VALUES (1, 'picasso', 'Picasso', '127.0.0.1', 'picasso', 'root', 'root5');
INSERT INTO s_db_access (f_db, f_user, f_permit) VALUES (1, 1, 1);
INSERT INTO c_units (f_id, f_name, f_fullname) VALUES
  (1, 'Հատ', 'Հատ'),
  (2, 'Կգ', 'Կիլոգրամ'),
  (3, 'Լ', 'Լիտր');
INSERT INTO s_settings_names (f_id, f_name) VALUES (1, 'Main'), (2, 'Sale');
INSERT INTO s_settings_values (f_settings, f_key, f_value) VALUES
  (1, 97, 'Arial LatArm Unicode'), (1, 28, '12'), (1, 106, ',');
INSERT INTO s_settings_values (f_settings, f_key, f_value) VALUES
  (2, 97, 'Arial LatArm Unicode'), (2, 28, '12'), (2, 10, 'Ս'), (2, 11, '1'),
  (2, 12, '1'), (2, 19, '1'), (2, 26, '0'), (2, 31, '1'), (2, 32, '1'),
  (2, 35, '2'), (2, 36, 'Կ'), (2, 37, 'Ա'), (2, 38, '1'), (2, 52, '1'),
  (2, 56, 'Շնարհակալություն այցելության համար'), (2, 61, '650'), (2, 64, '1'), (2, 106, ',');
INSERT INTO e_cash_names (f_id, f_name) VALUES (1, 'Կանխիկ'), (2, 'Անկանխիկ');
INSERT INTO d_menu_names (f_id, f_name, f_datestart, f_dateend, f_comment, f_enabled)
VALUES (1, 'Ճաշացանկ', CURRENT_DATE(), DATE_ADD(CURRENT_DATE(), INTERVAL 10 YEAR), '', 1);
INSERT INTO d_part1 (f_id, f_name) VALUES (1, 'Բար'), (2, 'Խոհ․'), (3, 'Այլ');
INSERT INTO h_halls (f_id, f_counter, f_name, f_prefix, f_settings, f_counterhall)
VALUES (1, 1, 'Սրահ', 'Ս', 2, 1);
INSERT INTO c_storages (f_id, f_name) VALUES (1, 'Պահեստ');

SET FOREIGN_KEY_CHECKS = 1;
DROP PROCEDURE IF EXISTS clear_if_exists;
DROP PROCEDURE IF EXISTS reset_autoinc;
