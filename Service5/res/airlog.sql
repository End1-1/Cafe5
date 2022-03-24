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


-- Дамп структуры базы данных airlog
CREATE DATABASE IF NOT EXISTS `airlog` /*!40100 DEFAULT CHARACTER SET utf8mb3 */;
USE `airlog`;

-- Дамп структуры для таблица airlog.log
CREATE TABLE IF NOT EXISTS `log` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_comp` tinytext DEFAULT NULL,
  `f_date` date DEFAULT NULL,
  `f_time` time DEFAULT NULL,
  `f_user` tinytext DEFAULT NULL,
  `f_type` int(11) DEFAULT NULL,
  `f_rec` varchar(36) DEFAULT NULL,
  `f_invoice` varchar(36) DEFAULT NULL,
  `f_reservation` varchar(36) DEFAULT NULL,
  `f_action` varchar(256) DEFAULT NULL,
  `f_value1` text DEFAULT NULL,
  `f_value2` text DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=12008 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица airlog.logt
CREATE TABLE IF NOT EXISTS `logt` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=49 DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

-- Дамп структуры для таблица airlog.sqllog
CREATE TABLE IF NOT EXISTS `sqllog` (
  `fid` int(11) NOT NULL AUTO_INCREMENT,
  `fop` tinyint(4) DEFAULT NULL,
  `fsql` text DEFAULT NULL,
  `felapsedtime` int(11) DEFAULT NULL,
  `fcompname` tinytext DEFAULT NULL,
  PRIMARY KEY (`fid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

-- Экспортируемые данные не выделены.

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
