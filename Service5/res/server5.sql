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


-- Дамп структуры базы данных server5
CREATE DATABASE IF NOT EXISTS `server5` /*!40100 DEFAULT CHARACTER SET utf8mb3 */;
USE `server5`;

-- Дамп структуры для таблица server5.acc_balance
CREATE TABLE IF NOT EXISTS `acc_balance` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fdate` date DEFAULT NULL,
  `ftime` time DEFAULT NULL,
  `fuser` int(11) DEFAULT NULL,
  `finout` tinyint(4) DEFAULT NULL,
  `famount` decimal(14,2) DEFAULT NULL,
  `fcomment` tinytext DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.system_databases
CREATE TABLE IF NOT EXISTS `system_databases` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fname` tinytext DEFAULT NULL,
  `fhost` varchar(15) DEFAULT NULL,
  `fschema` tinytext DEFAULT NULL,
  `fusername` tinytext DEFAULT NULL,
  `fpassword` tinytext DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.system_requests
CREATE TABLE IF NOT EXISTS `system_requests` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fdate` date DEFAULT NULL,
  `ftime` time DEFAULT NULL,
  `fhost` varchar(15) DEFAULT NULL,
  `felapsed` int(11) DEFAULT NULL,
  `froute` tinytext DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB AUTO_INCREMENT=84850 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.users_chat
CREATE TABLE IF NOT EXISTS `users_chat` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fdateserver` timestamp NULL DEFAULT NULL,
  `fdatereceive` timestamp NULL DEFAULT NULL,
  `fdateread` timestamp NULL DEFAULT NULL,
  `fstate` tinyint(4) DEFAULT NULL,
  `fsender` int(11) DEFAULT NULL,
  `freceiver` int(11) DEFAULT NULL,
  `fmessage` text COLLATE utf8mb3_bin DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB AUTO_INCREMENT=62 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_bin;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.users_devices
CREATE TABLE IF NOT EXISTS `users_devices` (
  `ftoken` char(255) NOT NULL,
  `fuser` int(11) DEFAULT NULL,
  PRIMARY KEY (`ftoken`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.users_groups
CREATE TABLE IF NOT EXISTS `users_groups` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fname` tinytext DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.users_list
CREATE TABLE IF NOT EXISTS `users_list` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fgroup` int(11) DEFAULT NULL,
  `fphone` varchar(16) DEFAULT NULL,
  `femail` varchar(32) DEFAULT NULL,
  `ffirstname` tinytext DEFAULT NULL,
  `flastname` tinytext DEFAULT NULL,
  `fpassword` char(32) DEFAULT NULL,
  PRIMARY KEY (`fid`),
  KEY `idx_users_list_phone` (`fphone`),
  KEY `idx_users_list_email` (`femail`),
  KEY `idx_users_list_password` (`fpassword`)
) ENGINE=InnoDB AUTO_INCREMENT=191 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.users_registration
CREATE TABLE IF NOT EXISTS `users_registration` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fstate` smallint(6) DEFAULT NULL,
  `fphone` tinytext DEFAULT NULL,
  `femail` tinytext DEFAULT NULL,
  `fpassword` tinytext DEFAULT NULL,
  `fconfirmation_code` char(6) DEFAULT NULL,
  `ftoken` tinytext DEFAULT NULL,
  `fcreatedate` date DEFAULT NULL,
  `fcreatetime` time DEFAULT NULL,
  `fconfirmdate` date DEFAULT NULL,
  `fconfirmtime` time DEFAULT NULL,
  `fconfirmid` int(11) DEFAULT NULL,
  PRIMARY KEY (`fid`),
  KEY `idx_users_registration_phone` (`fphone`(255)),
  KEY `idx_users_registration_token` (`ftoken`(255))
) ENGINE=InnoDB AUTO_INCREMENT=284 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.users_session
CREATE TABLE IF NOT EXISTS `users_session` (
  `fid` char(36) COLLATE utf8mb3_bin NOT NULL,
  `fuser` int(11) DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_bin;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица server5.users_store
CREATE TABLE IF NOT EXISTS `users_store` (
  `fid` int(11) NOT NULL,
  `fuser` int(11) DEFAULT NULL,
  `fqueue` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_bin;

-- Экспортируемые данные не выделены.

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
