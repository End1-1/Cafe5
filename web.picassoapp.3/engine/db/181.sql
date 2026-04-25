#13/02/2025
UPDATE s_app SET f_version='181' WHERE f_app='DB';
ALTER TABLE `s_working_sessions`	CHANGE COLUMN `f_open` `f_open` TIMESTAMP NOT NULL DEFAULT current_timestamp() AFTER `f_id`;
ALTER TABLE c_goods_images ADD COLUMN f_size INTEGER;

#19/02/2025
UPDATE s_app SET f_version='182' WHERE f_app='DB';
ALTER TABLE mf_daily_process MODIFY f_laststep INT;
ALTER TABLE mf_daily_workers MODIFY f_checked INT;
ALTER TABLE mf_tasks MODIFY f_state INT;
ALTER TABLE o_goods MODIFY f_return INT, MODIFY f_row INT, MODIFY f_discountmode INT, MODIFY f_sign INT;
INSERT INTO o_goods_return_reason (f_id, f_name) VALUES (0, '-');
ALTER TABLE `o_goods`	ADD CONSTRAINT `fk_ogoods_return_reason` FOREIGN KEY (`f_return`) REFERENCES `o_goods_return_reason` (`f_id`) ON UPDATE NO ACTION ON DELETE NO ACTION;
UPDATE o_goods SET f_returnfrom=NULL WHERE LENGTH(f_returnfrom)=0;
UPDATE  o_goods SET f_returnfrom=null WHERE LENGTH(f_returnfrom)>0 AND f_returnfrom NOT IN (SELECT f_id FROM o_goods);
ALTER TABLE `o_goods`	CHANGE COLUMN `f_adgcode` `f_adgcode` TINYTEXT NULL DEFAULT NULL COLLATE 'latin1_general_ci' AFTER `f_taxdept`,	CHANGE COLUMN `f_returnfrom` `f_returnfrom` CHAR(36) NULL DEFAULT NULL COLLATE 'latin1_general_ci' AFTER `f_return`;
ALTER TABLE `o_goods`	ADD CONSTRAINT `fk_ogoods_return` FOREIGN KEY (`f_returnfrom`) REFERENCES `o_goods` (`f_id`) ON UPDATE NO ACTION ON DELETE NO ACTION;
ALTER TABLE `o_header`	ADD INDEX `idx_oheader_date` (`f_datecash`);


#21/02/2025
UPDATE s_app SET f_version='183' WHERE f_app='DB';
ALTER TABLE e_cash MODIFY f_row INT;
ALTER TABLE e_currency MODIFY f_rate FLOAT;
ALTER TABLE e_currency_cross_rate MODIFY f_rate FLOAT;
ALTER TABLE e_currency_cross_rate_history MODIFY f_rate FLOAT;
ALTER TABLE a_store_reserve MODIFY f_state INT, MODIFY f_prepaid FLOAT, MODIFY f_prepaidcard FLOAT;
ALTER TABLE a_store_sale MODIFY f_qtyreserve FLOAT, MODIFY f_qtyprogram FLOAT;
ALTER TABLE a_store_temp MODIFY f_qty FLOAT;
ALTER TABLE a_store_current MODIFY f_state INT, MODIFY f_originalqty FLOAT, MODIFY f_currentqty FLOAT, MODIFY f_price FLOAT;


#23/02/2025
UPDATE s_app SET f_version='184' WHERE f_app='DB';
ALTER TABLE a_store_draft MODIFY f_type INT;
ALTER TABLE `a_store`	DROP FOREIGN KEY `fk_store_type`;
ALTER TABLE `a_store`	DROP INDEX `fk_store_type_idx`;
ALTER TABLE a_type_sign MODIFY f_id INT;
ALTER TABLE a_store MODIFY f_type INT;
ALTER TABLE `a_store`	ADD CONSTRAINT `fk_astore_ftype` FOREIGN KEY (`f_type`) REFERENCES `a_type_sign` (`f_id`) ON UPDATE NO ACTION ON DELETE NO ACTION;


#25/02/2025
UPDATE s_app SET f_version='185' WHERE f_app='DB';
ALTER TABLE a_header MODIFY f_payment INT;
ALTER TABLE a_header_cash MODIFY f_related INT;
ALTER TABLE a_header_store MODIFY f_baseonsale INT;
ALTER TABLE b_history MODIFY f_type INT;
ALTER TABLE c_goods_scancode MODIFY f_receipt INT;
ALTER TABLE d_menu MODIFY f_state INT;
ALTER TABLE d_menu_names MODIFY f_enabled INT;
ALTER TABLE d_translator MODIFY f_mode INT;
ALTER TABLE h_tables MODIFY f_lock INT;
ALTER TABLE o_body MODIFY f_row INT, modify f_remind INT;
ALTER TABLE o_header_flags MODIFY f_1 INT, MODIFY f_2 INT, MODIFY f_3 INT, MODIFY f_4 INT, MODIFY f_5 INT;
ALTER TABLE s_db_access MODIFY f_permit INT;
ALTER TABLE c_groups MODIFY f_taxdept INT;
ALTER TABLE c_goods MODIFY f_saleprice FLOAT, MODIFY f_saleprice2 FLOAT, MODIFY f_storeinputbeforesale INT, MODIFY f_component_exit INT;

#26/02/2025
UPDATE s_app SET f_version='186' WHERE f_app='DB';
ALTER TABLE b_clients_debts MODIFY f_amount FLOAT;
ALTER TABLE `e_cash_names`	ADD CONSTRAINT `fk_cash_names_currency` FOREIGN KEY (`f_currency`) REFERENCES `e_currency` (`f_id`) ON UPDATE NO ACTION ON DELETE NO ACTION;


#27/02/2025
UPDATE s_app SET f_version='187' WHERE f_app='DB';
ALTER TABLE `b_gift_card_history` CHANGE COLUMN `f_trsale` `f_trsale` VARCHAR(36) NULL DEFAULT NULL COLLATE 'latin1_general_ci' AFTER `f_card`,	ADD CONSTRAINT `fk_bcardhistory_trsale` FOREIGN KEY (`f_trsale`) REFERENCES `o_header` (`f_id`) ON UPDATE NO ACTION ON DELETE NO ACTION;


#09/03/2025
UPDATE s_app SET f_version='188' WHERE f_app='DB';
ALTER TABLE a_store_sale MODIFY f_qtyreserve FLOAT, MODIFY f_qtyprogram FLOAT;

#18/03/2025
UPDATE s_app SET f_version='189' WHERE f_app='DB';
ALTER TABLE a_store_draft MODIFY f_qty FLOAT(12,4);
ALTER TABLE a_store MODIFY f_qty FLOAT(12,4);

#20/03/2025
UPDATE s_app SET f_version='190' WHERE f_app='DB';
ALTER TABLE d_package MODIFY f_enabled INT;
ALTER TABLE c_goods_images ADD f_bigimage LONGTEXT AFTER f_image, ADD f_bigimagesize INT;


#25/03/2025
UPDATE s_app SET f_version='191' WHERE f_app='DB';
ALTER TABLE c_partners DROP f_price_polytic;
ALTER TABLE c_partners MODIFY f_permanent_discount FLOAT (6,3);

#29/03/2025
UPDATE s_app SET f_version='192' WHERE f_app='DB';
CREATE TABLE s_web_log (f_id INTEGER PRIMARY KEY AUTO_INCREMENT, f_date DATE DEFAULT current_date, f_time TIME DEFAULT current_time,  f_session CHAR(36), f_user INT, f_path TINYTEXT, f_params LONGTEXT);
ALTER TABLE o_draft_sale_body ADD COLUMN f_row INT;


#31/03/2025
UPDATE s_app SET f_version='193' WHERE f_app='DB';
ALTER TABLE o_header_flags ADD COLUMN f_store_check INT;
ALTER TABLE o_goods MODIFY f_price FLOAT (12,2);

#03/04/2025
UPDATE s_app SET f_version='194' WHERE f_app='DB';
CREATE TABLE o_qr (f_id INT PRIMARY KEY AUTO_INCREMENT, f_header CHAR(36) , f_qr TEXT);
CREATE TABLE `s_web_log` (
	`f_id` INT(11) NOT NULL AUTO_INCREMENT,
	`f_date` DATE NULL DEFAULT curdate(),
	`f_time` TIME NULL DEFAULT curtime(),
	`f_session` CHAR(36) NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci',
	`f_user` INT(11) NULL DEFAULT NULL,
	`f_path` TINYTEXT NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci',
	`f_params` LONGTEXT NULL DEFAULT NULL COLLATE 'utf8mb3_general_ci',
	PRIMARY KEY (`f_id`) USING BTREE
) COLLATE='utf8mb3_general_ci' ENGINE=InnoDB AUTO_INCREMENT=2026;





#08/04/2025
UPDATE s_app SET f_version='195' WHERE f_app='DB';
ALTER TABLE o_draft_sale ADD COLUMN f_hall INT;

#24/04/2025
UPDATE s_app SET f_version='196' WHERE f_app='DB';
ALTER TABLE a_header MODIFY f_payment CHAR(36) COLLATE LATIN1_GENERAL_CI;
UPDATE a_header SET f_payment =NULL;
UPDATE a_header
JOIN a_header_cash ON a_header_cash.f_storedoc = a_header.f_id
SET a_header.f_payment = a_header_cash.f_id
WHERE LENGTH(COALESCE(a_header_cash.f_storedoc, '')) > 0;
ALTER TABLE `a_header_cash`	ADD INDEX `idx_hc_storedoc` (`f_storedoc`);


#26/04/2025
UPDATE s_app SET f_version='197' WHERE f_app='DB';
ALTER TABLE `a_store`	ADD INDEX `idx_store_goodsstore` (`f_store`, `f_goods`);

#19/05/2025
UPDATE s_app SET f_version='198' WHERE f_app='DB';
CREATE TABLE users_elina_day_end (f_id INTEGER PRIMARY KEY AUTO_INCREMENT, f_state INT, f_date DATE, f_hall INT, f_prevday FLOAT (12,0), f_income FLOAT(12,0), f_inputother FLOAT(12,0), f_return FLOAT(12,0), f_sale FLOAT(12,0), f_output FLOAT(12,0), f_final FLOAT(12,0), f_check FLOAT(12,0));
ALTER TABLE `a_store` CHANGE COLUMN `f_base` `f_base` CHAR(36) NULL DEFAULT NULL COLLATE 'latin1_general_ci' AFTER `f_total`, CHANGE COLUMN `f_baseDoc` `f_baseDoc` CHAR(36) NULL DEFAULT NULL AFTER `f_base`;

