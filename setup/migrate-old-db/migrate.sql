-- Old restaurant DB (d_part1/d_part2/d_dish/d_menu, DB version 198)
-- into an empty DB that already has the current schema (DB version 249).
-- Placeholders: {SRC} old database, {DST} new database, {TPL} template database for dictionaries.
-- Order history, cash movements and store documents are not migrated.
SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;
SET SESSION sql_mode = 'NO_ENGINE_SUBSTITUTION';

-- System dictionaries from the template DB
INSERT INTO {DST}.a_reason SELECT * FROM {TPL}.a_reason;
INSERT INTO {DST}.a_state SELECT * FROM {TPL}.a_state;
INSERT INTO {DST}.a_type SELECT * FROM {TPL}.a_type;
UPDATE {DST}.a_type t JOIN {SRC}.a_type s ON s.f_id = t.f_id SET t.f_counter = s.f_counter;
INSERT INTO {DST}.a_type_sign SELECT * FROM {TPL}.a_type_sign;
INSERT INTO {DST}.a_store_reserve_state SELECT * FROM {TPL}.a_store_reserve_state;
INSERT INTO {DST}.b_card_types SELECT * FROM {TPL}.b_card_types;
INSERT INTO {DST}.cash_operations_types SELECT * FROM {TPL}.cash_operations_types;
INSERT INTO {DST}.cash_payment_types SELECT * FROM {TPL}.cash_payment_types;
INSERT INTO {DST}.c_goods_type SELECT * FROM {TPL}.c_goods_type;
INSERT INTO {DST}.c_partners_category SELECT * FROM {TPL}.c_partners_category;
INSERT INTO {DST}.c_partners_state SELECT * FROM {TPL}.c_partners_state;
INSERT INTO {DST}.c_storages_state SELECT * FROM {TPL}.c_storages_state;
INSERT INTO {DST}.d_dish_state SELECT * FROM {TPL}.d_dish_state;
INSERT INTO {DST}.l_languages SELECT * FROM {TPL}.l_languages;
INSERT INTO {DST}.l_dictionary SELECT * FROM {TPL}.l_dictionary;
INSERT INTO {DST}.s_languages SELECT * FROM {TPL}.s_languages;
INSERT INTO {DST}.mf_actions_state SELECT * FROM {TPL}.mf_actions_state;
INSERT INTO {DST}.mf_tasks_state SELECT * FROM {TPL}.mf_tasks_state;
INSERT INTO {DST}.m_goal_product_reason SELECT * FROM {TPL}.m_goal_product_reason;
INSERT INTO {DST}.m_goal_product_status SELECT * FROM {TPL}.m_goal_product_status;
INSERT INTO {DST}.o_body_state SELECT * FROM {TPL}.o_body_state;
INSERT INTO {DST}.o_draft_sale_state SELECT * FROM {TPL}.o_draft_sale_state;
INSERT INTO {DST}.o_flags SELECT * FROM {TPL}.o_flags;
INSERT INTO {DST}.o_goods_return_reason SELECT * FROM {TPL}.o_goods_return_reason;
INSERT INTO {DST}.o_meal_plan SELECT * FROM {TPL}.o_meal_plan;
INSERT INTO {DST}.o_sale_type SELECT * FROM {TPL}.o_sale_type;
INSERT INTO {DST}.o_state SELECT * FROM {TPL}.o_state;
INSERT INTO {DST}.o_vat_mode SELECT * FROM {TPL}.o_vat_mode;
INSERT INTO {DST}.op_header_state SELECT * FROM {TPL}.op_header_state;
INSERT INTO {DST}.p_preorder_state SELECT * FROM {TPL}.p_preorder_state;
INSERT INTO {DST}.store_statuses SELECT * FROM {TPL}.store_statuses;
INSERT INTO {DST}.store_types SELECT * FROM {TPL}.store_types;
INSERT INTO {DST}.s_user_state SELECT * FROM {TPL}.s_user_state;
INSERT INTO {DST}.workstations_type SELECT * FROM {TPL}.workstations_type;
INSERT INTO {DST}.sys_cache_tables SELECT * FROM {TPL}.sys_cache_tables;
INSERT INTO {DST}.s_translator SELECT * FROM {TPL}.s_translator;
INSERT INTO {DST}.s_app SELECT * FROM {TPL}.s_app;
UPDATE {DST}.s_app SET f_version = '1' WHERE lower(f_app) = 'menu';

-- Venue reference data
INSERT INTO {DST}.e_currency SELECT * FROM {SRC}.e_currency;
INSERT INTO {DST}.c_units (f_id, f_name, f_defaultqty, f_fullname) SELECT f_id, f_name, f_defaultqty, f_fullname FROM {SRC}.c_units;
INSERT INTO {DST}.c_storages SELECT * FROM {SRC}.c_storages;
INSERT INTO {DST}.d_printers SELECT * FROM {SRC}.d_printers;
INSERT INTO {DST}.o_credit_card SELECT * FROM {SRC}.o_credit_card;
INSERT INTO {DST}.o_service_values (f_id, f_value) SELECT f_id, f_value FROM {SRC}.o_service_values;
INSERT INTO {DST}.o_dish_remove_reason SELECT * FROM {SRC}.o_dish_remove_reason;

INSERT INTO {DST}.cash_box (f_id, f_name, f_currency_id)
SELECT f_id, f_name, COALESCE(f_currency, 1) FROM {SRC}.e_cash_names;

INSERT INTO {DST}.h_halls (f_id, f_name, f_counter, f_prefix, f_settings, f_counterhall, f_onlinereport, f_shift)
SELECT f_id, f_name, f_counter, f_prefix, f_settings, f_counterhall, f_onlinereport, 0 FROM {SRC}.h_halls;
INSERT INTO {DST}.h_tables (f_id, f_hall, f_name, f_lock, f_locksrc, f_special_config, f_locktime, f_hourly)
SELECT f_id, f_hall, f_name, 0, NULL, f_special_config, NULL, f_hourly FROM {SRC}.h_tables;
INSERT INTO {DST}.sys_json_config SELECT * FROM {SRC}.sys_json_config;
INSERT INTO {DST}.s_settings_names SELECT * FROM {SRC}.s_settings_names;
INSERT INTO {DST}.s_settings_values SELECT * FROM {SRC}.s_settings_values;

-- Users and permissions
INSERT INTO {DST}.s_user_group (f_id, f_name, lu) SELECT f_id, f_name, lu FROM {SRC}.s_user_group;
INSERT INTO {DST}.s_user (f_id, f_group, f_teamlead, f_state, f_first, f_last, f_login, f_password, f_altpassword, f_config, f_passhash, f_firebase_token, lu)
SELECT f_id, f_group, f_teamlead, f_state, f_first, f_last, f_login, f_password, f_altpassword, f_config, f_passhash, f_firebase_token, lu FROM {SRC}.s_user;
INSERT INTO {DST}.s_user_access SELECT * FROM {SRC}.s_user_access;
INSERT INTO {DST}.s_user_config SELECT * FROM {SRC}.s_user_config;

-- Partners
INSERT INTO {DST}.c_partners_group SELECT * FROM {SRC}.c_partners_group;
INSERT INTO {DST}.c_partners (f_id, f_category, f_state, f_group, f_taxname, f_name, f_taxcode, f_contact, f_info, f_phone, f_email, f_address, f_legal_address, f_store, f_cash, f_permanent_discount, f_price_polyitic, f_price_politic, f_manager)
SELECT f_id, COALESCE(f_category, 1), f_state, f_group, f_taxname, f_name, f_taxcode, f_contact, f_info, f_phone, f_email, f_address, f_legaladdress, f_store, f_cash, f_permanent_discount, f_price_polyitic, f_price_politic, f_manager FROM {SRC}.c_partners;

-- Menu: d_part2 -> c_groups (d_part1 becomes c_groups.f_class)
INSERT INTO {DST}.c_groups (f_id, f_parent, f_name, f_taxdept, f_adgcode, f_order, f_class, f_color)
SELECT f_id, NULLIF(f_parent, 0), f_name, f_taxdept, f_adgCode, f_queue, f_part, NULLIF(NULLIF(f_color, -1), 0)
FROM {SRC}.d_part2;

-- d_dish -> c_goods with new ids
CREATE TABLE {DST}._migrate_dish_map (
    f_old INT PRIMARY KEY,
    f_new INT NOT NULL AUTO_INCREMENT,
    UNIQUE KEY (f_new)
);
INSERT INTO {DST}._migrate_dish_map (f_old) SELECT f_id FROM {SRC}.d_dish ORDER BY f_id;

INSERT INTO {DST}.c_goods (f_id, f_type, f_group, f_unit, f_name, f_saleprice, f_lastinputprice, f_enabled, f_scancode,
                           f_service, f_adg, f_queue, f_candiscount, f_color, f_data)
SELECT mp.f_new, 1, NULLIF(d.f_part, 0), 1, TRIM(d.f_name),
       (SELECT MAX(m.f_price) FROM {SRC}.d_menu m WHERE m.f_dish = d.f_id AND m.f_state = 1),
       d.f_cost, 1, NULL,
       0, NULLIF(TRIM(d.f_adgt), ''), d.f_queue, IF(d.f_discount = 1, 1, 0), NULLIF(NULLIF(d.f_color, -1), 0),
       JSON_OBJECT('f_count_service', d.f_service = 1, 'f_count_discount', d.f_discount = 1)
FROM {SRC}.d_dish d
JOIN {DST}._migrate_dish_map mp ON mp.f_old = d.f_id;

-- Barcodes: one code may sit on several old dishes; the lowest old id keeps it as primary
CREATE TABLE {DST}._migrate_codes (
    f_dish INT NOT NULL,
    f_code VARCHAR(64) NOT NULL,
    f_pos INT NOT NULL,
    KEY (f_code),
    KEY (f_dish)
);
INSERT INTO {DST}._migrate_codes (f_dish, f_code, f_pos)
WITH RECURSIVE codes AS (
    SELECT d.f_id AS f_dish,
           TRIM(SUBSTRING_INDEX(d.f_barcode, ',', 1)) AS f_code,
           IF(LOCATE(',', d.f_barcode) > 0, SUBSTRING(d.f_barcode, LOCATE(',', d.f_barcode) + 1), '') AS f_rest,
           1 AS f_pos
    FROM {SRC}.d_dish d
    WHERE COALESCE(TRIM(d.f_barcode), '') <> ''
    UNION ALL
    SELECT f_dish,
           TRIM(SUBSTRING_INDEX(f_rest, ',', 1)),
           IF(LOCATE(',', f_rest) > 0, SUBSTRING(f_rest, LOCATE(',', f_rest) + 1), ''),
           f_pos + 1
    FROM codes
    WHERE f_rest <> ''
)
SELECT mp.f_new, c.f_code, c.f_pos
FROM codes c
JOIN {DST}._migrate_dish_map mp ON mp.f_old = c.f_dish
WHERE c.f_code <> '' AND CHAR_LENGTH(c.f_code) <= 32;

UPDATE {DST}.c_goods g
JOIN (
    SELECT f_dish, f_code
    FROM (
        SELECT f_dish, f_code, ROW_NUMBER() OVER (PARTITION BY f_dish ORDER BY f_pos) AS rn
        FROM (
            SELECT f_dish, f_code, f_pos,
                   ROW_NUMBER() OVER (PARTITION BY f_code ORDER BY f_dish, f_pos) AS rc
            FROM {DST}._migrate_codes
        ) owners
        WHERE rc = 1
    ) firsts
    WHERE rn = 1
) p ON p.f_dish = g.f_id
SET g.f_scancode = p.f_code;

INSERT IGNORE INTO {DST}.c_goods_multiscancode (f_id, f_goods)
SELECT c.f_code, c.f_dish
FROM {DST}._migrate_codes c
WHERE NOT EXISTS (SELECT 1 FROM {DST}.c_goods g WHERE g.f_scancode = c.f_code)
ORDER BY c.f_dish, c.f_pos;

DROP TABLE {DST}._migrate_codes;

INSERT INTO {DST}.c_menu_names (f_id, f_name, f_dateStart, f_dateEnd, f_comment, f_enabled)
SELECT f_id, f_name, f_dateStart, f_dateEnd, f_comment, f_enabled FROM {SRC}.d_menu_names;

INSERT INTO {DST}.c_menu (f_menu, f_dish, f_price, f_store, f_print1, f_print2, f_state, f_recent)
SELECT m.f_menu, mp.f_new, m.f_price, NULLIF(m.f_store, 0), m.f_print1, m.f_print2, m.f_state, m.f_recent
FROM {SRC}.d_menu m
JOIN {DST}._migrate_dish_map mp ON mp.f_old = m.f_dish
ORDER BY m.f_id;

-- Dish comments (shared list in the new schema)
INSERT INTO {DST}.c_goods_comment (f_name)
SELECT DISTINCT TRIM(f_name) FROM {SRC}.d_dish_comment WHERE COALESCE(TRIM(f_name), '') <> '';

-- Packages: d_package -> c_goods type 5 + c_goods_complectation + c_menu
CREATE TABLE {DST}._migrate_package_map (
    f_old INT PRIMARY KEY,
    f_new INT NOT NULL
);
SET @pkg_base = (SELECT COALESCE(MAX(f_id), 0) FROM {DST}.c_goods);
INSERT INTO {DST}._migrate_package_map (f_old, f_new)
SELECT f_id, @pkg_base + ROW_NUMBER() OVER (ORDER BY f_id) FROM {SRC}.d_package;

INSERT INTO {DST}.c_goods (f_id, f_type, f_group, f_unit, f_name, f_saleprice, f_enabled, f_service, f_queue, f_data)
SELECT pm.f_new, 5,
       (SELECT d.f_part FROM {SRC}.d_package_list pl JOIN {SRC}.d_dish d ON d.f_id = pl.f_dish
         WHERE pl.f_package = p.f_id ORDER BY pl.f_id LIMIT 1),
       1, p.f_name, p.f_price, p.f_enabled, 0, 0,
       JSON_OBJECT('f_count_service', FALSE, 'f_count_discount', FALSE)
FROM {SRC}.d_package p
JOIN {DST}._migrate_package_map pm ON pm.f_old = p.f_id;

INSERT INTO {DST}.c_goods_complectation (f_base, f_goods, f_qty, f_price)
SELECT pm.f_new, mp.f_new, pl.f_qty, pl.f_price
FROM {SRC}.d_package_list pl
JOIN {DST}._migrate_package_map pm ON pm.f_old = pl.f_package
JOIN {DST}._migrate_dish_map mp ON mp.f_old = pl.f_dish;

INSERT INTO {DST}.c_menu (f_menu, f_dish, f_price, f_store, f_print1, f_print2, f_state, f_recent)
SELECT p.f_menu, pm.f_new, p.f_price,
       (SELECT NULLIF(pl.f_store, 0) FROM {SRC}.d_package_list pl WHERE pl.f_package = p.f_id ORDER BY pl.f_id LIMIT 1),
       (SELECT pl.f_printer FROM {SRC}.d_package_list pl WHERE pl.f_package = p.f_id ORDER BY pl.f_id LIMIT 1),
       '', IF(p.f_enabled = 1, 1, 0), 0
FROM {SRC}.d_package p
JOIN {DST}._migrate_package_map pm ON pm.f_old = p.f_id;

-- Stop list
INSERT INTO {DST}.c_stoplist (f_dish, f_qty)
SELECT mp.f_new, s.f_qty FROM {SRC}.d_stoplist s JOIN {DST}._migrate_dish_map mp ON mp.f_old = s.f_dish;

DROP TABLE {DST}._migrate_package_map;
DROP TABLE {DST}._migrate_dish_map;

SET FOREIGN_KEY_CHECKS = 1;
