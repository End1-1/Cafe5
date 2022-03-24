-- --------------------------------------------------------
-- Хост:                         127.0.0.1
-- Версия сервера:               10.6.4-MariaDB - mariadb.org binary distribution
-- Операционная система:         Win64
-- HeidiSQL Версия:              11.3.0.6295
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


-- Дамп структуры базы данных cafe5
CREATE DATABASE IF NOT EXISTS `cafe5` /*!40100 DEFAULT CHARACTER SET utf8mb3 */;
USE `cafe5`;

-- Дамп структуры для таблица cafe5.a_comments
CREATE TABLE IF NOT EXISTS `a_comments` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` text DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_complectation_additions
CREATE TABLE IF NOT EXISTS `a_complectation_additions` (
  `f_id` char(36) NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_name` tinytext DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_row` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_dc
CREATE TABLE IF NOT EXISTS `a_dc` (
  `f_doc` char(36) DEFAULT NULL,
  `f_debit` varchar(12) DEFAULT NULL,
  `f_credit` varchar(12) DEFAULT NULL,
  `f_dc` tinyint(4) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  KEY `id_adc_dc` (`f_dc`),
  KEY `fk_adc_doc_idx` (`f_doc`),
  CONSTRAINT `fk_adc_doc` FOREIGN KEY (`f_doc`) REFERENCES `a_header` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_header
CREATE TABLE IF NOT EXISTS `a_header` (
  `f_id` char(36) NOT NULL,
  `f_userId` varchar(32) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_type` int(11) DEFAULT NULL,
  `f_operator` int(11) DEFAULT NULL,
  `f_date` date DEFAULT NULL,
  `f_createDate` date DEFAULT NULL,
  `f_createTime` time DEFAULT NULL,
  `f_partner` int(11) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_comment` varchar(512) DEFAULT NULL,
  `f_payment` smallint(6) DEFAULT NULL,
  `f_paid` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_date` (`f_date`),
  KEY `fk_operator_idx` (`f_operator`),
  KEY `fk_state_idx` (`f_state`),
  KEY `fk_type_idx` (`f_type`),
  KEY `id_partner` (`f_partner`),
  CONSTRAINT `fk_operator` FOREIGN KEY (`f_operator`) REFERENCES `s_user` (`f_id`),
  CONSTRAINT `fk_state` FOREIGN KEY (`f_state`) REFERENCES `a_state` (`f_id`),
  CONSTRAINT `fk_type` FOREIGN KEY (`f_type`) REFERENCES `a_type` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_header_cash
CREATE TABLE IF NOT EXISTS `a_header_cash` (
  `f_id` char(36) NOT NULL,
  `f_cashin` int(11) DEFAULT NULL,
  `f_cashout` int(11) DEFAULT NULL,
  `f_related` smallint(6) DEFAULT NULL,
  `f_storedoc` char(36) DEFAULT NULL,
  `f_oheader` char(36) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_header_paid
CREATE TABLE IF NOT EXISTS `a_header_paid` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_header_payment
CREATE TABLE IF NOT EXISTS `a_header_payment` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_header_shop2partner
CREATE TABLE IF NOT EXISTS `a_header_shop2partner` (
  `f_id` char(36) NOT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_accept` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_header_shop2partneraccept
CREATE TABLE IF NOT EXISTS `a_header_shop2partneraccept` (
  `f_id` char(36) NOT NULL,
  `f_datetime` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_header_store
CREATE TABLE IF NOT EXISTS `a_header_store` (
  `f_id` char(36) NOT NULL,
  `f_useraccept` int(11) DEFAULT NULL,
  `f_userpass` int(11) DEFAULT NULL,
  `f_invoice` tinytext DEFAULT NULL,
  `f_invoicedate` date DEFAULT NULL,
  `f_storein` int(11) DEFAULT NULL,
  `f_storeout` int(11) DEFAULT NULL,
  `f_baseonsale` smallint(6) DEFAULT 0,
  `f_cashuuid` char(36) DEFAULT NULL,
  `f_complectation` int(11) DEFAULT NULL,
  `f_complectationqty` decimal(14,3) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_reason
CREATE TABLE IF NOT EXISTS `a_reason` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_sale_temp
CREATE TABLE IF NOT EXISTS `a_sale_temp` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_station` tinytext DEFAULT NULL,
  `f_saletype` smallint(6) DEFAULT NULL,
  `f_window` smallint(6) DEFAULT NULL,
  `f_row` smallint(6) DEFAULT NULL,
  `f_order` char(36) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_goodsid` int(11) DEFAULT NULL,
  `f_name` tinytext DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_unit` tinytext DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  `f_taxdept` int(11) DEFAULT NULL,
  `f_adgcode` tinytext DEFAULT NULL,
  `f_tablerec` smallint(6) DEFAULT NULL,
  `f_unitcode` int(11) DEFAULT NULL,
  `f_service` smallint(6) DEFAULT NULL,
  `f_discountvalue` decimal(14,3) DEFAULT NULL,
  `f_discountmode` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=109 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_state
CREATE TABLE IF NOT EXISTS `a_state` (
  `f_id` smallint(6) NOT NULL,
  `f_name` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store
CREATE TABLE IF NOT EXISTS `a_store` (
  `f_id` char(36) NOT NULL,
  `f_document` char(36) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_type` smallint(6) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  `f_base` char(36) DEFAULT NULL,
  `f_baseDoc` char(36) DEFAULT NULL,
  `f_reason` int(11) DEFAULT NULL,
  `f_draft` char(36) DEFAULT NULL,
  `f_qtyleft` decimal(14,4) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_store_idx` (`f_store`),
  KEY `fk_goods_idx` (`f_goods`),
  KEY `fk_store_type_idx` (`f_type`),
  KEY `idx_astore_base` (`f_base`),
  KEY `fk_store_document_idx` (`f_document`),
  KEY `fk_store_draft_idx` (`f_draft`),
  CONSTRAINT `fk_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`),
  CONSTRAINT `fk_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`),
  CONSTRAINT `fk_store_document` FOREIGN KEY (`f_document`) REFERENCES `a_header` (`f_id`),
  CONSTRAINT `fk_store_draft` FOREIGN KEY (`f_draft`) REFERENCES `a_store_draft` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_store_type` FOREIGN KEY (`f_type`) REFERENCES `a_type_sign` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_current
CREATE TABLE IF NOT EXISTS `a_store_current` (
  `f_id` char(36) DEFAULT NULL,
  `f_document` char(36) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_originalqty` decimal(14,4) DEFAULT NULL,
  `f_currentqty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_dish_waste
CREATE TABLE IF NOT EXISTS `a_store_dish_waste` (
  `f_id` char(36) NOT NULL,
  `f_document` char(36) DEFAULT NULL,
  `f_dish` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_data` longtext CHARACTER SET utf8mb4 COLLATE utf8mb4_bin DEFAULT NULL CHECK (json_valid(`f_data`)),
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_draft
CREATE TABLE IF NOT EXISTS `a_store_draft` (
  `f_id` char(36) NOT NULL,
  `f_document` char(36) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_type` smallint(6) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,3) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  `f_reason` int(11) DEFAULT NULL,
  `f_base` char(36) DEFAULT NULL,
  `f_row` int(11) DEFAULT NULL,
  `f_comment` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_store_draft_goods_idx` (`f_goods`),
  KEY `fk_store_draft_store_idx` (`f_store`),
  KEY `fk_store_draft_base_idx` (`f_base`),
  KEY `fk_store_draft_document` (`f_document`),
  CONSTRAINT `fk_store_draft_document` FOREIGN KEY (`f_document`) REFERENCES `a_header` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_store_draft_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_inventory
CREATE TABLE IF NOT EXISTS `a_store_inventory` (
  `f_id` char(36) NOT NULL,
  `f_document` char(36) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_si_store_idx` (`f_store`),
  KEY `fk_si_goosd_idx` (`f_goods`),
  CONSTRAINT `fk_si_goosd` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`),
  CONSTRAINT `fk_si_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_reserve
CREATE TABLE IF NOT EXISTS `a_store_reserve` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_date` date DEFAULT NULL,
  `f_time` time DEFAULT NULL,
  `f_enddate` date DEFAULT NULL,
  `f_state` tinyint(4) DEFAULT NULL,
  `f_source` int(11) DEFAULT NULL,
  `f_completed` int(11) DEFAULT NULL,
  `f_completeddate` date DEFAULT NULL,
  `f_completedtime` time DEFAULT NULL,
  `f_canceled` int(11) DEFAULT NULL,
  `f_canceleddate` date DEFAULT NULL,
  `f_canceledtime` time DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,3) DEFAULT NULL,
  `f_message` text DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=31 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_reserve_state
CREATE TABLE IF NOT EXISTS `a_store_reserve_state` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_sale
CREATE TABLE IF NOT EXISTS `a_store_sale` (
  `f_store` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,3) DEFAULT NULL,
  `f_qtyreserve` decimal(14,3) DEFAULT NULL,
  `f_qtyprogram` decimal(14,3) DEFAULT NULL,
  KEY `idx_store_goods` (`f_goods`,`f_store`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_store_temp
CREATE TABLE IF NOT EXISTS `a_store_temp` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_store` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,3) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_type
CREATE TABLE IF NOT EXISTS `a_type` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(256) DEFAULT NULL,
  `f_counter` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.a_type_sign
CREATE TABLE IF NOT EXISTS `a_type_sign` (
  `f_id` smallint(6) NOT NULL,
  `f_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.b_car
CREATE TABLE IF NOT EXISTS `b_car` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_car` int(11) DEFAULT NULL,
  `f_costumer` int(11) DEFAULT NULL,
  `f_govnumber` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.b_cards_discount
CREATE TABLE IF NOT EXISTS `b_cards_discount` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_mode` smallint(6) DEFAULT NULL,
  `f_value` decimal(14,3) DEFAULT NULL,
  `f_code` text DEFAULT NULL,
  `f_client` int(11) DEFAULT NULL,
  `f_datestart` date DEFAULT NULL,
  `f_dateend` date DEFAULT NULL,
  `f_active` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.b_card_types
CREATE TABLE IF NOT EXISTS `b_card_types` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.b_clients_debts
CREATE TABLE IF NOT EXISTS `b_clients_debts` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_header` char(36) DEFAULT NULL,
  `f_date` date DEFAULT NULL,
  `f_costumer` int(11) DEFAULT NULL,
  `f_order` char(36) DEFAULT NULL,
  `f_cash` char(36) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_govnumber` tinytext DEFAULT NULL,
  `f_storedoc` char(36) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=207 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.b_history
CREATE TABLE IF NOT EXISTS `b_history` (
  `f_id` char(36) NOT NULL,
  `f_type` smallint(6) DEFAULT NULL,
  `f_card` int(11) DEFAULT NULL,
  `f_value` decimal(14,3) DEFAULT NULL,
  `f_data` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.b_marks
CREATE TABLE IF NOT EXISTS `b_marks` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_active` smallint(6) DEFAULT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods
CREATE TABLE IF NOT EXISTS `c_goods` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_supplier` int(11) DEFAULT NULL,
  `f_group` int(11) DEFAULT NULL,
  `f_unit` int(11) DEFAULT NULL,
  `f_name` tinytext DEFAULT NULL,
  `f_salePrice` decimal(12,3) DEFAULT NULL,
  `f_lastinputprice` decimal(14,3) DEFAULT NULL,
  `f_lowlevel` decimal(12,4) DEFAULT NULL,
  `f_group1` int(11) DEFAULT NULL,
  `f_group2` int(11) DEFAULT NULL,
  `f_group3` int(11) DEFAULT NULL,
  `f_group4` int(11) DEFAULT NULL,
  `f_saleprice2` decimal(14,2) DEFAULT NULL,
  `f_enabled` smallint(6) DEFAULT NULL,
  `f_scancode` varchar(32) DEFAULT NULL,
  `f_service` smallint(6) DEFAULT 0,
  `f_description` text DEFAULT NULL,
  `f_storeid` int(11) DEFAULT NULL,
  `f_wholenumber` smallint(6) DEFAULT NULL,
  `f_weight` decimal(14,4) DEFAULT NULL,
  `f_storeinputbeforesale` tinyint(4) DEFAULT NULL,
  `f_complectout` decimal(14,3) DEFAULT 1.000,
  `f_iscomplect` tinyint(4) DEFAULT 0,
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `f_scancode_UNIQUE` (`f_scancode`),
  KEY `fk_group_idx` (`f_group`),
  KEY `fk_unit_idx` (`f_unit`),
  KEY `idx_goods_enabled` (`f_enabled`),
  KEY `idx_storeid` (`f_storeid`),
  CONSTRAINT `fk_group` FOREIGN KEY (`f_group`) REFERENCES `c_groups` (`f_id`),
  CONSTRAINT `fk_unit` FOREIGN KEY (`f_unit`) REFERENCES `c_units` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=642 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods_classes
CREATE TABLE IF NOT EXISTS `c_goods_classes` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods_complectation
CREATE TABLE IF NOT EXISTS `c_goods_complectation` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_base` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,3) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_goods_complectation_1_idx` (`f_base`),
  KEY `fk_goods_complectation_2_idx` (`f_goods`),
  CONSTRAINT `fk_goods_complectation_1` FOREIGN KEY (`f_base`) REFERENCES `c_goods` (`f_id`),
  CONSTRAINT `fk_goods_complectation_2` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=97 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods_images
CREATE TABLE IF NOT EXISTS `c_goods_images` (
  `f_id` int(11) NOT NULL,
  `f_data` mediumblob DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods_multiscancode
CREATE TABLE IF NOT EXISTS `c_goods_multiscancode` (
  `f_id` varchar(32) NOT NULL,
  `f_goods` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods_option
CREATE TABLE IF NOT EXISTS `c_goods_option` (
  `f_id` int(11) NOT NULL,
  `f_storeinputbeforesale` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods_scancode
CREATE TABLE IF NOT EXISTS `c_goods_scancode` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_code` varchar(32) NOT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_receipt` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_goods_waste
CREATE TABLE IF NOT EXISTS `c_goods_waste` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_goods` int(11) DEFAULT NULL,
  `f_waste` decimal(9,3) DEFAULT NULL,
  `f_reason` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_goods_waste_goods_idx` (`f_goods`),
  CONSTRAINT `fk_goods_waste_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_groups
CREATE TABLE IF NOT EXISTS `c_groups` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  `f_taxDept` smallint(6) DEFAULT NULL,
  `f_adgcode` varchar(16) DEFAULT NULL,
  `f_order` int(11) DEFAULT NULL,
  `f_chargevalue` decimal(14,3) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_partners
CREATE TABLE IF NOT EXISTS `c_partners` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_taxname` varchar(128) DEFAULT NULL,
  `f_taxcode` varchar(45) DEFAULT NULL,
  `f_contact` varchar(255) DEFAULT NULL,
  `f_info` varchar(256) DEFAULT NULL,
  `f_phone` varchar(255) DEFAULT NULL,
  `f_email` varchar(255) DEFAULT NULL,
  `f_address` tinytext DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_cash` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_storages
CREATE TABLE IF NOT EXISTS `c_storages` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  `f_outcounter` int(11) DEFAULT NULL,
  `f_inputcounter` int(11) DEFAULT NULL,
  `f_movecounter` int(11) DEFAULT NULL,
  `f_complectcounter` int(11) DEFAULT NULL,
  `f_partner` int(11) DEFAULT NULL,
  `f_queue` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.c_units
CREATE TABLE IF NOT EXISTS `c_units` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  `f_defaultqty` decimal(14,3) DEFAULT NULL,
  `f_fullname` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.droid_message
CREATE TABLE IF NOT EXISTS `droid_message` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `msg` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_dish
CREATE TABLE IF NOT EXISTS `d_dish` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_part` int(11) DEFAULT NULL,
  `f_name` varchar(128) DEFAULT NULL,
  `f_color` int(11) DEFAULT NULL,
  `f_queue` int(11) DEFAULT NULL,
  `f_remind` smallint(6) DEFAULT NULL,
  `f_service` smallint(6) DEFAULT NULL,
  `f_discount` smallint(6) DEFAULT NULL,
  `f_comment` varchar(512) DEFAULT NULL,
  `f_recipeqty` decimal(14,2) DEFAULT NULL,
  `f_netweight` decimal(14,5) DEFAULT 0.00000,
  `f_cost` decimal(14,2) DEFAULT 0.00,
  `f_hourlypayment` int(11) DEFAULT NULL,
  `f_extra` tinyint(4) DEFAULT 0,
  PRIMARY KEY (`f_id`),
  KEY `fk_dish_part_idx` (`f_part`),
  CONSTRAINT `fk_dish_part` FOREIGN KEY (`f_part`) REFERENCES `d_part2` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1292 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_dish_comment
CREATE TABLE IF NOT EXISTS `d_dish_comment` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_dish_state
CREATE TABLE IF NOT EXISTS `d_dish_state` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_image
CREATE TABLE IF NOT EXISTS `d_image` (
  `f_id` int(11) NOT NULL,
  `f_image` mediumblob DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_menu
CREATE TABLE IF NOT EXISTS `d_menu` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_menu` int(11) DEFAULT NULL,
  `f_dish` int(11) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_print1` tinytext DEFAULT NULL,
  `f_print2` tinytext DEFAULT NULL,
  `f_state` smallint(6) DEFAULT 1,
  `f_recent` tinyint(4) DEFAULT 0,
  PRIMARY KEY (`f_id`),
  KEY `fk_menu_menu_idx` (`f_menu`),
  KEY `fk_menu_dish_idx` (`f_dish`),
  KEY `fk_menu_store_idx` (`f_store`),
  CONSTRAINT `fk_menu_dish` FOREIGN KEY (`f_dish`) REFERENCES `d_dish` (`f_id`),
  CONSTRAINT `fk_menu_menu` FOREIGN KEY (`f_menu`) REFERENCES `d_menu_names` (`f_id`),
  CONSTRAINT `fk_menu_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=296 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_menu_names
CREATE TABLE IF NOT EXISTS `d_menu_names` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  `f_dateStart` date DEFAULT NULL,
  `f_dateEnd` date DEFAULT NULL,
  `f_comment` varchar(128) DEFAULT NULL,
  `f_enabled` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_package
CREATE TABLE IF NOT EXISTS `d_package` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_menu` int(11) DEFAULT NULL,
  `f_name` tinytext DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_enabled` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_package_list
CREATE TABLE IF NOT EXISTS `d_package_list` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_package` int(11) DEFAULT NULL,
  `f_dish` int(11) DEFAULT NULL,
  `f_qty` decimal(14,2) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_printer` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_part1
CREATE TABLE IF NOT EXISTS `d_part1` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_part2
CREATE TABLE IF NOT EXISTS `d_part2` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_part` int(11) DEFAULT NULL,
  `f_name` varchar(64) DEFAULT NULL,
  `f_color` int(11) DEFAULT NULL,
  `f_adgCode` varchar(32) DEFAULT NULL,
  `f_queue` int(11) DEFAULT 0,
  `f_parent` int(11) DEFAULT 0,
  PRIMARY KEY (`f_id`),
  KEY `fk_part2_part_idx` (`f_part`),
  CONSTRAINT `fk_part2_part` FOREIGN KEY (`f_part`) REFERENCES `d_part1` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=33 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_printers
CREATE TABLE IF NOT EXISTS `d_printers` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_print_aliases
CREATE TABLE IF NOT EXISTS `d_print_aliases` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_alias` tinytext DEFAULT NULL,
  `f_printer` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_recipes
CREATE TABLE IF NOT EXISTS `d_recipes` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_dish` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_recipe_dish_idx` (`f_dish`),
  KEY `fk_recipe_goods_idx` (`f_goods`),
  CONSTRAINT `fk_recipe_dish` FOREIGN KEY (`f_dish`) REFERENCES `d_dish` (`f_id`),
  CONSTRAINT `fk_recipe_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1506 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_special
CREATE TABLE IF NOT EXISTS `d_special` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_dish` int(11) DEFAULT NULL,
  `f_comment` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_stoplist
CREATE TABLE IF NOT EXISTS `d_stoplist` (
  `f_dish` int(11) NOT NULL,
  `f_qty` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_dish`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.d_translator
CREATE TABLE IF NOT EXISTS `d_translator` (
  `f_id` int(11) NOT NULL,
  `f_mode` smallint(6) DEFAULT NULL,
  `f_en` tinytext DEFAULT NULL,
  `f_ru` tinytext DEFAULT NULL,
  `f_am` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.e_cash
CREATE TABLE IF NOT EXISTS `e_cash` (
  `f_id` char(36) NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_cash` int(11) DEFAULT NULL,
  `f_sign` smallint(6) DEFAULT NULL,
  `f_remarks` varchar(1024) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_base` char(36) DEFAULT NULL,
  `f_row` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_ecash_base` (`f_base`),
  KEY `fk_ecash_header_idx` (`f_header`),
  CONSTRAINT `fk_ecash_header` FOREIGN KEY (`f_header`) REFERENCES `a_header` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.e_cash_names
CREATE TABLE IF NOT EXISTS `e_cash_names` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.e_currency
CREATE TABLE IF NOT EXISTS `e_currency` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_short` char(3) DEFAULT NULL,
  `f_name` varchar(32) DEFAULT NULL,
  `f_rate` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.e_relation
CREATE TABLE IF NOT EXISTS `e_relation` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.h_halls
CREATE TABLE IF NOT EXISTS `h_halls` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(64) DEFAULT NULL,
  `f_counter` int(11) DEFAULT 0,
  `f_prefix` char(8) DEFAULT NULL,
  `f_settings` int(11) DEFAULT NULL,
  `f_counterhall` int(11) DEFAULT NULL,
  `f_booking` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `f_prefix_UNIQUE` (`f_prefix`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.h_tables
CREATE TABLE IF NOT EXISTS `h_tables` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_hall` int(11) DEFAULT NULL,
  `f_name` tinytext DEFAULT NULL,
  `f_lock` smallint(6) DEFAULT 0,
  `f_lockSrc` varchar(128) DEFAULT NULL,
  `f_special_config` int(11) DEFAULT NULL,
  `f_locktime` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_tables_hall_idx` (`f_hall`),
  CONSTRAINT `fk_tables_hall` FOREIGN KEY (`f_hall`) REFERENCES `h_halls` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=161 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.mf_actions
CREATE TABLE IF NOT EXISTS `mf_actions` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.mf_actions_group
CREATE TABLE IF NOT EXISTS `mf_actions_group` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.mf_daily_process
CREATE TABLE IF NOT EXISTS `mf_daily_process` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_date` date DEFAULT NULL,
  `f_worker` int(11) DEFAULT NULL,
  `f_product` int(11) DEFAULT NULL,
  `f_process` int(11) DEFAULT NULL,
  `f_qty` int(11) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_mfdailyprocess_date` (`f_date`),
  KEY `idx_mfdailyprocess_worker` (`f_worker`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.mf_daily_workers
CREATE TABLE IF NOT EXISTS `mf_daily_workers` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_date` date DEFAULT NULL,
  `f_worker` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `idx_mfdailyworkers_date_worker` (`f_date`,`f_worker`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.mf_process
CREATE TABLE IF NOT EXISTS `mf_process` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_rowid` int(11) DEFAULT NULL,
  `f_product` int(11) DEFAULT NULL,
  `f_process` int(11) DEFAULT NULL,
  `f_durationsec` int(11) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_goalprice` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_mfprocess_product` (`f_product`)
) ENGINE=InnoDB AUTO_INCREMENT=28 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.op_body
CREATE TABLE IF NOT EXISTS `op_body` (
  `f_id` binary(16) NOT NULL,
  `f_header` binary(16) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fx_op_body_header_idx` (`f_header`),
  CONSTRAINT `fx_op_body_header` FOREIGN KEY (`f_header`) REFERENCES `op_header` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.op_header
CREATE TABLE IF NOT EXISTS `op_header` (
  `f_id` binary(16) NOT NULL,
  `f_datecreate` date DEFAULT NULL,
  `f_timecreate` time DEFAULT NULL,
  `f_staff` int(11) DEFAULT NULL,
  `f_datefor` date DEFAULT NULL,
  `f_timefor` time DEFAULT NULL,
  `f_advance` decimal(14,2) DEFAULT NULL,
  `f_customer` int(11) DEFAULT NULL,
  `f_info` tinytext DEFAULT NULL,
  `f_advancetax` int(11) DEFAULT NULL,
  `f_state` tinyint(4) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.op_header_state
CREATE TABLE IF NOT EXISTS `op_header_state` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_additional
CREATE TABLE IF NOT EXISTS `o_additional` (
  `f_id` char(36) DEFAULT NULL,
  `f_coordinates` tinytext DEFAULT NULL,
  `f_customer` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_body
CREATE TABLE IF NOT EXISTS `o_body` (
  `f_id` char(36) NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_dish` int(11) DEFAULT NULL,
  `f_qty1` decimal(14,2) DEFAULT NULL,
  `f_qty2` decimal(14,2) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_service` decimal(3,2) DEFAULT NULL,
  `f_discount` decimal(3,2) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_print1` tinytext DEFAULT NULL,
  `f_print2` tinytext DEFAULT NULL,
  `f_comment` varchar(512) DEFAULT NULL,
  `f_remind` smallint(6) DEFAULT NULL,
  `f_adgcode` varchar(64) DEFAULT NULL,
  `f_removeReason` tinytext DEFAULT NULL,
  `f_timeorder` smallint(6) DEFAULT NULL,
  `f_package` int(11) DEFAULT 0,
  `f_canservice` tinyint(4) DEFAULT 0,
  `f_candiscount` tinyint(4) DEFAULT 0,
  `f_guest` tinyint(4) DEFAULT NULL,
  `f_row` smallint(6) DEFAULT NULL,
  `f_fromtable` int(11) DEFAULT 0,
  `f_appendtime` timestamp NULL DEFAULT NULL,
  `f_printtime` timestamp NULL DEFAULT NULL,
  `f_removetime` timestamp NULL DEFAULT NULL,
  `f_removeuser` int(11) DEFAULT NULL,
  `f_removeprecheck` tinyint(4) DEFAULT 0,
  `f_comment2` tinytext DEFAULT NULL,
  `f_reprint` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_obody_header_idx` (`f_header`),
  KEY `fk_obody_state_idx` (`f_state`),
  KEY `fk_obody_dish_idx` (`f_dish`),
  KEY `fk_obody_store_idx` (`f_store`),
  KEY `fk_obdy_removeuser_idx` (`f_removeuser`),
  CONSTRAINT `fk_obdy_removeuser` FOREIGN KEY (`f_removeuser`) REFERENCES `s_user` (`f_id`),
  CONSTRAINT `fk_obody_dish` FOREIGN KEY (`f_dish`) REFERENCES `d_dish` (`f_id`),
  CONSTRAINT `fk_obody_header` FOREIGN KEY (`f_header`) REFERENCES `o_header` (`f_id`),
  CONSTRAINT `fk_obody_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`),
  CONSTRAINT `fkk_body_state` FOREIGN KEY (`f_state`) REFERENCES `o_body_state` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_body_state
CREATE TABLE IF NOT EXISTS `o_body_state` (
  `f_id` smallint(6) NOT NULL,
  `f_name` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_credit_card
CREATE TABLE IF NOT EXISTS `o_credit_card` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  `f_image` text DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_dish_remove_reason
CREATE TABLE IF NOT EXISTS `o_dish_remove_reason` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_flags
CREATE TABLE IF NOT EXISTS `o_flags` (
  `f_id` tinyint(4) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  `f_enabled` tinyint(4) DEFAULT NULL,
  `f_field` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_goods
CREATE TABLE IF NOT EXISTS `o_goods` (
  `f_id` char(36) NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_body` char(36) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  `f_tax` int(11) DEFAULT NULL,
  `f_sign` smallint(6) DEFAULT 1,
  `f_taxdept` varchar(2) DEFAULT NULL,
  `f_row` smallint(6) DEFAULT NULL,
  `f_storerec` char(36) DEFAULT NULL,
  `f_discountfactor` decimal(14,2) DEFAULT 0.00,
  `f_discountmode` smallint(6) DEFAULT 0,
  `f_return` smallint(6) DEFAULT 0,
  `f_returnfrom` char(36) DEFAULT NULL,
  `f_discountAmount` decimal(14,3) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_oheader_id_idx` (`f_header`),
  KEY `fk_cgoods_id_idx` (`f_goods`),
  CONSTRAINT `fk_cgoods_id` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`),
  CONSTRAINT `fk_oheader_id` FOREIGN KEY (`f_header`) REFERENCES `o_header` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_goods_return_reason
CREATE TABLE IF NOT EXISTS `o_goods_return_reason` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_header
CREATE TABLE IF NOT EXISTS `o_header` (
  `f_id` char(36) NOT NULL,
  `f_hallid` int(11) DEFAULT NULL,
  `f_prefix` varchar(8) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_hall` int(11) DEFAULT NULL,
  `f_table` int(11) DEFAULT NULL,
  `f_dateOpen` date DEFAULT NULL,
  `f_dateClose` date DEFAULT NULL,
  `f_dateCash` date DEFAULT NULL,
  `f_timeOpen` time DEFAULT NULL,
  `f_timeClose` time DEFAULT NULL,
  `f_staff` int(11) DEFAULT 0,
  `f_comment` varchar(512) DEFAULT NULL,
  `f_print` int(11) DEFAULT 0,
  `f_amountTotal` decimal(14,2) DEFAULT NULL,
  `f_amountCash` decimal(14,2) DEFAULT 0.00,
  `f_amountCard` decimal(14,2) DEFAULT 0.00,
  `f_amountprepaid` decimal(14,2) DEFAULT NULL,
  `f_amountBank` decimal(14,2) DEFAULT 0.00,
  `f_amountOther` decimal(14,2) DEFAULT 0.00,
  `f_amountService` decimal(14,2) DEFAULT NULL,
  `f_amountDiscount` decimal(14,2) DEFAULT NULL,
  `f_serviceFactor` decimal(4,2) DEFAULT NULL,
  `f_discountFactor` decimal(14,2) DEFAULT NULL,
  `f_creditCardId` int(11) DEFAULT NULL,
  `f_otherId` int(11) DEFAULT NULL,
  `f_shift` int(11) DEFAULT NULL,
  `f_source` smallint(6) DEFAULT NULL,
  `f_saletype` smallint(6) DEFAULT NULL,
  `f_partner` int(11) DEFAULT NULL,
  `f_currentstaff` int(11) DEFAULT NULL,
  `f_guests` tinyint(4) DEFAULT 0,
  `f_precheck` tinyint(4) DEFAULT 0,
  `f_amountidram` decimal(14,3) DEFAULT NULL,
  `f_amountpayx` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_header_flags
CREATE TABLE IF NOT EXISTS `o_header_flags` (
  `f_id` char(36) NOT NULL,
  `f_1` smallint(6) DEFAULT NULL,
  `f_2` smallint(6) DEFAULT NULL,
  `f_3` smallint(6) DEFAULT NULL,
  `f_4` smallint(6) DEFAULT NULL,
  `f_5` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_header_options
CREATE TABLE IF NOT EXISTS `o_header_options` (
  `f_id` char(36) NOT NULL,
  `f_guests` tinyint(4) DEFAULT NULL,
  `f_splitted` tinyint(4) DEFAULT NULL,
  `f_car` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_package
CREATE TABLE IF NOT EXISTS `o_package` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_header` char(36) DEFAULT NULL,
  `f_package` int(11) DEFAULT NULL,
  `f_qty` decimal(14,2) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_payment
CREATE TABLE IF NOT EXISTS `o_payment` (
  `f_id` char(36) NOT NULL,
  `f_cash` decimal(14,2) DEFAULT 0.00,
  `f_card` decimal(14,2) DEFAULT 0.00,
  `f_prepaid` decimal(14,2) DEFAULT 0.00,
  `f_change` decimal(14,2) DEFAULT 0.00,
  `f_idram` decimal(14,2) DEFAULT 0.00,
  `f_tellcell` decimal(14,2) DEFAULT 0.00,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_pay_cl
CREATE TABLE IF NOT EXISTS `o_pay_cl` (
  `f_id` char(36) CHARACTER SET latin1 NOT NULL,
  `f_code` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_name` tinytext CHARACTER SET latin1 DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_pay_other
CREATE TABLE IF NOT EXISTS `o_pay_other` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_pay_room
CREATE TABLE IF NOT EXISTS `o_pay_room` (
  `f_id` char(36) CHARACTER SET latin1 NOT NULL,
  `f_res` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_inv` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_room` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_guest` varchar(128) CHARACTER SET latin1 DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_preorder
CREATE TABLE IF NOT EXISTS `o_preorder` (
  `f_id` char(36) DEFAULT NULL,
  `f_datefor` date DEFAULT NULL,
  `f_timefor` time DEFAULT NULL,
  `f_guests` tinyint(4) DEFAULT NULL,
  `f_prepaidcash` decimal(14,2) DEFAULT NULL,
  `f_prepaidcard` decimal(14,2) DEFAULT NULL,
  `f_prepaidpayx` decimal(14,2) DEFAULT NULL,
  `f_fiscal` int(11) DEFAULT 0,
  `f_guestname` tinytext DEFAULT NULL,
  `f_cityledger` int(11) DEFAULT NULL,
  `f_fortable` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_sale_type
CREATE TABLE IF NOT EXISTS `o_sale_type` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_service_values
CREATE TABLE IF NOT EXISTS `o_service_values` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_value` decimal(5,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_state
CREATE TABLE IF NOT EXISTS `o_state` (
  `f_id` smallint(6) NOT NULL,
  `f_name` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_tax
CREATE TABLE IF NOT EXISTS `o_tax` (
  `f_id` char(36) NOT NULL,
  `f_dept` varchar(16) DEFAULT NULL,
  `f_firmname` varchar(128) DEFAULT NULL,
  `f_address` varchar(1024) DEFAULT NULL,
  `f_devnum` varchar(64) DEFAULT NULL,
  `f_serial` varchar(64) DEFAULT NULL,
  `f_fiscal` varchar(64) DEFAULT NULL,
  `f_receiptnumber` varchar(64) DEFAULT NULL,
  `f_hvhh` varchar(64) DEFAULT NULL,
  `f_fiscalmode` varchar(32) DEFAULT NULL,
  `f_time` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_tax_debug
CREATE TABLE IF NOT EXISTS `o_tax_debug` (
  `f_id` char(36) NOT NULL,
  `f_date` date DEFAULT NULL,
  `f_time` time DEFAULT NULL,
  `f_mark` tinytext DEFAULT NULL,
  `f_message` tinytext DEFAULT NULL,
  `f_elapsed` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_tax_log
CREATE TABLE IF NOT EXISTS `o_tax_log` (
  `f_id` char(36) NOT NULL,
  `f_order` char(36) DEFAULT NULL,
  `f_date` date DEFAULT NULL,
  `f_time` time DEFAULT NULL,
  `f_in` text DEFAULT NULL,
  `f_out` text DEFAULT NULL,
  `f_err` text DEFAULT NULL,
  `f_result` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.o_waiterserver_debug
CREATE TABLE IF NOT EXISTS `o_waiterserver_debug` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_json` mediumtext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=268739 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.sys_cache_tables
CREATE TABLE IF NOT EXISTS `sys_cache_tables` (
  `f_name` varchar(128) NOT NULL,
  `f_sql` text DEFAULT NULL,
  PRIMARY KEY (`f_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_app
CREATE TABLE IF NOT EXISTS `s_app` (
  `f_app` varchar(32) NOT NULL,
  `f_version` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_app`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_cache
CREATE TABLE IF NOT EXISTS `s_cache` (
  `f_id` int(11) NOT NULL,
  `f_version` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_car
CREATE TABLE IF NOT EXISTS `s_car` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=94 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_custom_reports
CREATE TABLE IF NOT EXISTS `s_custom_reports` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_access` smallint(6) DEFAULT NULL,
  `f_name` tinytext DEFAULT NULL,
  `f_sql` text DEFAULT NULL,
  `f_params` text DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_db
CREATE TABLE IF NOT EXISTS `s_db` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  `f_description` tinytext DEFAULT NULL,
  `f_host` tinytext DEFAULT NULL,
  `f_db` tinytext DEFAULT NULL,
  `f_user` tinytext DEFAULT NULL,
  `f_password` tinytext DEFAULT NULL,
  `f_main` smallint(6) DEFAULT 0,
  `f_syncin` int(11) DEFAULT 0,
  `f_syncout` int(11) DEFAULT 0,
  `f_synctime` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_db_access
CREATE TABLE IF NOT EXISTS `s_db_access` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_db` int(11) DEFAULT NULL,
  `f_user` int(11) DEFAULT NULL,
  `f_permit` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=32 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_draft
CREATE TABLE IF NOT EXISTS `s_draft` (
  `f_name` tinytext DEFAULT NULL,
  `f_value` tinytext DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_login_session
CREATE TABLE IF NOT EXISTS `s_login_session` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_session` binary(16) DEFAULT NULL,
  `f_user` int(11) DEFAULT NULL,
  `f_iplogin` char(15) DEFAULT NULL,
  `f_iplogout` char(15) DEFAULT NULL,
  `f_datestart` date DEFAULT NULL,
  `f_timestart` time DEFAULT NULL,
  `f_dateend` date DEFAULT NULL,
  `f_timeend` time DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_login_session` (`f_session`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_log_store_price
CREATE TABLE IF NOT EXISTS `s_log_store_price` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_date` datetime DEFAULT NULL,
  `f_user` int(11) DEFAULT NULL,
  `f_base` char(36) DEFAULT NULL,
  `f_old_goods` int(11) DEFAULT NULL,
  `f_new_goods` int(11) DEFAULT NULL,
  `f_old_price` decimal(14,2) DEFAULT NULL,
  `f_new_price` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_receipt_print
CREATE TABLE IF NOT EXISTS `s_receipt_print` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_station` varchar(64) DEFAULT NULL,
  `f_ip` varchar(64) DEFAULT NULL,
  `f_printer` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `f_station` (`f_station`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_salary_account
CREATE TABLE IF NOT EXISTS `s_salary_account` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_position` int(11) DEFAULT NULL,
  `f_shift` int(11) DEFAULT NULL,
  `f_individual` int(11) DEFAULT NULL,
  `f_percent` decimal(3,3) DEFAULT NULL,
  `f_round` int(11) DEFAULT NULL,
  `f_raw` text DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_salary_body
CREATE TABLE IF NOT EXISTS `s_salary_body` (
  `f_id` char(36) NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_user` int(11) DEFAULT NULL,
  `f_position` int(11) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_row` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_salary_inout
CREATE TABLE IF NOT EXISTS `s_salary_inout` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_user` int(11) DEFAULT NULL,
  `f_datein` date DEFAULT NULL,
  `f_timein` time DEFAULT NULL,
  `f_dateout` date DEFAULT NULL,
  `f_timeout` time DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=42 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_salary_options
CREATE TABLE IF NOT EXISTS `s_salary_options` (
  `f_id` char(36) NOT NULL,
  `f_cashdoc` char(36) DEFAULT NULL,
  `f_shift` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_salary_shift
CREATE TABLE IF NOT EXISTS `s_salary_shift` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  `f_number` smallint(6) DEFAULT NULL,
  `f_formula` text DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_salary_shift_time
CREATE TABLE IF NOT EXISTS `s_salary_shift_time` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_shift` int(11) DEFAULT NULL,
  `f_start` time DEFAULT NULL,
  `f_end` time DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_settings_names
CREATE TABLE IF NOT EXISTS `s_settings_names` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(64) DEFAULT NULL,
  `f_description` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_settings_values
CREATE TABLE IF NOT EXISTS `s_settings_values` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_settings` int(11) DEFAULT NULL,
  `f_key` int(11) DEFAULT NULL,
  `f_value` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_settings_values_s_idx` (`f_settings`),
  CONSTRAINT `fk_settings_values_s` FOREIGN KEY (`f_settings`) REFERENCES `s_settings_names` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3553 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_stranslator
CREATE TABLE IF NOT EXISTS `s_stranslator` (
  `f_name` tinytext DEFAULT NULL,
  `f_en` tinytext DEFAULT NULL,
  `f_ru` tinytext DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_syncronize
CREATE TABLE IF NOT EXISTS `s_syncronize` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_db` int(11) DEFAULT NULL,
  `f_table` tinytext DEFAULT NULL,
  `f_recid` char(36) DEFAULT NULL,
  `f_op` smallint(6) DEFAULT NULL,
  `f_date` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_syncronize_in
CREATE TABLE IF NOT EXISTS `s_syncronize_in` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_table` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=101 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_translator
CREATE TABLE IF NOT EXISTS `s_translator` (
  `f_text` varchar(128) NOT NULL,
  `f_en` tinytext DEFAULT NULL,
  `f_ru` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_text`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_user
CREATE TABLE IF NOT EXISTS `s_user` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_group` int(11) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_first` tinytext DEFAULT NULL,
  `f_last` tinytext DEFAULT NULL,
  `f_login` tinytext DEFAULT NULL,
  `f_password` tinytext DEFAULT NULL,
  `f_altPassword` tinytext DEFAULT NULL,
  `f_config` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_group` (`f_group`),
  KEY `fk_user_state_idx` (`f_state`),
  CONSTRAINT `fk_user_group` FOREIGN KEY (`f_group`) REFERENCES `s_user_group` (`f_id`),
  CONSTRAINT `fk_user_state` FOREIGN KEY (`f_state`) REFERENCES `s_user_state` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=72 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_user_access
CREATE TABLE IF NOT EXISTS `s_user_access` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_group` int(11) DEFAULT NULL,
  `f_key` int(11) DEFAULT NULL,
  `f_value` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_group` (`f_group`),
  KEY `idx_value` (`f_value`),
  CONSTRAINT `fk_user_access_group` FOREIGN KEY (`f_group`) REFERENCES `s_user_group` (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1942 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_user_config
CREATE TABLE IF NOT EXISTS `s_user_config` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_user` int(11) DEFAULT NULL,
  `f_settings` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_user_group
CREATE TABLE IF NOT EXISTS `s_user_group` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_user_photo
CREATE TABLE IF NOT EXISTS `s_user_photo` (
  `f_id` int(11) NOT NULL,
  `f_data` mediumblob DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_user_state
CREATE TABLE IF NOT EXISTS `s_user_state` (
  `f_id` smallint(6) NOT NULL,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица cafe5.s_waiter_reports
CREATE TABLE IF NOT EXISTS `s_waiter_reports` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  `f_sql` text DEFAULT NULL,
  `f_logo` text DEFAULT NULL,
  `f_subtotal` tinytext DEFAULT NULL,
  `f_total` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
