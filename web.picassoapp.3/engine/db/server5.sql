/*
 Navicat Premium Data Transfer

 Source Server         : local
 Source Server Type    : MariaDB
 Source Server Version : 110302 (11.3.2-MariaDB)
 Source Host           : localhost:3306
 Source Schema         : server5

 Target Server Type    : MariaDB
 Target Server Version : 110302 (11.3.2-MariaDB)
 File Encoding         : 65001

 Date: 04/05/2024 11:02:18
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for plugins_options
-- ----------------------------
DROP TABLE IF EXISTS `plugins_options`;
CREATE TABLE `plugins_options`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fplugin` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fkey` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fvalue` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 32 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for queries
-- ----------------------------
DROP TABLE IF EXISTS `queries`;
CREATE TABLE `queries`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fkey` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fquery` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 6 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for system_databases
-- ----------------------------
DROP TABLE IF EXISTS `system_databases`;
CREATE TABLE `system_databases`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fname` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fhost` varchar(15) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fschema` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fusername` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fpassword` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for system_requests
-- ----------------------------
DROP TABLE IF EXISTS `system_requests`;
CREATE TABLE `system_requests`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fdate` date NULL DEFAULT NULL,
  `ftime` time NULL DEFAULT NULL,
  `fhost` varchar(15) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `felapsed` int(11) NULL DEFAULT NULL,
  `froute` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `frequest` longtext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fresponse` longtext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 305846 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users_chat
-- ----------------------------
DROP TABLE IF EXISTS `users_chat`;
CREATE TABLE `users_chat`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fdateserver` timestamp NULL DEFAULT NULL,
  `fdatereceive` timestamp NULL DEFAULT NULL,
  `fdateread` timestamp NULL DEFAULT NULL,
  `fstate` tinyint(4) NULL DEFAULT NULL,
  `fsender` int(11) NULL DEFAULT NULL,
  `freceiver` int(11) NULL DEFAULT NULL,
  `fmessage` text CHARACTER SET utf8mb3 COLLATE utf8mb3_bin NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 146 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users_devices
-- ----------------------------
DROP TABLE IF EXISTS `users_devices`;
CREATE TABLE `users_devices`  (
  `ftoken` char(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `fuser` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`ftoken`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users_groups
-- ----------------------------
DROP TABLE IF EXISTS `users_groups`;
CREATE TABLE `users_groups`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fname` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 3 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users_list
-- ----------------------------
DROP TABLE IF EXISTS `users_list`;
CREATE TABLE `users_list`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fgroup` int(11) NULL DEFAULT NULL,
  `fphone` varchar(16) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `femail` varchar(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `ffirstname` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `flastname` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fpassword` char(32) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE,
  INDEX `idx_users_list_phone`(`fphone` ASC) USING BTREE,
  INDEX `idx_users_list_email`(`femail` ASC) USING BTREE,
  INDEX `idx_users_list_password`(`fpassword` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 192 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users_registration
-- ----------------------------
DROP TABLE IF EXISTS `users_registration`;
CREATE TABLE `users_registration`  (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fstate` smallint(6) NULL DEFAULT NULL,
  `fphone` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `femail` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fpassword` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fconfirmation_code` char(6) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `ftoken` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NULL DEFAULT NULL,
  `fcreatedate` date NULL DEFAULT NULL,
  `fcreatetime` time NULL DEFAULT NULL,
  `fconfirmdate` date NULL DEFAULT NULL,
  `fconfirmtime` time NULL DEFAULT NULL,
  `fconfirmid` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE,
  INDEX `idx_users_registration_phone`(`fphone`(255) ASC) USING BTREE,
  INDEX `idx_users_registration_token`(`ftoken`(255) ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 286 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users_session
-- ----------------------------
DROP TABLE IF EXISTS `users_session`;
CREATE TABLE `users_session`  (
  `fid` char(36) CHARACTER SET utf8mb3 COLLATE utf8mb3_bin NOT NULL,
  `fuser` int(11) NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb3 COLLATE = utf8mb3_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for users_store
-- ----------------------------
DROP TABLE IF EXISTS `users_store`;
CREATE TABLE `users_store`  (
  `fid` int(11) NOT NULL,
  `fuser` int(11) NULL DEFAULT NULL,
  `fqueue` tinyint(4) NULL DEFAULT NULL,
  PRIMARY KEY (`fid`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb3 COLLATE = utf8mb3_bin ROW_FORMAT = Dynamic;

-- ----------------------------
-- Procedure structure for sf_create_store_document
-- ----------------------------
DROP PROCEDURE IF EXISTS `sf_create_store_document`;
delimiter ;;
CREATE PROCEDURE `sf_create_store_document`(IN `params` JSON,
	OUT `result` json)
sp:BEGIN
DECLARE EXIT HANDLER FOR SQLEXCEPTION 
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;
    
START TRANSACTION;

#data
select JSON_EXTRACT(params, '$.header') INTO @header;
SELECT json_extract(params, '$.body') INTO @header_body;
SELECT JSON_EXTRACT(params, '$.goods') INTO @goods;

#delete old document
delete from a_store_valid where f_document=JSON_VALUE(@header, '$.f_id');
DELETE FROM a_store WHERE f_document=JSON_VALUE(@header, '$.f_id');
DELETE FROM a_store_draft WHERE f_document=JSON_VALUE(@header, '$.f_id');
DELETE FROM a_header WHERE f_id=JSON_VALUE(@header, '$.f_id');
DELETE FROM a_header_store WHERE f_id=JSON_VALUE(@header, '$.f_id');
DELETE FROM b_clients_debts WHERE f_storedoc=JSON_VALUE(@header, '$.f_id');
delete from a_complectation_additions where f_header=JSON_VALUE(@header, '$.f_id');

#header
INSERT INTO a_header (f_id, f_userid, f_state, f_type, 
	f_operator, f_date, f_createdate, f_createtime, 
	f_partner, f_amount, f_comment, f_payment, f_paid, f_currency, f_sessionid, f_body)
	VALUES (
	JSON_VALUE(@header, '$.f_id'), JSON_VALUE(@header, '$.f_userid'), JSON_VALUE(@header, '$.f_state'), JSON_VALUE(@header, '$.f_type'),
	JSON_VALUE(@header, '$.f_operator'), JSON_VALUE(@header, '$.f_date'), CURRENT_DATE(), CURRENT_TIME(),
	JSON_VALUE(@header, '$.f_partner'), JSON_VALUE(@header, '$.f_amount'), JSON_VALUE(@header, '$.f_comment'), JSON_VALUE(@header, '$.f_payment'),
	JSON_VALUE(@header, '$.f_paid'), JSON_VALUE(@header, '$.f_currency'), JSON_VALUE(@header, '$.f_sessionid'), @header_body);

#a_store_draft input
if (JSON_VALUE(@header, '$.f_type')=1) then 
	select JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', 1, 'store', JSON_VALUE(@header, '$.f_storein'), 'state', JSON_VALUE(@header, '$.f_state')) INTO @goods;
	SELECT sf_create_store_goods(@goods) INTO @result;
ELSEif (JSON_VALUE(@header, '$.f_type')=2) then
	select JSON_OBJECT('goods', JSON_DETAILED(@goods), 'ftype', -1, 'store', JSON_VALUE(@header, '$.f_storeout'), 'state', JSON_VALUE(@header, '$.f_state')) INTO @goods;	
	SELECT sf_create_store_goods(@goods) INTO @result;	
else
 SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'UNKNOWN DOCUMENT TYPE';
END if;
COMMIT;
SET result = JSON_OBJECT('status', 1, 'state', cast(JSON_VALUE(@header, '$.f_state') AS INT));
end
;;
delimiter ;

-- ----------------------------
-- Function structure for sf_create_store_goods
-- ----------------------------
DROP FUNCTION IF EXISTS `sf_create_store_goods`;
delimiter ;;
CREATE FUNCTION `sf_create_store_goods`(params JSON)
 RETURNS longtext CHARSET utf8mb4 COLLATE utf8mb4_bin
BEGIN
SET @ftype = JSON_VALUE(params, '$.ftype');
SET @store = JSON_VALUE(params, '$.store');
SET @state = JSON_VALUE(params, '$.state');
SELECT JSON_EXTRACT(params, '$.goods') INTO @goods;
SET @i = 0;
while (@i < JSON_LENGTH(@goods)) DO 
SELECT JSON_EXTRACT(@goods, CONCAT('$[', @i, ']')) INTO @g;
#draft
SET @draftid = UUID();
INSERT INTO a_store_draft (f_id, f_document, f_store, f_type, f_goods, 
	f_qty, f_price, f_reason, f_base, f_row, f_comment)
	VALUES (@draftid, JSON_VALUE(@header, '$.f_id'), @store, @ftype, JSON_VALUE(@g, '$.f_goods'), 
	JSON_VALUE(@g, '$.f_qty'), JSON_VALUE(@g, '$.f_price'), JSON_VALUE(@header, '$.f_reason'), @draftid, JSON_VALUE(@g, '$.f_row'), JSON_VALUE(@g, '$.f_comment'));
	
if (@state = 1 AND @ftype = 1) then	
INSERT INTO a_store (f_id, f_document, f_store, f_type, f_goods, 
	f_qty, f_price, f_reason, f_base, f_draft)
	VALUES (UUID(), JSON_VALUE(@header, '$.f_id'), @store, @ftype, JSON_VALUE(@g, '$.f_goods'), 
	JSON_VALUE(@g, '$.f_qty'), JSON_VALUE(@g, '$.f_price'), JSON_VALUE(@header, '$.f_reason'), @draftid, @draftid);
END if;
	
SET @i = @i + 1;
END while;
RETURN JSON_OBJECT('status', 1);
END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
