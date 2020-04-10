-- MySQL dump 10.13  Distrib 5.7.28, for Win64 (x86_64)
--
-- Host: localhost    Database: cafe5
-- ------------------------------------------------------
-- Server version	5.7.28-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `a_comments`
--

DROP TABLE IF EXISTS `a_comments`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_comments` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` text,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_comments`
--

LOCK TABLES `a_comments` WRITE;
/*!40000 ALTER TABLE `a_comments` DISABLE KEYS */;
/*!40000 ALTER TABLE `a_comments` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_complectation_additions`
--

DROP TABLE IF EXISTS `a_complectation_additions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_complectation_additions` (
  `f_id` char(36) NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_name` tinytext,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_row` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_complectation_additions`
--

LOCK TABLES `a_complectation_additions` WRITE;
/*!40000 ALTER TABLE `a_complectation_additions` DISABLE KEYS */;
/*!40000 ALTER TABLE `a_complectation_additions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_header`
--

DROP TABLE IF EXISTS `a_header`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_header` (
  `f_id` char(36) NOT NULL,
  `f_userId` varchar(32) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_type` int(11) DEFAULT NULL,
  `f_operator` int(11) DEFAULT NULL,
  `f_date` date DEFAULT NULL,
  `f_createDate` date DEFAULT NULL,
  `f_createTime` time DEFAULT NULL,
  `f_partner` int(11) DEFAULT NULL,
  `f_invoice` varchar(64) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_comment` varchar(512) DEFAULT NULL,
  `f_raw` text,
  `f_payment` smallint(6) DEFAULT NULL,
  `f_paid` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_date` (`f_date`),
  KEY `fk_operator_idx` (`f_operator`),
  KEY `fk_state_idx` (`f_state`),
  KEY `fk_type_idx` (`f_type`),
  KEY `id_partner` (`f_partner`),
  KEY `idx_invoice` (`f_invoice`),
  CONSTRAINT `fk_operator` FOREIGN KEY (`f_operator`) REFERENCES `s_user` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_state` FOREIGN KEY (`f_state`) REFERENCES `a_state` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_type` FOREIGN KEY (`f_type`) REFERENCES `a_type` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_header`
--

LOCK TABLES `a_header` WRITE;
/*!40000 ALTER TABLE `a_header` DISABLE KEYS */;
INSERT INTO `a_header` VALUES ('1e76132d-6e1f-11ea-85d4-1078d2d2b808','',2,2,4,'2020-03-25','2020-03-25','02:30:46',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"6\"}',0,0),('2dcbd005-6b96-11ea-85d4-1078d2d2b808','',2,2,4,'2020-03-21','2020-03-21','21:05:31',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"6\"}',0,0),('3a569a96-6c49-11ea-85d4-1078d2d2b808','',2,2,4,'2020-03-22','2020-03-22','18:27:11',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"0\"}',0,0),('4f578133-682c-11ea-85d4-1078d2d2b808','74',1,5,1,'2020-03-17','2020-03-17','12:50:09',0,NULL,60.00,'CASHINPUT 17.03.2020/B5','{\n    \"cashin\": 1,\n    \"cashout\": 0,\n    \"relation\": \"1\",\n    \"storedoc\": \"\"\n}\n',NULL,NULL),('4f9aa853-668d-11ea-85d4-1078d2d2b808','',2,2,1,'2020-03-15','2020-03-15','11:19:30',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"1\"}',0,0),('5f9108fe-67ad-11ea-85d4-1078d2d2b808','0',1,1,1,'2020-03-16','2020-03-16','21:41:31',0,'',250.00,'','{\"based_on_sale\":0,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"\",\"f_complectationqty\":\"\",\"f_invoice\":\"\",\"f_invoicedate\":\"16.03.2020\",\"f_passed\":\"\",\"f_storein\":\"3\",\"f_storeout\":\"\"}',2,0),('6d2236cb-6a7c-11ea-85d4-1078d2d2b808','',2,2,1,'2020-02-05','2020-03-20','11:28:40',0,'',0.00,'Ավտոմատ ելք','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"4\"}',0,0),('6e90194f-682c-11ea-85d4-1078d2d2b808','',2,2,1,'2020-03-17','2020-03-17','12:51:02',0,'',0.00,'Ավտոմատ ելք','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"2\"}',0,0),('72f95063-6c7d-11ea-85d4-1078d2d2b808','',2,2,1,'2020-03-23','2020-03-23','00:41:00',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"0\"}',0,0),('744cb1bc-6c15-11ea-85d4-1078d2d2b808','',2,2,4,'2020-03-22','2020-03-22','12:16:35',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"6\"}',0,0),('8388ca61-6a7d-11ea-85d4-1078d2d2b808','',2,2,1,'2020-03-20','2020-03-20','11:36:27',0,'',0.00,'Ավտոմատ ելք','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"2\"}',0,0),('8ca1c372-66ad-11ea-85d4-1078d2d2b808','',2,2,1,'2020-03-15','2020-03-15','15:10:16',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"2\"}',0,0),('95d6974d-67ad-11ea-85d4-1078d2d2b808','0',1,2,1,'2020-03-16','2020-03-16','21:43:02',0,'',125.00,'','{\"based_on_sale\":0,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"\",\"f_complectationqty\":\"\",\"f_invoice\":\"\",\"f_invoicedate\":\"16.03.2020\",\"f_passed\":\"\",\"f_storein\":\"\",\"f_storeout\":\"3\"}',0,0),('96447164-789a-11ea-805f-1078d2d2b808','0',1,1,1,'2020-04-07','2020-04-07','10:39:52',0,'',400.00,'','{\"based_on_sale\":0,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"\",\"f_complectationqty\":\"\",\"f_invoice\":\"\",\"f_invoicedate\":\"07.04.2020\",\"f_passed\":\"\",\"f_storein\":\"3\",\"f_storeout\":\"\"}',0,0),('a9a818b3-6c7b-11ea-85d4-1078d2d2b808','',2,2,4,'2020-03-23','2020-03-23','00:28:13',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"6\"}',0,0),('acb74182-67b3-11ea-85d4-1078d2d2b808','0',1,1,1,'2020-03-16','2020-03-16','22:26:37',0,'',2500.00,'','{\"based_on_sale\":0,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"\",\"f_complectationqty\":\"\",\"f_invoice\":\"\",\"f_invoicedate\":\"16.03.2020\",\"f_passed\":\"\",\"f_storein\":\"3\",\"f_storeout\":\"\"}',1,0),('b216626f-7b03-11ea-a303-54ab3a2a8093','0',1,1,1,'2020-04-10','2020-04-10','12:17:19',0,'',800.00,'','{\"based_on_sale\":0,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"\",\"f_complectationqty\":\"\",\"f_invoice\":\"\",\"f_invoicedate\":\"10.04.2020\",\"f_passed\":\"\",\"f_storein\":\"3\",\"f_storeout\":\"\"}',0,0),('b387ce4f-6a7e-11ea-85d4-1078d2d2b808','',2,2,1,'2020-03-20','2020-03-20','11:44:57',0,'',0.00,'Ավտոմատ ելք','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"4\"}',0,0),('c0bb7b44-67b3-11ea-85d4-1078d2d2b808','0',1,6,1,'2020-03-16','2020-03-16','22:27:11',0,'',1500.00,'','{\"based_on_sale\":0,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"37\",\"f_complectationqty\":\"2\",\"f_invoice\":\"\",\"f_invoicedate\":\"16.03.2020\",\"f_passed\":\"\",\"f_storein\":\"2\",\"f_storeout\":\"3\"}',0,0),('c51a1c87-6f2d-11ea-85d4-1078d2d2b808','',2,2,4,'2020-03-26','2020-03-26','10:48:09',0,'',0.00,'Sale autooutput','{\"based_on_sale\":1,\"f_storein\":\"\",\"f_storeout\":\"6\"}',0,0),('cd798be8-682b-11ea-85d4-1078d2d2b808','0',1,2,1,'2020-03-17','2020-03-17','12:50:42',0,'',600.00,'Ավտոմատ ելք','{\"based_on_sale\":1,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"\",\"f_complectationqty\":\"\",\"f_invoice\":\"\",\"f_invoicedate\":\"17.03.2020\",\"f_passed\":\"\",\"f_storein\":\"\",\"f_storeout\":\"2\"}',0,0),('d8725f3f-67ad-11ea-85d4-1078d2d2b808','0',1,3,1,'2020-03-16','2020-03-16','21:44:54',0,'',125.00,'','{\"based_on_sale\":0,\"f_accepted\":\"\",\"f_cash\":0,\"f_cashuuid\":\"\",\"f_complectation\":\"\",\"f_complectationqty\":\"\",\"f_invoice\":\"\",\"f_invoicedate\":\"16.03.2020\",\"f_passed\":\"\",\"f_storein\":\"2\",\"f_storeout\":\"3\"}',0,0);
/*!40000 ALTER TABLE `a_header` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_header_paid`
--

DROP TABLE IF EXISTS `a_header_paid`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_header_paid` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_header_paid`
--

LOCK TABLES `a_header_paid` WRITE;
/*!40000 ALTER TABLE `a_header_paid` DISABLE KEYS */;
INSERT INTO `a_header_paid` VALUES (1,'Չվճարված'),(2,'Վճարված'),(3,'Բոլորը');
/*!40000 ALTER TABLE `a_header_paid` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_header_payment`
--

DROP TABLE IF EXISTS `a_header_payment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_header_payment` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_header_payment`
--

LOCK TABLES `a_header_payment` WRITE;
/*!40000 ALTER TABLE `a_header_payment` DISABLE KEYS */;
INSERT INTO `a_header_payment` VALUES (1,'Կանխիկ'),(2,'Բանկային քարտ'),(3,'Փոխանցում');
/*!40000 ALTER TABLE `a_header_payment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_reason`
--

DROP TABLE IF EXISTS `a_reason`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_reason` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_reason`
--

LOCK TABLES `a_reason` WRITE;
/*!40000 ALTER TABLE `a_reason` DISABLE KEYS */;
INSERT INTO `a_reason` VALUES (1,'Ձեռք բերում'),(2,'Տեղաշարժ'),(3,'Դուրս գրում'),(4,'Վաճառք'),(5,'Կորուստ'),(6,'Ավելցուկ'),(7,'Թափոն'),(8,'Կոմպլեկտավորում'),(9,'Հետ վերադարձ');
/*!40000 ALTER TABLE `a_reason` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_state`
--

DROP TABLE IF EXISTS `a_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_state` (
  `f_id` smallint(6) NOT NULL,
  `f_name` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_state`
--

LOCK TABLES `a_state` WRITE;
/*!40000 ALTER TABLE `a_state` DISABLE KEYS */;
INSERT INTO `a_state` VALUES (1,'Գրանցված'),(2,'Սևագիր');
/*!40000 ALTER TABLE `a_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_store`
--

DROP TABLE IF EXISTS `a_store`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_store` (
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
  PRIMARY KEY (`f_id`),
  KEY `fk_store_idx` (`f_store`),
  KEY `fk_goods_idx` (`f_goods`),
  KEY `fk_store_type_idx` (`f_type`),
  CONSTRAINT `fk_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_store_type` FOREIGN KEY (`f_type`) REFERENCES `a_type_sign` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_store`
--

LOCK TABLES `a_store` WRITE;
/*!40000 ALTER TABLE `a_store` DISABLE KEYS */;
INSERT INTO `a_store` VALUES ('5f96adbe-67ad-11ea-85d4-1078d2d2b808','5f9108fe-67ad-11ea-85d4-1078d2d2b808',3,1,2,1.0000,250.00,250.00,'5f96adbe-67ad-11ea-85d4-1078d2d2b808','5f9108fe-67ad-11ea-85d4-1078d2d2b808',1,NULL),('62a8524d-682c-11ea-85d4-1078d2d2b808','cd798be8-682b-11ea-85d4-1078d2d2b808',2,-1,37,0.8000,750.00,600.00,'c0c3c746-67b3-11ea-85d4-1078d2d2b808','c0bb7b44-67b3-11ea-85d4-1078d2d2b808',4,'62a4972c-682c-11ea-85d4-1078d2d2b808'),('95dd58fb-67ad-11ea-85d4-1078d2d2b808','95d6974d-67ad-11ea-85d4-1078d2d2b808',3,-1,2,0.5000,250.00,125.00,'5f96adbe-67ad-11ea-85d4-1078d2d2b808','5f9108fe-67ad-11ea-85d4-1078d2d2b808',3,'95d9f6b7-67ad-11ea-85d4-1078d2d2b808'),('967682f6-789a-11ea-805f-1078d2d2b808','96447164-789a-11ea-805f-1078d2d2b808',3,1,3,2.0000,200.00,400.00,'967682f6-789a-11ea-805f-1078d2d2b808','96447164-789a-11ea-805f-1078d2d2b808',1,NULL),('acbd0fa0-67b3-11ea-85d4-1078d2d2b808','acb74182-67b3-11ea-85d4-1078d2d2b808',3,1,2,10.0000,250.00,2500.00,'acbd0fa0-67b3-11ea-85d4-1078d2d2b808','acb74182-67b3-11ea-85d4-1078d2d2b808',1,NULL),('b26a7684-7b03-11ea-a303-54ab3a2a8093','b216626f-7b03-11ea-a303-54ab3a2a8093',3,1,4,1.0000,200.00,200.00,'b26a7684-7b03-11ea-a303-54ab3a2a8093','b216626f-7b03-11ea-a303-54ab3a2a8093',1,NULL),('b28549f6-7b03-11ea-a303-54ab3a2a8093','b216626f-7b03-11ea-a303-54ab3a2a8093',3,1,2,2.0000,300.00,600.00,'b28549f6-7b03-11ea-a303-54ab3a2a8093','b216626f-7b03-11ea-a303-54ab3a2a8093',1,NULL),('c0c3c746-67b3-11ea-85d4-1078d2d2b808','c0bb7b44-67b3-11ea-85d4-1078d2d2b808',3,-1,2,6.0000,250.00,1500.00,'acbd0fa0-67b3-11ea-85d4-1078d2d2b808','acb74182-67b3-11ea-85d4-1078d2d2b808',8,'c0beef49-67b3-11ea-85d4-1078d2d2b808'),('c0c57323-67b3-11ea-85d4-1078d2d2b808','c0bb7b44-67b3-11ea-85d4-1078d2d2b808',2,1,37,2.0000,750.00,1500.00,'c0c3c746-67b3-11ea-85d4-1078d2d2b808','c0bb7b44-67b3-11ea-85d4-1078d2d2b808',8,NULL),('c1e3db89-67ad-11ea-85d4-1078d2d2b808','69085db3-67ad-11ea-85d4-1078d2d2b808',3,-1,2,0.5000,250.00,125.00,'5f96adbe-67ad-11ea-85d4-1078d2d2b808','5f9108fe-67ad-11ea-85d4-1078d2d2b808',3,'c1e05c79-67ad-11ea-85d4-1078d2d2b808'),('d879a563-67ad-11ea-85d4-1078d2d2b808','d8725f3f-67ad-11ea-85d4-1078d2d2b808',3,-1,2,0.5000,250.00,125.00,'5f96adbe-67ad-11ea-85d4-1078d2d2b808','5f9108fe-67ad-11ea-85d4-1078d2d2b808',2,'d87582f3-67ad-11ea-85d4-1078d2d2b808'),('d87bdd3a-67ad-11ea-85d4-1078d2d2b808','d8725f3f-67ad-11ea-85d4-1078d2d2b808',2,1,2,0.5000,250.00,125.00,'5f96adbe-67ad-11ea-85d4-1078d2d2b808','5f9108fe-67ad-11ea-85d4-1078d2d2b808',2,'d87582f3-67ad-11ea-85d4-1078d2d2b808');
/*!40000 ALTER TABLE `a_store` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_store_draft`
--

DROP TABLE IF EXISTS `a_store_draft`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_store_draft` (
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
  PRIMARY KEY (`f_id`),
  KEY `fk_store_draft_goods_idx` (`f_goods`),
  KEY `fk_store_draft_document_idx` (`f_document`),
  KEY `fk_store_draft_store_idx` (`f_store`),
  KEY `fk_store_draft_base_idx` (`f_base`),
  CONSTRAINT `fk_store_draft_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_store_draft`
--

LOCK TABLES `a_store_draft` WRITE;
/*!40000 ALTER TABLE `a_store_draft` DISABLE KEYS */;
INSERT INTO `a_store_draft` VALUES ('056d6c5f-6c73-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,130,34.0000,0.000,0.00,4,NULL),('1e7a1bff-6e1f-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,294,11.5000,0.000,0.00,4,NULL),('1ffdd359-6c7c-11ea-85d4-1078d2d2b808','a9a818b3-6c7b-11ea-85d4-1078d2d2b808',6,-1,14,2.0000,0.000,0.00,4,NULL),('208a6093-6a7f-11ea-85d4-1078d2d2b808','8388ca61-6a7d-11ea-85d4-1078d2d2b808',2,-1,229,0.2500,0.000,0.00,4,NULL),('24c23b9a-6c68-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,4,43.0000,0.000,0.00,4,NULL),('264e6ee0-6ce1-11ea-85d4-1078d2d2b808','72f95063-6c7d-11ea-85d4-1078d2d2b808',0,-1,86,3.0000,0.000,0.00,4,NULL),('26600b01-6ce1-11ea-85d4-1078d2d2b808','72f95063-6c7d-11ea-85d4-1078d2d2b808',0,-1,6,35.0000,0.000,0.00,4,NULL),('2755958d-6c16-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,385,2.0000,0.000,0.00,4,NULL),('2757c7da-6c16-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,290,4.0000,0.000,0.00,4,NULL),('284ca5ce-6a80-11ea-85d4-1078d2d2b808','8388ca61-6a7d-11ea-85d4-1078d2d2b808',2,-1,345,1.0000,0.000,0.00,4,NULL),('2dcecdbe-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,294,124.0000,0.000,0.00,4,NULL),('2dddf2f6-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,14,1288.0000,0.000,0.00,4,NULL),('2de0050b-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,313,86.0000,0.000,0.00,4,NULL),('2de1dde8-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,86,196.0000,0.000,0.00,4,NULL),('2df1cadf-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,6,280.0000,0.000,0.00,4,NULL),('2df3a3b5-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,43,7280.0000,0.000,0.00,4,NULL),('2df5761e-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,196,84.0000,0.000,0.00,4,NULL),('2e246460-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,4,2030.0000,0.000,0.00,4,NULL),('2e3ac5b5-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,380,28.0000,0.000,0.00,4,NULL),('2e3c7ae9-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,253,28.0000,0.000,0.00,4,NULL),('2e82109e-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,3,350.0000,0.000,0.00,4,NULL),('2e842420-6b96-11ea-85d4-1078d2d2b808','2dcbd005-6b96-11ea-85d4-1078d2d2b808',6,-1,347,14.0000,0.000,0.00,4,NULL),('3a5a539c-6c49-11ea-85d4-1078d2d2b808','3a569a96-6c49-11ea-85d4-1078d2d2b808',0,-1,294,36.0000,0.000,0.00,4,NULL),('3a9caf4f-6f2d-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,347,1.0000,0.000,0.00,4,NULL),('3c40ca62-6e1d-11ea-85d4-1078d2d2b808','a9a818b3-6c7b-11ea-85d4-1078d2d2b808',6,-1,86,5.0000,0.000,0.00,4,NULL),('3c547d29-6e1d-11ea-85d4-1078d2d2b808','a9a818b3-6c7b-11ea-85d4-1078d2d2b808',6,-1,253,1.0000,0.000,0.00,4,NULL),('43f6833a-6c5f-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,86,1.0000,0.000,0.00,4,NULL),('43f97277-6c5f-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,3,25.0000,0.000,0.00,4,NULL),('47ab98f7-6ea1-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,67,3.0000,0.000,0.00,4,NULL),('47afc0da-6ea1-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,307,2.0000,0.000,0.00,4,NULL),('4f9d073a-668d-11ea-85d4-1078d2d2b808','4f9aa853-668d-11ea-85d4-1078d2d2b808',1,-1,186,7.0000,0.000,0.00,4,NULL),('5f947e2b-67ad-11ea-85d4-1078d2d2b808','5f9108fe-67ad-11ea-85d4-1078d2d2b808',3,1,2,1.0000,250.000,250.00,1,'5f947e2b-67ad-11ea-85d4-1078d2d2b808'),('62a4972c-682c-11ea-85d4-1078d2d2b808','cd798be8-682b-11ea-85d4-1078d2d2b808',2,-1,37,0.8000,750.000,600.00,4,'62a4972c-682c-11ea-85d4-1078d2d2b808'),('64ab097a-6ce4-11ea-85d4-1078d2d2b808','72f95063-6c7d-11ea-85d4-1078d2d2b808',0,-1,14,3.0000,0.000,0.00,4,NULL),('6d2561eb-6a7c-11ea-85d4-1078d2d2b808','6d2236cb-6a7c-11ea-85d4-1078d2d2b808',4,-1,227,0.0630,0.000,0.00,4,NULL),('6e927fca-682c-11ea-85d4-1078d2d2b808','6e90194f-682c-11ea-85d4-1078d2d2b808',2,-1,37,0.5000,0.000,0.00,4,NULL),('72fbee1c-6c7d-11ea-85d4-1078d2d2b808','72f95063-6c7d-11ea-85d4-1078d2d2b808',0,-1,380,4.0000,0.000,0.00,4,NULL),('74504eca-6c15-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,14,60.0000,0.000,0.00,4,NULL),('74537afc-6c15-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,313,11.0000,0.000,0.00,4,NULL),('74558ab8-6c15-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,380,17.0000,0.000,0.00,4,NULL),('838b90cd-6a7d-11ea-85d4-1078d2d2b808','8388ca61-6a7d-11ea-85d4-1078d2d2b808',2,-1,358,1.0000,0.000,0.00,4,NULL),('8537a2d3-6c5d-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,352,1.0000,0.000,0.00,4,NULL),('8539d10e-6c5d-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,47,5.0000,0.000,0.00,4,NULL),('853b96d6-6c5d-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,138,2.0000,0.000,0.00,4,NULL),('87c9eb9b-6c6b-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,6,34.0000,0.000,0.00,4,NULL),('8c9b0af0-66ad-11ea-85d4-1078d2d2b808','4f9aa853-668d-11ea-85d4-1078d2d2b808',1,-1,375,0.0250,0.000,0.00,4,NULL),('8c9cd27a-66ad-11ea-85d4-1078d2d2b808','4f9aa853-668d-11ea-85d4-1078d2d2b808',1,-1,151,1.0000,0.000,0.00,4,NULL),('8c9e84d3-66ad-11ea-85d4-1078d2d2b808','4f9aa853-668d-11ea-85d4-1078d2d2b808',1,-1,183,1.0000,0.000,0.00,4,NULL),('8ca47586-66ad-11ea-85d4-1078d2d2b808','8ca1c372-66ad-11ea-85d4-1078d2d2b808',2,-1,98,0.0050,0.000,0.00,4,NULL),('8ca663a0-66ad-11ea-85d4-1078d2d2b808','8ca1c372-66ad-11ea-85d4-1078d2d2b808',2,-1,66,0.0100,0.000,0.00,4,NULL),('95d9f6b7-67ad-11ea-85d4-1078d2d2b808','95d6974d-67ad-11ea-85d4-1078d2d2b808',3,-1,2,0.5000,250.000,125.00,3,'95d9f6b7-67ad-11ea-85d4-1078d2d2b808'),('965e7017-789a-11ea-805f-1078d2d2b808','96447164-789a-11ea-805f-1078d2d2b808',3,1,3,2.0000,200.000,400.00,1,'965e7017-789a-11ea-805f-1078d2d2b808'),('999ec7cc-6697-11ea-85d4-1078d2d2b808','4f9aa853-668d-11ea-85d4-1078d2d2b808',1,-1,166,1.0000,0.000,0.00,4,NULL),('9f21f315-6697-11ea-85d4-1078d2d2b808','4f9aa853-668d-11ea-85d4-1078d2d2b808',1,-1,155,1.0000,0.000,0.00,4,NULL),('a00706d3-6c5f-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,276,6.0000,0.000,0.00,4,NULL),('a96f56c8-6e68-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,313,3.2000,0.000,0.00,4,NULL),('a9ad3f14-6c7b-11ea-85d4-1078d2d2b808','a9a818b3-6c7b-11ea-85d4-1078d2d2b808',6,-1,294,2.0000,0.000,0.00,4,NULL),('acbab616-67b3-11ea-85d4-1078d2d2b808','acb74182-67b3-11ea-85d4-1078d2d2b808',3,1,2,10.0000,250.000,2500.00,1,'acbab616-67b3-11ea-85d4-1078d2d2b808'),('b2341814-7b03-11ea-a303-54ab3a2a8093','b216626f-7b03-11ea-a303-54ab3a2a8093',3,1,4,1.0000,200.000,200.00,1,'b2341814-7b03-11ea-a303-54ab3a2a8093'),('b24f5082-7b03-11ea-a303-54ab3a2a8093','b216626f-7b03-11ea-a303-54ab3a2a8093',3,1,2,2.0000,300.000,600.00,1,'b24f5082-7b03-11ea-a303-54ab3a2a8093'),('b38a65eb-6a7e-11ea-85d4-1078d2d2b808','b387ce4f-6a7e-11ea-85d4-1078d2d2b808',4,-1,227,0.1890,0.000,0.00,4,NULL),('b97b2b72-6ce2-11ea-85d4-1078d2d2b808','a9a818b3-6c7b-11ea-85d4-1078d2d2b808',6,-1,290,10.0000,0.000,0.00,4,NULL),('b97dbb3a-6ce2-11ea-85d4-1078d2d2b808','a9a818b3-6c7b-11ea-85d4-1078d2d2b808',6,-1,130,4.0000,0.000,0.00,4,NULL),('b9a655db-6ce2-11ea-85d4-1078d2d2b808','a9a818b3-6c7b-11ea-85d4-1078d2d2b808',6,-1,313,11.0000,0.000,0.00,4,NULL),('c0beef49-67b3-11ea-85d4-1078d2d2b808','c0bb7b44-67b3-11ea-85d4-1078d2d2b808',3,-1,2,6.0000,250.000,1500.00,8,'c0beef49-67b3-11ea-85d4-1078d2d2b808'),('c0c0b27e-67b3-11ea-85d4-1078d2d2b808','c0bb7b44-67b3-11ea-85d4-1078d2d2b808',2,1,37,2.0000,750.000,1500.00,8,NULL),('c1e05c79-67ad-11ea-85d4-1078d2d2b808','69085db3-67ad-11ea-85d4-1078d2d2b808',3,-1,2,0.5000,250.000,125.00,3,'c1e05c79-67ad-11ea-85d4-1078d2d2b808'),('c51ea5a7-6f2d-11ea-85d4-1078d2d2b808','c51a1c87-6f2d-11ea-85d4-1078d2d2b808',6,-1,14,15.0000,0.000,0.00,4,NULL),('d053feb4-6c57-11ea-85d4-1078d2d2b808','3a569a96-6c49-11ea-85d4-1078d2d2b808',0,-1,86,10.0000,0.000,0.00,4,NULL),('d056df5d-6c57-11ea-85d4-1078d2d2b808','3a569a96-6c49-11ea-85d4-1078d2d2b808',0,-1,380,2.0000,0.000,0.00,4,NULL),('d058b598-6c57-11ea-85d4-1078d2d2b808','3a569a96-6c49-11ea-85d4-1078d2d2b808',0,-1,253,4.0000,0.000,0.00,4,NULL),('d05ad454-6c57-11ea-85d4-1078d2d2b808','3a569a96-6c49-11ea-85d4-1078d2d2b808',0,-1,6,70.0000,0.000,0.00,4,NULL),('d79c96a7-6e1f-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,14,59.0000,0.000,0.00,4,NULL),('d87582f3-67ad-11ea-85d4-1078d2d2b808','d8725f3f-67ad-11ea-85d4-1078d2d2b808',2,1,2,0.5000,250.000,125.00,2,'d87582f3-67ad-11ea-85d4-1078d2d2b808'),('d8771767-67ad-11ea-85d4-1078d2d2b808','d8725f3f-67ad-11ea-85d4-1078d2d2b808',3,-1,2,0.5000,250.000,125.00,2,'d87582f3-67ad-11ea-85d4-1078d2d2b808'),('daac2c09-6c74-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,67,3.0000,0.000,0.00,4,NULL),('dfbd1ad5-6c66-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,294,3.0000,0.000,0.00,4,NULL),('f0703196-6e6c-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,380,2.9000,0.000,0.00,4,NULL),('f9dc00c9-6e6b-11ea-85d4-1078d2d2b808','1e76132d-6e1f-11ea-85d4-1078d2d2b808',6,-1,4,40.5000,0.000,0.00,4,NULL),('fc13185a-6c5f-11ea-85d4-1078d2d2b808','744cb1bc-6c15-11ea-85d4-1078d2d2b808',6,-1,371,6.0000,0.000,0.00,4,NULL);
/*!40000 ALTER TABLE `a_store_draft` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_store_inventory`
--

DROP TABLE IF EXISTS `a_store_inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_store_inventory` (
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
  CONSTRAINT `fk_si_goosd` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_si_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_store_inventory`
--

LOCK TABLES `a_store_inventory` WRITE;
/*!40000 ALTER TABLE `a_store_inventory` DISABLE KEYS */;
/*!40000 ALTER TABLE `a_store_inventory` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_type`
--

DROP TABLE IF EXISTS `a_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_type` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(256) DEFAULT NULL,
  `f_counter` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_type`
--

LOCK TABLES `a_type` WRITE;
/*!40000 ALTER TABLE `a_type` DISABLE KEYS */;
INSERT INTO `a_type` VALUES (1,'Պահեստի մուտք',0),(2,'Պահեստի ելք',0),(3,'Պահեստի տեղաշարժ',0),(4,'Պահեստի մնացորդ',0),(5,'Դրամարկղի փասթաթուղթ',74),(6,'Կոմպլեկտավորում',9),(7,'Աշխատավարձ',6);
/*!40000 ALTER TABLE `a_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `a_type_sign`
--

DROP TABLE IF EXISTS `a_type_sign`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `a_type_sign` (
  `f_id` smallint(6) NOT NULL,
  `f_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `a_type_sign`
--

LOCK TABLES `a_type_sign` WRITE;
/*!40000 ALTER TABLE `a_type_sign` DISABLE KEYS */;
INSERT INTO `a_type_sign` VALUES (-1,'Ելք'),(1,'Մուտք');
/*!40000 ALTER TABLE `a_type_sign` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `b_car`
--

DROP TABLE IF EXISTS `b_car`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `b_car` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_car` int(11) DEFAULT NULL,
  `f_costumer` int(11) DEFAULT NULL,
  `f_govnumber` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=26 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `b_car`
--

LOCK TABLES `b_car` WRITE;
/*!40000 ALTER TABLE `b_car` DISABLE KEYS */;
INSERT INTO `b_car` VALUES (22,1,32,'QWE'),(23,4,33,'KU888LL'),(24,28,34,'23EE432'),(25,25,35,'12QQ333');
/*!40000 ALTER TABLE `b_car` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `b_car_orders`
--

DROP TABLE IF EXISTS `b_car_orders`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `b_car_orders` (
  `f_order` char(36) NOT NULL,
  `f_car` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_order`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `b_car_orders`
--

LOCK TABLES `b_car_orders` WRITE;
/*!40000 ALTER TABLE `b_car_orders` DISABLE KEYS */;
INSERT INTO `b_car_orders` VALUES ('1f067601-6a80-11ea-85d4-1078d2d2b808',25),('233f4d1d-6a7c-11ea-85d4-1078d2d2b808',22),('24bc9675-6a7f-11ea-85d4-1078d2d2b808',24),('2a25b520-6a77-11ea-85d4-1078d2d2b808',22),('672feb0d-629e-11ea-b2dd-1078d2d2b808',20),('740c0b24-6a7d-11ea-85d4-1078d2d2b808',23),('83c9b806-6a7c-11ea-85d4-1078d2d2b808',23),('a100954f-65b8-11ea-81d5-1078d2d2b808',21),('aa5ccdfd-6a7d-11ea-85d4-1078d2d2b808',24),('bc4414d7-6a7f-11ea-85d4-1078d2d2b808',25),('dece8ddc-6a7e-11ea-85d4-1078d2d2b808',24);
/*!40000 ALTER TABLE `b_car_orders` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `b_card_types`
--

DROP TABLE IF EXISTS `b_card_types`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `b_card_types` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `b_card_types`
--

LOCK TABLES `b_card_types` WRITE;
/*!40000 ALTER TABLE `b_card_types` DISABLE KEYS */;
INSERT INTO `b_card_types` VALUES (1,'Զեղչ տոկոսով'),(2,'Զեղչ գումար'),(3,'Հաճախորդի գնումներ'),(4,'Կուտակային'),(5,'Ավտոմատ զեղչ տոկոսով');
/*!40000 ALTER TABLE `b_card_types` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `b_cards_discount`
--

DROP TABLE IF EXISTS `b_cards_discount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `b_cards_discount` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_mode` smallint(6) DEFAULT NULL,
  `f_value` decimal(6,3) DEFAULT NULL,
  `f_code` tinytext CHARACTER SET latin1,
  `f_client` int(11) DEFAULT NULL,
  `f_datestart` date DEFAULT NULL,
  `f_dateend` date DEFAULT NULL,
  `f_active` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `b_cards_discount`
--

LOCK TABLES `b_cards_discount` WRITE;
/*!40000 ALTER TABLE `b_cards_discount` DISABLE KEYS */;
INSERT INTO `b_cards_discount` VALUES (1,1,0.010,'54847',16,'2020-03-17','2030-03-15',1),(2,2,0.100,'2255',17,'2020-03-17','2030-03-15',1),(3,5,100.000,'auto_QWE',32,'2020-03-20','2030-01-27',1),(4,5,100.000,'auto_KU888LL',33,'2020-03-20','2030-01-27',1),(5,5,100.000,'auto_23EE432',34,'2020-03-20','2030-01-27',1),(6,5,100.000,'auto_12QQ333',35,'2020-03-20','2030-01-27',1),(7,3,11.000,'VISIT',36,'2020-03-20','2030-03-18',1);
/*!40000 ALTER TABLE `b_cards_discount` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `b_clients_debts`
--

DROP TABLE IF EXISTS `b_clients_debts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `b_clients_debts` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_date` date DEFAULT NULL,
  `f_costumer` int(11) DEFAULT NULL,
  `f_order` char(36) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_cash` char(36) DEFAULT NULL,
  `f_govnumer` tinytext,
  `f_govnumber` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `b_clients_debts`
--

LOCK TABLES `b_clients_debts` WRITE;
/*!40000 ALTER TABLE `b_clients_debts` DISABLE KEYS */;
/*!40000 ALTER TABLE `b_clients_debts` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `b_history`
--

DROP TABLE IF EXISTS `b_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `b_history` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_order` char(36) DEFAULT NULL,
  `f_type` smallint(6) DEFAULT NULL,
  `f_card` int(11) DEFAULT NULL,
  `f_value` decimal(6,3) DEFAULT NULL,
  `f_data` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `b_history`
--

LOCK TABLES `b_history` WRITE;
/*!40000 ALTER TABLE `b_history` DISABLE KEYS */;
/*!40000 ALTER TABLE `b_history` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_goods`
--

DROP TABLE IF EXISTS `c_goods`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_goods` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_supplier` int(11) DEFAULT NULL,
  `f_group` int(11) DEFAULT NULL,
  `f_unit` int(11) DEFAULT NULL,
  `f_name` tinytext,
  `f_salePrice` decimal(12,3) DEFAULT NULL,
  `f_lastinputprice` decimal(14,3) DEFAULT NULL,
  `f_lowlevel` decimal(12,4) DEFAULT NULL,
  `f_group1` int(11) DEFAULT NULL,
  `f_group2` int(11) DEFAULT NULL,
  `f_group3` int(11) DEFAULT NULL,
  `f_group4` int(11) DEFAULT NULL,
  `f_saleprice2` decimal(14,2) DEFAULT NULL,
  `f_enabled` smallint(6) DEFAULT NULL,
  `f_scancode` char(64) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_group_idx` (`f_group`),
  KEY `fk_unit_idx` (`f_unit`),
  CONSTRAINT `fk_group` FOREIGN KEY (`f_group`) REFERENCES `c_groups` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_unit` FOREIGN KEY (`f_unit`) REFERENCES `c_units` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=403 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_goods`
--

LOCK TABLES `c_goods` WRITE;
/*!40000 ALTER TABLE `c_goods` DISABLE KEYS */;
INSERT INTO `c_goods` VALUES (2,0,2,1,'Կարտոֆիլ',20.000,300.000,0.0000,0,0,0,0,20.00,1,'3000240222'),(3,0,2,1,'Գազար',30.000,200.000,0.0000,0,0,0,0,30.00,1,'0100530157'),(4,0,2,1,'Լոլիկ',40.000,200.000,0.0000,0,0,0,0,40.00,1,'0400135140'),(5,NULL,2,1,'Սմբուկ',50.000,NULL,NULL,NULL,NULL,NULL,NULL,50.00,1,'5'),(6,0,2,1,'Բրոկոլի',60.000,0.000,0.0000,0,0,0,0,60.00,1,'6'),(7,0,2,1,'վարունգ',70.000,0.000,0.0000,0,0,0,0,70.00,1,'7'),(8,NULL,2,1,'Սոխ',80.000,NULL,NULL,NULL,NULL,NULL,NULL,80.00,1,'8'),(9,NULL,2,1,'Սունկ շամպինյոն',90.000,NULL,NULL,NULL,NULL,NULL,NULL,90.00,1,'9'),(10,NULL,2,1,'Սխտոր',100.000,NULL,NULL,NULL,NULL,NULL,NULL,100.00,1,'10'),(11,0,2,1,'Ծաղկակաղամբ',110.000,0.000,0.0000,0,0,0,0,110.00,1,'11'),(12,NULL,18,3,'Յան',120.000,NULL,NULL,NULL,NULL,NULL,NULL,120.00,1,'12'),(13,NULL,6,1,'Բրնձի լապշա',130.000,NULL,NULL,NULL,NULL,NULL,NULL,130.00,1,'13'),(14,NULL,2,1,'Բազուկ',140.000,NULL,NULL,NULL,NULL,NULL,NULL,140.00,1,'14'),(15,NULL,9,1,'Հավի բուդ',150.000,NULL,NULL,NULL,NULL,NULL,NULL,150.00,1,'15'),(16,NULL,9,1,'Հավի դոշ',160.000,NULL,NULL,NULL,NULL,NULL,NULL,160.00,1,'16'),(17,NULL,9,1,'Հավի թևիկ',170.000,NULL,NULL,NULL,NULL,NULL,NULL,170.00,1,'17'),(18,NULL,9,1,'Տավարի ֆիլե',180.000,NULL,NULL,NULL,NULL,NULL,NULL,180.00,1,'18'),(19,NULL,9,1,'Տավարի փափուկ միս',190.000,NULL,NULL,NULL,NULL,NULL,NULL,190.00,1,'19'),(20,NULL,9,1,'Տավարի բուդ',200.000,NULL,NULL,NULL,NULL,NULL,NULL,200.00,1,'20'),(21,NULL,9,1,'Տավարի սիրտ ու  թոք',210.000,NULL,NULL,NULL,NULL,NULL,NULL,210.00,1,'21'),(22,NULL,9,1,'Խոզի փափուկ միս',220.000,NULL,NULL,NULL,NULL,NULL,NULL,220.00,1,'22'),(23,NULL,9,1,'Ձուկ  սիգա',230.000,NULL,NULL,NULL,NULL,NULL,NULL,230.00,1,'23'),(24,NULL,9,1,'Խոզի մատ',240.000,NULL,NULL,NULL,NULL,NULL,NULL,240.00,1,'24'),(25,NULL,8,1,'Հ. կոկոսի քերուկ',250.000,NULL,NULL,NULL,NULL,NULL,NULL,250.00,1,'25'),(26,NULL,4,3,'կանաչի',260.000,NULL,NULL,NULL,NULL,NULL,NULL,260.00,1,'26'),(27,NULL,4,1,'Մառոլ',270.000,NULL,NULL,NULL,NULL,NULL,NULL,270.00,1,'27'),(28,NULL,2,3,'Կուկուռուզ',280.000,NULL,NULL,NULL,NULL,NULL,NULL,280.00,1,'28'),(29,NULL,6,1,'Գերկուլես',290.000,NULL,NULL,NULL,NULL,NULL,NULL,290.00,1,'29'),(30,NULL,2,3,'Կծու բիբար',300.000,NULL,NULL,NULL,NULL,NULL,NULL,300.00,1,'30'),(31,NULL,3,1,'Զեյթուն',310.000,NULL,NULL,NULL,NULL,NULL,NULL,310.00,1,'31'),(32,NULL,7,1,'Թթվասեր',320.000,NULL,NULL,NULL,NULL,NULL,NULL,320.00,1,'32'),(33,NULL,7,1,'Պանիր լոռի',330.000,NULL,NULL,NULL,NULL,NULL,NULL,330.00,1,'33'),(34,NULL,7,1,'Պանիր Ֆետա',340.000,NULL,NULL,NULL,NULL,NULL,NULL,340.00,1,'34'),(35,NULL,7,1,'Մոցառելլա',350.000,NULL,NULL,NULL,NULL,NULL,NULL,350.00,1,'35'),(36,NULL,7,1,'Կարագ',360.000,NULL,NULL,NULL,NULL,NULL,NULL,360.00,1,'36'),(37,0,7,1,'Մածուն',370.000,0.000,0.0000,0,0,0,0,370.00,1,'40001509'),(38,NULL,7,3,'ձու',380.000,NULL,NULL,NULL,NULL,NULL,NULL,380.00,1,'38'),(39,NULL,6,1,'Ոսպ',390.000,NULL,NULL,NULL,NULL,NULL,NULL,390.00,1,'39'),(40,NULL,6,1,'Կարմիր լոբի',400.000,NULL,NULL,NULL,NULL,NULL,NULL,400.00,1,'40'),(41,NULL,6,1,'Բլղուր',410.000,NULL,NULL,NULL,NULL,NULL,NULL,410.00,1,'41'),(42,NULL,2,1,'կանաչ լոբի',420.000,NULL,NULL,NULL,NULL,NULL,NULL,420.00,1,'42'),(43,NULL,2,1,'Կաբաչկի',430.000,NULL,NULL,NULL,NULL,NULL,NULL,430.00,1,'43'),(44,NULL,5,1,'Նարինջ',440.000,NULL,NULL,NULL,NULL,NULL,NULL,440.00,1,'44'),(45,NULL,5,1,'Խնձոր',450.000,NULL,NULL,NULL,NULL,NULL,NULL,450.00,1,'45'),(46,NULL,5,1,'Խաղող',460.000,NULL,NULL,NULL,NULL,NULL,NULL,460.00,1,'46'),(47,NULL,5,1,'տանձ',470.000,NULL,NULL,NULL,NULL,NULL,NULL,470.00,1,'47'),(48,NULL,5,1,'Սերկեֆիլ',480.000,NULL,NULL,NULL,NULL,NULL,NULL,480.00,1,'48'),(49,NULL,5,1,'Ելակ',490.000,NULL,NULL,NULL,NULL,NULL,NULL,490.00,1,'49'),(50,NULL,9,3,'Նրբերշիկ կանադա',500.000,NULL,NULL,NULL,NULL,NULL,NULL,500.00,1,'50'),(51,NULL,3,1,'Կանաչ ոլոռ',510.000,NULL,NULL,NULL,NULL,NULL,NULL,510.00,1,'51'),(52,NULL,3,1,'Տոմատ',520.000,NULL,NULL,NULL,NULL,NULL,NULL,520.00,1,'52'),(53,NULL,9,3,'Փարդա',530.000,NULL,NULL,NULL,NULL,NULL,NULL,530.00,1,'53'),(54,NULL,6,1,'Վերմիշել',540.000,NULL,NULL,NULL,NULL,NULL,NULL,540.00,1,'54'),(55,NULL,6,1,'Բրինձ',550.000,NULL,NULL,NULL,NULL,NULL,NULL,550.00,1,'55'),(56,NULL,6,1,'Հնդկաձավար',560.000,NULL,NULL,NULL,NULL,NULL,NULL,560.00,1,'56'),(57,NULL,6,1,'Ձավար',570.000,NULL,NULL,NULL,NULL,NULL,NULL,570.00,1,'57'),(58,NULL,6,1,'պաքսիմատ',580.000,NULL,NULL,NULL,NULL,NULL,NULL,580.00,1,'58'),(59,NULL,5,1,'Ծիրան',590.000,NULL,NULL,NULL,NULL,NULL,NULL,590.00,1,'59'),(60,NULL,7,1,'մայոնեզ',600.000,NULL,NULL,NULL,NULL,NULL,NULL,600.00,1,'60'),(61,NULL,6,1,'Ալյուր',610.000,NULL,NULL,NULL,NULL,NULL,NULL,610.00,1,'61'),(62,NULL,9,1,'Սարդելկա',620.000,NULL,NULL,NULL,NULL,NULL,NULL,620.00,1,'62'),(63,NULL,4,1,'Եղինջ',630.000,NULL,NULL,NULL,NULL,NULL,NULL,630.00,1,'63'),(64,NULL,9,3,'Կռաբ',640.000,NULL,NULL,NULL,NULL,NULL,NULL,640.00,1,'64'),(65,NULL,9,1,'Նրբերշիկ',650.000,NULL,NULL,NULL,NULL,NULL,NULL,650.00,1,'65'),(66,NULL,6,1,'շաքարավազ',660.000,NULL,NULL,NULL,NULL,NULL,NULL,660.00,1,'66'),(67,NULL,2,1,'Դդում',670.000,NULL,NULL,NULL,NULL,NULL,NULL,670.00,1,'67'),(68,NULL,18,3,'Հյութ սանդորա0.25',680.000,NULL,NULL,NULL,NULL,NULL,NULL,680.00,1,'68'),(69,NULL,7,1,'Շաքարի փոշի',690.000,NULL,NULL,NULL,NULL,NULL,NULL,690.00,1,'69'),(70,NULL,6,1,'Մակարոն',700.000,NULL,NULL,NULL,NULL,NULL,NULL,700.00,1,'70'),(71,NULL,2,1,'Պեկինյան կաղամբ',710.000,NULL,NULL,NULL,NULL,NULL,NULL,710.00,1,'71'),(72,NULL,6,1,'Բրինձ էժան',720.000,NULL,NULL,NULL,NULL,NULL,NULL,720.00,1,'72'),(73,NULL,6,1,'սպագետի',730.000,NULL,NULL,NULL,NULL,NULL,NULL,730.00,1,'73'),(74,NULL,6,1,'հաճար',740.000,NULL,NULL,NULL,NULL,NULL,NULL,740.00,1,'74'),(75,NULL,6,1,'Ոլոռ',750.000,NULL,NULL,NULL,NULL,NULL,NULL,750.00,1,'75'),(76,NULL,3,1,'Եգիպացորեն',760.000,NULL,NULL,NULL,NULL,NULL,NULL,760.00,1,'76'),(77,NULL,6,4,'ձեթ',770.000,NULL,NULL,NULL,NULL,NULL,NULL,770.00,1,'77'),(78,NULL,3,4,'Լիմոնի էքստ',780.000,NULL,NULL,NULL,NULL,NULL,NULL,780.00,1,'78'),(79,NULL,8,1,'Հ. ռոզմարի',790.000,NULL,NULL,NULL,NULL,NULL,NULL,790.00,1,'79'),(80,NULL,3,1,'կետչուպ',800.000,NULL,NULL,NULL,NULL,NULL,NULL,800.00,1,'80'),(81,NULL,9,1,'խոզապուխտ',810.000,NULL,NULL,NULL,NULL,NULL,NULL,810.00,1,'81'),(82,NULL,6,1,'Խմորի վիտամին',820.000,NULL,NULL,NULL,NULL,NULL,NULL,820.00,1,'82'),(83,NULL,6,1,'դրոժ',830.000,NULL,NULL,NULL,NULL,NULL,NULL,830.00,1,'83'),(84,NULL,5,1,'Կիվի',840.000,NULL,NULL,NULL,NULL,NULL,NULL,840.00,1,'84'),(85,NULL,6,4,'Սոյայի սոուս',850.000,NULL,NULL,NULL,NULL,NULL,NULL,850.00,1,'85'),(86,NULL,2,1,'Բամիա',860.000,NULL,NULL,NULL,NULL,NULL,NULL,860.00,1,'86'),(87,NULL,6,4,'ժիդկիյ դիմ',870.000,NULL,NULL,NULL,NULL,NULL,NULL,870.00,1,'87'),(88,NULL,8,1,'Հ. սալորաչիր',880.000,NULL,NULL,NULL,NULL,NULL,NULL,880.00,1,'88'),(89,NULL,9,1,'բաստուրմա',890.000,NULL,NULL,NULL,NULL,NULL,NULL,890.00,1,'89'),(90,NULL,6,4,'քացախ',900.000,NULL,NULL,NULL,NULL,NULL,NULL,900.00,1,'90'),(91,NULL,8,1,'Հ. կորեական աղ',910.000,NULL,NULL,NULL,NULL,NULL,NULL,910.00,1,'91'),(92,NULL,4,3,'Թարխուն',920.000,NULL,NULL,NULL,NULL,NULL,NULL,920.00,1,'92'),(93,NULL,8,1,'Հ. լիմոնի աղ',930.000,NULL,NULL,NULL,NULL,NULL,NULL,930.00,1,'93'),(94,NULL,8,1,'Հ. ծիտրոն',940.000,NULL,NULL,NULL,NULL,NULL,NULL,940.00,1,'94'),(95,NULL,6,1,'Մարգարին',950.000,NULL,NULL,NULL,NULL,NULL,NULL,950.00,1,'95'),(96,NULL,8,1,'Հ. սև բիբար',960.000,NULL,NULL,NULL,NULL,NULL,NULL,960.00,1,'96'),(97,NULL,8,1,'Հ. վանիլին',970.000,NULL,NULL,NULL,NULL,NULL,NULL,970.00,1,'97'),(98,NULL,22,1,'Nescafe 100gr',980.000,NULL,NULL,NULL,NULL,NULL,NULL,980.00,1,'98'),(99,NULL,6,1,'Սոդա',990.000,NULL,NULL,NULL,NULL,NULL,NULL,990.00,1,'99'),(100,NULL,6,1,'Մանի',1000.000,NULL,NULL,NULL,NULL,NULL,NULL,1000.00,1,'100'),(101,NULL,8,1,'Հ. քունջութ',1010.000,NULL,NULL,NULL,NULL,NULL,NULL,1010.00,1,'101'),(102,NULL,18,3,'Մասուր',1020.000,NULL,NULL,NULL,NULL,NULL,NULL,1020.00,1,'102'),(103,NULL,8,1,'Հ. բահար',1030.000,NULL,NULL,NULL,NULL,NULL,NULL,1030.00,1,'103'),(104,NULL,8,1,'Հ. դարչին',1040.000,NULL,NULL,NULL,NULL,NULL,NULL,1040.00,1,'104'),(105,NULL,9,1,'Խոզի չալաղաջ',1050.000,NULL,NULL,NULL,NULL,NULL,NULL,1050.00,1,'105'),(106,NULL,6,1,'սուրճ',1060.000,NULL,NULL,NULL,NULL,NULL,NULL,1060.00,1,'106'),(107,NULL,8,1,'աղ',1070.000,NULL,NULL,NULL,NULL,NULL,NULL,1070.00,1,'107'),(108,NULL,4,3,'Դանդուռ',1080.000,NULL,NULL,NULL,NULL,NULL,NULL,1080.00,1,'108'),(109,NULL,8,1,'Հ. կարմիր  բիբար',1090.000,NULL,NULL,NULL,NULL,NULL,NULL,1090.00,1,'109'),(110,NULL,18,3,'Լիմոնադ ՌՌՌ կապռոն',1100.000,NULL,NULL,NULL,NULL,NULL,NULL,1100.00,1,'110'),(111,NULL,18,3,'Ջուր ապարան',1110.000,NULL,NULL,NULL,NULL,NULL,NULL,1110.00,1,'111'),(112,NULL,8,1,'հատիկ պղպեղ',1120.000,NULL,NULL,NULL,NULL,NULL,NULL,1120.00,1,'112'),(113,NULL,8,1,'Հ. օրեգոնո',1130.000,NULL,NULL,NULL,NULL,NULL,NULL,1130.00,1,'113'),(114,NULL,8,1,'Հ. զիռա',1140.000,NULL,NULL,NULL,NULL,NULL,NULL,1140.00,1,'114'),(115,NULL,8,1,'Հ. զաֆռան',1150.000,NULL,NULL,NULL,NULL,NULL,NULL,1150.00,1,'115'),(116,NULL,12,3,'Կռուասան',1160.000,NULL,NULL,NULL,NULL,NULL,NULL,1160.00,1,'116'),(117,NULL,8,1,'Հ.լավր.լիստ',1170.000,NULL,NULL,NULL,NULL,NULL,NULL,1170.00,1,'117'),(118,NULL,8,1,'քիմիոն',1180.000,NULL,NULL,NULL,NULL,NULL,NULL,1180.00,1,'118'),(119,NULL,8,1,'սխտորի փոշի',1190.000,NULL,NULL,NULL,NULL,NULL,NULL,1190.00,1,'119'),(120,NULL,9,1,'Ձուկ իշխան',1200.000,NULL,NULL,NULL,NULL,NULL,NULL,1200.00,1,'120'),(121,NULL,8,3,'բուլյոն',1210.000,NULL,NULL,NULL,NULL,NULL,NULL,1210.00,1,'121'),(122,NULL,9,3,'Կիսաֆաբրիկատ',1220.000,NULL,NULL,NULL,NULL,NULL,NULL,1220.00,1,'122'),(123,NULL,8,1,'Հ. ուրց',1230.000,NULL,NULL,NULL,NULL,NULL,NULL,1230.00,1,'123'),(124,NULL,6,1,'սիսեռ',1240.000,NULL,NULL,NULL,NULL,NULL,NULL,1240.00,1,'124'),(125,NULL,2,1,'Կարմիր բիբար',1250.000,NULL,NULL,NULL,NULL,NULL,NULL,1250.00,1,'125'),(126,NULL,8,1,'Հ. բարբարիս',1260.000,NULL,NULL,NULL,NULL,NULL,NULL,1260.00,1,'126'),(127,NULL,6,4,'Զեյթունի ձեթ',1270.000,NULL,NULL,NULL,NULL,NULL,NULL,1270.00,1,'127'),(128,NULL,8,1,'Պեստո սոուս',1280.000,NULL,NULL,NULL,NULL,NULL,NULL,1280.00,1,'128'),(129,NULL,2,3,'սպանախ',1290.000,NULL,NULL,NULL,NULL,NULL,NULL,1290.00,1,'129'),(130,NULL,2,1,'կանաչ բիբար',1300.000,NULL,NULL,NULL,NULL,NULL,NULL,1300.00,1,'130'),(131,NULL,9,3,'Հավի բդիկ',1310.000,NULL,NULL,NULL,NULL,NULL,NULL,1310.00,1,'131'),(132,NULL,8,1,'Հ. աջիկա',1320.000,NULL,NULL,NULL,NULL,NULL,NULL,1320.00,1,'132'),(133,NULL,7,4,'Սլիվկի',1330.000,NULL,NULL,NULL,NULL,NULL,NULL,1330.00,1,'133'),(134,NULL,18,3,'Հյութ արտֆուդ',1340.000,NULL,NULL,NULL,NULL,NULL,NULL,1340.00,1,'134'),(135,NULL,9,1,'Խեցգենի միս',1350.000,NULL,NULL,NULL,NULL,NULL,NULL,1350.00,1,'135'),(136,NULL,5,1,'Հոն',1360.000,NULL,NULL,NULL,NULL,NULL,NULL,1360.00,1,'136'),(137,NULL,18,3,'Տոմատի հյութ RRR',1370.000,NULL,NULL,NULL,NULL,NULL,NULL,1370.00,1,'137'),(138,NULL,5,1,'Ֆրեշի միրգ',1380.000,NULL,NULL,NULL,NULL,NULL,NULL,1380.00,1,'138'),(139,NULL,7,1,'Յուղ',1390.000,NULL,NULL,NULL,NULL,NULL,NULL,1390.00,1,'139'),(140,NULL,12,1,'Հրուշակեղեն',1400.000,NULL,NULL,NULL,NULL,NULL,NULL,1400.00,1,'140'),(141,NULL,3,1,'Սունկ մարին.',1410.000,NULL,NULL,NULL,NULL,NULL,NULL,1410.00,1,'141'),(142,NULL,22,3,'Տաք շոկոլադ',1420.000,NULL,NULL,NULL,NULL,NULL,NULL,1420.00,1,'142'),(143,NULL,18,3,'Հյ. մանգո',1430.000,NULL,NULL,NULL,NULL,NULL,NULL,1430.00,1,'143'),(144,NULL,5,1,'Շլոր',1440.000,NULL,NULL,NULL,NULL,NULL,NULL,1440.00,1,'144'),(145,NULL,5,1,'Անանաս',1450.000,NULL,NULL,NULL,NULL,NULL,NULL,1450.00,1,'145'),(146,NULL,14,3,'Կարելիա',1460.000,NULL,NULL,NULL,NULL,NULL,NULL,1460.00,1,'146'),(147,NULL,3,1,'Լոխում',1470.000,NULL,NULL,NULL,NULL,NULL,NULL,1470.00,1,'147'),(148,NULL,15,3,'Էկլիպս',1480.000,NULL,NULL,NULL,NULL,NULL,NULL,1480.00,1,'148'),(149,NULL,15,3,'Օր',1490.000,NULL,NULL,NULL,NULL,NULL,NULL,1490.00,1,'149'),(150,NULL,15,3,'Օրբիտ կրիստալ',1500.000,NULL,NULL,NULL,NULL,NULL,NULL,1500.00,1,'150'),(151,NULL,15,3,'Օրբիտ',1510.000,NULL,NULL,NULL,NULL,NULL,NULL,1510.00,1,'151'),(152,NULL,15,3,'Ռիգլի',1520.000,NULL,NULL,NULL,NULL,NULL,NULL,1520.00,1,'152'),(153,NULL,18,3,'7Ափ լցովի',1530.000,NULL,NULL,NULL,NULL,NULL,NULL,1530.00,1,'153'),(154,NULL,15,3,'Օր',1540.000,NULL,NULL,NULL,NULL,NULL,NULL,1540.00,1,'154'),(155,NULL,18,3,'Բոռժոմի',1550.000,NULL,NULL,NULL,NULL,NULL,NULL,1550.00,1,'155'),(156,NULL,6,1,'Չամիչ',1560.000,NULL,NULL,NULL,NULL,NULL,NULL,1560.00,1,'156'),(157,NULL,22,1,'Էսպրեսսո սուրճ',1570.000,NULL,NULL,NULL,NULL,NULL,NULL,1570.00,1,'157'),(158,NULL,18,3,'Ջուր լուսաղբյուր',1580.000,NULL,NULL,NULL,NULL,NULL,NULL,1580.00,1,'158'),(159,NULL,8,1,'Հ. սունելի',1590.000,NULL,NULL,NULL,NULL,NULL,NULL,1590.00,1,'159'),(160,NULL,8,1,'Հ. կարի',1600.000,NULL,NULL,NULL,NULL,NULL,NULL,1600.00,1,'160'),(161,NULL,9,1,'հավի ատվալկա',1610.000,NULL,NULL,NULL,NULL,NULL,NULL,1610.00,1,'161'),(162,NULL,9,1,'Հավի թև ուսիկ',1620.000,NULL,NULL,NULL,NULL,NULL,NULL,1620.00,1,'162'),(163,NULL,2,3,'քյարաուզ',1630.000,NULL,NULL,NULL,NULL,NULL,NULL,1630.00,1,'163'),(164,NULL,18,3,'Թան գազով',1640.000,NULL,NULL,NULL,NULL,NULL,NULL,1640.00,1,'164'),(165,NULL,18,3,'Տոմատի հյութ ոսկե ցլիկ',1650.000,NULL,NULL,NULL,NULL,NULL,NULL,1650.00,1,'165'),(166,NULL,18,3,'Լիմոնչելլա',1660.000,NULL,NULL,NULL,NULL,NULL,NULL,1660.00,1,'166'),(167,NULL,7,3,'Պաղպաղակ',1670.000,NULL,NULL,NULL,NULL,NULL,NULL,1670.00,1,'167'),(168,NULL,17,3,'Կա',1680.000,NULL,NULL,NULL,NULL,NULL,NULL,1680.00,1,'168'),(169,NULL,22,3,'Կաֆֆա',1690.000,NULL,NULL,NULL,NULL,NULL,NULL,1690.00,1,'169'),(170,NULL,17,3,'Kent HDI',1700.000,NULL,NULL,NULL,NULL,NULL,NULL,1700.00,1,'170'),(171,NULL,3,3,'Ջեմ',1710.000,NULL,NULL,NULL,NULL,NULL,NULL,1710.00,1,'171'),(172,NULL,8,1,'Էսենցիա կարագ',1720.000,NULL,NULL,NULL,NULL,NULL,NULL,1720.00,1,'172'),(173,NULL,19,3,'Marlboro',1730.000,NULL,NULL,NULL,NULL,NULL,NULL,1730.00,1,'173'),(174,NULL,19,3,'Marlboro lift',1740.000,NULL,NULL,NULL,NULL,NULL,NULL,1740.00,1,'174'),(175,NULL,19,3,'Parliament',1750.000,NULL,NULL,NULL,NULL,NULL,NULL,1750.00,1,'175'),(176,NULL,19,3,'parliament karat',1760.000,NULL,NULL,NULL,NULL,NULL,NULL,1760.00,1,'176'),(177,NULL,13,1,'Շոկոլադ',1770.000,NULL,NULL,NULL,NULL,NULL,NULL,1770.00,1,'177'),(178,NULL,8,4,'Էսենցիա',1780.000,NULL,NULL,NULL,NULL,NULL,NULL,1780.00,1,'178'),(179,NULL,19,3,'Bond slims',1790.000,NULL,NULL,NULL,NULL,NULL,NULL,1790.00,1,'179'),(180,NULL,12,3,'Սենդվիչի հաց հատ',1800.000,NULL,NULL,NULL,NULL,NULL,NULL,1800.00,1,'180'),(181,NULL,24,3,'MilkyWay',1810.000,NULL,NULL,NULL,NULL,NULL,NULL,1810.00,1,'181'),(182,NULL,24,3,'MM maxi',1820.000,NULL,NULL,NULL,NULL,NULL,NULL,1820.00,1,'182'),(183,NULL,18,3,'Ջուր ապարան 1լ',1830.000,NULL,NULL,NULL,NULL,NULL,NULL,1830.00,1,'183'),(184,NULL,24,3,'Պլիտկա Gc',1840.000,NULL,NULL,NULL,NULL,NULL,NULL,1840.00,1,'184'),(185,NULL,21,3,'Պեպսի ապակյա',1850.000,NULL,NULL,NULL,NULL,NULL,NULL,1850.00,1,'185'),(186,NULL,21,3,'Պեպսի 0.5',1860.000,NULL,NULL,NULL,NULL,NULL,NULL,1860.00,1,'186'),(187,NULL,21,1,'Պեպսի 1.00',1870.000,NULL,NULL,NULL,NULL,NULL,NULL,1870.00,1,'187'),(188,NULL,21,3,'Լիպտոն թեյ',1880.000,NULL,NULL,NULL,NULL,NULL,NULL,1880.00,1,'188'),(189,NULL,18,3,'ֆրեշ բառ',1890.000,NULL,NULL,NULL,NULL,NULL,NULL,1890.00,1,'189'),(190,NULL,22,3,'Թեյ',1900.000,NULL,NULL,NULL,NULL,NULL,NULL,1900.00,1,'190'),(191,NULL,22,3,'Nescafe',1910.000,NULL,NULL,NULL,NULL,NULL,NULL,1910.00,1,'191'),(192,NULL,22,1,'Սուրճ փարիզյան',1920.000,NULL,NULL,NULL,NULL,NULL,NULL,1920.00,1,'192'),(193,NULL,22,3,'Սառը սուրճ',1930.000,NULL,NULL,NULL,NULL,NULL,NULL,1930.00,1,'193'),(194,NULL,18,4,'Սառը',1940.000,NULL,NULL,NULL,NULL,NULL,NULL,1940.00,1,'194'),(195,NULL,3,1,'ֆրի',1950.000,NULL,NULL,NULL,NULL,NULL,NULL,1950.00,1,'195'),(196,NULL,2,1,'կաղամբ',1960.000,NULL,NULL,NULL,NULL,NULL,NULL,1960.00,1,'196'),(197,NULL,3,1,'սունկ մարինացված',1970.000,NULL,NULL,NULL,NULL,NULL,NULL,1970.00,1,'197'),(198,NULL,9,1,'սալյամի',1980.000,NULL,NULL,NULL,NULL,NULL,NULL,1980.00,1,'198'),(199,NULL,6,1,'սոյա',1990.000,NULL,NULL,NULL,NULL,NULL,NULL,1990.00,1,'199'),(200,NULL,24,3,'snickers mini',2000.000,NULL,NULL,NULL,NULL,NULL,NULL,2000.00,1,'200'),(201,NULL,24,3,'M&',2010.000,NULL,NULL,NULL,NULL,NULL,NULL,2010.00,1,'201'),(202,NULL,24,3,'Mi',2020.000,NULL,NULL,NULL,NULL,NULL,NULL,2020.00,1,'202'),(203,NULL,24,3,'Snickers maxi',2030.000,NULL,NULL,NULL,NULL,NULL,NULL,2030.00,1,'203'),(204,NULL,18,3,'Նատախտարի',2040.000,NULL,NULL,NULL,NULL,NULL,NULL,2040.00,1,'204'),(205,NULL,23,3,'Մաքուր ջուր ապակի գազով',2050.000,NULL,NULL,NULL,NULL,NULL,NULL,2050.00,1,'205'),(206,NULL,18,3,'Հյութ մաս',2060.000,NULL,NULL,NULL,NULL,NULL,NULL,2060.00,1,'206'),(207,NULL,18,3,'Հյութ մասուրի',2070.000,NULL,NULL,NULL,NULL,NULL,NULL,2070.00,1,'207'),(208,NULL,6,1,'Սպագետտի իտ',2080.000,NULL,NULL,NULL,NULL,NULL,NULL,2080.00,1,'208'),(209,NULL,23,3,'Ջերմուկ  1լ',2090.000,NULL,NULL,NULL,NULL,NULL,NULL,2090.00,1,'209'),(210,NULL,23,3,'Մաքուր ջուր',2100.000,NULL,NULL,NULL,NULL,NULL,NULL,2100.00,1,'210'),(211,NULL,7,3,'Անի թան',2110.000,NULL,NULL,NULL,NULL,NULL,NULL,2110.00,1,'211'),(212,NULL,7,1,'պանիր ֆենդել',2120.000,NULL,NULL,NULL,NULL,NULL,NULL,2120.00,1,'212'),(213,NULL,8,1,'Հ. համեմի սերմ',2130.000,NULL,NULL,NULL,NULL,NULL,NULL,2130.00,1,'213'),(214,NULL,8,1,'Հ. սխտորի փոշի',2140.000,NULL,NULL,NULL,NULL,NULL,NULL,2140.00,1,'214'),(215,NULL,16,3,'Ռուլետ  կակաո',2150.000,NULL,NULL,NULL,NULL,NULL,NULL,2150.00,1,'215'),(216,NULL,2,1,'Կանաչ լոլիկ',2160.000,NULL,NULL,NULL,NULL,NULL,NULL,2160.00,1,'216'),(217,NULL,18,3,'Ջուսինադ',2170.000,NULL,NULL,NULL,NULL,NULL,NULL,2170.00,1,'217'),(218,NULL,8,1,'Հ. քիմիոն',2180.000,NULL,NULL,NULL,NULL,NULL,NULL,2180.00,1,'218'),(219,NULL,16,3,'Ռուլետ  Մալինա բալ ելակ',2190.000,NULL,NULL,NULL,NULL,NULL,NULL,2190.00,1,'219'),(220,NULL,18,3,'Մաազա',2200.000,NULL,NULL,NULL,NULL,NULL,NULL,2200.00,1,'220'),(221,NULL,22,3,'Կ',2210.000,NULL,NULL,NULL,NULL,NULL,NULL,2210.00,1,'221'),(222,NULL,17,3,'Winston Xstile',2220.000,NULL,NULL,NULL,NULL,NULL,NULL,2220.00,1,'222'),(223,NULL,17,3,'Winston x sense',2230.000,NULL,NULL,NULL,NULL,NULL,NULL,2230.00,1,'223'),(224,NULL,17,3,'Winston BWS',2240.000,NULL,NULL,NULL,NULL,NULL,NULL,2240.00,1,'224'),(225,NULL,17,3,'Monte Carlo',2250.000,NULL,NULL,NULL,NULL,NULL,NULL,2250.00,1,'225'),(226,NULL,7,1,'Սպրեդ',2260.000,NULL,NULL,NULL,NULL,NULL,NULL,2260.00,1,'226'),(227,NULL,12,3,'հաց մատնաքաշ',2270.000,NULL,NULL,NULL,NULL,NULL,NULL,2270.00,1,'227'),(228,NULL,12,3,'հաց սեռի',2280.000,NULL,NULL,NULL,NULL,NULL,NULL,2280.00,1,'228'),(229,NULL,12,3,'լավաշ',2290.000,NULL,NULL,NULL,NULL,NULL,NULL,2290.00,1,'229'),(230,NULL,11,3,'դոմեստաս',2300.000,NULL,NULL,NULL,NULL,NULL,NULL,2300.00,1,'230'),(231,NULL,12,1,'Սենդվիչի հաց',2310.000,NULL,NULL,NULL,NULL,NULL,NULL,2310.00,1,'231'),(232,NULL,9,3,'սարդելկա դելիկատես',2320.000,NULL,NULL,NULL,NULL,NULL,NULL,2320.00,1,'232'),(233,NULL,6,1,'Պոպոկ',2330.000,NULL,NULL,NULL,NULL,NULL,NULL,2330.00,1,'233'),(234,NULL,11,5,'չոփիկ',2340.000,NULL,NULL,NULL,NULL,NULL,NULL,2340.00,1,'234'),(235,NULL,7,1,'պանիր էժան',2350.000,NULL,NULL,NULL,NULL,NULL,NULL,2350.00,1,'235'),(236,NULL,9,1,'Տավարի լեզու',2360.000,NULL,NULL,NULL,NULL,NULL,NULL,2360.00,1,'236'),(237,NULL,3,1,'Մեղր',2370.000,NULL,NULL,NULL,NULL,NULL,NULL,2370.00,1,'237'),(238,NULL,3,1,'Կոնյակ',2380.000,NULL,NULL,NULL,NULL,NULL,NULL,2380.00,1,'238'),(239,NULL,5,1,'միրգ',2390.000,NULL,NULL,NULL,NULL,NULL,NULL,2390.00,1,'239'),(240,NULL,11,3,'ձեռնոց',2400.000,NULL,NULL,NULL,NULL,NULL,NULL,2400.00,1,'240'),(241,NULL,11,3,'ցելոֆան միանգամյա',2410.000,NULL,NULL,NULL,NULL,NULL,NULL,2410.00,1,'241'),(242,NULL,11,3,'բաժակ միանգամյա',2420.000,NULL,NULL,NULL,NULL,NULL,NULL,2420.00,1,'242'),(243,NULL,9,1,'Գառի միս',2430.000,NULL,NULL,NULL,NULL,NULL,NULL,2430.00,1,'243'),(244,NULL,11,3,'պատարաքաղ միանգամյա',2440.000,NULL,NULL,NULL,NULL,NULL,NULL,2440.00,1,'244'),(245,NULL,9,3,'Նրբերշիկ տապակելու',2450.000,NULL,NULL,NULL,NULL,NULL,NULL,2450.00,1,'245'),(246,NULL,7,4,'կաթ',2460.000,NULL,NULL,NULL,NULL,NULL,NULL,2460.00,1,'246'),(247,NULL,18,3,'Հյութ ֆրուտինգ',2470.000,NULL,NULL,NULL,NULL,NULL,NULL,2470.00,1,'247'),(248,NULL,3,1,'Զեյթուն կորիզով',2480.000,NULL,NULL,NULL,NULL,NULL,NULL,2480.00,1,'248'),(249,NULL,9,1,'Քյաբաբի ֆարշ',2490.000,NULL,NULL,NULL,NULL,NULL,NULL,2490.00,1,'249'),(250,NULL,11,3,'գազ մաքրող հեղուկ',2500.000,NULL,NULL,NULL,NULL,NULL,NULL,2500.00,1,'250'),(251,NULL,7,1,'Կարագ զելանդական',2510.000,NULL,NULL,NULL,NULL,NULL,NULL,2510.00,1,'251'),(252,NULL,6,1,'Մակարոն իտալական',2520.000,NULL,NULL,NULL,NULL,NULL,NULL,2520.00,1,'252'),(253,NULL,2,3,'Բոլոկի',2530.000,NULL,NULL,NULL,NULL,NULL,NULL,2530.00,1,'253'),(254,NULL,2,1,'սամիթ',2540.000,NULL,NULL,NULL,NULL,NULL,NULL,2540.00,1,'254'),(255,NULL,7,1,'Թարմ մոցառելլա',2550.000,NULL,NULL,NULL,NULL,NULL,NULL,2550.00,1,'255'),(256,NULL,8,1,'Հ. շաֆրան',2560.000,NULL,NULL,NULL,NULL,NULL,NULL,2560.00,1,'256'),(257,NULL,2,1,'Ռեհան',2570.000,NULL,NULL,NULL,NULL,NULL,NULL,2570.00,1,'257'),(258,NULL,2,3,'կանաչ սոխ',2580.000,NULL,NULL,NULL,NULL,NULL,NULL,2580.00,1,'258'),(259,NULL,2,1,'սալոր',2590.000,NULL,NULL,NULL,NULL,NULL,NULL,2590.00,1,'259'),(260,NULL,2,1,'սև խաղող',2600.000,NULL,NULL,NULL,NULL,NULL,NULL,2600.00,1,'260'),(261,NULL,3,1,'Մարինացված բամիա',2610.000,NULL,NULL,NULL,NULL,NULL,NULL,2610.00,1,'261'),(262,NULL,18,3,'պեպսի 0.33 բ',2620.000,NULL,NULL,NULL,NULL,NULL,NULL,2620.00,1,'262'),(263,NULL,7,1,'կաթնաշոռ',2630.000,NULL,NULL,NULL,NULL,NULL,NULL,2630.00,1,'263'),(264,NULL,3,1,'վարունգ մարինացված',2640.000,NULL,NULL,NULL,NULL,NULL,NULL,2640.00,1,'264'),(265,NULL,9,1,'խոզի ֆիլե',2650.000,NULL,NULL,NULL,NULL,NULL,NULL,2650.00,1,'265'),(266,NULL,3,1,'Վարունգ մարինացված աելո',2660.000,NULL,NULL,NULL,NULL,NULL,NULL,2660.00,1,'266'),(267,NULL,9,1,'հավի սիրտ ու թոք',2670.000,NULL,NULL,NULL,NULL,NULL,NULL,2670.00,1,'267'),(268,NULL,7,1,'Սերուցք',2680.000,NULL,NULL,NULL,NULL,NULL,NULL,2680.00,1,'268'),(269,NULL,7,1,'խտացրած կաթ',2690.000,NULL,NULL,NULL,NULL,NULL,NULL,2690.00,1,'269'),(270,NULL,6,4,'ձեթ ֆրի',2700.000,NULL,NULL,NULL,NULL,NULL,NULL,2700.00,1,'270'),(271,NULL,9,1,'քյուֆթա',2710.000,NULL,NULL,NULL,NULL,NULL,NULL,2710.00,1,'271'),(272,NULL,3,3,'Տոլմի թուփ',2720.000,NULL,NULL,NULL,NULL,NULL,NULL,2720.00,1,'272'),(273,NULL,9,1,'Երշիկ կապչոնի',2730.000,NULL,NULL,NULL,NULL,NULL,NULL,2730.00,1,'273'),(274,NULL,6,1,'Բլղուր մառանիկ',2740.000,NULL,NULL,NULL,NULL,NULL,NULL,2740.00,1,'274'),(275,NULL,6,1,'Բրինձ ժասմին',2750.000,NULL,NULL,NULL,NULL,NULL,NULL,2750.00,1,'275'),(276,NULL,2,1,'Խաղողի տերև',2760.000,NULL,NULL,NULL,NULL,NULL,NULL,2760.00,1,'276'),(277,NULL,11,3,'Տարրա',2770.000,NULL,NULL,NULL,NULL,NULL,NULL,2770.00,1,'277'),(278,NULL,27,3,'Խաչապուրի',2780.000,NULL,NULL,NULL,NULL,NULL,NULL,2780.00,1,'278'),(279,NULL,27,3,'Լոխումով բանտիկ',2790.000,NULL,NULL,NULL,NULL,NULL,NULL,2790.00,1,'279'),(280,NULL,27,3,'Ժենգյալով հաց',2800.000,NULL,NULL,NULL,NULL,NULL,NULL,2800.00,1,'280'),(281,NULL,27,3,'Մեղրով',2810.000,NULL,NULL,NULL,NULL,NULL,NULL,2810.00,1,'281'),(282,NULL,27,3,'կեքս',2820.000,NULL,NULL,NULL,NULL,NULL,NULL,2820.00,1,'282'),(283,NULL,27,3,'մեղրով',2830.000,NULL,NULL,NULL,NULL,NULL,NULL,2830.00,1,'283'),(284,NULL,27,3,'վետնամ',2840.000,NULL,NULL,NULL,NULL,NULL,NULL,2840.00,1,'284'),(285,NULL,16,3,'բիսկվիտային նապաստակ',2850.000,NULL,NULL,NULL,NULL,NULL,NULL,2850.00,1,'285'),(286,NULL,12,3,'էկլեր',2860.000,NULL,NULL,NULL,NULL,NULL,NULL,2860.00,1,'286'),(287,NULL,9,1,'Տավարի ֆիլե սառ',2870.000,NULL,NULL,NULL,NULL,NULL,NULL,2870.00,1,'287'),(288,NULL,6,1,'Կակաո',2880.000,NULL,NULL,NULL,NULL,NULL,NULL,2880.00,1,'288'),(289,NULL,7,1,'Քամած մածուն',2890.000,NULL,NULL,NULL,NULL,NULL,NULL,2890.00,1,'289'),(290,NULL,2,1,'Կաղամբ նոր',2900.000,NULL,NULL,NULL,NULL,NULL,NULL,2900.00,1,'290'),(291,NULL,21,4,'Պեպսի սիրոպ',2910.000,NULL,NULL,NULL,NULL,NULL,NULL,2910.00,1,'291'),(292,NULL,21,3,'Պեպսի բաժակ',2920.000,NULL,NULL,NULL,NULL,NULL,NULL,2920.00,1,'292'),(293,NULL,5,1,'մանդարին',2930.000,NULL,NULL,NULL,NULL,NULL,NULL,2930.00,1,'293'),(294,NULL,2,1,'Ավելուկ',2940.000,NULL,NULL,NULL,NULL,NULL,NULL,2940.00,1,'294'),(295,NULL,9,1,'խեկ',2950.000,NULL,NULL,NULL,NULL,NULL,NULL,2950.00,1,'295'),(296,NULL,9,1,'Պանգասիուս',2960.000,NULL,NULL,NULL,NULL,NULL,NULL,2960.00,1,'296'),(297,NULL,22,5,'Թեյ 100գր',2970.000,NULL,NULL,NULL,NULL,NULL,NULL,2970.00,1,'297'),(298,NULL,8,3,'Կանաչի չոր',2980.000,NULL,NULL,NULL,NULL,NULL,NULL,2980.00,1,'298'),(299,NULL,9,1,'Երշիկ կրակով.',2990.000,NULL,NULL,NULL,NULL,NULL,NULL,2990.00,1,'299'),(300,NULL,3,1,'Մարինացված կորնիշոններ',3000.000,NULL,NULL,NULL,NULL,NULL,NULL,3000.00,1,'300'),(301,NULL,3,1,'Մարինացված պոմիդոր',3010.000,NULL,NULL,NULL,NULL,NULL,NULL,3010.00,1,'301'),(302,NULL,18,3,'Լիմոնադ RRR',3020.000,NULL,NULL,NULL,NULL,NULL,NULL,3020.00,1,'302'),(303,NULL,22,3,'CARTE NOIRE',3030.000,NULL,NULL,NULL,NULL,NULL,NULL,3030.00,1,'303'),(304,NULL,12,3,'հոթ դոգի հաց',3040.000,NULL,NULL,NULL,NULL,NULL,NULL,3040.00,1,'304'),(305,NULL,7,1,'Պանիր չեդդեր',3050.000,NULL,NULL,NULL,NULL,NULL,NULL,3050.00,1,'305'),(306,NULL,9,1,'Ֆիլե',3060.000,NULL,NULL,NULL,NULL,NULL,NULL,3060.00,1,'306'),(307,NULL,3,1,'Բադրջանի խորոված',3070.000,NULL,NULL,NULL,NULL,NULL,NULL,3070.00,1,'307'),(308,NULL,6,1,'Արիշտա',3080.000,NULL,NULL,NULL,NULL,NULL,NULL,3080.00,1,'308'),(309,NULL,9,3,'ճուտ',3090.000,NULL,NULL,NULL,NULL,NULL,NULL,3090.00,1,'309'),(310,NULL,18,3,'Տոմատի հյութ',3100.000,NULL,NULL,NULL,NULL,NULL,NULL,3100.00,1,'310'),(311,NULL,3,1,'Լեչո',3110.000,NULL,NULL,NULL,NULL,NULL,NULL,3110.00,1,'311'),(312,NULL,10,1,'Լիմոն',3120.000,NULL,NULL,NULL,NULL,NULL,NULL,3120.00,1,'312'),(313,NULL,2,3,'Բազուկի ճավ',3130.000,NULL,NULL,NULL,NULL,NULL,NULL,3130.00,1,'313'),(314,NULL,2,1,'Ճնճղապաշար',3140.000,NULL,NULL,NULL,NULL,NULL,NULL,3140.00,1,'314'),(315,NULL,24,3,'M',3150.000,NULL,NULL,NULL,NULL,NULL,NULL,3150.00,1,'315'),(316,NULL,17,3,'Rothmans',3160.000,NULL,NULL,NULL,NULL,NULL,NULL,3160.00,1,'316'),(317,NULL,9,1,'Հավ',3170.000,NULL,NULL,NULL,NULL,NULL,NULL,3170.00,1,'317'),(318,NULL,8,1,'Հ. իմբիր',3180.000,NULL,NULL,NULL,NULL,NULL,NULL,3180.00,1,'318'),(319,NULL,9,1,'Հավի քարաջիկ',3190.000,NULL,NULL,NULL,NULL,NULL,NULL,3190.00,1,'319'),(320,NULL,5,1,'Ծյորի',3200.000,NULL,NULL,NULL,NULL,NULL,NULL,3200.00,1,'320'),(321,NULL,24,3,'Kitkat maxi',3210.000,NULL,NULL,NULL,NULL,NULL,NULL,3210.00,1,'321'),(322,NULL,24,3,'Kitkat mini',3220.000,NULL,NULL,NULL,NULL,NULL,NULL,3220.00,1,'322'),(323,NULL,24,3,'Kitkat 4',3230.000,NULL,NULL,NULL,NULL,NULL,NULL,3230.00,1,'323'),(324,NULL,24,3,'MM',3240.000,NULL,NULL,NULL,NULL,NULL,NULL,3240.00,1,'324'),(325,NULL,24,3,'Nesquik',3250.000,NULL,NULL,NULL,NULL,NULL,NULL,3250.00,1,'325'),(326,NULL,24,3,'Nestle shokolad',3260.000,NULL,NULL,NULL,NULL,NULL,NULL,3260.00,1,'326'),(327,NULL,24,3,'N',3270.000,NULL,NULL,NULL,NULL,NULL,NULL,3270.00,1,'327'),(328,NULL,2,1,'Սիբեխ',3280.000,NULL,NULL,NULL,NULL,NULL,NULL,3280.00,1,'328'),(329,NULL,9,3,'Գերմանական նրբերշիկ',3290.000,NULL,NULL,NULL,NULL,NULL,NULL,3290.00,1,'329'),(330,NULL,2,1,'Կարտոֆիլ մեծ',3300.000,NULL,NULL,NULL,NULL,NULL,NULL,3300.00,1,'330'),(331,NULL,7,1,'Մածուն էժան',3310.000,NULL,NULL,NULL,NULL,NULL,NULL,3310.00,1,'331'),(332,NULL,2,1,'Սունկ վեշինկա',3320.000,NULL,NULL,NULL,NULL,NULL,NULL,3320.00,1,'332'),(333,NULL,2,1,'Մալականի թթու',3330.000,NULL,NULL,NULL,NULL,NULL,NULL,3330.00,1,'333'),(334,NULL,4,1,'Մանդակ',3340.000,NULL,NULL,NULL,NULL,NULL,NULL,3340.00,1,'334'),(335,NULL,7,3,'Պաղպաղակ վալիո մեծ',3350.000,NULL,NULL,NULL,NULL,NULL,NULL,3350.00,1,'335'),(336,NULL,7,3,'Պ. չարլի փայտիկ',3360.000,NULL,NULL,NULL,NULL,NULL,NULL,3360.00,1,'336'),(337,NULL,7,3,'Պ. չարլի բաժակ',3370.000,NULL,NULL,NULL,NULL,NULL,NULL,3370.00,1,'337'),(338,NULL,7,3,'Պ. կոն',3380.000,NULL,NULL,NULL,NULL,NULL,NULL,3380.00,1,'338'),(339,NULL,7,3,'Պ. անիտա',3390.000,NULL,NULL,NULL,NULL,NULL,NULL,3390.00,1,'339'),(340,NULL,7,3,'Պ. էսկիմո',3400.000,NULL,NULL,NULL,NULL,NULL,NULL,3400.00,1,'340'),(341,NULL,7,3,'Պ. սամբա',3410.000,NULL,NULL,NULL,NULL,NULL,NULL,3410.00,1,'341'),(342,NULL,7,3,'Պ. անիքապ',3420.000,NULL,NULL,NULL,NULL,NULL,NULL,3420.00,1,'342'),(343,NULL,7,3,'Պ. վիպ',3430.000,NULL,NULL,NULL,NULL,NULL,NULL,3430.00,1,'343'),(344,NULL,7,3,'Պ. վենեցիա 0.5',3440.000,NULL,NULL,NULL,NULL,NULL,NULL,3440.00,1,'344'),(345,NULL,7,3,'Պ. անչուկ',3450.000,NULL,NULL,NULL,NULL,NULL,NULL,3450.00,1,'345'),(346,NULL,2,1,'Շուշան',3460.000,NULL,NULL,NULL,NULL,NULL,NULL,3460.00,1,'346'),(347,NULL,2,1,'Թարմ կարտոֆիլ',3470.000,NULL,NULL,NULL,NULL,NULL,NULL,3470.00,1,'347'),(348,NULL,7,3,'Պ. բրիկետ',3480.000,NULL,NULL,NULL,NULL,NULL,NULL,3480.00,1,'348'),(349,NULL,7,3,'Պ. ջինգլ',3490.000,NULL,NULL,NULL,NULL,NULL,NULL,3490.00,1,'349'),(350,NULL,8,3,'Մանանեխ',3500.000,NULL,NULL,NULL,NULL,NULL,NULL,3500.00,1,'350'),(351,NULL,7,3,'Մածնաբրդոշ',3510.000,NULL,NULL,NULL,NULL,NULL,NULL,3510.00,1,'351'),(352,NULL,5,1,'Վիշնյա',3520.000,NULL,NULL,NULL,NULL,NULL,NULL,3520.00,1,'352'),(353,NULL,2,1,'Սունկ ծառի',3530.000,NULL,NULL,NULL,NULL,NULL,NULL,3530.00,1,'353'),(354,NULL,5,1,'Դամբուլ',3540.000,NULL,NULL,NULL,NULL,NULL,NULL,3540.00,1,'354'),(355,NULL,5,1,'Դեղձ',3550.000,NULL,NULL,NULL,NULL,NULL,NULL,3550.00,1,'355'),(356,NULL,5,1,'Գիլաս',3560.000,NULL,NULL,NULL,NULL,NULL,NULL,3560.00,1,'356'),(357,NULL,5,1,'Բանան',3570.000,NULL,NULL,NULL,NULL,NULL,NULL,3570.00,1,'357'),(358,NULL,7,3,'Թ. 20 կոպեկ',3580.000,NULL,NULL,NULL,NULL,NULL,NULL,3580.00,1,'358'),(359,NULL,7,3,'Վալիո բաժակ',3590.000,NULL,NULL,NULL,NULL,NULL,NULL,3590.00,1,'359'),(360,NULL,7,3,'Թ. կոն փոքր',3600.000,NULL,NULL,NULL,NULL,NULL,NULL,3600.00,1,'360'),(361,NULL,7,3,'Թ. կոն մեծ',3610.000,NULL,NULL,NULL,NULL,NULL,NULL,3610.00,1,'361'),(362,NULL,7,3,'Թ. դուետ',3620.000,NULL,NULL,NULL,NULL,NULL,NULL,3620.00,1,'362'),(363,NULL,7,3,'Թ. ССР',3630.000,NULL,NULL,NULL,NULL,NULL,NULL,3630.00,1,'363'),(364,NULL,5,1,'Ձմերուկ',3640.000,NULL,NULL,NULL,NULL,NULL,NULL,3640.00,1,'364'),(365,NULL,6,1,'Ֆիստաշկա',3650.000,NULL,NULL,NULL,NULL,NULL,NULL,3650.00,1,'365'),(366,NULL,7,1,'Պանիր սուլուգուլի',3660.000,NULL,NULL,NULL,NULL,NULL,NULL,3660.00,1,'366'),(367,NULL,18,3,'Բյուրեղ կապրոն',3670.000,NULL,NULL,NULL,NULL,NULL,NULL,3670.00,1,'367'),(368,NULL,18,3,'Ջերմուկ ապակյա',3680.000,NULL,NULL,NULL,NULL,NULL,NULL,3680.00,1,'368'),(369,NULL,18,3,'Ջերմուկ կապրոն',3690.000,NULL,NULL,NULL,NULL,NULL,NULL,3690.00,1,'369'),(370,NULL,18,3,'Հյ․ սադաչոկ',3700.000,NULL,NULL,NULL,NULL,NULL,NULL,3700.00,1,'370'),(371,NULL,5,1,'Մալինա',3710.000,NULL,NULL,NULL,NULL,NULL,NULL,3710.00,1,'371'),(372,NULL,18,3,'Բյուրեղ ապակի',3720.000,NULL,NULL,NULL,NULL,NULL,NULL,3720.00,1,'372'),(373,NULL,3,1,'Մարին. կանաչ լոբի',3730.000,NULL,NULL,NULL,NULL,NULL,NULL,3730.00,1,'373'),(374,NULL,18,3,'Լիմոնադ արագած',3740.000,NULL,NULL,NULL,NULL,NULL,NULL,3740.00,1,'374'),(375,NULL,22,4,'Թեյի սիրոպ',3750.000,NULL,NULL,NULL,NULL,NULL,NULL,3750.00,1,'375'),(376,NULL,7,3,'Ապարան թան',3760.000,NULL,NULL,NULL,NULL,NULL,NULL,3760.00,1,'376'),(377,NULL,18,3,'Santrope',3770.000,NULL,NULL,NULL,NULL,NULL,NULL,3770.00,1,'377'),(378,NULL,2,1,'Սունկ շամպինյոն մեծ',3780.000,NULL,NULL,NULL,NULL,NULL,NULL,3780.00,1,'378'),(379,NULL,8,1,'Ժելատին',3790.000,NULL,NULL,NULL,NULL,NULL,NULL,3790.00,1,'379'),(380,NULL,2,1,'Բիբար գունավոր',3800.000,NULL,NULL,NULL,NULL,NULL,NULL,3800.00,1,'380'),(381,NULL,8,1,'Համեմունք',3810.000,NULL,NULL,NULL,NULL,NULL,NULL,3810.00,1,'381'),(382,NULL,6,1,'Մակարոն գունավոր',3820.000,NULL,NULL,NULL,NULL,NULL,NULL,3820.00,1,'382'),(383,NULL,18,3,'Հյութ սանդորա0.5',3830.000,NULL,NULL,NULL,NULL,NULL,NULL,3830.00,1,'383'),(384,NULL,18,3,'էներգետիկ ըմպելիք',3840.000,NULL,NULL,NULL,NULL,NULL,NULL,3840.00,1,'384'),(385,NULL,2,1,'Կաղամբ գունավոր',3850.000,NULL,NULL,NULL,NULL,NULL,NULL,3850.00,1,'385'),(386,NULL,9,1,'տավարի բոշ',3860.000,NULL,NULL,NULL,NULL,NULL,NULL,3860.00,1,'386'),(387,NULL,27,1,'Պետչենի',3870.000,NULL,NULL,NULL,NULL,NULL,NULL,3870.00,1,'387'),(388,NULL,9,1,'Հավի միս',3880.000,NULL,NULL,NULL,NULL,NULL,NULL,3880.00,1,'388'),(389,NULL,6,1,'Հ. չաման',3890.000,NULL,NULL,NULL,NULL,NULL,NULL,3890.00,1,'389'),(390,NULL,6,3,'կծու կետչուպ',3900.000,NULL,NULL,NULL,NULL,NULL,NULL,3900.00,1,'390'),(391,NULL,6,1,'մարին բիբար կծու',3910.000,NULL,NULL,NULL,NULL,NULL,NULL,3910.00,1,'391'),(392,NULL,18,3,'և',3920.000,NULL,NULL,NULL,NULL,NULL,NULL,3920.00,1,'392'),(393,NULL,6,3,'շաքարավազ սաշետ',3930.000,NULL,NULL,NULL,NULL,NULL,NULL,3930.00,1,'393'),(394,NULL,6,1,'բլղուր թուրքական',3940.000,NULL,NULL,NULL,NULL,NULL,NULL,3940.00,1,'394'),(395,NULL,6,1,'գորիսի լոբի',3950.000,NULL,NULL,NULL,NULL,NULL,NULL,3950.00,1,'395'),(396,NULL,6,1,'կաթի փոշի',3960.000,NULL,NULL,NULL,NULL,NULL,NULL,3960.00,1,'396'),(397,NULL,18,3,'Շիշ տոմատի',3970.000,NULL,NULL,NULL,NULL,NULL,NULL,3970.00,1,'397'),(398,NULL,18,3,'Շիշ տոմատի',3980.000,NULL,NULL,NULL,NULL,NULL,NULL,3980.00,1,'398'),(399,NULL,18,3,'Հ',3990.000,NULL,NULL,NULL,NULL,NULL,NULL,3990.00,1,'399'),(400,NULL,18,3,'Բ',4000.000,NULL,NULL,NULL,NULL,NULL,NULL,4000.00,1,'400'),(401,NULL,18,3,'Ֆ',4010.000,NULL,NULL,NULL,NULL,NULL,NULL,4010.00,1,'401'),(402,NULL,18,3,'հյութ մասուրի',4020.000,NULL,NULL,NULL,NULL,NULL,NULL,4020.00,1,'402');
/*!40000 ALTER TABLE `c_goods` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_goods_classes`
--

DROP TABLE IF EXISTS `c_goods_classes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_goods_classes` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  `f_class` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_goods_classes`
--

LOCK TABLES `c_goods_classes` WRITE;
/*!40000 ALTER TABLE `c_goods_classes` DISABLE KEYS */;
/*!40000 ALTER TABLE `c_goods_classes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_goods_complectation`
--

DROP TABLE IF EXISTS `c_goods_complectation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_goods_complectation` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_base` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,2) DEFAULT NULL,
  `f_price` decimal(14,3) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_goods_complectation`
--

LOCK TABLES `c_goods_complectation` WRITE;
/*!40000 ALTER TABLE `c_goods_complectation` DISABLE KEYS */;
INSERT INTO `c_goods_complectation` VALUES (2,7,4,1.00,0.000),(3,7,8,2.00,0.000),(7,11,5,0.00,0.000),(8,6,2,1.00,0.000),(9,6,3,1.00,0.000),(10,6,4,1.00,0.000),(11,2,2,1.00,4.000),(12,3,4,1.00,0.000),(13,3,6,2.00,0.000),(14,3,7,3.00,0.000);
/*!40000 ALTER TABLE `c_goods_complectation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_goods_waste`
--

DROP TABLE IF EXISTS `c_goods_waste`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_goods_waste` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_goods` int(11) DEFAULT NULL,
  `f_waste` decimal(9,3) DEFAULT NULL,
  `f_reason` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_goods_waste_goods_idx` (`f_goods`),
  CONSTRAINT `fk_goods_waste_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_goods_waste`
--

LOCK TABLES `c_goods_waste` WRITE;
/*!40000 ALTER TABLE `c_goods_waste` DISABLE KEYS */;
/*!40000 ALTER TABLE `c_goods_waste` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_groups`
--

DROP TABLE IF EXISTS `c_groups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_groups` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  `f_order` int(11) DEFAULT NULL,
  `f_taxDept` smallint(6) DEFAULT NULL,
  `f_adgcode` varchar(16) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=28 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_groups`
--

LOCK TABLES `c_groups` WRITE;
/*!40000 ALTER TABLE `c_groups` DISABLE KEYS */;
INSERT INTO `c_groups` VALUES (2,'Բանջարեղեն',NULL,NULL,NULL),(3,'Պահածո',NULL,NULL,NULL),(4,'Կանաչի',NULL,NULL,NULL),(5,'Միրգ',NULL,NULL,NULL),(6,'Նպարեղեն',NULL,NULL,NULL),(7,'Կաթնամթերք',NULL,NULL,NULL),(8,'Համեմունք',NULL,NULL,NULL),(9,'Մսեղեն',NULL,NULL,NULL),(10,'միրգ',NULL,NULL,NULL),(11,'Տնտեսական',NULL,NULL,NULL),(12,'Հացաբուլկեղեն',NULL,NULL,NULL),(13,'ֆեռեռո -ռոշեռ',NULL,NULL,NULL),(14,'Դավիդոֆֆ',NULL,NULL,NULL),(15,'օրբիտ մաստակ',NULL,NULL,NULL),(16,'լ',NULL,NULL,NULL),(17,'Ծխախոտ',NULL,NULL,NULL),(18,'Զովացուցիչ Ըմպելիք',NULL,NULL,NULL),(19,'Filip moris',NULL,NULL,NULL),(20,'Չիպսեր',NULL,NULL,NULL),(21,'Pepsi',NULL,NULL,NULL),(22,'թեյ,սուրճ',NULL,NULL,NULL),(23,'Ջերմուկ',NULL,NULL,NULL),(24,'snickers',NULL,NULL,NULL),(25,'Ջերմուկ',NULL,NULL,NULL),(26,'վիսկի',NULL,NULL,NULL),(27,'հրուշակեղեն',NULL,NULL,NULL);
/*!40000 ALTER TABLE `c_groups` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_partners`
--

DROP TABLE IF EXISTS `c_partners`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_partners` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_taxname` varchar(128) DEFAULT NULL,
  `f_taxcode` varchar(45) DEFAULT NULL,
  `f_contact` varchar(255) DEFAULT NULL,
  `f_info` varchar(256) DEFAULT NULL,
  `f_phone` varchar(255) DEFAULT NULL,
  `f_email` varchar(255) DEFAULT NULL,
  `f_address` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=37 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_partners`
--

LOCK TABLES `c_partners` WRITE;
/*!40000 ALTER TABLE `c_partners` DISABLE KEYS */;
INSERT INTO `c_partners` VALUES (16,'LLC YoYO','00022255','Gagulik','Koshiki budka','77558899','','P-Sevak 1/1'),(17,'Jujulik LLC','2154','Jujulikyan Jujulik','SAS supermerkato','+374 99 558877','','Hayastan 7'),(18,'Gorik ADZ','000198002','Gor','Գորիկի խանութ','091 009170','','Պուշկինի 51'),(19,'Varsak LLC','00154875','Manager','Tuyn xanut','093698522','','Saryan 2'),(20,'Roga and Kopita LLC','00154887','Roga','RogaKopita','','','Moskovyan 5'),(21,'Kerama Maraci LLC','0012255','Gevor','KM','','','Massiv 3'),(22,'Koshaki LLC','10001548','Mesrop','Limon','+374 0111002','','Pushkin 40'),(23,'Perdun ADZ','10458770','Vacho','Trani xanut','','','Mashtoci Poghota 4'),(24,'Baba Yaga ADZ','00125400','23EE432','Stupa','','','Mashtoci Poghota 1'),(25,'Fistashki LLC','10000133','Esim Ov','Fishtashkeqi xanut','091 1000891','','Abovyan 12'),(26,'Havi dosh LLC','2000911','Vahe','Havi dosh','93 336555','','Saryan 6'),(27,'Kokosi Yugh','2099912','Armanchik','Yogher','+374 10 558777','','Proshyan 8'),(28,'Vonyuchka ADZ','880000123','Araik','Duxiner','98 524174','','Komitas 21'),(29,'Kabachok LLC','0913000','Gorik','Vegitables','','','Baghramyan 4'),(30,'Tiran LLC','01938499','Migran','Tigran GiperMarket','+374 11 225487','','Shinararneri 22'),(31,'Nigga LLC','9903421','Nigger','Black Star','','','Abobyan 4'),(32,NULL,NULL,'PESKAR PESKARICH','',NULL,NULL,NULL),(33,NULL,NULL,'JOPALIZ','',NULL,NULL,NULL),(34,NULL,NULL,'ԱSTON','',NULL,NULL,NULL),(35,NULL,NULL,'OSLIK ISHAK','',NULL,NULL,NULL),(36,'VISIT','','VISIT','','','','');
/*!40000 ALTER TABLE `c_partners` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_storages`
--

DROP TABLE IF EXISTS `c_storages`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_storages` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  `f_outcounter` int(11) DEFAULT NULL,
  `f_inputcounter` int(11) DEFAULT NULL,
  `f_movecounter` int(11) DEFAULT NULL,
  `f_complectcounter` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_storages`
--

LOCK TABLES `c_storages` WRITE;
/*!40000 ALTER TABLE `c_storages` DISABLE KEYS */;
INSERT INTO `c_storages` VALUES (1,'Սառնարան',NULL,NULL,NULL,NULL),(2,'Խոհանոց',NULL,NULL,NULL,NULL),(3,'Կենտրոնական',NULL,NULL,NULL,NULL),(4,'Հրուշակեղեն',NULL,NULL,NULL,NULL),(5,'Աղցաններ',NULL,NULL,NULL,NULL),(6,'Vanoi pahest',0,0,0,0);
/*!40000 ALTER TABLE `c_storages` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `c_units`
--

DROP TABLE IF EXISTS `c_units`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_units` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `c_units`
--

LOCK TABLES `c_units` WRITE;
/*!40000 ALTER TABLE `c_units` DISABLE KEYS */;
INSERT INTO `c_units` VALUES (1,'Կգ'),(3,'Հատ'),(4,'Լիտր'),(5,'Տուփ');
/*!40000 ALTER TABLE `c_units` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_dish`
--

DROP TABLE IF EXISTS `d_dish`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_dish` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_part` int(11) DEFAULT NULL,
  `f_name` varchar(128) DEFAULT NULL,
  `f_color` int(11) DEFAULT NULL,
  `f_queue` int(11) DEFAULT NULL,
  `f_remind` smallint(6) DEFAULT NULL,
  `f_service` smallint(6) DEFAULT NULL,
  `f_discount` smallint(6) DEFAULT NULL,
  `f_comment` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_dish_part_idx` (`f_part`),
  CONSTRAINT `fk_dish_part` FOREIGN KEY (`f_part`) REFERENCES `d_part2` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=641 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_dish`
--

LOCK TABLES `d_dish` WRITE;
/*!40000 ALTER TABLE `d_dish` DISABLE KEYS */;
INSERT INTO `d_dish` VALUES (2,2,'Աղցան մայրաքաղաքային',-11141249,0,NULL,NULL,NULL,NULL),(3,6,'Լոխումով բանտիկ',-6969458,0,NULL,NULL,NULL,NULL),(4,2,'Աղցան կայիրiii',-1,0,0,0,0,''),(5,2,'Աղցան իսպանական',-65281,0,NULL,NULL,NULL,NULL),(6,6,'Խրճիթ',-6969458,0,NULL,NULL,NULL,NULL),(7,6,'Շախմատ',-129,0,NULL,NULL,NULL,NULL),(8,2,'Աղցան հունական',-16776961,0,NULL,NULL,NULL,NULL),(9,18,'Պոժառսկի կոտլետ',-6969458,0,NULL,NULL,NULL,NULL),(10,2,' Աղցան հավով',-11141249,0,NULL,NULL,NULL,NULL),(11,2,' Աղցան վիոլա',-6969458,0,NULL,NULL,NULL,NULL),(12,5,'Բրնձ,հավով և սիսեռով փլավ',-6969458,0,NULL,NULL,NULL,NULL),(13,18,'Ջեռեփուկ հավով բրոկոլիյով',-6969458,0,NULL,NULL,NULL,NULL),(14,6,'Միկադո',-6969458,0,NULL,NULL,NULL,NULL),(15,5,'Բազուկի ճավ',-11141249,0,NULL,NULL,NULL,NULL),(16,2,'Աղցան վինեգրետ',-5636096,0,NULL,NULL,NULL,NULL),(17,2,'Աղցան կաղամբով',-21761,0,NULL,NULL,NULL,NULL),(18,2,'Աղցան վենեցիա',-6969458,0,NULL,NULL,NULL,NULL),(19,2,'Աղցան կորեական',-11141249,0,NULL,NULL,NULL,NULL),(20,2,'Աղցան խոզապուխտով',-1,0,NULL,NULL,NULL,NULL),(21,6,'Իզբուշկա',-6969458,0,NULL,NULL,NULL,NULL),(22,18,'Չիքեն մնյու',-6969458,0,NULL,NULL,NULL,NULL),(23,2,'Աղցան թաբուլե',-21761,0,NULL,NULL,NULL,NULL),(24,6,'Կարտոֆիլով կարկանդակ',-6969458,0,NULL,NULL,NULL,NULL),(25,2,'Աղցան բրոկոլիով',-1,0,NULL,NULL,NULL,NULL),(26,6,'Նեգռի ժպիտ',-6969458,0,NULL,NULL,NULL,NULL),(27,2,' Աղցան ամառային',-21761,0,NULL,NULL,NULL,NULL),(28,2,' Աղցան ռուսական (կարմիր)',-65536,0,NULL,NULL,NULL,NULL),(29,2,' Աղցան մեքսիկական',-256,0,NULL,NULL,NULL,NULL),(30,2,'Աղցան ծիածան',-1,0,NULL,NULL,NULL,NULL),(31,2,'Աղցան կռաբով',-65536,0,NULL,NULL,NULL,NULL),(32,2,'Աղցան բազուկով',-11141249,0,NULL,NULL,NULL,NULL),(33,2,'Աղցան գազարով և ոլոռով',-16202497,0,NULL,NULL,NULL,NULL),(34,5,'Բինձ տավարի մսով',-6969458,0,NULL,NULL,NULL,NULL),(35,18,'Տավարի լանգետ',-21761,0,NULL,NULL,NULL,NULL),(36,3,'Ֆրի',-65536,3,NULL,NULL,NULL,NULL),(37,5,'Պյուրե',-21761,3,NULL,NULL,NULL,NULL),(38,5,'Հնդկաձավար',-21761,3,NULL,NULL,NULL,NULL),(39,18,'Խոզի բուժենինա',-16776961,3,NULL,NULL,NULL,NULL),(40,5,'Խաշած բրինձ',-21761,3,NULL,NULL,NULL,NULL),(41,5,'Վերմիշել',-256,3,NULL,NULL,NULL,NULL),(42,4,'Ապուր մինեստրոնե',-11162881,3,NULL,NULL,NULL,NULL),(43,5,'Դաշտի բանջարեղեն',-43649,3,NULL,NULL,NULL,NULL),(44,18,'Օմլետ',-4669767,3,NULL,NULL,NULL,NULL),(45,5,'Սպագետտի',-65536,3,NULL,NULL,NULL,NULL),(46,5,'Հաճար սնկով',-256,3,NULL,NULL,NULL,NULL),(47,18,'Պելմենի',-4669767,3,NULL,NULL,NULL,NULL),(48,18,'Հավի կոտլետ տոնական',-4669767,3,NULL,NULL,NULL,NULL),(49,4,'Սպաս',-21761,6,NULL,NULL,NULL,NULL),(50,4,'Ապուր հավով',-256,6,NULL,NULL,NULL,NULL),(51,4,'Ապուր սոլյանկա',-65536,6,NULL,NULL,NULL,NULL),(52,4,'Ապուր բորշ',-65281,6,NULL,NULL,NULL,NULL),(53,4,'Ապուր սնկով',-256,6,NULL,NULL,NULL,NULL),(54,17,'Ձմերուկ',-256,6,NULL,NULL,NULL,NULL),(55,18,'Հավի դոշով գրպանիկ',-11141249,6,NULL,NULL,NULL,NULL),(56,4,'Ապուր կարմիր լոբով',-65281,6,NULL,NULL,NULL,NULL),(57,2,'Աղցան մարգարիտա',-65281,6,NULL,NULL,NULL,NULL),(58,4,'Ապուր ոլոռով',-16776961,6,NULL,NULL,NULL,NULL),(59,4,'Ապուր ոսպով',-65536,6,NULL,NULL,NULL,NULL),(60,4,'Ապուր մակարոնով',-1,6,NULL,NULL,NULL,NULL),(61,18,'Քյուֆթով ռուլետ',-16972,2,NULL,NULL,NULL,NULL),(62,18,'Հավի կոտլետ',-65536,2,NULL,NULL,NULL,NULL),(63,3,'Տապակած դդմիկ',-16972,2,NULL,NULL,NULL,NULL),(64,5,'Ծաղկակաղամբ ջեռոցում',-21761,2,NULL,NULL,NULL,NULL),(66,3,'Դդումը թթվասերով',-16711809,2,NULL,NULL,NULL,NULL),(67,5,'Ուզբեկական փլավ',-65281,2,NULL,NULL,NULL,NULL),(68,5,'Բլղուր լիբանանական',-256,2,NULL,NULL,NULL,NULL),(69,18,'Հավի մսով և բանջարեղենով գնդիկներ',-43521,2,NULL,NULL,NULL,NULL),(70,18,'Տավարի  կոտլետ  տնական',-16972,2,NULL,NULL,NULL,NULL),(71,3,'Բոլոնեզ',-16972,2,NULL,NULL,NULL,NULL),(72,5,'Ծաղկակաղամբ  ձվով',-65536,2,NULL,NULL,NULL,NULL),(73,18,'Խոզապուխտով կոտլետ',-16972,2,NULL,NULL,NULL,NULL),(74,3,'Բեֆստրոգոնով',-65281,2,NULL,NULL,NULL,NULL),(75,18,'Ալբանական կոտլետ',-16202497,2,NULL,NULL,NULL,NULL),(76,5,'Թեմուր',-256,2,NULL,NULL,NULL,NULL),(77,3,'Ռագու դդումով',-16972,2,NULL,NULL,NULL,NULL),(78,18,'Դդմիկով ռուլետ',-16972,2,NULL,NULL,NULL,NULL),(79,5,'Պասուց տոլմայի միջուկ',-65281,2,NULL,NULL,NULL,NULL),(80,6,'Մուրաբա',-11163136,12,NULL,NULL,NULL,NULL),(81,6,'Տորթ գետնանուշով',-65536,1,NULL,NULL,NULL,NULL),(82,6,'Սնիկերս',-1,12,NULL,NULL,NULL,NULL),(83,6,'Խաչապուրի վիչ պանրով',-11141121,2,NULL,NULL,NULL,NULL),(84,6,'Ռուլետ ընկույզով',-5570561,12,NULL,NULL,NULL,NULL),(85,6,'Նապոլեոն',-16202497,12,NULL,NULL,NULL,NULL),(86,6,'Նուտելլա',-129,3,NULL,NULL,NULL,NULL),(87,15,'Fresh բանան կիվի',-65536,1,NULL,NULL,NULL,NULL),(88,6,'Նապոլեոն',-1,5,NULL,NULL,NULL,NULL),(90,6,'Նեգրուշկա',-43521,12,NULL,NULL,NULL,NULL),(95,18,'Լազանիա',-5570561,12,NULL,NULL,NULL,NULL),(96,7,'Հոթ-դոգ',-5614081,5,NULL,NULL,NULL,NULL),(97,7,'Գամբուրգեր',-5614081,5,NULL,NULL,NULL,NULL),(98,18,'Հավի գրպանիկ սոուսով',-256,5,NULL,NULL,NULL,NULL),(99,7,'Վեչինայով սենդվիչ',-5614081,5,NULL,NULL,NULL,NULL),(100,18,'Կերուսուս',-1,0,NULL,NULL,NULL,NULL),(102,8,'Կետչուպ',-16202497,0,NULL,NULL,NULL,NULL),(103,8,'Կծու բիբար',-16202497,0,NULL,NULL,NULL,NULL),(105,8,'Լիմոն',-1,0,NULL,NULL,NULL,NULL),(106,9,'Կապուչինո',-5910882,10,NULL,NULL,NULL,NULL),(107,9,'Գաթա',-5910882,10,NULL,NULL,NULL,NULL),(108,18,'Խոզի ֆիլե',-5910882,10,NULL,NULL,NULL,NULL),(109,9,'Տիռամիսու',-5910882,10,NULL,NULL,NULL,NULL),(110,3,'Մեքսիկական սոուս',-5910882,10,NULL,NULL,NULL,NULL),(111,9,'Տիրամիսու',-5910882,10,NULL,NULL,NULL,NULL),(112,10,'Պեպսի ապակյա',-16202497,4,NULL,NULL,NULL,NULL),(113,10,'Պեպսի 0.5',-16202497,4,NULL,NULL,NULL,NULL),(114,10,'Բոռժոմի',-5592321,4,NULL,NULL,NULL,NULL),(115,10,'Լիպտոն թեյ',-16202497,4,NULL,NULL,NULL,NULL),(116,18,'Շնիցել',-256,6,NULL,NULL,NULL,NULL),(117,6,'Իմերիթական խաչապուրի',-16202497,6,NULL,NULL,NULL,NULL),(118,3,'Տապակած սմբուկ',-256,6,NULL,NULL,NULL,NULL),(119,5,'Մակարոնի պո ֆլոտսկի',-16776961,3,NULL,NULL,NULL,NULL),(122,14,'Davidoff  slims',-1,9000,NULL,NULL,NULL,NULL),(124,14,'D',-1,9000,NULL,NULL,NULL,NULL),(126,14,'Davidoff',-1,9000,NULL,NULL,NULL,NULL),(128,14,'Davidoff shape',-1,9000,NULL,NULL,NULL,NULL),(129,14,'D',-1,9000,NULL,NULL,NULL,NULL),(130,14,'West Compact',-1,9000,NULL,NULL,NULL,NULL),(131,14,'West Fusion',-1,9000,NULL,NULL,NULL,NULL),(132,10,'Լեմոնչելլա',-4501,9000,NULL,NULL,NULL,NULL),(133,14,'Karelia',-1,9000,NULL,NULL,NULL,NULL),(134,10,'Հյութ Ֆրուտինգ',-1,9000,NULL,NULL,NULL,NULL),(135,14,'Ռոթհմանս 1+1',-1,9000,NULL,NULL,NULL,NULL),(136,14,'Marllboro touch',-1,9000,NULL,NULL,NULL,NULL),(137,14,'Marllboro flavour plus',-1,9000,NULL,NULL,NULL,NULL),(138,10,'Ջուր ապարան',-16202497,9000,NULL,NULL,NULL,NULL),(139,14,'Parliament',-1,9000,NULL,NULL,NULL,NULL),(140,14,'Parliament Karat',-1,9000,NULL,NULL,NULL,NULL),(141,14,'LM',-1,9000,NULL,NULL,NULL,NULL),(142,14,'Bond Red',-1,9000,NULL,NULL,NULL,NULL),(143,10,'Մ',-1,9000,NULL,NULL,NULL,NULL),(144,14,'Bond slims',-1,9000,NULL,NULL,NULL,NULL),(145,14,'Bond slims',-1,9000,NULL,NULL,NULL,NULL),(146,14,'R',-1,9000,NULL,NULL,NULL,NULL),(147,14,'Kent 8.41',-1,9000,NULL,NULL,NULL,NULL),(148,15,'Կաֆֆա',-16202497,9000,NULL,NULL,NULL,NULL),(149,15,'Կաֆա հատով',-1,9000,NULL,NULL,NULL,NULL),(150,14,'Kent HDI',-1,9000,NULL,NULL,NULL,NULL),(151,14,'Vogue',-1,9000,NULL,NULL,NULL,NULL),(152,14,'Dunhill',-1,9000,NULL,NULL,NULL,NULL),(153,14,'Dunhill F.C',-1,9000,NULL,NULL,NULL,NULL),(154,14,'Monte Carlo',-1,9000,NULL,NULL,NULL,NULL),(155,14,'WistonXStyle',-1,9000,NULL,NULL,NULL,NULL),(156,14,'WistonXSense',-1,9000,NULL,NULL,NULL,NULL),(157,14,'Wiston B.W.S',-1,9000,NULL,NULL,NULL,NULL),(158,13,'Eclips',-5592449,10000,NULL,NULL,NULL,NULL),(160,13,'Orbit  cristal',-5592449,10000,NULL,NULL,NULL,NULL),(163,13,'Orbit',-5592449,10000,NULL,NULL,NULL,NULL),(164,13,'Skitels',-5592449,10000,NULL,NULL,NULL,NULL),(165,10,'Թան գազով',-1,4,NULL,NULL,NULL,NULL),(166,14,'Kent HD',-43649,4,NULL,NULL,NULL,NULL),(167,10,'Յան',-16202497,4,NULL,NULL,NULL,NULL),(168,10,'Պեպսի լցնովի',-16202497,4,NULL,NULL,NULL,NULL),(169,10,'Էներգետիկ ըմպ․',-16202497,4,NULL,NULL,NULL,NULL),(170,10,'Լիմոնադ RRR կապռոն',-16202497,4,NULL,NULL,NULL,NULL),(171,10,'Sandora 0.25',-16202497,4,NULL,NULL,NULL,NULL),(172,10,'Հյութ',-5614336,4,NULL,NULL,NULL,NULL),(173,15,'Կ',-1,4,NULL,NULL,NULL,NULL),(174,13,'Kinder chocolat maxi',-5592449,10000,NULL,NULL,NULL,NULL),(177,13,'Kinder delis',-5592449,10000,NULL,NULL,NULL,NULL),(178,13,'Kinder chocolate mini',-5592449,10000,NULL,NULL,NULL,NULL),(179,13,'Kinder bueno',-5592449,10000,NULL,NULL,NULL,NULL),(180,13,'Kinder  delis',-5592449,10000,NULL,NULL,NULL,NULL),(181,13,'Ferrero rocher',-5592449,10000,NULL,NULL,NULL,NULL),(182,13,'Raffaello 3',-5592449,10000,NULL,NULL,NULL,NULL),(183,13,'Raffaello',-5592449,10000,NULL,NULL,NULL,NULL),(184,13,'Tic-tac',-5592449,10000,NULL,NULL,NULL,NULL),(185,15,'Fresh բանան ազնվամորի',-65536,0,NULL,NULL,NULL,NULL),(186,3,'Չիքեն-քարի',-4669767,3,NULL,NULL,NULL,NULL),(187,15,'Թեյ,',-16202497,0,NULL,NULL,NULL,NULL),(188,16,'Հաց',-16202497,12,NULL,NULL,NULL,NULL),(189,10,'Կոմպոտ',-16202497,4,NULL,NULL,NULL,NULL),(191,10,'Ջուր ապարան 1լ',-16202497,0,NULL,NULL,NULL,NULL),(192,13,'Պլիտկա GC',-1,0,NULL,NULL,NULL,NULL),(193,13,'MilkyWay',-1,0,NULL,NULL,NULL,NULL),(194,10,'Ֆռեշ բառ',-16202497,0,NULL,NULL,NULL,NULL),(195,13,'M&M maxi',-1,0,NULL,NULL,NULL,NULL),(197,13,'Snichers',-16202497,0,NULL,NULL,NULL,NULL),(199,13,'Snichers maxi',-16202497,0,NULL,NULL,NULL,NULL),(200,10,'Թան Անի',-16202497,0,NULL,NULL,NULL,NULL),(201,15,'Nescafe',-16202497,0,NULL,NULL,NULL,NULL),(202,16,'Լավաշ',-16202497,0,NULL,NULL,NULL,NULL),(203,10,'Լիմոնադ արագած',-16202497,0,NULL,NULL,NULL,NULL),(205,13,'լյուբյատովո եռ.կաթ շոկ.',-1,0,NULL,NULL,NULL,NULL),(206,13,'Հարիբո ժելե',-1,0,NULL,NULL,NULL,NULL),(207,13,'Կեքս Մագդալենա',-1,0,NULL,NULL,NULL,NULL),(208,6,'Հավի կրծքամսով և սնկով ռուլետ',-11162881,0,NULL,NULL,NULL,NULL),(209,13,'Մուլինոլլի',-1,0,NULL,NULL,NULL,NULL),(210,13,'Ռուլետ  կակաո',-1,0,NULL,NULL,NULL,NULL),(212,13,'Ռուլետ  Մալինա բալ ելակ',-1,0,NULL,NULL,NULL,NULL),(213,13,'Չոկո բոյ',-1,0,NULL,NULL,NULL,NULL),(216,13,'Չոկո պայ112գ',-1,0,NULL,NULL,NULL,NULL),(218,10,'Մաազա',-1,0,NULL,NULL,NULL,NULL),(220,4,'Ապուր բանջարեղենով',-1,0,NULL,NULL,NULL,NULL),(221,18,'Սարդելկա',-65281,0,NULL,NULL,NULL,NULL),(222,8,'պանիր',-16202497,0,NULL,NULL,NULL,NULL),(223,5,'Մանդակ',-5614081,0,NULL,NULL,NULL,NULL),(224,5,'Հաճար',-65536,0,NULL,NULL,NULL,NULL),(225,5,'Խավարտ կարմիր լոբի',-256,0,NULL,NULL,NULL,NULL),(226,18,'Ոսպով կոտլետ',-1,0,NULL,NULL,NULL,NULL),(227,18,'Ձվածեղ մաղադանոսով',-1,0,NULL,NULL,NULL,NULL),(228,15,'սուրճ',-16202497,0,NULL,NULL,NULL,NULL),(229,3,'Սոուս չինական',-65536,0,NULL,NULL,NULL,NULL),(230,5,' Մեքսիկական  ռագու',-65536,0,NULL,NULL,NULL,NULL),(231,14,'Marllboro gold',-1,0,NULL,NULL,NULL,NULL),(232,18,'Հավի շիշ թաուկ',-256,0,NULL,NULL,NULL,NULL),(233,18,'Քյուֆթա պիցցա',-1,0,NULL,NULL,NULL,NULL),(234,5,'Բրինձ ֆր.ձև',-65536,0,NULL,NULL,NULL,NULL),(235,3,'Սոուս կեր ու սուս',-1,0,NULL,NULL,NULL,NULL),(237,18,'Ռիկոտա',-65281,0,NULL,NULL,NULL,NULL),(238,18,'Կարտոֆիլով պիցցա',-1,0,NULL,NULL,NULL,NULL),(239,18,'Կիևյան կոտլետ հավի դոշով կարագով',-1,0,NULL,NULL,NULL,NULL),(240,18,'Ճուտ ֆրիյով',-1,0,NULL,NULL,NULL,NULL),(241,5,'Մակարոն սից.ձևով',-65281,0,NULL,NULL,NULL,NULL),(242,5,'Խաշլամա գառի',-1,0,NULL,NULL,NULL,NULL),(243,2,'Աղցան քամած մածուն',-5570689,0,NULL,NULL,NULL,NULL),(244,18,'Բանջարեղենային ջեռեփուկ',-1,0,NULL,NULL,NULL,NULL),(245,18,'Բուրեկ',-1,0,NULL,NULL,NULL,NULL),(246,3,'Մեքսիկական ֆայլա',-21761,0,NULL,NULL,NULL,NULL),(247,18,'Լցոնած հավի բդիկ',-1,0,NULL,NULL,NULL,NULL),(248,13,'Չոկո պայ հատ',-1,0,NULL,NULL,NULL,NULL),(249,18,'Կարտոֆիլով կոտլետ',-21761,0,NULL,NULL,NULL,NULL),(250,18,'Ռուլետ դդմիկով ու տավ մսով',-11141249,0,NULL,NULL,NULL,NULL),(251,18,'խոզի շիշ-թաուկ',-65536,0,NULL,NULL,NULL,NULL),(252,18,'Սարդելկա',-1,0,NULL,NULL,NULL,NULL),(254,18,'Հավի բուդ տապակած',-16776961,0,NULL,NULL,NULL,NULL),(255,18,'Խոզի ռուլետ սպանախով',-256,0,NULL,NULL,NULL,NULL),(256,5,'Խոզի մսով  ժառկոե',-11141249,0,NULL,NULL,NULL,NULL),(257,6,'Պանչո',-16733441,0,NULL,NULL,NULL,NULL),(258,18,'Լցոնած քյուֆթա',-1,0,NULL,NULL,NULL,NULL),(259,6,'Կարկանդակ  հավի դոշով',-16202497,0,NULL,NULL,NULL,NULL),(260,6,'Փախլավա',-22016,0,NULL,NULL,NULL,NULL),(261,10,'պեպսի 0.33 բ',-16202497,0,NULL,NULL,NULL,NULL),(262,18,'Բադրջան քյաբաբ',-21761,0,NULL,NULL,NULL,NULL),(263,3,'Հավի թոք',-43521,0,NULL,NULL,NULL,NULL),(264,5,'Սպագետի տավառի մսով',-16776961,0,0,0,0,''),(265,3,'Քյասիբի տոլմա',-11141121,0,NULL,NULL,NULL,NULL),(266,6,'Սենդվիչ բեկոն պանիր',-43776,9,NULL,NULL,NULL,NULL),(267,3,'Տոլմա ամառային',-11163136,0,NULL,NULL,NULL,NULL),(268,6,'Ոզնի',-16733696,4,NULL,NULL,NULL,NULL),(269,18,'Սամսա',-256,0,NULL,NULL,NULL,NULL),(270,18,'Կաղամբի կոտլետ',-5570689,0,NULL,NULL,NULL,NULL),(271,3,'Չախոբիլի',-16202497,0,NULL,NULL,NULL,NULL),(272,18,'Լցոնած դդմիկ հավի մսով',-1,0,NULL,NULL,NULL,NULL),(273,6,'Մուժսկոյ իդեալ',-11141121,7,NULL,NULL,NULL,NULL),(274,4,'Ապուր բրինձով',-11141249,0,NULL,NULL,NULL,NULL),(275,18,'Տապակած սուլուգունի',-16776961,0,NULL,NULL,NULL,NULL),(276,6,'Միկադո',-16202497,0,NULL,NULL,NULL,NULL),(277,18,'Տավարի քյաբաբ',-1,0,NULL,NULL,NULL,NULL),(278,4,'Ապուր խարչո',-11141249,0,NULL,NULL,NULL,NULL),(279,6,'Կարկանդակ պանրով',-21889,0,NULL,NULL,NULL,NULL),(280,2,'Աղցան ջամբո',-1,0,NULL,NULL,NULL,NULL),(281,5,'Տապակած սիբեխ ձվով',-1,0,NULL,NULL,NULL,NULL),(282,16,'Տարրա',-16202497,0,NULL,NULL,NULL,NULL),(283,6,'Սերկեֆիլով պիռոգ',-65281,10,NULL,NULL,NULL,NULL),(284,6,'Տորթ էմանութլա',-1,6,NULL,NULL,NULL,NULL),(285,6,'Մսային պիռոգ',-11162881,8,NULL,NULL,NULL,NULL),(286,18,'Բլինչիկ',-21761,0,NULL,NULL,NULL,NULL),(289,2,'Աղցան բանջարեղենային',-1,0,NULL,NULL,NULL,NULL),(291,18,'Տապակած հավի դոշ',-65281,0,NULL,NULL,NULL,NULL),(292,5,'Բրինձ սնկով',-1,0,NULL,NULL,NULL,NULL),(293,2,'Աղցան կապրեզե',-1,0,NULL,NULL,NULL,NULL),(294,2,'Աղցան կուչինա',-16776961,0,NULL,NULL,NULL,NULL),(295,18,'Խոզի չալաղաջ մեծ',-1,0,NULL,NULL,NULL,NULL),(296,18,'Սմբուկով քյաբաբ',-256,0,NULL,NULL,NULL,NULL),(297,5,'Տապակած ավելուկ',-65281,0,NULL,NULL,NULL,NULL),(298,5,'Մեքսիկական նախուտեստ',-1,0,NULL,NULL,NULL,NULL),(299,2,'Աղցան հավով և անանասով',-1,0,NULL,NULL,NULL,NULL),(300,18,'Բանջարեղեն խորոված',-1,0,NULL,NULL,NULL,NULL),(301,5,'Մակարոն',-16776961,0,NULL,NULL,NULL,NULL),(302,18,'Կիևյան կոտլետ պանրով',-256,0,NULL,NULL,NULL,NULL),(303,18,'Խոզի սթեյկ',-1,0,NULL,NULL,NULL,NULL),(304,18,'Ֆլամինգո',-1,0,NULL,NULL,NULL,NULL),(305,8,'Մարինադ',-16202497,0,NULL,NULL,NULL,NULL),(306,18,'Կիևյան կոտլետ կարագով',-16776961,0,NULL,NULL,NULL,NULL),(307,5,'Տապակած մանդակ ձվով',-1,0,NULL,NULL,NULL,NULL),(308,18,'Կիևյան կոտլետ սնկով',-1,0,NULL,NULL,NULL,NULL),(309,10,'Լիմոնադ RRR',-16202497,0,NULL,NULL,NULL,NULL),(310,8,'Սոուս',-16202497,0,NULL,NULL,NULL,NULL),(311,18,'Հավի քյաբաբ',-1,0,NULL,NULL,NULL,NULL),(312,18,'Հավ կարտոֆիլով և պանրով',-1,0,NULL,NULL,NULL,NULL),(313,3,'Գարուն',-43649,0,NULL,NULL,NULL,NULL),(314,15,'CARTE NOIRE',-1,0,NULL,NULL,NULL,NULL),(315,6,'Հոթ դոգ',-1,0,NULL,NULL,NULL,NULL),(317,2,'Աղցան ոսպով',-21761,0,NULL,NULL,NULL,NULL),(318,5,'Սամուռայ',-1,0,NULL,NULL,NULL,NULL),(319,6,'Հոթ դոգ բաստուրմայով',-1,0,NULL,NULL,NULL,NULL),(320,18,'Հավի կրծք կարտոֆիլով(յոժիկ)',-65281,0,NULL,NULL,NULL,NULL),(321,6,'Հոթ դոգ հավով',-1,0,NULL,NULL,NULL,NULL),(322,10,'7Ափ լցովի',-1,0,NULL,NULL,NULL,NULL),(323,4,'Ապուր կոլոլակով',-1,0,NULL,NULL,NULL,NULL),(324,5,'Արիշտա',-11141249,0,NULL,NULL,NULL,NULL),(325,18,'Հավի բդիկ կարտոֆիլով',-1,0,NULL,NULL,NULL,NULL),(326,18,'Քյասիբի տս',-11141121,0,NULL,NULL,NULL,NULL),(327,3,'Լոլիկով ձվածեղ',-129,0,NULL,NULL,NULL,NULL),(328,10,'Տոմատի հյութ',-16202497,0,NULL,NULL,NULL,NULL),(329,18,'Իռլանդական ջեռեփուկ',-1,0,NULL,NULL,NULL,NULL),(330,3,'Բադրջանի խորոված',-21761,0,NULL,NULL,NULL,NULL),(331,5,'Տապակած սպանախ',-16733569,0,NULL,NULL,NULL,NULL),(332,18,'Հավի ռոմշտեքս',-1,0,NULL,NULL,NULL,NULL),(333,18,'Պղպեղ հավով պանրով',-1,0,NULL,NULL,NULL,NULL),(334,8,'Թթու',-16202497,0,NULL,NULL,NULL,NULL),(335,18,'Հավով ալադի',-256,0,NULL,NULL,NULL,NULL),(336,5,'Սունկ կարագով',-5592449,0,NULL,NULL,NULL,NULL),(337,18,'Հավի բիֆշտեքս',-65536,0,NULL,NULL,NULL,NULL),(338,18,'Ժուլիեն',-65536,0,NULL,NULL,NULL,NULL),(339,2,'Աղցան մոնակո',-65536,0,NULL,NULL,NULL,NULL),(340,18,'Տավարի ռոմշտեքս',-1,0,NULL,NULL,NULL,NULL),(341,8,'Տոմատի հյութ բաժակով',-1,0,NULL,NULL,NULL,NULL),(342,8,'Թան',-16202497,0,NULL,NULL,NULL,NULL),(343,18,'Լցոնած կարտոֆիլ',-1,0,NULL,NULL,NULL,NULL),(344,18,'Կորդոն բլյու',-11141376,0,NULL,NULL,NULL,NULL),(345,2,'Աղցան բրնձի լապշա',-1,0,NULL,NULL,NULL,NULL),(346,2,'Աղցան մառոլով',-16776961,0,NULL,NULL,NULL,NULL),(347,18,'Հավի կրծքամիս լավաշով',-1,0,NULL,NULL,NULL,NULL),(348,18,'Թրջնի բուն',-1,0,NULL,NULL,NULL,NULL),(349,18,'Խոզի ռոմշտեքս',-16776961,0,NULL,NULL,NULL,NULL),(350,18,'Ծիծակ խորոված',-65281,0,NULL,NULL,NULL,NULL),(351,13,'Kinder countruy',-1,0,NULL,NULL,NULL,NULL),(352,5,'Սպանախ',-1,0,NULL,NULL,NULL,NULL),(353,8,'Մուրաբա',-11184641,0,NULL,NULL,NULL,NULL),(354,14,'Rothmans',-1,0,NULL,NULL,NULL,NULL),(355,18,'Խաշած հավ',-256,0,NULL,NULL,NULL,NULL),(356,18,'Ձվաձեղ նրբերշիկով',-1,0,NULL,NULL,NULL,NULL),(357,5,'Մակարոն գազարով և ոլոռով',-1,0,NULL,NULL,NULL,NULL),(358,3,'Քարաճիկով սոուս',-11141249,0,NULL,NULL,NULL,NULL),(361,13,'KitKat 4',-1,0,NULL,NULL,NULL,NULL),(362,13,'M&M',-1,0,NULL,NULL,NULL,NULL),(365,13,'Nestle պլիտկա',-1,0,NULL,NULL,NULL,NULL),(366,18,'Սուլթանի մաշիկ',-256,0,NULL,NULL,NULL,NULL),(367,5,'Շրիլանկյան նախուտեստ',-1,0,NULL,NULL,NULL,NULL),(368,18,'Գերմանական նրբերշիկ',-1,0,NULL,NULL,NULL,NULL),(369,18,'Խաշած տավարի լեզու',-11141249,0,NULL,NULL,NULL,NULL),(370,5,'Խավարտ օրիգինալ',-1,0,NULL,NULL,NULL,NULL),(371,18,'Հավի դոշը մուշտակով',-1,0,NULL,NULL,NULL,NULL),(372,17,'Պաղպաղակ',-1,0,NULL,NULL,NULL,NULL),(373,17,'Ani 0.5',-1,0,NULL,NULL,NULL,NULL),(374,17,'Ice vip',-1,0,NULL,NULL,NULL,NULL),(375,17,'Ani cup',-1,0,NULL,NULL,NULL,NULL),(376,17,'Samba',-1,0,NULL,NULL,NULL,NULL),(377,17,'Eskimo',-1,0,NULL,NULL,NULL,NULL),(378,17,'Anita',-1,0,NULL,NULL,NULL,NULL),(379,17,'Charlie բաժակ',-1,0,NULL,NULL,NULL,NULL),(380,17,'Charlie փայտիկ',-1,0,NULL,NULL,NULL,NULL),(381,17,'Պաղպաղակ վալիո',-1,0,NULL,NULL,NULL,NULL),(382,17,'Cone Ani',-1,0,NULL,NULL,NULL,NULL),(383,5,'Շոգեխաշած բանջարեղեն',-65536,0,NULL,NULL,NULL,NULL),(384,18,'Տավարի մսով ռուլետ',-1,0,NULL,NULL,NULL,NULL),(385,18,'Դդմիկով կոտլետ',-21761,0,NULL,NULL,NULL,NULL),(386,5,'Հարիսա',-256,0,NULL,NULL,NULL,NULL),(387,5,'Խաշլամա տավարի',-1,0,NULL,NULL,NULL,NULL),(388,17,'Jingle',-1,0,NULL,NULL,NULL,NULL),(389,17,'Briquette',-1,0,NULL,NULL,NULL,NULL),(390,8,'Մածնաբրդոշ',-16202497,0,NULL,NULL,NULL,NULL),(391,6,'Խաչապուրի',-16202497,0,NULL,NULL,NULL,NULL),(392,4,'Ապուր դդմիկով',-1,0,NULL,NULL,NULL,NULL),(393,15,'Fresh բանան,թուրինջ,նարինջ',-43776,0,NULL,NULL,NULL,NULL),(394,17,'Ժելե',-11141121,0,NULL,NULL,NULL,NULL),(395,17,'Բանան և գիլաս',-1,0,NULL,NULL,NULL,NULL),(396,17,'Դուետ',-1,0,NULL,NULL,NULL,NULL),(397,17,'Պաղպաղակ',-5614209,0,NULL,NULL,NULL,NULL),(398,17,'Վալիո բաժակ',-5570561,0,NULL,NULL,NULL,NULL),(399,17,'Կոն մեծ',-1,0,NULL,NULL,NULL,NULL),(400,17,'Կոն փոքր',-1,0,NULL,NULL,NULL,NULL),(401,17,'CCCP',-1,0,NULL,NULL,NULL,NULL),(402,17,'Եգիպտացորեն',-1,0,NULL,NULL,NULL,NULL),(403,8,'Լիմոն',-1,0,NULL,NULL,NULL,NULL),(404,3,'Տավարի  թոք սիրտ',-1,0,NULL,NULL,NULL,NULL),(405,18,'Պաշոտ նրբերշիկով',-1,0,NULL,NULL,NULL,NULL),(406,18,'Խաշած հավի դոշ',-1,0,NULL,NULL,NULL,NULL),(407,4,'Ապուր մինեստրոնե',-129,0,NULL,NULL,NULL,NULL),(408,15,'Սառը թեյ',-65536,0,NULL,NULL,NULL,NULL),(409,18,'Գառնի յարախ',-1,0,NULL,NULL,NULL,NULL),(410,18,'Խորտիկ հավով',-1,0,NULL,NULL,NULL,NULL),(411,10,'Ջերմուկ կապրոն',-129,0,NULL,NULL,NULL,NULL),(412,10,'Ջերմուկ ապակի',-16202497,0,NULL,NULL,NULL,NULL),(413,10,'Բյուրեղ կապրոն',-256,0,NULL,NULL,NULL,NULL),(414,4,'Ապուր սիսեռով',-256,0,NULL,NULL,NULL,NULL),(415,18,'Կարտոֆիլի  ռուլետ',-1,0,NULL,NULL,NULL,NULL),(416,18,'Բրնձով տոլմա 2 հատը',-1,0,NULL,NULL,NULL,NULL),(417,2,'Աղցան սորենտո',-1,0,NULL,NULL,NULL,NULL),(418,18,'Ղափամա',-16733441,0,NULL,NULL,NULL,NULL),(419,6,'Համբուրգեր',-1,0,NULL,NULL,NULL,NULL),(420,3,'Բանջարեղենով լց սմբուկ',-1,0,NULL,NULL,NULL,NULL),(421,10,'Բյուրեղ ապակի',-16202497,0,NULL,NULL,NULL,NULL),(422,5,'Ռիզոտո',-1,0,NULL,NULL,NULL,NULL),(423,17,'Մանանա',-1,0,NULL,NULL,NULL,NULL),(424,5,'Շամպինյոն կարտոֆիլով թթվասերով',-1,0,NULL,NULL,NULL,NULL),(425,3,'Մուսակա',-1,0,NULL,NULL,NULL,NULL),(426,18,'Կիևյան կոտլետ հավի դոշով',-16202497,0,NULL,NULL,NULL,NULL),(427,2,'Աղցան վիկտորիյա',-1,0,NULL,NULL,NULL,NULL),(428,2,'Աղցան ձորաբերդ',-16776961,0,NULL,NULL,NULL,NULL),(429,18,'Ոսպով, բրնձով տոլմա 2 հատը',-1,0,NULL,NULL,NULL,NULL),(430,18,'Տապակած կարմիր պղպեղ',-11163136,0,NULL,NULL,NULL,NULL),(431,18,'Ջեռեփուկ',-21761,0,NULL,NULL,NULL,NULL),(432,10,'Նատախտարի',-1,0,NULL,NULL,NULL,NULL),(433,10,'Ապարան թան',-16202497,0,NULL,NULL,NULL,NULL),(434,10,'Հյութ մանգո',-65536,0,NULL,NULL,NULL,NULL),(435,10,'Ջուր լւսաղբյուր',-256,0,NULL,NULL,NULL,NULL),(436,10,'Տոմատի հյութ ոսկե ցլիկ',-16202497,0,NULL,NULL,NULL,NULL),(437,14,'Rothmans nano',-1,0,NULL,NULL,NULL,NULL),(438,3,'Իսպանական ռագու',-11163136,0,NULL,NULL,NULL,NULL),(439,4,'Ապուր եղինջով',-1,0,NULL,NULL,NULL,NULL),(440,5,'Սպանախ ձվով',-21761,0,NULL,NULL,NULL,NULL),(441,2,'Աղցան կեսար',-65281,0,NULL,NULL,NULL,NULL),(442,5,' Կանաչ լոբով ուտեստ',-11163136,0,NULL,NULL,NULL,NULL),(443,5,'Տապակած ոսպ',-11141249,0,NULL,NULL,NULL,NULL),(444,3,'Սոուս բեշամել',-1,0,NULL,NULL,NULL,NULL),(445,5,'Կանաչ ոլոռով ձվաձեղ',-1,0,NULL,NULL,NULL,NULL),(446,2,'Աղցան ապխտա',-129,0,NULL,NULL,NULL,NULL),(447,2,'Աղցան չինական',-1,0,NULL,NULL,NULL,NULL),(448,10,'Juicinad',-16202497,0,NULL,NULL,NULL,NULL),(449,10,'Սադաչոկ',-16202497,0,NULL,NULL,NULL,NULL),(450,15,'Կ',-1,0,NULL,NULL,NULL,NULL),(451,5,'Տապակած կաղամբ',-11141249,0,NULL,NULL,NULL,NULL),(452,15,'Առաքում',-16202497,0,NULL,NULL,NULL,NULL),(453,18,'Վիկտորիա',-1,0,NULL,NULL,NULL,NULL),(454,2,'Աղցան վիեն',-1,0,NULL,NULL,NULL,NULL),(455,5,'Պաստաքուչեն',-1,0,NULL,NULL,NULL,NULL),(457,5,'Տապակած շուշան ձվով',-1,0,NULL,NULL,NULL,NULL),(458,5,'Թաթար բորակի',-1,0,NULL,NULL,NULL,NULL),(459,5,'Կուռնիկ',-11184641,0,NULL,NULL,NULL,NULL),(460,18,'Ջոնս',-21761,0,NULL,NULL,NULL,NULL),(461,18,'Տապակած հավ',-21761,0,NULL,NULL,NULL,NULL),(462,5,'Կանաչ լոբի ձվով',-65536,0,NULL,NULL,NULL,NULL),(463,18,'Հավի կրծքամիս ցիտրուսի սոուսով',-65536,0,NULL,NULL,NULL,NULL),(464,2,'Աղցան ապխտած երշիկով',-256,0,NULL,NULL,NULL,NULL),(465,18,'Խաշած տավարի միս',-11162881,0,NULL,NULL,NULL,NULL),(466,18,'Լցոնած սմբուկ հավի մսով',-11206401,0,NULL,NULL,NULL,NULL),(467,6,'Շոկոլադե տորթ',-11163136,0,NULL,NULL,NULL,NULL),(468,6,'Հեքիաթ',-43649,0,NULL,NULL,NULL,NULL),(469,3,'Ռագու հավով',-11163136,0,NULL,NULL,NULL,NULL),(470,5,'Բուլղարական  լոբի ձվով',-43776,0,NULL,NULL,NULL,NULL),(471,4,'Ապուր կանաչ լոբով',-11141121,0,NULL,NULL,NULL,NULL),(472,6,'Էսկիմո',-65281,0,NULL,NULL,NULL,NULL),(473,18,'Բրոկոլիյով կոտլետ',-21761,0,NULL,NULL,NULL,NULL),(474,18,'Խորտիկ տավ մսով',-65536,0,NULL,NULL,NULL,NULL),(475,3,'Կարտոֆիլով  նրբերշիկ',-21889,0,NULL,NULL,NULL,NULL),(476,6,'Տորթ նրբություն',-65536,0,NULL,NULL,NULL,NULL),(478,3,'Աջապ սանդալ',-11163136,0,NULL,NULL,NULL,NULL),(479,2,'Աղցան դանդուռով',-129,0,NULL,NULL,NULL,NULL),(480,14,'Marlboro premium',-1,0,NULL,NULL,NULL,NULL),(481,18,'Հավի դոշով ուտեստ',-11141376,0,NULL,NULL,NULL,NULL),(482,3,'Իմամ բայալդի',-1,0,NULL,NULL,NULL,NULL),(483,5,'Տապակած դդմիկ',-11163136,0,NULL,NULL,NULL,NULL),(484,3,'Սոուս հավով ու պանրով',-11163009,0,NULL,NULL,NULL,NULL),(485,6,'Լոբիանո',-21761,0,NULL,NULL,NULL,NULL),(486,18,'Խեցգետնի քյաբաբ',-11141121,0,NULL,NULL,NULL,NULL),(487,3,' Տավարի թոքով սոուս',-21761,0,NULL,NULL,NULL,NULL),(488,2,'Աղցան սմբուկով',-65281,0,NULL,NULL,NULL,NULL),(489,6,'Ձյունիկ',-21761,0,NULL,NULL,NULL,NULL),(490,5,'Սունկ վեշենկա',-11141249,0,NULL,NULL,NULL,NULL),(491,5,'Բրոկոլի սոյայի սոուսով',-16755201,0,NULL,NULL,NULL,NULL),(492,5,'Խաշած եգիպտացորեն',-1,0,NULL,NULL,NULL,NULL),(493,18,'Խոզի չալաղաջ փոքր',-11141249,0,NULL,NULL,NULL,NULL),(494,18,'Բրոկոլիյով ջեռեփուկ',-129,0,NULL,NULL,NULL,NULL),(495,18,'Խոզի միս տոնական',-5570561,0,NULL,NULL,NULL,NULL),(496,5,'Տապակած սմբուկ',-11141121,0,NULL,NULL,NULL,NULL),(497,2,' Աղցան տավարի մսով',-5592449,0,NULL,NULL,NULL,NULL),(498,3,'Սոուս սնկով',-21889,0,NULL,NULL,NULL,NULL),(499,2,'Աղցան պեկինյան կաղամբով',-11184769,0,NULL,NULL,NULL,NULL),(500,2,'Աղցան կարտոֆիլով',-11163136,0,NULL,NULL,NULL,NULL),(501,18,'Խոզի մատ',-65281,0,NULL,NULL,NULL,NULL),(502,18,'Պասուց տոլմա',-11141249,0,NULL,NULL,NULL,NULL),(503,18,'Հավի կրծքամիս սպանախով',-16202497,0,NULL,NULL,NULL,NULL),(504,18,'Կարասիկ',-129,0,NULL,NULL,NULL,NULL),(505,10,'Santrope',-16202497,0,NULL,NULL,NULL,NULL),(506,6,'Թռչնի կաթ',-21761,0,NULL,NULL,NULL,NULL),(507,5,'Բրոկոլի շոգեխաշած',-5592321,0,NULL,NULL,NULL,NULL),(508,2,'Աղցան լոբիանո',-11141376,0,NULL,NULL,NULL,NULL),(509,18,'Քյուֆթա',-21761,0,NULL,NULL,NULL,NULL),(510,18,'Իքի բիր',-43776,0,NULL,NULL,NULL,NULL),(511,18,'Հավի կրծքամիս բազուկի ճավով',-5570561,0,NULL,NULL,NULL,NULL),(512,6,'Պետչենի',-129,0,NULL,NULL,NULL,NULL),(513,6,'Կարկանդակ մսով',-5570561,0,NULL,NULL,NULL,NULL),(514,6,'Հրաշք',-16711681,0,NULL,NULL,NULL,NULL),(515,5,' Բրնձով և չամիչով փլավ',-5570561,0,NULL,NULL,NULL,NULL),(516,18,'Ձուկը լավաշով',-16711681,0,NULL,NULL,NULL,NULL),(517,3,'Սացիվի',-256,0,NULL,NULL,NULL,NULL),(518,18,'Պարմեջանա',-16733441,0,NULL,NULL,NULL,NULL),(519,18,'Հավով և բանջարեղենով ջեռեփուկ',-16711681,0,NULL,NULL,NULL,NULL),(520,6,'Fresh բանան',-21889,0,NULL,NULL,NULL,NULL),(521,18,'Գերկուլեսով և սնկով կոտլետ',-129,0,NULL,NULL,NULL,NULL),(522,18,'Հավի կրծքամիս սերուցքի մեջ',-129,0,NULL,NULL,NULL,NULL),(523,18,'Խինկալի',-21761,0,NULL,NULL,NULL,NULL),(524,2,'Աղցան հավով',-16711681,0,NULL,NULL,NULL,NULL),(525,18,'Կռոշկա կարտոշկա',-22016,0,NULL,NULL,NULL,NULL),(526,18,'Մակարոնով և սնկով տապակա',-129,0,NULL,NULL,NULL,NULL),(527,6,'՝էկլեր',-16202497,0,NULL,NULL,NULL,NULL),(528,6,'Կարկանդակ կարտֆիլով',-1,0,NULL,NULL,NULL,NULL),(529,18,'Խոզի սթեյք լիմոնով',-5592449,0,NULL,NULL,NULL,NULL),(530,2,'Աղցան մասկարադ',-21761,0,NULL,NULL,NULL,NULL),(531,15,'Ամերիկանո',-16202497,0,NULL,NULL,NULL,NULL),(532,15,'Էսպրեսսո',-16202497,0,NULL,NULL,NULL,NULL),(533,18,'Հավի կոտլետ տնական',-11141249,0,NULL,NULL,NULL,NULL),(534,4,'Ապուր չխրթմա',-16776961,0,NULL,NULL,NULL,NULL),(535,6,'Զեբրա',-11141121,0,NULL,NULL,NULL,NULL),(536,18,'Նրբերշիկ',-16711681,0,NULL,NULL,NULL,NULL),(537,18,'Ջեռեփուկ սնկով մսով',-129,0,NULL,NULL,NULL,NULL),(538,3,'Սոուս հավի մսով և բանջարեղենով',-16733441,0,NULL,NULL,NULL,NULL),(539,5,'Ձվաձեղ ոլոռով նրբերշիկով',-256,0,NULL,NULL,NULL,NULL),(540,18,'Ռուլետ դդմիկով և պղպեղով',-256,0,NULL,NULL,NULL,NULL),(541,6,'Պիռամիդա',-129,0,NULL,NULL,NULL,NULL),(542,18,'Տապակած նրբերշիկ պանրով',-11141249,0,NULL,NULL,NULL,NULL),(543,15,'Տաք շոկոլադ',-16202497,0,NULL,NULL,NULL,NULL),(544,6,'Մրջնաբույն',-16733441,0,NULL,NULL,NULL,NULL),(545,18,'Նրբերշիկ վիենական',-65281,0,NULL,NULL,NULL,NULL),(546,17,'Կարագ',-21761,0,NULL,NULL,NULL,NULL),(547,6,'Ֆռեշ բանան ազնվամորի',-256,0,NULL,NULL,NULL,NULL),(548,18,'Ֆալաֆել',-5570561,0,NULL,NULL,NULL,NULL),(549,18,'Տապակած սմբուկ պանրով',-43776,0,NULL,NULL,NULL,NULL),(550,6,'Սլիվկիյով տորթ',-65536,0,NULL,NULL,NULL,NULL),(551,6,'Միկադո',-43776,0,NULL,NULL,NULL,NULL),(552,6,'Նռան հյութ',-256,0,NULL,NULL,NULL,NULL),(553,18,'Հավի կրծքամիս ֆր  ձևով',-43649,0,NULL,NULL,NULL,NULL),(554,6,'Խնձորով պիռոգ',-256,0,NULL,NULL,NULL,NULL),(555,6,'Շտրուդել',-11141249,0,NULL,NULL,NULL,NULL),(556,18,'Հավի կրծքամսով և պանրով կոտլետ',-65409,0,NULL,NULL,NULL,NULL),(557,6,'Սիգարետ',-129,0,NULL,NULL,NULL,NULL),(558,6,'Տրյուֆելե',-129,0,NULL,NULL,NULL,NULL),(559,6,'Գեներալ',-16202497,0,NULL,NULL,NULL,NULL),(560,17,'Fresh',-11141376,0,NULL,NULL,NULL,NULL),(561,18,'Խաշած հավի բուդ',-65281,0,NULL,NULL,NULL,NULL),(562,18,'Տավարի միս ֆր ձևով',-65536,0,NULL,NULL,NULL,NULL),(563,6,'Իմերիթական խաչապուրի փոքր',-256,0,NULL,NULL,NULL,NULL),(564,18,'Ժուլիենը  կարտոֆիլի մեջ',-5570561,0,NULL,NULL,NULL,NULL),(565,3,'Սոուս հավի կրծքամսով և սերուցքով',-65536,0,NULL,NULL,NULL,NULL),(566,5,'Մակարոն պղպեղով և բեկոնով',-129,0,NULL,NULL,NULL,NULL),(567,3,'Սոուս տավարի մսով և բանջարեղենով',-11141376,0,NULL,NULL,NULL,NULL),(568,3,'Սոուս հավի կրծ և քարիյով',-256,0,NULL,NULL,NULL,NULL),(569,6,'Տորթ',-5570816,0,NULL,NULL,NULL,NULL),(570,18,'Խուրջին',-11141121,0,NULL,NULL,NULL,NULL),(571,2,'Աղցան պանրով և կռաբով',-11141121,0,NULL,NULL,NULL,NULL),(572,18,'Տավարի մսով կոտլետ',-256,0,NULL,NULL,NULL,NULL),(573,3,'Սոուս տավարի լեզվով',-11141376,0,NULL,NULL,NULL,NULL),(574,18,'Ռուլետ սնկով և դդմիկով',-5570561,0,NULL,NULL,NULL,NULL),(575,18,'Տավարի միս սնկով',-65281,0,NULL,NULL,NULL,NULL),(576,18,'Ծաղկակաղմբով զապեկանկա',-5570689,0,NULL,NULL,NULL,NULL),(577,6,'Կոկոսով տորթ',-129,0,NULL,NULL,NULL,NULL),(578,5,'Վրացական ռագու',-11141121,0,NULL,NULL,NULL,NULL),(579,5,'Ժարիտ տնական',-11141249,0,NULL,NULL,NULL,NULL),(580,3,'Տապակած բանջարեղեն',-11141249,0,NULL,NULL,NULL,NULL),(581,18,'Դդմիկով և լոլիկով ջեռեուկ',-11141121,0,NULL,NULL,NULL,NULL),(582,18,'Պանրով և ձվով լավաշ ռոլ',-129,0,NULL,NULL,NULL,NULL),(583,18,'Հավի կրծքամիս սնկով',-11141121,0,NULL,NULL,NULL,NULL),(584,18,'Հավի կրծք  պանրով և խոզապուխտով',-16711681,0,NULL,NULL,NULL,NULL),(585,18,'Ռուլետ կարտոֆիլով և խոզապուխտով',-5570561,0,NULL,NULL,NULL,NULL),(586,2,'Աղցան սալվադոր',-129,0,NULL,NULL,NULL,NULL),(587,18,'Խոզապուխտով և պանրով ջեռեփուկ',-256,0,NULL,NULL,NULL,NULL),(588,18,'Գարուն',-65281,0,NULL,NULL,NULL,NULL),(589,18,'Սպագետի մսով շամպուր',-256,0,NULL,NULL,NULL,NULL),(590,18,'Տապակած կրծքամիս սմբուկով',-256,0,NULL,NULL,NULL,NULL),(591,18,'Լցոնած կարտոֆիլ  ասորտի',-43521,0,NULL,NULL,NULL,NULL),(592,18,'Տեֆտել սոուսով',-5570561,0,NULL,NULL,NULL,NULL),(593,18,'Քլաբ սենդվիչ',-11141376,0,NULL,NULL,NULL,NULL),(594,18,'Հավի բդիկ',-11163136,0,NULL,NULL,NULL,NULL),(595,18,'Քլաբ սենդվիչ բեկոնով և պանրով',-5570561,0,NULL,NULL,NULL,NULL),(596,18,'Ռուլետ բազուկով և գազարով',-5570561,0,NULL,NULL,NULL,NULL),(597,18,'Կոտլետ Միքս',-43776,0,NULL,NULL,NULL,NULL),(598,18,'Հավով նրբաբլիթ',-5592576,0,NULL,NULL,NULL,NULL),(599,18,'Հավի ուսիկ',-11141249,0,NULL,NULL,NULL,NULL),(600,18,'Զապեկանկա բրոկոլիյով հավով',-5592321,0,NULL,NULL,NULL,NULL),(601,18,'Կարտոֆիլով և մսով  ջեռեփուկ',-5570561,0,NULL,NULL,NULL,NULL),(602,6,'Շտիռլեց',-256,0,NULL,NULL,NULL,NULL),(603,18,'Սունկ բեկոն պանիր',-16711681,0,NULL,NULL,NULL,NULL),(604,18,'Ծաղկակաղամբով կոտլետ',-43649,0,NULL,NULL,NULL,NULL),(605,5,'Տապակած կարտոֆիլ',-16202497,0,NULL,NULL,NULL,NULL),(606,5,'Կարտոֆիլ տուշոնկայով',-11141249,0,NULL,NULL,NULL,NULL),(607,5,'Բրնձով և ոսպով փլավ',-256,0,NULL,NULL,NULL,NULL),(608,18,'Խաշած ձուկ սիգա',-256,0,NULL,NULL,NULL,NULL),(609,8,'Թթու',-16202497,0,NULL,NULL,NULL,NULL),(610,18,'Դդումով կոտլետ',-21761,0,NULL,NULL,NULL,NULL),(611,5,'Կարտոֆիլ գյուղական',-21761,0,NULL,NULL,NULL,NULL),(612,18,'Իշխանի ֆիլե',-129,0,NULL,NULL,NULL,NULL),(613,18,'Տապակած սունկ ձվով',-129,0,NULL,NULL,NULL,NULL),(614,6,'Տորթ շոկոլադային',-5570561,0,NULL,NULL,NULL,NULL),(615,18,'Ջեռեփուկ հավի մսով',-129,0,NULL,NULL,NULL,NULL),(616,5,'Մեքսիկակական  ռագու',-129,0,NULL,NULL,NULL,NULL),(617,5,'Բրինձ հավի մսով',-16711681,0,NULL,NULL,NULL,NULL),(618,6,'Տղամարդու կապրիզ',-1,0,NULL,NULL,NULL,NULL),(619,18,'Լցոնած սունկ ձվով',-129,0,NULL,NULL,NULL,NULL),(620,6,'Պետչենի ֆռանսուա',-256,0,NULL,NULL,NULL,NULL),(621,6,'Fresh բանան կիվի',-1,0,NULL,NULL,NULL,NULL),(622,18,'Բրոկոլիյով և հավի մսով ջեռեփուկ',-5570689,0,NULL,NULL,NULL,NULL),(623,2,'Աղցան բանջարեղենային',-11162881,0,NULL,NULL,NULL,NULL),(624,6,'Մրգի ասորտի',-1,0,NULL,NULL,NULL,NULL),(625,2,'Աղցան սմբուկով և հավի կրծքամսով',-256,0,NULL,NULL,NULL,NULL),(626,5,'Բլղուրով և սմբուկով փլավ',-5570561,0,NULL,NULL,NULL,NULL),(627,5,'Կարտոֆիլ ջեռոցում',-21761,0,NULL,NULL,NULL,NULL),(628,17,'Խաշած ձու',-256,0,NULL,NULL,NULL,NULL),(631,18,'Հավի կրծքամիսը նրբաբլիթով',-5570561,0,NULL,NULL,NULL,NULL),(634,18,'Կիևյան կոտլետ պան  և հավի կրծք',-65536,0,NULL,NULL,NULL,NULL),(636,6,'Ֆռեշ բանան նուտելա',-129,0,NULL,NULL,NULL,NULL),(638,5,'Բանջարեղենային ռագու',-1,0,NULL,NULL,NULL,NULL),(639,15,'Սուրճ փարիզյան',-1,0,NULL,NULL,NULL,NULL),(640,2,'Աղցան մաղցան',-1,0,0,0,0,'Աղցան մաղցան');
/*!40000 ALTER TABLE `d_dish` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_dish_comment`
--

DROP TABLE IF EXISTS `d_dish_comment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_dish_comment` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_dish_comment`
--

LOCK TABLES `d_dish_comment` WRITE;
/*!40000 ALTER TABLE `d_dish_comment` DISABLE KEYS */;
/*!40000 ALTER TABLE `d_dish_comment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_dish_state`
--

DROP TABLE IF EXISTS `d_dish_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_dish_state` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_dish_state`
--

LOCK TABLES `d_dish_state` WRITE;
/*!40000 ALTER TABLE `d_dish_state` DISABLE KEYS */;
INSERT INTO `d_dish_state` VALUES (0,'Ոչ ակտիվ'),(1,'Ակտիվ');
/*!40000 ALTER TABLE `d_dish_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_img`
--

DROP TABLE IF EXISTS `d_img`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_img` (
  `f_id` int(11) NOT NULL,
  `f_data` mediumblob,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_img`
--

LOCK TABLES `d_img` WRITE;
/*!40000 ALTER TABLE `d_img` DISABLE KEYS */;
/*!40000 ALTER TABLE `d_img` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_menu`
--

DROP TABLE IF EXISTS `d_menu`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_menu` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_menu` int(11) DEFAULT NULL,
  `f_dish` int(11) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_print1` tinytext,
  `f_print2` tinytext,
  `f_state` smallint(6) DEFAULT '1',
  PRIMARY KEY (`f_id`),
  KEY `fk_menu_menu_idx` (`f_menu`),
  KEY `fk_menu_dish_idx` (`f_dish`),
  KEY `fk_menu_store_idx` (`f_store`),
  CONSTRAINT `fk_menu_dish` FOREIGN KEY (`f_dish`) REFERENCES `d_dish` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_menu_menu` FOREIGN KEY (`f_menu`) REFERENCES `d_menu_names` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_menu_store` FOREIGN KEY (`f_store`) REFERENCES `c_storages` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=595 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_menu`
--

LOCK TABLES `d_menu` WRITE;
/*!40000 ALTER TABLE `d_menu` DISABLE KEYS */;
INSERT INTO `d_menu` VALUES (1,1,27,300.00,5,NULL,NULL,1),(2,1,10,450.00,2,NULL,NULL,0),(3,1,29,450.00,5,NULL,NULL,1),(4,1,28,250.00,2,NULL,NULL,1),(5,1,11,450.00,2,NULL,NULL,1),(6,1,497,400.00,5,NULL,NULL,0),(7,1,515,400.00,2,NULL,NULL,0),(8,1,442,750.00,2,NULL,NULL,0),(9,1,230,650.00,2,NULL,NULL,0),(10,1,487,800.00,2,NULL,NULL,0),(11,1,527,300.00,4,NULL,NULL,1),(12,1,322,250.00,1,NULL,NULL,0),(13,1,373,650.00,1,NULL,NULL,0),(14,1,375,120.00,1,NULL,NULL,0),(15,1,378,120.00,1,NULL,NULL,0),(16,1,142,300.00,1,NULL,NULL,0),(17,1,144,320.00,1,NULL,NULL,0),(18,1,145,320.00,1,NULL,NULL,0),(19,1,389,120.00,1,NULL,NULL,0),(20,1,314,250.00,1,NULL,NULL,0),(21,1,401,140.00,1,NULL,NULL,0),(22,1,379,120.00,1,NULL,NULL,0),(23,1,380,120.00,1,NULL,NULL,0),(24,1,382,120.00,1,NULL,NULL,0),(25,1,124,6.00,1,NULL,NULL,0),(26,1,129,5.00,1,NULL,NULL,0),(27,1,126,600.00,1,NULL,NULL,0),(28,1,122,500.00,1,NULL,NULL,0),(29,1,128,550.00,1,NULL,NULL,0),(30,1,152,850.00,1,NULL,NULL,0),(31,1,153,850.00,1,NULL,NULL,0),(32,1,158,220.00,1,NULL,NULL,0),(33,1,377,120.00,1,NULL,NULL,0),(34,1,181,2600.00,1,NULL,NULL,0),(35,1,560,600.00,1,NULL,NULL,0),(36,1,520,500.00,2,NULL,NULL,1),(37,1,185,550.00,2,NULL,NULL,0),(38,1,87,600.00,2,NULL,NULL,1),(39,1,621,600.00,4,NULL,NULL,1),(40,1,393,500.00,2,NULL,NULL,1),(41,1,374,200.00,1,NULL,NULL,0),(42,1,388,150.00,1,NULL,NULL,0),(43,1,448,150.00,1,NULL,NULL,1),(44,1,133,400.00,1,NULL,NULL,0),(45,1,147,550.00,1,NULL,NULL,0),(46,1,166,600.00,1,NULL,NULL,0),(47,1,150,550.00,1,NULL,NULL,0),(48,1,180,250.00,1,NULL,NULL,0),(49,1,179,350.00,1,NULL,NULL,0),(50,1,174,650.00,1,NULL,NULL,0),(51,1,178,350.00,1,NULL,NULL,0),(52,1,351,200.00,1,NULL,NULL,0),(53,1,177,250.00,1,NULL,NULL,0),(54,1,361,250.00,1,NULL,NULL,0),(55,1,141,370.00,1,NULL,NULL,0),(56,1,362,250.00,1,NULL,NULL,1),(57,1,195,380.00,1,NULL,NULL,1),(58,1,480,1000.00,1,NULL,NULL,0),(59,1,137,600.00,1,NULL,NULL,0),(60,1,231,600.00,1,NULL,NULL,0),(61,1,136,550.00,1,NULL,NULL,0),(62,1,193,150.00,1,NULL,NULL,1),(63,1,154,280.00,1,NULL,NULL,0),(64,1,201,200.00,1,NULL,NULL,1),(65,1,365,500.00,1,NULL,NULL,1),(66,1,163,200.00,1,NULL,NULL,1),(67,1,160,250.00,1,NULL,NULL,1),(68,1,139,850.00,1,NULL,NULL,0),(69,1,140,850.00,1,NULL,NULL,0),(70,1,146,300.00,1,NULL,NULL,0),(71,1,183,1650.00,1,NULL,NULL,0),(72,1,182,350.00,1,NULL,NULL,0),(73,1,354,370.00,1,NULL,NULL,0),(74,1,437,400.00,1,NULL,NULL,0),(75,1,376,60.00,1,NULL,NULL,0),(76,1,171,300.00,1,NULL,NULL,0),(77,1,505,250.00,1,NULL,NULL,1),(78,1,164,250.00,1,NULL,NULL,0),(79,1,197,250.00,1,NULL,NULL,1),(80,1,199,380.00,1,NULL,NULL,1),(81,1,184,250.00,1,NULL,NULL,0),(82,1,151,650.00,1,NULL,NULL,0),(83,1,130,350.00,1,NULL,NULL,0),(84,1,131,320.00,1,NULL,NULL,0),(85,1,157,420.00,1,NULL,NULL,0),(86,1,156,450.00,1,NULL,NULL,0),(87,1,155,470.00,1,NULL,NULL,0),(88,1,75,600.00,2,NULL,NULL,0),(89,1,446,450.00,5,NULL,NULL,0),(90,1,464,450.00,5,NULL,NULL,0),(91,1,32,300.00,5,NULL,NULL,0),(92,1,289,300.00,5,NULL,NULL,0),(93,1,623,300.00,5,NULL,NULL,0),(94,1,345,450.00,5,NULL,NULL,0),(95,1,25,350.00,5,NULL,NULL,0),(96,1,33,350.00,5,NULL,NULL,0),(97,1,479,300.00,2,NULL,NULL,0),(98,1,23,300.00,5,NULL,NULL,0),(99,1,5,300.00,5,NULL,NULL,0),(100,1,508,300.00,5,NULL,NULL,0),(101,1,20,450.00,5,NULL,NULL,0),(102,1,30,350.00,5,NULL,NULL,0),(103,1,17,200.00,5,NULL,NULL,0),(104,1,4,350.00,5,'','',0),(105,1,293,650.00,5,NULL,NULL,0),(106,1,500,300.00,5,NULL,NULL,0),(107,1,441,400.00,5,NULL,NULL,0),(108,1,19,300.00,5,NULL,NULL,0),(109,1,294,400.00,5,NULL,NULL,0),(110,1,31,350.00,5,NULL,NULL,0),(111,1,524,400.00,5,NULL,NULL,0),(112,1,299,450.00,5,NULL,NULL,0),(113,1,8,350.00,5,NULL,NULL,0),(114,1,428,450.00,5,NULL,NULL,0),(115,1,2,400.00,5,NULL,NULL,1),(116,1,346,300.00,5,NULL,NULL,0),(117,1,530,350.00,5,NULL,NULL,0),(118,1,57,450.00,5,NULL,NULL,0),(119,1,339,450.00,5,NULL,NULL,0),(120,1,317,300.00,5,NULL,NULL,0),(121,1,447,450.00,5,NULL,NULL,0),(122,1,571,400.00,5,NULL,NULL,0),(123,1,499,400.00,5,NULL,NULL,0),(124,1,280,450.00,5,NULL,NULL,0),(125,1,586,450.00,5,NULL,NULL,0),(126,1,488,450.00,5,NULL,NULL,0),(127,1,625,400.00,5,NULL,NULL,0),(128,1,417,450.00,5,NULL,NULL,0),(129,1,18,450.00,5,NULL,NULL,0),(130,1,454,450.00,5,NULL,NULL,0),(131,1,427,300.00,5,NULL,NULL,0),(132,1,16,300.00,5,NULL,NULL,0),(133,1,243,300.00,5,NULL,NULL,0),(134,1,531,250.00,1,NULL,NULL,1),(135,1,433,250.00,1,NULL,NULL,0),(136,1,220,300.00,2,NULL,NULL,0),(137,1,52,550.00,2,NULL,NULL,0),(138,1,274,300.00,2,NULL,NULL,0),(139,1,392,250.00,2,NULL,NULL,0),(140,1,439,300.00,2,NULL,NULL,0),(141,1,278,550.00,2,NULL,NULL,0),(142,1,471,300.00,2,NULL,NULL,0),(143,1,56,300.00,2,NULL,NULL,0),(144,1,323,300.00,2,NULL,NULL,0),(145,1,50,400.00,2,NULL,NULL,0),(146,1,60,300.00,2,NULL,NULL,0),(147,1,42,300.00,2,NULL,NULL,0),(148,1,407,300.00,2,NULL,NULL,0),(149,1,58,250.00,2,NULL,NULL,0),(150,1,59,400.00,2,NULL,NULL,0),(151,1,534,450.00,2,NULL,NULL,0),(152,1,414,600.00,5,NULL,NULL,0),(153,1,53,350.00,2,NULL,NULL,0),(154,1,51,550.00,2,NULL,NULL,0),(155,1,478,650.00,2,NULL,NULL,0),(156,1,452,100.00,2,NULL,NULL,1),(157,1,324,400.00,2,NULL,NULL,0),(158,1,262,750.00,2,NULL,NULL,0),(159,1,330,650.00,2,NULL,NULL,0),(160,1,15,650.00,2,NULL,NULL,0),(161,1,395,700.00,2,NULL,NULL,0),(162,1,300,500.00,2,NULL,NULL,0),(163,1,244,450.00,2,NULL,NULL,0),(164,1,638,650.00,2,NULL,NULL,1),(165,1,420,400.00,2,NULL,NULL,0),(166,1,74,750.00,2,NULL,NULL,0),(167,1,34,650.00,2,NULL,NULL,0),(168,1,286,250.00,2,NULL,NULL,0),(169,1,68,450.00,2,NULL,NULL,0),(170,1,626,450.00,2,NULL,NULL,0),(171,1,421,200.00,1,NULL,NULL,1),(172,1,413,200.00,1,NULL,NULL,1),(173,1,71,800.00,2,NULL,NULL,0),(174,1,114,400.00,1,NULL,NULL,1),(175,1,470,650.00,2,NULL,NULL,0),(176,1,245,600.00,2,NULL,NULL,0),(177,1,617,500.00,2,NULL,NULL,0),(178,1,292,400.00,5,NULL,NULL,0),(179,1,234,400.00,2,NULL,NULL,0),(180,1,12,500.00,2,NULL,NULL,0),(181,1,607,400.00,2,NULL,NULL,0),(182,1,416,150.00,2,NULL,NULL,0),(183,1,507,800.00,2,NULL,NULL,0),(184,1,491,800.00,2,NULL,NULL,0),(185,1,622,450.00,2,NULL,NULL,0),(186,1,473,300.00,2,NULL,NULL,0),(187,1,494,500.00,2,NULL,NULL,0),(188,1,107,300.00,2,NULL,NULL,0),(189,1,97,350.00,2,NULL,NULL,0),(190,1,409,650.00,2,NULL,NULL,0),(191,1,313,500.00,2,NULL,NULL,0),(192,1,588,500.00,2,NULL,NULL,0),(193,1,559,350.00,4,NULL,NULL,0),(194,1,521,900.00,2,NULL,NULL,0),(195,1,368,750.00,2,NULL,NULL,0),(196,1,43,650.00,2,NULL,NULL,0),(197,1,581,350.00,2,NULL,NULL,0),(198,1,385,250.00,2,NULL,NULL,0),(199,1,78,400.00,2,NULL,NULL,0),(200,1,66,500.00,2,NULL,NULL,0),(201,1,610,250.00,2,NULL,NULL,0),(202,1,396,120.00,1,NULL,NULL,0),(203,1,402,250.00,2,NULL,NULL,0),(204,1,600,450.00,2,NULL,NULL,0),(205,1,535,300.00,4,NULL,NULL,0),(206,1,169,350.00,1,NULL,NULL,1),(207,1,472,300.00,4,NULL,NULL,0),(208,1,532,250.00,1,NULL,NULL,1),(209,1,458,300.00,2,NULL,NULL,0),(210,1,342,150.00,2,NULL,NULL,1),(211,1,200,250.00,1,NULL,NULL,0),(212,1,165,250.00,1,NULL,NULL,1),(213,1,76,650.00,2,NULL,NULL,0),(214,1,187,150.00,1,NULL,NULL,1),(215,1,334,150.00,2,NULL,NULL,1),(216,1,609,150.00,2,NULL,NULL,1),(217,1,506,300.00,4,NULL,NULL,0),(218,1,348,450.00,2,NULL,NULL,0),(219,1,579,250.00,2,NULL,NULL,0),(220,1,394,350.00,2,NULL,NULL,0),(221,1,338,250.00,2,NULL,NULL,0),(222,1,564,650.00,2,NULL,NULL,0),(223,1,21,400.00,4,NULL,NULL,0),(224,1,482,650.00,2,NULL,NULL,0),(225,1,117,250.00,2,NULL,NULL,1),(226,1,563,200.00,4,NULL,NULL,0),(227,1,612,850.00,2,NULL,NULL,0),(228,1,329,400.00,2,NULL,NULL,0),(229,1,438,650.00,2,NULL,NULL,0),(230,1,510,850.00,2,NULL,NULL,0),(231,1,95,500.00,2,NULL,NULL,0),(232,1,202,100.00,2,NULL,NULL,1),(233,1,132,250.00,1,NULL,NULL,1),(234,1,105,250.00,2,NULL,NULL,0),(235,1,403,350.00,2,NULL,NULL,1),(236,1,309,250.00,1,NULL,NULL,0),(237,1,170,300.00,1,NULL,NULL,0),(238,1,203,200.00,1,NULL,NULL,0),(239,1,115,450.00,1,NULL,NULL,1),(240,1,205,550.00,1,NULL,NULL,0),(241,1,485,250.00,4,NULL,NULL,0),(242,1,327,800.00,2,NULL,NULL,0),(243,1,3,80.00,4,NULL,NULL,0),(244,1,272,650.00,2,NULL,NULL,0),(245,1,343,450.00,2,NULL,NULL,0),(246,1,591,450.00,2,NULL,NULL,0),(247,1,247,450.00,2,NULL,NULL,0),(248,1,466,700.00,2,NULL,NULL,0),(249,1,619,550.00,2,NULL,NULL,0),(250,1,258,650.00,2,NULL,NULL,0),(251,1,40,350.00,2,NULL,NULL,0),(252,1,492,250.00,2,NULL,NULL,0),(253,1,355,650.00,2,NULL,NULL,0),(254,1,561,650.00,2,NULL,NULL,0),(255,1,406,650.00,2,NULL,NULL,0),(256,1,628,150.00,1,NULL,NULL,1),(257,1,608,500.00,2,NULL,NULL,0),(258,1,369,700.00,2,NULL,NULL,0),(259,1,465,800.00,2,NULL,NULL,0),(260,1,242,850.00,2,NULL,NULL,0),(261,1,387,950.00,2,NULL,NULL,0),(262,1,391,200.00,4,NULL,NULL,1),(263,1,83,250.00,4,NULL,NULL,0),(264,1,225,450.00,2,NULL,NULL,0),(265,1,370,550.00,2,NULL,NULL,0),(266,1,486,650.00,2,NULL,NULL,0),(267,1,523,200.00,2,NULL,NULL,0),(268,1,554,250.00,4,NULL,NULL,0),(269,1,587,650.00,2,NULL,NULL,0),(270,1,73,450.00,2,NULL,NULL,0),(271,1,39,700.00,2,NULL,NULL,0),(272,1,501,750.00,2,NULL,NULL,0),(273,1,495,700.00,2,NULL,NULL,0),(274,1,256,800.00,2,NULL,NULL,0),(275,1,251,550.00,2,NULL,NULL,0),(276,1,295,850.00,2,NULL,NULL,0),(277,1,493,650.00,2,NULL,NULL,0),(278,1,349,650.00,2,NULL,NULL,0),(279,1,255,750.00,2,NULL,NULL,0),(280,1,303,800.00,2,NULL,NULL,0),(281,1,529,700.00,2,NULL,NULL,0),(282,1,108,650.00,2,NULL,NULL,0),(283,1,410,350.00,2,NULL,NULL,0),(284,1,474,350.00,2,NULL,NULL,0),(285,1,570,350.00,2,NULL,NULL,0),(286,1,6,300.00,2,NULL,NULL,0),(287,1,72,650.00,2,NULL,NULL,0),(288,1,64,650.00,2,NULL,NULL,0),(289,1,604,250.00,2,NULL,NULL,0),(290,1,576,550.00,2,NULL,NULL,0),(291,1,350,200.00,2,NULL,NULL,0),(292,1,173,200.00,1,NULL,NULL,0),(293,1,450,200.00,1,NULL,NULL,0),(294,1,270,250.00,2,NULL,NULL,0),(295,1,462,750.00,2,NULL,NULL,0),(296,1,445,700.00,2,NULL,NULL,0),(297,1,106,200.00,2,NULL,NULL,0),(298,1,546,150.00,2,NULL,NULL,1),(299,1,504,650.00,2,NULL,NULL,0),(300,1,259,250.00,4,NULL,NULL,1),(301,1,528,200.00,3,NULL,NULL,1),(302,1,513,250.00,4,NULL,NULL,0),(303,1,279,200.00,4,NULL,NULL,0),(304,1,611,400.00,2,NULL,NULL,0),(305,1,627,400.00,2,NULL,NULL,0),(306,1,606,550.00,2,NULL,NULL,0),(307,1,415,150.00,2,NULL,NULL,0),(308,1,475,700.00,2,NULL,NULL,0),(309,1,601,500.00,2,NULL,NULL,0),(310,1,24,150.00,4,NULL,NULL,0),(311,1,249,200.00,2,NULL,NULL,0),(312,1,238,450.00,2,NULL,NULL,0),(313,1,149,100.00,1,NULL,NULL,0),(314,1,148,150.00,1,NULL,NULL,1),(315,1,102,80.00,2,NULL,NULL,1),(316,1,100,800.00,2,NULL,NULL,0),(317,1,207,650.00,1,NULL,NULL,0),(318,1,306,550.00,2,NULL,NULL,0),(319,1,426,650.00,2,NULL,NULL,0),(320,1,239,700.00,2,NULL,NULL,0),(321,1,634,700.00,2,NULL,NULL,0),(322,1,302,550.00,2,NULL,NULL,0),(323,1,308,450.00,2,NULL,NULL,0),(324,1,103,250.00,2,NULL,NULL,0),(325,1,577,350.00,4,NULL,NULL,0),(326,1,189,150.00,2,NULL,NULL,1),(327,1,399,220.00,1,NULL,NULL,0),(328,1,400,180.00,1,NULL,NULL,0),(329,1,597,550.00,2,NULL,NULL,0),(330,1,344,600.00,2,NULL,NULL,0),(331,1,459,450.00,2,NULL,NULL,0),(332,1,525,500.00,2,NULL,NULL,0),(333,1,224,350.00,3,NULL,NULL,0),(334,1,46,450.00,2,NULL,NULL,0),(335,1,419,650.00,5,NULL,NULL,0),(336,1,312,650.00,2,NULL,NULL,0),(337,1,594,400.00,2,NULL,NULL,0),(338,1,325,550.00,2,NULL,NULL,0),(339,1,337,650.00,2,NULL,NULL,0),(340,1,254,650.00,2,NULL,NULL,0),(341,1,98,650.00,2,NULL,NULL,0),(342,1,371,650.00,2,NULL,NULL,0),(343,1,55,650.00,2,NULL,NULL,0),(344,1,481,800.00,2,NULL,NULL,0),(345,1,263,750.00,2,NULL,NULL,0),(346,1,62,400.00,2,NULL,NULL,0),(347,1,533,600.00,2,NULL,NULL,0),(348,1,48,600.00,2,NULL,NULL,0),(349,1,584,750.00,2,NULL,NULL,0),(350,1,320,650.00,2,NULL,NULL,0),(351,1,511,650.00,2,NULL,NULL,0),(352,1,347,700.00,5,NULL,NULL,0),(353,1,522,700.00,2,NULL,NULL,0),(354,1,583,700.00,2,NULL,NULL,0),(355,1,503,650.00,2,NULL,NULL,0),(356,1,463,650.00,2,NULL,NULL,0),(357,1,553,650.00,2,NULL,NULL,0),(358,1,631,650.00,2,NULL,NULL,0),(359,1,556,600.00,2,NULL,NULL,0),(360,1,208,400.00,2,NULL,NULL,0),(361,1,69,450.00,2,NULL,NULL,0),(362,1,232,650.00,2,NULL,NULL,0),(363,1,599,650.00,2,NULL,NULL,0),(364,1,332,600.00,2,NULL,NULL,0),(365,1,311,550.00,2,NULL,NULL,0),(366,1,335,600.00,2,NULL,NULL,0),(367,1,519,700.00,2,NULL,NULL,0),(368,1,598,450.00,2,NULL,NULL,0),(369,1,206,450.00,1,NULL,NULL,0),(370,1,386,400.00,2,NULL,NULL,0),(371,1,188,30.00,4,NULL,NULL,1),(372,1,468,250.00,2,NULL,NULL,0),(373,1,172,250.00,1,NULL,NULL,0),(374,1,434,350.00,1,NULL,NULL,0),(375,1,134,450.00,1,NULL,NULL,1),(376,1,38,350.00,2,NULL,NULL,0),(377,1,315,200.00,2,NULL,NULL,1),(378,1,319,250.00,2,NULL,NULL,0),(379,1,321,250.00,2,NULL,NULL,0),(380,1,96,200.00,2,NULL,NULL,0),(381,1,514,300.00,4,NULL,NULL,0),(382,1,54,250.00,2,NULL,NULL,0),(383,1,489,250.00,4,NULL,NULL,0),(384,1,516,600.00,2,NULL,NULL,0),(385,1,227,350.00,2,NULL,NULL,0),(386,1,356,600.00,2,NULL,NULL,0),(387,1,539,800.00,2,NULL,NULL,0),(388,1,418,450.00,2,NULL,NULL,0),(389,1,240,950.00,2,NULL,NULL,0),(390,1,143,350.00,1,NULL,NULL,0),(391,1,218,300.00,1,NULL,NULL,1),(392,1,390,200.00,2,NULL,NULL,1),(393,1,301,400.00,2,NULL,NULL,0),(394,1,357,350.00,2,NULL,NULL,0),(395,1,566,450.00,2,NULL,NULL,0),(396,1,241,450.00,2,NULL,NULL,0),(397,1,119,550.00,2,NULL,NULL,0),(398,1,526,450.00,2,NULL,NULL,0),(399,1,423,400.00,5,NULL,NULL,0),(400,1,223,650.00,2,NULL,NULL,0),(401,1,305,200.00,2,NULL,NULL,1),(402,1,616,650.00,2,NULL,NULL,0),(403,1,298,500.00,2,NULL,NULL,0),(404,1,110,750.00,2,NULL,NULL,0),(405,1,246,600.00,2,NULL,NULL,0),(406,1,14,450.00,4,NULL,NULL,0),(407,1,276,200.00,4,NULL,NULL,1),(408,1,551,400.00,4,NULL,NULL,0),(409,1,273,300.00,4,NULL,NULL,0),(410,1,209,850.00,1,NULL,NULL,0),(411,1,425,650.00,2,NULL,NULL,0),(412,1,80,350.00,4,NULL,NULL,1),(413,1,353,200.00,2,NULL,NULL,0),(414,1,285,300.00,4,NULL,NULL,0),(415,1,624,1000.00,2,NULL,NULL,1),(416,1,544,150.00,4,NULL,NULL,1),(417,1,167,370.00,1,NULL,NULL,1),(418,1,85,300.00,4,NULL,NULL,0),(419,1,88,300.00,4,NULL,NULL,1),(420,1,432,300.00,1,NULL,NULL,0),(421,1,26,350.00,4,NULL,NULL,1),(422,1,90,350.00,4,NULL,NULL,0),(423,1,86,200.00,4,NULL,NULL,0),(424,1,552,600.00,4,NULL,NULL,1),(425,1,536,250.00,2,NULL,NULL,0),(426,1,545,550.00,2,NULL,NULL,0),(427,1,7,300.00,4,NULL,NULL,0),(428,1,424,850.00,2,NULL,NULL,0),(429,1,116,600.00,2,NULL,NULL,0),(430,1,383,700.00,2,NULL,NULL,0),(431,1,467,350.00,4,NULL,NULL,0),(432,1,602,250.00,4,NULL,NULL,0),(433,1,555,250.00,4,NULL,NULL,0),(434,1,367,600.00,2,NULL,NULL,0),(435,1,268,250.00,4,NULL,NULL,0),(436,1,226,250.00,2,NULL,NULL,0),(437,1,429,150.00,2,NULL,NULL,0),(438,1,67,450.00,2,NULL,NULL,0),(439,1,271,750.00,2,NULL,NULL,0),(440,1,22,650.00,2,NULL,NULL,0),(441,1,186,650.00,2,NULL,NULL,0),(442,1,213,500.00,1,NULL,NULL,0),(443,1,248,150.00,1,NULL,NULL,0),(444,1,216,600.00,1,NULL,NULL,0),(445,1,372,200.00,2,NULL,NULL,1),(446,1,397,300.00,2,NULL,NULL,1),(447,1,381,1900.00,1,NULL,NULL,0),(448,1,222,100.00,2,NULL,NULL,1),(449,1,257,300.00,4,NULL,NULL,0),(450,1,582,350.00,2,NULL,NULL,0),(451,1,405,550.00,2,NULL,NULL,0),(452,1,502,350.00,2,NULL,NULL,0),(453,1,79,450.00,2,NULL,NULL,0),(454,1,455,450.00,2,NULL,NULL,0),(455,1,518,400.00,2,NULL,NULL,0),(456,1,47,100.00,2,NULL,NULL,0),(457,1,261,350.00,1,NULL,NULL,1),(458,1,113,350.00,1,NULL,NULL,1),(459,1,112,200.00,1,NULL,NULL,1),(460,1,168,250.00,1,NULL,NULL,1),(461,1,512,150.00,4,NULL,NULL,0),(462,1,620,200.00,4,NULL,NULL,0),(463,1,541,300.00,2,NULL,NULL,0),(464,1,192,600.00,1,NULL,NULL,1),(465,1,333,450.00,2,NULL,NULL,0),(466,1,37,350.00,2,NULL,NULL,0),(467,1,9,600.00,5,NULL,NULL,0),(468,1,431,350.00,2,NULL,NULL,0),(469,1,615,550.00,2,NULL,NULL,0),(470,1,13,400.00,2,NULL,NULL,0),(471,1,537,650.00,2,NULL,NULL,0),(472,1,412,200.00,1,NULL,NULL,1),(473,1,411,250.00,1,NULL,NULL,1),(474,1,460,800.00,2,NULL,NULL,0),(475,1,138,200.00,1,NULL,NULL,0),(476,1,191,300.00,1,NULL,NULL,1),(477,1,435,170.00,1,NULL,NULL,1),(478,1,77,650.00,2,NULL,NULL,0),(479,1,469,700.00,2,NULL,NULL,0),(480,1,422,450.00,2,NULL,NULL,0),(481,1,237,600.00,2,NULL,NULL,0),(482,1,135,600.00,1,NULL,NULL,0),(483,1,210,600.00,1,NULL,NULL,0),(484,1,212,500.00,1,NULL,NULL,0),(485,1,596,250.00,2,NULL,NULL,0),(486,1,540,400.00,2,NULL,NULL,0),(487,1,250,550.00,2,NULL,NULL,0),(488,1,84,300.00,4,NULL,NULL,0),(489,1,585,400.00,2,NULL,NULL,0),(490,1,574,450.00,2,NULL,NULL,0),(491,1,449,250.00,1,NULL,NULL,0),(492,1,318,450.00,2,NULL,NULL,0),(493,1,269,450.00,2,NULL,NULL,0),(494,1,408,300.00,1,NULL,NULL,1),(495,1,221,500.00,2,NULL,NULL,0),(496,1,252,500.00,2,NULL,NULL,0),(497,1,517,850.00,2,NULL,NULL,0),(498,1,266,450.00,4,NULL,NULL,1),(499,1,283,250.00,4,NULL,NULL,0),(500,1,557,75.00,4,NULL,NULL,0),(501,1,550,350.00,4,NULL,NULL,0),(502,1,296,600.00,2,NULL,NULL,0),(503,1,82,350.00,4,NULL,NULL,0),(504,1,310,100.00,2,NULL,NULL,1),(505,1,444,750.00,2,NULL,NULL,0),(506,1,235,800.00,2,NULL,NULL,0),(507,1,568,750.00,2,NULL,NULL,0),(508,1,565,750.00,2,NULL,NULL,0),(509,1,538,700.00,2,NULL,NULL,0),(510,1,484,750.00,5,NULL,NULL,0),(511,1,229,650.00,2,NULL,NULL,0),(512,1,498,550.00,2,NULL,NULL,0),(513,1,573,850.00,2,NULL,NULL,0),(514,1,567,850.00,2,NULL,NULL,0),(515,1,366,600.00,2,NULL,NULL,0),(516,1,603,400.00,2,NULL,NULL,0),(517,1,336,400.00,2,NULL,NULL,0),(518,1,490,850.00,2,NULL,NULL,0),(519,1,228,150.00,2,NULL,NULL,1),(520,1,639,200.00,2,NULL,NULL,1),(521,1,589,700.00,2,NULL,NULL,0),(522,1,264,550.00,2,'','',0),(523,1,45,400.00,2,NULL,NULL,0),(524,1,352,600.00,2,NULL,NULL,0),(525,1,440,650.00,2,NULL,NULL,0),(526,1,49,300.00,2,NULL,NULL,0),(527,1,398,250.00,1,NULL,NULL,0),(528,1,99,350.00,2,NULL,NULL,0),(529,1,41,300.00,2,NULL,NULL,0),(530,1,453,550.00,2,NULL,NULL,0),(531,1,578,650.00,2,NULL,NULL,0),(532,1,297,500.00,2,NULL,NULL,0),(533,1,580,800.00,2,NULL,NULL,0),(534,1,63,600.00,2,NULL,NULL,0),(535,1,483,200.00,2,NULL,NULL,0),(536,1,451,400.00,2,NULL,NULL,0),(537,1,430,250.00,2,NULL,NULL,0),(538,1,605,450.00,2,NULL,NULL,0),(539,1,590,700.00,2,NULL,NULL,0),(540,1,461,650.00,2,NULL,NULL,0),(541,1,291,650.00,2,NULL,NULL,0),(542,1,307,650.00,2,NULL,NULL,0),(543,1,542,550.00,2,NULL,NULL,0),(544,1,457,700.00,2,NULL,NULL,0),(545,1,443,400.00,2,NULL,NULL,0),(546,1,281,750.00,2,NULL,NULL,0),(547,1,118,600.00,2,NULL,NULL,0),(548,1,496,500.00,2,NULL,NULL,0),(549,1,549,350.00,4,NULL,NULL,0),(550,1,275,400.00,2,NULL,NULL,0),(551,1,613,650.00,2,NULL,NULL,0),(552,1,331,650.00,2,NULL,NULL,0),(553,1,404,800.00,2,NULL,NULL,0),(554,1,70,550.00,2,NULL,NULL,0),(555,1,35,750.00,2,NULL,NULL,0),(556,1,575,800.00,2,NULL,NULL,0),(557,1,562,800.00,2,NULL,NULL,0),(558,1,572,550.00,2,NULL,NULL,0),(559,1,384,650.00,2,NULL,NULL,0),(560,1,340,700.00,2,NULL,NULL,0),(561,1,277,550.00,2,NULL,NULL,0),(562,1,282,70.00,3,NULL,NULL,0),(563,1,543,200.00,1,NULL,NULL,1),(564,1,592,550.00,2,NULL,NULL,0),(565,1,109,500.00,2,NULL,NULL,0),(566,1,111,500.00,2,NULL,NULL,0),(567,1,618,400.00,4,NULL,NULL,0),(568,1,267,350.00,2,NULL,NULL,0),(569,1,328,250.00,1,NULL,NULL,0),(570,1,341,200.00,2,NULL,NULL,0),(571,1,436,250.00,1,NULL,NULL,1),(572,1,569,350.00,4,NULL,NULL,1),(573,1,81,350.00,4,NULL,NULL,0),(574,1,284,350.00,4,NULL,NULL,0),(575,1,476,350.00,4,NULL,NULL,0),(576,1,614,350.00,4,NULL,NULL,0),(577,1,558,250.00,4,NULL,NULL,0),(578,1,260,350.00,4,NULL,NULL,0),(579,1,358,750.00,2,NULL,NULL,0),(580,1,593,500.00,2,NULL,NULL,0),(581,1,595,300.00,2,NULL,NULL,0),(582,1,265,150.00,2,NULL,NULL,0),(583,1,326,1100.00,2,NULL,NULL,0),(584,1,509,450.00,2,NULL,NULL,0),(585,1,233,650.00,2,NULL,NULL,0),(586,1,61,650.00,2,NULL,NULL,0),(587,1,44,150.00,2,NULL,NULL,0),(588,1,548,300.00,2,NULL,NULL,0),(589,1,304,400.00,2,NULL,NULL,0),(590,1,547,550.00,4,NULL,NULL,0),(591,1,636,600.00,4,NULL,NULL,1),(592,1,194,350.00,1,NULL,NULL,0),(593,1,36,600.00,2,NULL,NULL,0),(594,1,640,800.00,2,'','',1);
/*!40000 ALTER TABLE `d_menu` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_menu_names`
--

DROP TABLE IF EXISTS `d_menu_names`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_menu_names` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  `f_dateStart` date DEFAULT NULL,
  `f_dateEnd` date DEFAULT NULL,
  `f_comment` varchar(128) DEFAULT NULL,
  `f_enabled` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_menu_names`
--

LOCK TABLES `d_menu_names` WRITE;
/*!40000 ALTER TABLE `d_menu_names` DISABLE KEYS */;
INSERT INTO `d_menu_names` VALUES (1,'Երկուշաբթի','2020-03-15','2020-03-15','',1);
/*!40000 ALTER TABLE `d_menu_names` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_package`
--

DROP TABLE IF EXISTS `d_package`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_package` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_enabled` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_package`
--

LOCK TABLES `d_package` WRITE;
/*!40000 ALTER TABLE `d_package` DISABLE KEYS */;
/*!40000 ALTER TABLE `d_package` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_package_list`
--

DROP TABLE IF EXISTS `d_package_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_package_list` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_package` int(11) DEFAULT NULL,
  `f_dish` int(11) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_package_list`
--

LOCK TABLES `d_package_list` WRITE;
/*!40000 ALTER TABLE `d_package_list` DISABLE KEYS */;
/*!40000 ALTER TABLE `d_package_list` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_part1`
--

DROP TABLE IF EXISTS `d_part1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_part1` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_part1`
--

LOCK TABLES `d_part1` WRITE;
/*!40000 ALTER TABLE `d_part1` DISABLE KEYS */;
INSERT INTO `d_part1` VALUES (1,'Խոհ');
/*!40000 ALTER TABLE `d_part1` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_part2`
--

DROP TABLE IF EXISTS `d_part2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_part2` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_part` int(11) DEFAULT NULL,
  `f_name` varchar(64) DEFAULT NULL,
  `f_color` int(11) DEFAULT NULL,
  `f_adgCode` varchar(32) DEFAULT NULL,
  `f_queue` int(11) DEFAULT '0',
  PRIMARY KEY (`f_id`),
  KEY `fk_part2_part_idx` (`f_part`),
  CONSTRAINT `fk_part2_part` FOREIGN KEY (`f_part`) REFERENCES `d_part1` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_part2`
--

LOCK TABLES `d_part2` WRITE;
/*!40000 ALTER TABLE `d_part2` DISABLE KEYS */;
INSERT INTO `d_part2` VALUES (2,1,'Աղցան',-1,'5910',0),(3,1,'Տաք ուտեստ',-1,'5910',0),(4,1,'Ապուրներ',-1,'5910',0),(5,1,'Խավարտ',-1,'5910',0),(6,1,'Հրուշակեղեն',-1,'5910',0),(7,1,'Էքսպրես',-1,'5910',0),(8,1,'Սոուս',-1,'5910',0),(9,1,'Դեսերտ',-1,'5910',0),(10,1,'Զովացուցիչ ըմպելիքներ',-1,'5910',0),(11,1,'Լիմոնադ',-1,'5910',0),(12,1,'Ջուր',-1,'5910',0),(13,1,'Մաստակ',-1,'5910',0),(14,1,'Ծխախոտ',-1,'5910',0),(15,1,'Թեյ,սուրճ',-1,'5910',0),(16,1,'հաց',-1,'5910',0),(17,1,'Պաղպաղակ',-1,'5910',0),(18,1,'Հատային',-1,'5910',0);
/*!40000 ALTER TABLE `d_part2` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_printers`
--

DROP TABLE IF EXISTS `d_printers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_printers` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_printers`
--

LOCK TABLES `d_printers` WRITE;
/*!40000 ALTER TABLE `d_printers` DISABLE KEYS */;
INSERT INTO `d_printers` VALUES (1,'');
/*!40000 ALTER TABLE `d_printers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_recipes`
--

DROP TABLE IF EXISTS `d_recipes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_recipes` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_dish` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_complex` int(11) DEFAULT NULL,
  `f_complexQty` decimal(14,4) DEFAULT NULL,
  `f_complexBaseQty` decimal(14,4) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_recipe_dish_idx` (`f_dish`),
  KEY `fk_recipe_goods_idx` (`f_goods`),
  CONSTRAINT `fk_recipe_dish` FOREIGN KEY (`f_dish`) REFERENCES `d_dish` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_recipe_goods` FOREIGN KEY (`f_goods`) REFERENCES `c_goods` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=1092 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_recipes`
--

LOCK TABLES `d_recipes` WRITE;
/*!40000 ALTER TABLE `d_recipes` DISABLE KEYS */;
INSERT INTO `d_recipes` VALUES (1,61,11,0.0620,NULL,NULL,NULL,NULL),(2,61,77,0.0060,NULL,NULL,NULL,NULL),(3,61,32,0.0140,NULL,NULL,NULL,NULL),(4,61,51,0.0060,NULL,NULL,NULL,NULL),(5,61,26,0.0030,NULL,NULL,NULL,NULL),(6,61,160,0.0010,NULL,NULL,NULL,NULL),(7,80,38,0.0010,NULL,NULL,NULL,NULL),(8,80,61,0.0550,NULL,NULL,NULL,NULL),(9,80,83,0.0020,NULL,NULL,NULL,NULL),(10,80,77,0.0040,NULL,NULL,NULL,NULL),(11,80,160,0.0010,NULL,NULL,NULL,NULL),(12,80,196,0.0600,NULL,NULL,NULL,NULL),(13,48,73,0.0900,NULL,NULL,NULL,NULL),(14,48,77,0.0150,NULL,NULL,NULL,NULL),(15,48,9,0.0300,NULL,NULL,NULL,NULL),(16,48,26,0.0020,NULL,NULL,NULL,NULL),(17,48,160,0.0010,NULL,NULL,NULL,NULL),(18,192,184,1.0000,NULL,NULL,NULL,NULL),(19,205,218,1.0000,NULL,NULL,NULL,NULL),(20,145,179,1.0000,NULL,NULL,NULL,NULL),(21,124,139,1.0000,NULL,NULL,NULL,NULL),(22,147,167,1.0000,NULL,NULL,NULL,NULL),(23,150,170,1.0000,NULL,NULL,NULL,NULL),(24,148,169,1.0000,NULL,NULL,NULL,NULL),(25,149,168,1.0000,NULL,NULL,NULL,NULL),(26,141,177,1.0000,NULL,NULL,NULL,NULL),(27,136,173,1.0000,NULL,NULL,NULL,NULL),(28,137,174,1.0000,NULL,NULL,NULL,NULL),(29,154,221,1.0000,NULL,NULL,NULL,NULL),(30,139,175,1.0000,NULL,NULL,NULL,NULL),(31,140,176,1.0000,NULL,NULL,NULL,NULL),(32,146,180,1.0000,NULL,NULL,NULL,NULL),(33,151,166,1.0000,NULL,NULL,NULL,NULL),(34,130,144,1.0000,NULL,NULL,NULL,NULL),(35,157,224,1.0000,NULL,NULL,NULL,NULL),(36,156,223,1.0000,NULL,NULL,NULL,NULL),(37,155,222,1.0000,NULL,NULL,NULL,NULL),(38,218,220,1.0000,NULL,NULL,NULL,NULL),(39,28,5,0.2800,NULL,NULL,NULL,NULL),(40,28,77,0.0400,NULL,NULL,NULL,NULL),(41,28,60,0.0200,NULL,NULL,NULL,NULL),(42,28,32,0.0200,NULL,NULL,NULL,NULL),(43,28,26,0.0020,NULL,NULL,NULL,NULL),(44,28,160,0.0010,NULL,NULL,NULL,NULL),(45,11,196,0.0450,NULL,NULL,NULL,NULL),(46,11,197,0.0220,NULL,NULL,NULL,NULL),(47,11,19,0.0200,NULL,NULL,NULL,NULL),(48,11,26,0.0020,NULL,NULL,NULL,NULL),(49,11,264,0.0370,NULL,NULL,NULL,NULL),(50,11,60,0.0140,NULL,NULL,NULL,NULL),(51,11,32,0.0140,NULL,NULL,NULL,NULL),(52,11,160,0.0010,NULL,NULL,NULL,NULL),(53,293,3,0.0300,NULL,NULL,NULL,NULL),(54,293,81,0.0400,NULL,NULL,NULL,NULL),(55,293,130,0.0300,NULL,NULL,NULL,NULL),(56,293,35,0.0200,NULL,NULL,NULL,NULL),(57,293,60,0.0150,NULL,NULL,NULL,NULL),(58,293,264,0.0200,NULL,NULL,NULL,NULL),(59,293,77,0.0200,NULL,NULL,NULL,NULL),(60,293,26,0.0020,NULL,NULL,NULL,NULL),(61,293,160,0.0010,NULL,NULL,NULL,NULL),(62,55,194,0.0600,NULL,NULL,NULL,NULL),(63,55,5,0.0600,NULL,NULL,NULL,NULL),(64,55,130,0.0050,NULL,NULL,NULL,NULL),(65,55,2,0.0300,NULL,NULL,NULL,NULL),(66,55,8,0.0140,NULL,NULL,NULL,NULL),(67,55,26,0.0020,NULL,NULL,NULL,NULL),(68,55,77,0.0070,NULL,NULL,NULL,NULL),(69,55,160,0.0010,NULL,NULL,NULL,NULL),(70,278,19,0.0700,NULL,NULL,NULL,NULL),(71,278,72,0.0100,NULL,NULL,NULL,NULL),(72,278,77,0.0100,NULL,NULL,NULL,NULL),(73,278,8,0.0010,NULL,NULL,NULL,NULL),(74,278,160,0.0010,NULL,NULL,NULL,NULL),(75,278,26,0.0010,NULL,NULL,NULL,NULL),(76,245,265,0.1200,NULL,NULL,NULL,NULL),(77,245,9,0.0250,NULL,NULL,NULL,NULL),(78,245,60,0.0500,NULL,NULL,NULL,NULL),(79,245,32,0.0050,NULL,NULL,NULL,NULL),(80,245,35,0.0150,NULL,NULL,NULL,NULL),(81,88,278,1.0000,NULL,NULL,NULL,NULL),(82,284,283,1.0000,NULL,NULL,NULL,NULL),(83,296,292,1.0000,NULL,NULL,NULL,NULL),(84,285,284,1.0000,NULL,NULL,NULL,NULL),(85,266,291,1.0000,NULL,NULL,NULL,NULL),(86,283,282,1.0000,NULL,NULL,NULL,NULL),(87,262,18,0.1100,NULL,NULL,NULL,NULL),(88,262,52,0.0150,NULL,NULL,NULL,NULL),(89,262,8,0.0010,NULL,NULL,NULL,NULL),(90,262,61,0.0010,NULL,NULL,NULL,NULL),(91,262,160,0.0010,NULL,NULL,NULL,NULL),(92,262,77,0.0050,NULL,NULL,NULL,NULL),(93,81,279,1.0000,NULL,NULL,NULL,NULL),(94,83,280,1.0000,NULL,NULL,NULL,NULL),(95,86,281,1.0000,NULL,NULL,NULL,NULL),(96,273,290,1.0000,NULL,NULL,NULL,NULL),(97,268,286,1.0000,NULL,NULL,NULL,NULL),(98,6,253,0.0300,NULL,NULL,NULL,NULL),(99,6,27,0.0200,NULL,NULL,NULL,NULL),(100,6,7,0.0150,NULL,NULL,NULL,NULL),(101,6,60,0.0290,NULL,NULL,NULL,NULL),(102,6,32,0.0290,NULL,NULL,NULL,NULL),(103,6,160,0.0010,NULL,NULL,NULL,NULL),(104,274,287,0.0500,NULL,NULL,NULL,NULL),(105,274,72,0.0100,NULL,NULL,NULL,NULL),(106,274,77,0.0100,NULL,NULL,NULL,NULL),(107,274,3,0.0100,NULL,NULL,NULL,NULL),(108,274,8,0.0010,NULL,NULL,NULL,NULL),(109,274,26,0.0010,NULL,NULL,NULL,NULL),(110,274,160,0.0010,NULL,NULL,NULL,NULL),(111,200,211,1.0000,NULL,NULL,NULL,NULL),(112,115,188,1.0000,NULL,NULL,NULL,NULL),(113,189,239,0.0620,NULL,NULL,NULL,NULL),(114,189,66,0.0310,NULL,NULL,NULL,NULL),(115,113,186,1.0000,NULL,NULL,NULL,NULL),(116,112,185,1.0000,NULL,NULL,NULL,NULL),(117,173,208,1.0000,NULL,NULL,NULL,NULL),(118,172,207,1.0000,NULL,NULL,NULL,NULL),(119,261,262,1.0000,NULL,NULL,NULL,NULL),(120,187,190,1.0000,NULL,NULL,NULL,NULL),(121,185,193,1.0000,NULL,NULL,NULL,NULL),(122,143,178,1.0000,NULL,NULL,NULL,NULL),(123,142,178,1.0000,NULL,NULL,NULL,NULL),(124,144,179,1.0000,NULL,NULL,NULL,NULL),(125,152,164,1.0000,NULL,NULL,NULL,NULL),(126,153,165,1.0000,NULL,NULL,NULL,NULL),(127,133,146,1.0000,NULL,NULL,NULL,NULL),(128,282,277,1.0000,NULL,NULL,NULL,NULL),(129,66,161,0.0700,NULL,NULL,NULL,NULL),(130,66,9,0.0500,NULL,NULL,NULL,NULL),(131,66,32,0.0400,NULL,NULL,NULL,NULL),(132,66,77,0.0100,NULL,NULL,NULL,NULL),(133,66,160,0.0010,NULL,NULL,NULL,NULL),(134,210,215,1.0000,NULL,NULL,NULL,NULL),(135,212,219,1.0000,NULL,NULL,NULL,NULL),(136,216,157,1.0000,NULL,NULL,NULL,NULL),(137,248,155,1.0000,NULL,NULL,NULL,NULL),(138,213,158,1.0000,NULL,NULL,NULL,NULL),(139,209,213,1.0000,NULL,NULL,NULL,NULL),(140,206,214,1.0000,NULL,NULL,NULL,NULL),(141,207,216,1.0000,NULL,NULL,NULL,NULL),(142,184,134,1.0000,NULL,NULL,NULL,NULL),(143,199,203,1.0000,NULL,NULL,NULL,NULL),(144,197,200,1.0000,NULL,NULL,NULL,NULL),(145,164,153,1.0000,NULL,NULL,NULL,NULL),(146,182,132,1.0000,NULL,NULL,NULL,NULL),(147,183,132,1.0000,NULL,NULL,NULL,NULL),(148,163,151,1.0000,NULL,NULL,NULL,NULL),(149,160,150,1.0000,NULL,NULL,NULL,NULL),(150,193,181,1.0000,NULL,NULL,NULL,NULL),(151,195,182,1.0000,NULL,NULL,NULL,NULL),(152,178,137,1.0000,NULL,NULL,NULL,NULL),(153,174,138,1.0000,NULL,NULL,NULL,NULL),(154,179,136,1.0000,NULL,NULL,NULL,NULL),(155,180,135,1.0000,NULL,NULL,NULL,NULL),(156,181,131,1.0000,NULL,NULL,NULL,NULL),(157,158,148,1.0000,NULL,NULL,NULL,NULL),(158,191,183,1.0000,NULL,NULL,NULL,NULL),(159,228,66,0.0020,NULL,NULL,NULL,NULL),(160,228,106,0.0050,NULL,NULL,NULL,NULL),(161,309,302,1.0000,NULL,NULL,NULL,NULL),(162,314,303,1.0000,NULL,NULL,NULL,NULL),(163,328,310,1.0000,NULL,NULL,NULL,NULL),(164,354,316,1.0000,NULL,NULL,NULL,NULL),(165,345,289,0.1000,NULL,NULL,NULL,NULL),(166,345,37,0.0300,NULL,NULL,NULL,NULL),(167,345,7,0.0300,NULL,NULL,NULL,NULL),(168,345,26,0.0010,NULL,NULL,NULL,NULL),(169,345,160,0.0010,NULL,NULL,NULL,NULL),(170,350,73,0.0900,NULL,NULL,NULL,NULL),(171,350,268,0.0600,NULL,NULL,NULL,NULL),(172,350,38,1.0000,NULL,NULL,NULL,NULL),(173,350,306,0.0800,NULL,NULL,NULL,NULL),(174,361,323,1.0000,NULL,NULL,NULL,NULL),(175,351,315,1.0000,NULL,NULL,NULL,NULL),(176,373,344,1.0000,NULL,NULL,NULL,NULL),(177,375,342,1.0000,NULL,NULL,NULL,NULL),(178,378,339,1.0000,NULL,NULL,NULL,NULL),(179,379,337,1.0000,NULL,NULL,NULL,NULL),(180,380,336,1.0000,NULL,NULL,NULL,NULL),(181,382,338,1.0000,NULL,NULL,NULL,NULL),(182,377,340,1.0000,NULL,NULL,NULL,NULL),(183,374,343,1.0000,NULL,NULL,NULL,NULL),(184,376,341,1.0000,NULL,NULL,NULL,NULL),(185,381,335,1.0000,NULL,NULL,NULL,NULL),(186,365,326,1.0000,NULL,NULL,NULL,NULL),(187,177,135,1.0000,NULL,NULL,NULL,NULL),(188,389,348,1.0000,NULL,NULL,NULL,NULL),(189,388,349,1.0000,NULL,NULL,NULL,NULL),(190,397,358,1.0000,NULL,NULL,NULL,NULL),(191,396,362,1.0000,NULL,NULL,NULL,NULL),(192,398,359,1.0000,NULL,NULL,NULL,NULL),(193,399,361,1.0000,NULL,NULL,NULL,NULL),(194,400,360,1.0000,NULL,NULL,NULL,NULL),(195,372,345,1.0000,NULL,NULL,NULL,NULL),(196,403,365,1.0000,NULL,NULL,NULL,NULL),(197,412,368,1.0000,NULL,NULL,NULL,NULL),(198,411,369,1.0000,NULL,NULL,NULL,NULL),(199,413,367,1.0000,NULL,NULL,NULL,NULL),(200,421,372,1.0000,NULL,NULL,NULL,NULL),(201,432,204,1.0000,NULL,NULL,NULL,NULL),(202,437,374,1.0000,NULL,NULL,NULL,NULL),(203,128,141,1.0000,NULL,NULL,NULL,NULL),(204,126,140,1.0000,NULL,NULL,NULL,NULL),(205,27,130,0.0400,NULL,NULL,NULL,NULL),(206,27,26,0.0010,NULL,NULL,NULL,NULL),(207,27,77,0.0150,NULL,NULL,NULL,NULL),(208,27,160,0.0010,NULL,NULL,NULL,NULL),(209,27,4,0.0700,NULL,NULL,NULL,NULL),(210,27,7,0.0700,NULL,NULL,NULL,NULL),(211,10,15,0.1100,NULL,NULL,NULL,NULL),(212,10,3,0.0500,NULL,NULL,NULL,NULL),(213,10,51,0.0300,NULL,NULL,NULL,NULL),(214,10,26,0.0020,NULL,NULL,NULL,NULL),(215,10,60,0.0150,NULL,NULL,NULL,NULL),(216,10,32,0.0150,NULL,NULL,NULL,NULL),(217,10,160,0.0010,NULL,NULL,NULL,NULL),(218,32,14,0.1900,NULL,NULL,NULL,NULL),(219,32,60,0.0120,NULL,NULL,NULL,NULL),(220,32,32,0.0120,NULL,NULL,NULL,NULL),(221,32,26,0.0020,NULL,NULL,NULL,NULL),(222,32,160,0.0010,NULL,NULL,NULL,NULL),(223,33,3,0.1000,NULL,NULL,NULL,NULL),(224,33,51,0.0150,NULL,NULL,NULL,NULL),(225,33,26,0.0020,NULL,NULL,NULL,NULL),(226,33,76,0.0150,NULL,NULL,NULL,NULL),(227,33,32,0.0200,NULL,NULL,NULL,NULL),(228,33,60,0.0200,NULL,NULL,NULL,NULL),(229,33,160,0.0010,NULL,NULL,NULL,NULL),(230,23,41,0.0300,NULL,NULL,NULL,NULL),(231,23,26,0.0350,NULL,NULL,NULL,NULL),(232,23,27,0.0300,NULL,NULL,NULL,NULL),(233,23,4,0.0100,NULL,NULL,NULL,NULL),(234,23,52,0.0100,NULL,NULL,NULL,NULL),(235,23,77,0.0100,NULL,NULL,NULL,NULL),(236,23,78,0.0040,NULL,NULL,NULL,NULL),(237,23,160,0.0050,NULL,NULL,NULL,NULL),(238,5,3,0.1300,NULL,NULL,NULL,NULL),(239,5,31,0.0200,NULL,NULL,NULL,NULL),(240,5,26,0.0040,NULL,NULL,NULL,NULL),(241,5,77,0.0200,NULL,NULL,NULL,NULL),(242,5,160,0.0010,NULL,NULL,NULL,NULL),(243,317,19,0.0400,NULL,NULL,NULL,NULL),(244,317,7,0.0200,NULL,NULL,NULL,NULL),(245,317,4,0.0200,NULL,NULL,NULL,NULL),(246,317,130,0.0200,NULL,NULL,NULL,NULL),(247,317,26,0.0020,NULL,NULL,NULL,NULL),(248,317,78,0.0020,NULL,NULL,NULL,NULL),(249,317,77,0.0100,NULL,NULL,NULL,NULL),(250,317,90,0.0010,NULL,NULL,NULL,NULL),(251,18,9,0.0200,NULL,NULL,NULL,NULL),(252,18,76,0.0100,NULL,NULL,NULL,NULL),(253,18,264,0.0400,NULL,NULL,NULL,NULL),(254,18,130,0.0700,NULL,NULL,NULL,NULL),(255,18,26,0.0010,NULL,NULL,NULL,NULL),(256,18,35,0.0150,NULL,NULL,NULL,NULL),(257,18,60,0.0200,NULL,NULL,NULL,NULL),(258,18,32,0.0200,NULL,NULL,NULL,NULL),(259,18,160,0.0010,NULL,NULL,NULL,NULL),(260,60,54,0.0400,NULL,NULL,NULL,NULL),(261,60,2,0.0600,NULL,NULL,NULL,NULL),(262,60,77,0.0050,NULL,NULL,NULL,NULL),(263,60,160,0.0010,NULL,NULL,NULL,NULL),(264,60,8,0.0050,NULL,NULL,NULL,NULL),(265,60,26,0.0010,NULL,NULL,NULL,NULL),(266,416,77,0.0240,NULL,NULL,NULL,NULL),(267,416,130,0.0850,NULL,NULL,NULL,NULL),(268,416,4,0.1200,NULL,NULL,NULL,NULL),(269,416,5,0.2200,NULL,NULL,NULL,NULL),(270,416,8,0.0400,NULL,NULL,NULL,NULL),(271,416,26,0.0020,NULL,NULL,NULL,NULL),(272,416,160,0.0010,NULL,NULL,NULL,NULL),(273,44,38,2.0000,NULL,NULL,NULL,NULL),(274,44,81,0.0250,NULL,NULL,NULL,NULL),(275,44,245,0.3300,NULL,NULL,NULL,NULL),(276,44,77,0.0100,NULL,NULL,NULL,NULL),(277,44,80,0.0050,NULL,NULL,NULL,NULL),(278,44,60,0.0050,NULL,NULL,NULL,NULL),(279,448,217,1.0000,NULL,NULL,NULL,NULL),(280,202,229,0.2500,NULL,NULL,NULL,NULL),(281,188,227,0.0630,NULL,NULL,NULL,NULL),(282,449,370,1.0000,NULL,NULL,NULL,NULL),(283,17,196,0.0680,NULL,NULL,NULL,NULL),(284,17,3,0.0200,NULL,NULL,NULL,NULL),(285,17,78,0.0030,NULL,NULL,NULL,NULL),(286,17,26,0.0020,NULL,NULL,NULL,NULL),(287,17,160,0.0010,NULL,NULL,NULL,NULL),(288,17,77,0.0200,NULL,NULL,NULL,NULL),(289,346,27,0.0500,NULL,NULL,NULL,NULL),(290,346,253,0.0300,NULL,NULL,NULL,NULL),(291,346,77,0.0100,NULL,NULL,NULL,NULL),(292,346,90,0.0010,NULL,NULL,NULL,NULL),(293,346,26,0.0100,NULL,NULL,NULL,NULL),(294,346,160,0.0010,NULL,NULL,NULL,NULL),(295,447,2,0.0500,NULL,NULL,NULL,NULL),(296,447,76,0.0200,NULL,NULL,NULL,NULL),(297,447,264,0.0200,NULL,NULL,NULL,NULL),(298,447,81,0.0100,NULL,NULL,NULL,NULL),(299,447,196,0.0200,NULL,NULL,NULL,NULL),(300,447,4,0.0100,NULL,NULL,NULL,NULL),(301,447,60,0.0100,NULL,NULL,NULL,NULL),(302,447,32,0.0100,NULL,NULL,NULL,NULL),(303,447,26,0.0020,NULL,NULL,NULL,NULL),(304,447,160,0.0010,NULL,NULL,NULL,NULL),(305,16,2,0.0600,NULL,NULL,NULL,NULL),(306,16,14,0.0500,NULL,NULL,NULL,NULL),(307,16,3,0.0500,NULL,NULL,NULL,NULL),(308,16,196,0.0400,NULL,NULL,NULL,NULL),(309,16,51,0.0300,NULL,NULL,NULL,NULL),(310,16,264,0.0300,NULL,NULL,NULL,NULL),(311,16,77,0.0150,NULL,NULL,NULL,NULL),(312,16,160,0.0010,NULL,NULL,NULL,NULL),(313,20,27,0.0250,NULL,NULL,NULL,NULL),(314,20,32,0.0150,NULL,NULL,NULL,NULL),(315,20,60,0.0150,NULL,NULL,NULL,NULL),(316,20,160,0.0010,NULL,NULL,NULL,NULL),(317,20,81,0.0450,NULL,NULL,NULL,NULL),(318,20,264,0.0600,NULL,NULL,NULL,NULL),(319,20,76,0.0250,NULL,NULL,NULL,NULL),(320,4,4,0.0500,0.00,0,0.0000,0.0000),(321,4,35,0.0150,0.00,0,0.0000,0.0000),(322,4,317,0.0300,0.00,0,0.0000,0.0000),(323,4,60,0.0120,0.00,0,0.0000,0.0000),(324,4,32,0.0120,0.00,0,0.0000,0.0000),(325,4,227,0.0100,0.00,0,0.0000,0.0000),(326,4,26,0.0020,0.00,0,0.0000,0.0000),(327,4,160,0.0010,0.00,0,0.0000,0.0000),(328,31,64,3.0000,NULL,NULL,NULL,NULL),(329,31,72,0.0500,NULL,NULL,NULL,NULL),(330,31,76,0.0100,NULL,NULL,NULL,NULL),(331,31,3,0.0700,NULL,NULL,NULL,NULL),(332,31,26,0.0010,NULL,NULL,NULL,NULL),(333,31,32,0.0150,NULL,NULL,NULL,NULL),(334,31,60,0.0150,NULL,NULL,NULL,NULL),(335,31,160,0.0010,NULL,NULL,NULL,NULL),(336,8,4,0.0500,NULL,NULL,NULL,NULL),(337,8,7,0.0500,NULL,NULL,NULL,NULL),(338,8,78,0.0030,NULL,NULL,NULL,NULL),(339,8,77,0.0200,NULL,NULL,NULL,NULL),(340,8,160,0.0010,NULL,NULL,NULL,NULL),(341,8,31,0.0080,NULL,NULL,NULL,NULL),(342,8,33,0.0080,NULL,NULL,NULL,NULL),(343,8,27,0.0300,NULL,NULL,NULL,NULL),(344,428,15,0.1100,NULL,NULL,NULL,NULL),(345,428,50,0.0300,NULL,NULL,NULL,NULL),(346,428,31,0.0200,NULL,NULL,NULL,NULL),(347,428,26,0.0020,NULL,NULL,NULL,NULL),(348,428,60,0.0150,NULL,NULL,NULL,NULL),(349,428,32,0.0150,NULL,NULL,NULL,NULL),(350,428,76,0.0150,NULL,NULL,NULL,NULL),(351,428,160,0.0010,NULL,NULL,NULL,NULL),(352,370,333,0.1000,NULL,NULL,NULL,NULL),(353,370,26,0.0040,NULL,NULL,NULL,NULL),(354,370,77,0.0100,NULL,NULL,NULL,NULL),(355,370,160,0.0010,NULL,NULL,NULL,NULL),(356,2,2,0.0650,NULL,NULL,NULL,NULL),(357,2,3,0.0400,NULL,NULL,NULL,NULL),(358,2,51,0.0250,NULL,NULL,NULL,NULL),(359,2,264,0.0200,NULL,NULL,NULL,NULL),(360,2,19,0.0300,NULL,NULL,NULL,NULL),(361,2,26,0.0020,NULL,NULL,NULL,NULL),(362,2,32,0.0150,NULL,NULL,NULL,NULL),(363,2,60,0.0150,NULL,NULL,NULL,NULL),(364,2,160,0.0010,NULL,NULL,NULL,NULL),(365,339,16,0.1000,NULL,NULL,NULL,NULL),(366,339,76,0.0200,NULL,NULL,NULL,NULL),(367,339,53,0.0200,NULL,NULL,NULL,NULL),(368,339,60,0.0150,NULL,NULL,NULL,NULL),(369,339,32,0.0150,NULL,NULL,NULL,NULL),(370,339,160,0.0010,NULL,NULL,NULL,NULL),(371,441,27,0.0500,NULL,NULL,NULL,NULL),(372,441,4,0.0100,NULL,NULL,NULL,NULL),(373,441,33,0.0100,NULL,NULL,NULL,NULL),(374,441,15,0.0300,NULL,NULL,NULL,NULL),(375,441,60,0.0150,NULL,NULL,NULL,NULL),(376,441,32,0.1500,NULL,NULL,NULL,NULL),(377,441,227,0.2000,NULL,NULL,NULL,NULL),(378,441,160,0.0010,NULL,NULL,NULL,NULL),(379,417,196,0.0500,NULL,NULL,NULL,NULL),(380,417,35,0.0250,NULL,NULL,NULL,NULL),(381,417,81,0.0250,NULL,NULL,NULL,NULL),(382,417,32,0.0150,NULL,NULL,NULL,NULL),(383,417,60,0.0150,NULL,NULL,NULL,NULL),(384,417,26,0.0010,NULL,NULL,NULL,NULL),(385,417,160,0.0010,NULL,NULL,NULL,NULL),(386,243,3,0.1200,NULL,NULL,NULL,NULL),(387,243,76,0.0100,NULL,NULL,NULL,NULL),(388,243,81,0.0200,NULL,NULL,NULL,NULL),(389,243,26,0.0010,NULL,NULL,NULL,NULL),(390,243,51,0.0150,NULL,NULL,NULL,NULL),(391,243,60,0.0200,NULL,NULL,NULL,NULL),(392,243,32,0.0200,NULL,NULL,NULL,NULL),(393,243,160,0.0010,NULL,NULL,NULL,NULL),(394,49,331,0.0800,NULL,NULL,NULL,NULL),(395,49,32,0.0150,NULL,NULL,NULL,NULL),(396,49,38,0.0500,NULL,NULL,NULL,NULL),(397,49,61,0.0100,NULL,NULL,NULL,NULL),(398,49,36,0.0020,NULL,NULL,NULL,NULL),(399,49,57,0.0150,NULL,NULL,NULL,NULL),(400,49,26,0.0020,NULL,NULL,NULL,NULL),(401,49,160,0.0010,NULL,NULL,NULL,NULL),(402,323,294,0.0120,NULL,NULL,NULL,NULL),(403,323,8,0.0050,NULL,NULL,NULL,NULL),(404,323,41,0.0050,NULL,NULL,NULL,NULL),(405,323,77,0.0050,NULL,NULL,NULL,NULL),(406,323,52,0.0050,NULL,NULL,NULL,NULL),(407,323,2,0.0600,NULL,NULL,NULL,NULL),(408,323,160,0.0020,NULL,NULL,NULL,NULL),(409,50,15,0.0700,NULL,NULL,NULL,NULL),(410,50,72,0.0100,NULL,NULL,NULL,NULL),(411,50,2,0.0600,NULL,NULL,NULL,NULL),(412,50,3,0.0120,NULL,NULL,NULL,NULL),(413,50,8,0.0100,NULL,NULL,NULL,NULL),(414,50,26,0.0010,NULL,NULL,NULL,NULL),(415,50,160,0.0010,NULL,NULL,NULL,NULL),(416,50,77,0.0050,NULL,NULL,NULL,NULL),(417,52,19,0.0700,NULL,NULL,NULL,NULL),(418,52,196,0.0200,NULL,NULL,NULL,NULL),(419,52,3,0.0150,NULL,NULL,NULL,NULL),(420,52,14,0.0150,NULL,NULL,NULL,NULL),(421,52,2,0.0600,NULL,NULL,NULL,NULL),(422,52,77,0.0050,NULL,NULL,NULL,NULL),(423,52,8,0.0050,NULL,NULL,NULL,NULL),(424,52,26,0.0020,NULL,NULL,NULL,NULL),(425,52,160,0.0020,NULL,NULL,NULL,NULL),(426,439,42,0.0300,NULL,NULL,NULL,NULL),(427,439,2,0.0600,NULL,NULL,NULL,NULL),(428,439,77,0.0020,NULL,NULL,NULL,NULL),(429,439,8,0.0050,NULL,NULL,NULL,NULL),(430,439,160,0.0020,NULL,NULL,NULL,NULL),(431,56,40,0.0500,NULL,NULL,NULL,NULL),(432,56,8,0.0040,NULL,NULL,NULL,NULL),(433,56,2,0.0400,NULL,NULL,NULL,NULL),(434,56,77,0.0050,NULL,NULL,NULL,NULL),(435,56,26,0.0020,NULL,NULL,NULL,NULL),(436,56,160,0.0010,NULL,NULL,NULL,NULL),(437,58,75,0.0400,NULL,NULL,NULL,NULL),(438,58,8,0.0050,NULL,NULL,NULL,NULL),(439,58,2,0.0600,NULL,NULL,NULL,NULL),(440,58,77,0.0050,NULL,NULL,NULL,NULL),(441,58,52,0.0040,NULL,NULL,NULL,NULL),(442,58,160,0.0010,NULL,NULL,NULL,NULL),(443,59,39,0.0400,NULL,NULL,NULL,NULL),(444,59,2,0.0600,NULL,NULL,NULL,NULL),(445,59,52,0.0020,NULL,NULL,NULL,NULL),(446,59,8,0.0020,NULL,NULL,NULL,NULL),(447,59,26,0.0010,NULL,NULL,NULL,NULL),(448,59,160,0.0010,NULL,NULL,NULL,NULL),(449,59,61,0.0010,NULL,NULL,NULL,NULL),(450,313,124,0.0400,NULL,NULL,NULL,NULL),(451,313,2,0.0700,NULL,NULL,NULL,NULL),(452,313,52,0.0050,NULL,NULL,NULL,NULL),(453,313,19,0.0700,NULL,NULL,NULL,NULL),(454,313,8,0.0050,NULL,NULL,NULL,NULL),(455,313,77,0.0050,NULL,NULL,NULL,NULL),(456,313,26,0.0020,NULL,NULL,NULL,NULL),(457,313,160,0.0020,NULL,NULL,NULL,NULL),(458,53,9,0.0300,NULL,NULL,NULL,NULL),(459,53,3,0.0100,NULL,NULL,NULL,NULL),(460,53,2,0.0600,NULL,NULL,NULL,NULL),(461,53,72,0.0100,NULL,NULL,NULL,NULL),(462,53,8,0.0050,NULL,NULL,NULL,NULL),(463,53,77,0.0050,NULL,NULL,NULL,NULL),(464,53,160,0.0020,NULL,NULL,NULL,NULL),(465,53,26,0.0020,NULL,NULL,NULL,NULL),(466,407,2,0.2000,NULL,NULL,NULL,NULL),(467,407,52,0.0050,NULL,NULL,NULL,NULL),(468,407,77,0.0050,NULL,NULL,NULL,NULL),(469,407,19,0.0700,NULL,NULL,NULL,NULL),(470,407,26,0.0020,NULL,NULL,NULL,NULL),(471,407,160,0.0020,NULL,NULL,NULL,NULL),(472,73,313,2.1000,NULL,NULL,NULL,NULL),(473,73,38,0.6500,NULL,NULL,NULL,NULL),(474,73,77,0.0100,NULL,NULL,NULL,NULL),(475,40,55,0.0950,NULL,NULL,NULL,NULL),(476,40,77,0.0200,NULL,NULL,NULL,NULL),(477,40,160,0.0010,NULL,NULL,NULL,NULL),(478,387,20,0.1100,NULL,NULL,NULL,NULL),(479,387,2,0.5000,NULL,NULL,NULL,NULL),(480,68,57,0.0600,NULL,NULL,NULL,NULL),(481,68,15,0.0500,NULL,NULL,NULL,NULL),(482,68,36,0.0050,NULL,NULL,NULL,NULL),(483,68,160,0.0010,NULL,NULL,NULL,NULL),(484,301,70,0.1100,NULL,NULL,NULL,NULL),(485,301,77,0.0100,NULL,NULL,NULL,NULL),(486,298,124,0.1000,NULL,NULL,NULL,NULL),(487,298,3,0.0500,NULL,NULL,NULL,NULL),(488,298,52,0.0200,NULL,NULL,NULL,NULL),(489,298,77,0.0050,NULL,NULL,NULL,NULL),(490,298,160,0.0070,NULL,NULL,NULL,NULL),(491,336,9,0.2200,NULL,NULL,NULL,NULL),(492,336,77,0.0030,NULL,NULL,NULL,NULL),(493,331,129,2.2000,NULL,NULL,NULL,NULL),(494,331,77,0.0150,NULL,NULL,NULL,NULL),(495,331,38,0.6000,NULL,NULL,NULL,NULL),(496,264,73,0.1000,0.00,0,0.0000,0.0000),(497,264,287,0.0400,0.00,0,0.0000,0.0000),(498,264,52,0.0200,0.00,0,0.0000,0.0000),(499,264,77,0.0100,0.00,0,0.0000,0.0000),(500,264,160,0.0010,0.00,0,0.0000,0.0000),(501,324,308,0.1000,NULL,NULL,NULL,NULL),(502,324,77,0.0200,NULL,NULL,NULL,NULL),(503,234,55,0.0950,NULL,NULL,NULL,NULL),(504,234,77,0.0200,NULL,NULL,NULL,NULL),(505,234,51,0.0100,NULL,NULL,NULL,NULL),(506,234,3,0.0100,NULL,NULL,NULL,NULL),(507,234,26,0.0020,NULL,NULL,NULL,NULL),(508,234,160,0.0010,NULL,NULL,NULL,NULL),(509,76,2,0.3000,NULL,NULL,NULL,NULL),(510,76,62,0.1000,NULL,NULL,NULL,NULL),(511,76,77,0.0200,NULL,NULL,NULL,NULL),(512,76,8,0.0200,NULL,NULL,NULL,NULL),(513,76,26,0.0050,NULL,NULL,NULL,NULL),(514,76,160,0.0050,NULL,NULL,NULL,NULL),(515,225,8,0.0150,NULL,NULL,NULL,NULL),(516,225,26,0.0030,NULL,NULL,NULL,NULL),(517,225,77,0.0150,NULL,NULL,NULL,NULL),(518,225,160,0.0020,NULL,NULL,NULL,NULL),(519,225,40,0.1200,NULL,NULL,NULL,NULL),(520,256,22,0.1000,NULL,NULL,NULL,NULL),(521,256,2,0.3000,NULL,NULL,NULL,NULL),(522,256,77,0.0200,NULL,NULL,NULL,NULL),(523,256,8,0.0250,NULL,NULL,NULL,NULL),(524,256,26,0.0050,NULL,NULL,NULL,NULL),(525,256,160,0.0050,NULL,NULL,NULL,NULL),(526,445,318,0.1200,NULL,NULL,NULL,NULL),(527,445,2,0.1000,NULL,NULL,NULL,NULL),(528,445,38,0.3000,NULL,NULL,NULL,NULL),(529,445,77,0.0100,NULL,NULL,NULL,NULL),(530,445,26,0.0050,NULL,NULL,NULL,NULL),(531,445,160,0.0010,NULL,NULL,NULL,NULL),(532,38,56,0.0950,NULL,NULL,NULL,NULL),(533,38,77,0.0050,NULL,NULL,NULL,NULL),(534,241,70,0.0100,NULL,NULL,NULL,NULL),(535,241,163,0.0250,NULL,NULL,NULL,NULL),(536,241,3,0.0150,NULL,NULL,NULL,NULL),(537,241,31,0.0020,NULL,NULL,NULL,NULL),(538,241,77,0.0050,NULL,NULL,NULL,NULL),(539,241,160,0.0010,NULL,NULL,NULL,NULL),(540,119,70,0.1000,NULL,NULL,NULL,NULL),(541,119,287,0.0800,NULL,NULL,NULL,NULL),(542,119,77,0.0050,NULL,NULL,NULL,NULL),(543,119,160,0.0050,NULL,NULL,NULL,NULL),(544,352,2,0.4000,NULL,NULL,NULL,NULL),(545,352,77,0.0200,NULL,NULL,NULL,NULL),(546,352,8,0.0200,NULL,NULL,NULL,NULL),(547,352,26,0.0100,NULL,NULL,NULL,NULL),(548,9,161,0.0800,NULL,NULL,NULL,NULL),(549,9,7,0.0600,NULL,NULL,NULL,NULL),(550,9,49,0.0300,NULL,NULL,NULL,NULL),(551,9,26,0.0020,NULL,NULL,NULL,NULL),(552,9,60,0.0250,NULL,NULL,NULL,NULL),(553,9,32,0.0200,NULL,NULL,NULL,NULL),(554,9,160,0.0010,NULL,NULL,NULL,NULL),(555,67,55,0.0950,NULL,NULL,NULL,NULL),(556,67,3,0.0150,NULL,NULL,NULL,NULL),(557,67,161,0.0700,NULL,NULL,NULL,NULL),(558,67,77,0.0050,NULL,NULL,NULL,NULL),(559,67,160,0.0010,NULL,NULL,NULL,NULL),(560,47,61,0.0200,NULL,NULL,NULL,NULL),(561,47,19,0.0080,NULL,NULL,NULL,NULL),(562,47,22,0.0050,NULL,NULL,NULL,NULL),(563,47,160,0.0020,NULL,NULL,NULL,NULL),(564,47,8,0.0010,NULL,NULL,NULL,NULL),(565,47,32,0.0200,NULL,NULL,NULL,NULL),(566,230,22,0.0800,NULL,NULL,NULL,NULL),(567,230,76,0.0100,NULL,NULL,NULL,NULL),(568,230,130,0.0100,NULL,NULL,NULL,NULL),(569,230,4,0.0100,NULL,NULL,NULL,NULL),(570,230,77,0.0070,NULL,NULL,NULL,NULL),(571,230,3,0.0200,NULL,NULL,NULL,NULL),(572,230,72,0.0050,NULL,NULL,NULL,NULL),(573,230,52,0.0200,NULL,NULL,NULL,NULL),(574,230,26,0.0030,NULL,NULL,NULL,NULL),(575,230,160,0.0010,NULL,NULL,NULL,NULL),(576,34,332,0.2100,NULL,NULL,NULL,NULL),(577,34,38,0.3300,NULL,NULL,NULL,NULL),(578,34,77,0.0100,NULL,NULL,NULL,NULL),(579,45,73,0.1000,NULL,NULL,NULL,NULL),(580,45,77,0.0200,NULL,NULL,NULL,NULL),(581,45,160,0.0010,NULL,NULL,NULL,NULL),(582,41,54,0.0950,NULL,NULL,NULL,NULL),(583,41,77,0.0020,NULL,NULL,NULL,NULL),(584,41,160,0.0010,NULL,NULL,NULL,NULL),(585,43,196,0.1710,NULL,NULL,NULL,NULL),(586,43,3,0.0570,NULL,NULL,NULL,NULL),(587,43,8,0.0350,NULL,NULL,NULL,NULL),(588,43,52,0.0130,NULL,NULL,NULL,NULL),(589,43,26,0.0020,NULL,NULL,NULL,NULL),(590,43,77,0.0200,NULL,NULL,NULL,NULL),(591,43,160,0.0010,NULL,NULL,NULL,NULL),(592,297,294,0.0500,NULL,NULL,NULL,NULL),(593,297,8,0.0300,NULL,NULL,NULL,NULL),(594,297,77,0.0200,NULL,NULL,NULL,NULL),(595,297,160,0.0010,NULL,NULL,NULL,NULL),(596,443,39,0.1000,NULL,NULL,NULL,NULL),(597,443,3,0.6500,NULL,NULL,NULL,NULL),(598,443,8,0.0150,NULL,NULL,NULL,NULL),(599,443,26,0.0050,NULL,NULL,NULL,NULL),(600,443,256,0.0050,NULL,NULL,NULL,NULL),(601,224,74,0.0900,NULL,NULL,NULL,NULL),(602,224,77,0.0200,NULL,NULL,NULL,NULL),(603,224,160,0.0010,NULL,NULL,NULL,NULL),(604,368,329,1.0000,NULL,NULL,NULL,NULL),(605,368,32,0.0100,NULL,NULL,NULL,NULL),(606,368,80,0.0100,NULL,NULL,NULL,NULL),(607,368,350,0.0100,NULL,NULL,NULL,NULL),(608,295,196,0.0300,NULL,NULL,NULL,NULL),(609,295,7,0.0300,NULL,NULL,NULL,NULL),(610,295,64,3.0000,NULL,NULL,NULL,NULL),(611,295,27,0.0400,NULL,NULL,NULL,NULL),(612,295,60,0.0200,NULL,NULL,NULL,NULL),(613,295,160,0.0010,NULL,NULL,NULL,NULL),(614,306,161,0.0800,NULL,NULL,NULL,NULL),(615,306,8,0.0150,NULL,NULL,NULL,NULL),(616,306,227,0.0400,NULL,NULL,NULL,NULL),(617,306,77,0.0200,NULL,NULL,NULL,NULL),(618,306,36,0.0100,NULL,NULL,NULL,NULL),(619,254,15,0.1800,NULL,NULL,NULL,NULL),(620,254,60,0.0100,NULL,NULL,NULL,NULL),(621,254,77,0.0100,NULL,NULL,NULL,NULL),(622,254,160,0.0020,NULL,NULL,NULL,NULL),(623,254,52,0.1000,NULL,NULL,NULL,NULL),(624,337,161,0.1000,NULL,NULL,NULL,NULL),(625,337,8,0.0300,NULL,NULL,NULL,NULL),(626,337,227,0.0500,NULL,NULL,NULL,NULL),(627,337,77,0.0250,NULL,NULL,NULL,NULL),(628,337,76,0.0070,NULL,NULL,NULL,NULL),(629,337,33,0.0300,NULL,NULL,NULL,NULL),(630,337,38,1.0000,NULL,NULL,NULL,NULL),(631,226,39,0.0350,NULL,NULL,NULL,NULL),(632,226,38,0.1500,NULL,NULL,NULL,NULL),(633,226,77,0.0070,NULL,NULL,NULL,NULL),(634,226,26,0.0150,NULL,NULL,NULL,NULL),(635,226,61,0.0070,NULL,NULL,NULL,NULL),(636,244,38,0.2000,NULL,NULL,NULL,NULL),(637,244,20,0.0700,NULL,NULL,NULL,NULL),(638,244,160,0.0050,NULL,NULL,NULL,NULL),(639,244,77,0.0100,NULL,NULL,NULL,NULL),(640,244,33,0.0150,NULL,NULL,NULL,NULL),(641,70,8,0.0150,NULL,NULL,NULL,NULL),(642,70,77,0.0200,NULL,NULL,NULL,NULL),(643,70,160,0.0010,NULL,NULL,NULL,NULL),(644,70,287,0.0700,NULL,NULL,NULL,NULL),(645,70,161,0.0100,NULL,NULL,NULL,NULL),(646,286,246,0.0200,NULL,NULL,NULL,NULL),(647,286,61,0.0100,NULL,NULL,NULL,NULL),(648,286,38,0.0500,NULL,NULL,NULL,NULL),(649,286,77,0.0010,NULL,NULL,NULL,NULL),(650,286,19,0.0200,NULL,NULL,NULL,NULL),(651,78,16,0.1000,NULL,NULL,NULL,NULL),(652,78,38,0.3300,NULL,NULL,NULL,NULL),(653,78,77,0.0200,NULL,NULL,NULL,NULL),(654,78,160,0.0010,NULL,NULL,NULL,NULL),(655,78,227,0.0300,NULL,NULL,NULL,NULL),(656,338,9,0.0400,NULL,NULL,NULL,NULL),(657,338,16,0.0400,NULL,NULL,NULL,NULL),(658,338,32,0.0200,NULL,NULL,NULL,NULL),(659,338,61,0.0100,NULL,NULL,NULL,NULL),(660,338,33,0.0150,NULL,NULL,NULL,NULL),(661,338,77,0.0050,NULL,NULL,NULL,NULL),(662,333,2,0.4500,NULL,NULL,NULL,NULL),(663,333,33,0.0250,NULL,NULL,NULL,NULL),(664,333,160,0.0020,NULL,NULL,NULL,NULL),(665,406,16,0.1500,NULL,NULL,NULL,NULL),(666,406,107,0.0050,NULL,NULL,NULL,NULL),(667,69,51,0.0070,NULL,NULL,NULL,NULL),(668,69,60,0.0050,NULL,NULL,NULL,NULL),(669,69,77,0.0050,NULL,NULL,NULL,NULL),(670,69,160,0.0050,NULL,NULL,NULL,NULL),(671,69,265,0.1250,NULL,NULL,NULL,NULL),(672,69,33,0.0300,NULL,NULL,NULL,NULL),(673,349,265,0.1250,NULL,NULL,NULL,NULL),(674,349,38,0.3300,NULL,NULL,NULL,NULL),(675,349,61,0.0100,NULL,NULL,NULL,NULL),(676,349,77,0.0150,NULL,NULL,NULL,NULL),(677,302,161,0.0800,NULL,NULL,NULL,NULL),(678,302,8,0.0200,NULL,NULL,NULL,NULL),(679,302,227,0.0400,NULL,NULL,NULL,NULL),(680,302,77,0.0200,NULL,NULL,NULL,NULL),(681,302,33,0.0100,NULL,NULL,NULL,NULL),(682,308,161,0.0800,NULL,NULL,NULL,NULL),(683,308,8,0.0150,NULL,NULL,NULL,NULL),(684,308,227,0.0400,NULL,NULL,NULL,NULL),(685,308,77,0.0200,NULL,NULL,NULL,NULL),(686,308,9,0.0150,NULL,NULL,NULL,NULL),(687,308,33,0.0050,NULL,NULL,NULL,NULL),(688,308,160,0.0050,NULL,NULL,NULL,NULL),(689,312,161,0.1500,NULL,NULL,NULL,NULL),(690,312,2,0.0600,NULL,NULL,NULL,NULL),(691,312,33,0.0300,NULL,NULL,NULL,NULL),(692,312,77,0.0050,NULL,NULL,NULL,NULL),(693,325,161,0.1500,NULL,NULL,NULL,NULL),(694,325,60,0.1500,NULL,NULL,NULL,NULL),(695,325,160,0.0100,NULL,NULL,NULL,NULL),(696,325,77,0.0050,NULL,NULL,NULL,NULL),(697,62,161,0.0800,NULL,NULL,NULL,NULL),(698,62,8,0.0020,NULL,NULL,NULL,NULL),(699,62,77,0.0200,NULL,NULL,NULL,NULL),(700,62,160,0.0010,NULL,NULL,NULL,NULL),(701,62,26,0.0010,NULL,NULL,NULL,NULL),(702,62,227,0.0400,NULL,NULL,NULL,NULL),(703,250,60,0.0100,NULL,NULL,NULL,NULL),(704,250,160,0.0100,NULL,NULL,NULL,NULL),(705,250,161,0.1000,NULL,NULL,NULL,NULL),(706,250,77,0.0050,NULL,NULL,NULL,NULL),(707,332,16,0.1000,NULL,NULL,NULL,NULL),(708,332,29,0.0150,NULL,NULL,NULL,NULL),(709,332,38,0.0300,NULL,NULL,NULL,NULL),(710,332,77,0.0200,NULL,NULL,NULL,NULL),(711,332,160,0.0050,NULL,NULL,NULL,NULL),(712,311,161,0.1200,NULL,NULL,NULL,NULL),(713,311,77,0.0050,NULL,NULL,NULL,NULL),(714,311,160,0.0100,NULL,NULL,NULL,NULL),(715,311,8,0.0050,NULL,NULL,NULL,NULL),(716,270,161,0.0800,NULL,NULL,NULL,NULL),(717,270,162,0.0100,NULL,NULL,NULL,NULL),(718,270,8,0.0150,NULL,NULL,NULL,NULL),(719,270,76,0.0050,NULL,NULL,NULL,NULL),(720,270,51,0.0050,NULL,NULL,NULL,NULL),(721,270,35,0.0150,NULL,NULL,NULL,NULL),(722,270,77,0.0100,NULL,NULL,NULL,NULL),(723,270,160,0.0010,NULL,NULL,NULL,NULL),(724,22,2,0.2700,NULL,NULL,NULL,NULL),(725,22,16,0.0550,NULL,NULL,NULL,NULL),(726,22,33,0.0200,NULL,NULL,NULL,NULL),(727,22,38,0.8000,NULL,NULL,NULL,NULL),(728,22,77,0.0200,NULL,NULL,NULL,NULL),(729,22,160,0.0020,NULL,NULL,NULL,NULL),(730,239,62,0.1100,NULL,NULL,NULL,NULL),(731,239,350,0.0050,NULL,NULL,NULL,NULL),(732,252,77,0.0050,NULL,NULL,NULL,NULL),(733,252,62,0.1200,NULL,NULL,NULL,NULL),(734,252,33,0.0150,NULL,NULL,NULL,NULL),(735,252,80,0.0100,NULL,NULL,NULL,NULL),(736,281,77,0.0200,NULL,NULL,NULL,NULL),(737,281,160,0.0050,NULL,NULL,NULL,NULL),(738,281,21,0.1000,NULL,NULL,NULL,NULL),(739,281,61,0.0100,NULL,NULL,NULL,NULL),(740,35,18,0.1000,NULL,NULL,NULL,NULL),(741,35,77,0.0150,NULL,NULL,NULL,NULL),(742,277,19,0.1000,NULL,NULL,NULL,NULL),(743,277,161,0.0100,NULL,NULL,NULL,NULL),(744,277,8,0.0200,NULL,NULL,NULL,NULL),(745,277,77,0.0050,NULL,NULL,NULL,NULL),(746,277,160,0.0030,NULL,NULL,NULL,NULL),(747,275,271,0.1000,NULL,NULL,NULL,NULL),(748,275,38,0.0100,NULL,NULL,NULL,NULL),(749,275,8,0.0100,NULL,NULL,NULL,NULL),(750,275,36,0.0050,NULL,NULL,NULL,NULL),(751,275,61,0.0100,NULL,NULL,NULL,NULL),(752,233,271,0.1250,NULL,NULL,NULL,NULL),(753,233,9,0.0350,NULL,NULL,NULL,NULL),(754,233,38,0.0850,NULL,NULL,NULL,NULL),(755,233,8,0.0080,NULL,NULL,NULL,NULL),(756,251,22,0.1200,NULL,NULL,NULL,NULL),(757,251,160,0.0030,NULL,NULL,NULL,NULL),(758,251,60,0.0100,NULL,NULL,NULL,NULL),(759,251,52,0.0100,NULL,NULL,NULL,NULL),(760,251,77,0.0050,NULL,NULL,NULL,NULL),(761,450,68,1.0000,NULL,NULL,NULL,NULL),(762,330,307,0.3300,NULL,NULL,NULL,NULL),(763,330,77,0.0050,NULL,NULL,NULL,NULL),(764,265,287,0.0600,NULL,NULL,NULL,NULL),(765,265,52,0.0200,NULL,NULL,NULL,NULL),(766,265,77,0.0100,NULL,NULL,NULL,NULL),(767,265,8,0.0050,NULL,NULL,NULL,NULL),(768,265,160,0.0010,NULL,NULL,NULL,NULL),(769,265,61,0.0050,NULL,NULL,NULL,NULL),(770,267,287,0.0300,NULL,NULL,NULL,NULL),(771,267,196,0.0350,NULL,NULL,NULL,NULL),(772,267,8,0.0160,NULL,NULL,NULL,NULL),(773,267,72,0.0020,NULL,NULL,NULL,NULL),(774,267,52,0.0050,NULL,NULL,NULL,NULL),(775,267,160,0.0020,NULL,NULL,NULL,NULL),(776,74,52,0.0050,NULL,NULL,NULL,NULL),(777,74,32,0.0300,NULL,NULL,NULL,NULL),(778,74,77,0.0100,NULL,NULL,NULL,NULL),(779,74,20,0.1000,NULL,NULL,NULL,NULL),(780,74,8,0.0050,NULL,NULL,NULL,NULL),(781,74,160,0.0050,NULL,NULL,NULL,NULL),(782,71,287,0.0400,NULL,NULL,NULL,NULL),(783,71,52,0.0200,NULL,NULL,NULL,NULL),(784,71,77,0.0100,NULL,NULL,NULL,NULL),(785,71,163,0.0400,NULL,NULL,NULL,NULL),(786,71,160,0.0010,NULL,NULL,NULL,NULL),(787,71,3,0.0250,NULL,NULL,NULL,NULL),(788,71,31,0.0050,NULL,NULL,NULL,NULL),(789,71,61,0.0200,NULL,NULL,NULL,NULL),(790,263,267,0.2000,NULL,NULL,NULL,NULL),(791,263,8,0.1000,NULL,NULL,NULL,NULL),(792,263,77,0.0100,NULL,NULL,NULL,NULL),(793,263,160,0.0010,NULL,NULL,NULL,NULL),(794,358,319,0.2000,NULL,NULL,NULL,NULL),(795,358,8,0.1000,NULL,NULL,NULL,NULL),(796,358,77,0.0100,NULL,NULL,NULL,NULL),(797,358,52,0.1500,NULL,NULL,NULL,NULL),(798,358,160,0.0050,NULL,NULL,NULL,NULL),(799,64,75,0.0350,NULL,NULL,NULL,NULL),(800,64,38,0.2000,NULL,NULL,NULL,NULL),(801,64,26,0.0050,NULL,NULL,NULL,NULL),(802,64,61,0.0070,NULL,NULL,NULL,NULL),(803,64,77,0.0100,NULL,NULL,NULL,NULL),(804,426,3,0.1500,NULL,NULL,NULL,NULL),(805,426,311,0.0350,NULL,NULL,NULL,NULL),(806,426,8,0.0500,NULL,NULL,NULL,NULL),(807,426,77,0.0080,NULL,NULL,NULL,NULL),(808,426,160,0.0020,NULL,NULL,NULL,NULL),(809,327,311,0.1700,NULL,NULL,NULL,NULL),(810,327,38,0.4000,NULL,NULL,NULL,NULL),(811,229,161,0.0800,NULL,NULL,NULL,NULL),(812,229,77,0.0050,NULL,NULL,NULL,NULL),(813,229,3,0.0100,NULL,NULL,NULL,NULL),(814,229,2,0.0150,NULL,NULL,NULL,NULL),(815,229,52,0.0150,NULL,NULL,NULL,NULL),(816,229,85,0.0050,NULL,NULL,NULL,NULL),(817,229,160,0.0050,NULL,NULL,NULL,NULL),(818,229,61,0.0050,NULL,NULL,NULL,NULL),(819,291,21,0.2000,NULL,NULL,NULL,NULL),(820,291,8,0.1000,NULL,NULL,NULL,NULL),(821,291,160,0.0100,NULL,NULL,NULL,NULL),(822,291,8,0.1000,NULL,NULL,NULL,NULL),(823,36,195,0.2800,NULL,NULL,NULL,NULL),(824,36,270,0.0250,NULL,NULL,NULL,NULL),(825,36,160,0.0010,NULL,NULL,NULL,NULL),(826,271,15,0.1300,NULL,NULL,NULL,NULL),(827,271,8,0.1000,NULL,NULL,NULL,NULL),(828,271,77,0.0100,NULL,NULL,NULL,NULL),(829,271,160,0.0050,NULL,NULL,NULL,NULL),(830,271,52,0.0200,NULL,NULL,NULL,NULL),(831,353,263,0.0100,NULL,NULL,NULL,NULL),(832,353,32,0.0100,NULL,NULL,NULL,NULL),(833,353,269,0.0150,NULL,NULL,NULL,NULL),(834,342,331,0.0850,NULL,NULL,NULL,NULL),(835,105,32,0.0500,NULL,NULL,NULL,NULL),(836,102,80,0.0700,NULL,NULL,NULL,NULL),(837,390,37,0.0850,NULL,NULL,NULL,NULL),(838,390,7,0.0100,NULL,NULL,NULL,NULL),(839,390,26,0.0020,NULL,NULL,NULL,NULL),(840,310,37,0.1000,NULL,NULL,NULL,NULL),(841,103,60,0.0500,NULL,NULL,NULL,NULL),(842,305,300,0.0250,NULL,NULL,NULL,NULL),(843,305,301,0.0300,NULL,NULL,NULL,NULL),(844,222,33,0.0250,NULL,NULL,NULL,NULL),(845,167,12,1.0000,NULL,NULL,NULL,NULL),(846,131,143,1.0000,NULL,NULL,NULL,NULL),(847,231,377,1.0000,NULL,NULL,NULL,NULL),(848,269,16,0.1000,NULL,NULL,NULL,NULL),(849,269,9,0.0100,NULL,NULL,NULL,NULL),(850,269,33,0.0150,NULL,NULL,NULL,NULL),(851,269,38,0.3300,NULL,NULL,NULL,NULL),(852,269,58,0.0500,NULL,NULL,NULL,NULL),(853,269,77,0.0300,NULL,NULL,NULL,NULL),(854,269,8,0.0040,NULL,NULL,NULL,NULL),(855,237,16,0.1000,NULL,NULL,NULL,NULL),(856,237,72,0.0010,NULL,NULL,NULL,NULL),(857,237,33,0.0150,NULL,NULL,NULL,NULL),(858,237,3,0.0080,NULL,NULL,NULL,NULL),(859,237,38,0.3300,NULL,NULL,NULL,NULL),(860,237,8,0.0040,NULL,NULL,NULL,NULL),(861,237,58,0.0500,NULL,NULL,NULL,NULL),(862,237,77,0.0300,NULL,NULL,NULL,NULL),(863,237,76,0.0020,NULL,NULL,NULL,NULL),(864,237,9,0.0100,NULL,NULL,NULL,NULL),(865,122,139,1.0000,NULL,NULL,NULL,NULL),(866,422,55,0.0800,NULL,NULL,NULL,NULL),(867,422,9,0.0200,NULL,NULL,NULL,NULL),(868,422,3,0.0200,NULL,NULL,NULL,NULL),(869,422,76,0.0100,NULL,NULL,NULL,NULL),(870,422,51,0.0100,NULL,NULL,NULL,NULL),(871,422,77,0.0120,NULL,NULL,NULL,NULL),(872,223,130,0.0450,NULL,NULL,NULL,NULL),(873,223,3,0.0450,NULL,NULL,NULL,NULL),(874,223,2,0.4500,NULL,NULL,NULL,NULL),(875,223,16,0.0650,NULL,NULL,NULL,NULL),(876,223,77,0.0450,NULL,NULL,NULL,NULL),(877,223,26,0.0050,NULL,NULL,NULL,NULL),(878,223,160,0.0020,NULL,NULL,NULL,NULL),(879,424,5,0.2100,NULL,NULL,NULL,NULL),(880,424,130,0.0800,NULL,NULL,NULL,NULL),(881,424,4,0.0800,NULL,NULL,NULL,NULL),(882,424,26,0.0050,NULL,NULL,NULL,NULL),(883,424,77,0.0700,NULL,NULL,NULL,NULL),(884,424,160,0.0010,NULL,NULL,NULL,NULL),(885,424,2,0.2500,NULL,NULL,NULL,NULL),(886,77,42,0.1400,NULL,NULL,NULL,NULL),(887,77,3,0.1000,NULL,NULL,NULL,NULL),(888,77,76,0.0100,NULL,NULL,NULL,NULL),(889,77,11,0.1000,NULL,NULL,NULL,NULL),(890,77,130,0.0330,NULL,NULL,NULL,NULL),(891,77,380,0.0330,NULL,NULL,NULL,NULL),(892,77,77,0.0500,NULL,NULL,NULL,NULL),(893,169,384,1.0000,NULL,NULL,NULL,NULL),(894,401,363,1.0000,NULL,NULL,NULL,NULL),(895,384,347,0.5000,NULL,NULL,NULL,NULL),(896,384,77,0.0100,NULL,NULL,NULL,NULL),(897,384,160,0.0020,NULL,NULL,NULL,NULL),(898,343,330,0.3000,NULL,NULL,NULL,NULL),(899,343,19,0.0500,NULL,NULL,NULL,NULL),(900,343,77,0.0100,NULL,NULL,NULL,NULL),(901,343,52,0.0050,NULL,NULL,NULL,NULL),(902,343,160,0.0050,NULL,NULL,NULL,NULL),(903,343,33,0.0100,NULL,NULL,NULL,NULL),(904,343,9,0.0500,NULL,NULL,NULL,NULL),(905,63,43,0.5000,NULL,NULL,NULL,NULL),(906,63,60,0.0200,NULL,NULL,NULL,NULL),(907,63,32,0.0200,NULL,NULL,NULL,NULL),(908,63,26,0.0060,NULL,NULL,NULL,NULL),(909,63,77,0.0550,NULL,NULL,NULL,NULL),(910,63,160,0.0020,NULL,NULL,NULL,NULL),(911,409,5,0.2000,NULL,NULL,NULL,NULL),(912,409,19,0.0400,NULL,NULL,NULL,NULL),(913,409,4,0.0200,NULL,NULL,NULL,NULL),(914,409,130,0.0100,NULL,NULL,NULL,NULL),(915,409,77,0.0400,NULL,NULL,NULL,NULL),(916,409,160,0.0020,NULL,NULL,NULL,NULL),(917,344,16,0.1000,NULL,NULL,NULL,NULL),(918,344,306,0.0100,NULL,NULL,NULL,NULL),(919,344,33,0.0100,NULL,NULL,NULL,NULL),(920,344,77,0.0200,NULL,NULL,NULL,NULL),(921,344,58,0.0500,NULL,NULL,NULL,NULL),(922,344,160,0.0050,NULL,NULL,NULL,NULL),(923,186,5,0.2200,NULL,NULL,NULL,NULL),(924,186,4,0.1100,NULL,NULL,NULL,NULL),(925,186,16,0.0300,NULL,NULL,NULL,NULL),(926,186,76,0.0300,NULL,NULL,NULL,NULL),(927,186,160,0.0030,NULL,NULL,NULL,NULL),(928,186,77,0.0220,NULL,NULL,NULL,NULL),(929,87,98,0.0050,NULL,NULL,NULL,NULL),(930,87,66,0.0100,NULL,NULL,NULL,NULL),(931,118,5,0.6200,NULL,NULL,NULL,NULL),(932,118,77,0.0600,NULL,NULL,NULL,NULL),(933,118,114,0.0010,NULL,NULL,NULL,NULL),(934,170,110,1.0000,NULL,NULL,NULL,NULL),(935,194,189,1.0000,NULL,NULL,NULL,NULL),(936,318,3,0.0150,NULL,NULL,NULL,NULL),(937,318,130,0.0150,NULL,NULL,NULL,NULL),(938,318,16,0.0220,NULL,NULL,NULL,NULL),(939,318,6,0.0150,NULL,NULL,NULL,NULL),(940,318,77,0.0120,NULL,NULL,NULL,NULL),(941,318,55,0.0720,NULL,NULL,NULL,NULL),(942,318,160,0.0020,NULL,NULL,NULL,NULL),(943,39,2,0.1700,NULL,NULL,NULL,NULL),(944,39,3,0.0250,NULL,NULL,NULL,NULL),(945,39,77,0.0350,NULL,NULL,NULL,NULL),(946,39,9,0.0250,NULL,NULL,NULL,NULL),(947,39,11,0.0170,NULL,NULL,NULL,NULL),(948,39,15,0.0350,NULL,NULL,NULL,NULL),(949,39,33,0.0250,NULL,NULL,NULL,NULL),(950,39,38,0.5000,NULL,NULL,NULL,NULL),(951,171,68,1.0000,NULL,NULL,NULL,NULL),(952,455,3,0.0170,NULL,NULL,NULL,NULL),(953,455,130,0.0090,NULL,NULL,NULL,NULL),(954,455,125,0.0080,NULL,NULL,NULL,NULL),(955,455,111,0.0180,NULL,NULL,NULL,NULL),(956,455,70,0.0850,NULL,NULL,NULL,NULL),(957,455,77,0.0140,NULL,NULL,NULL,NULL),(958,455,381,0.0020,NULL,NULL,NULL,NULL),(959,135,377,1.0000,NULL,NULL,NULL,NULL),(960,347,16,0.1300,NULL,NULL,NULL,NULL),(961,347,8,0.0200,NULL,NULL,NULL,NULL),(962,347,32,0.0150,NULL,NULL,NULL,NULL),(963,347,77,0.0080,NULL,NULL,NULL,NULL),(964,347,381,0.0040,NULL,NULL,NULL,NULL),(965,347,229,0.5000,NULL,NULL,NULL,NULL),(966,425,2,0.5600,NULL,NULL,NULL,NULL),(967,425,19,0.0560,NULL,NULL,NULL,NULL),(968,425,77,0.0600,NULL,NULL,NULL,NULL),(969,425,130,0.0450,NULL,NULL,NULL,NULL),(970,425,4,0.0700,NULL,NULL,NULL,NULL),(971,425,26,0.0050,NULL,NULL,NULL,NULL),(972,320,16,0.1000,NULL,NULL,NULL,NULL),(973,320,38,0.2500,NULL,NULL,NULL,NULL),(974,320,2,0.2300,NULL,NULL,NULL,NULL),(975,320,77,0.0400,NULL,NULL,NULL,NULL),(976,320,61,0.0050,NULL,NULL,NULL,NULL),(977,320,381,0.0010,NULL,NULL,NULL,NULL),(978,238,2,0.1500,NULL,NULL,NULL,NULL),(979,238,306,0.0200,NULL,NULL,NULL,NULL),(980,238,80,0.0200,NULL,NULL,NULL,NULL),(981,238,38,0.5000,NULL,NULL,NULL,NULL),(982,238,77,0.0120,NULL,NULL,NULL,NULL),(983,238,4,0.0200,NULL,NULL,NULL,NULL),(984,238,33,0.0200,NULL,NULL,NULL,NULL),(985,238,381,0.0030,NULL,NULL,NULL,NULL),(986,405,38,2.0000,NULL,NULL,NULL,NULL),(987,405,229,0.5000,NULL,NULL,NULL,NULL),(988,405,36,0.0100,NULL,NULL,NULL,NULL),(989,405,92,0.3000,NULL,NULL,NULL,NULL),(990,405,381,0.0020,NULL,NULL,NULL,NULL),(991,258,271,0.1000,NULL,NULL,NULL,NULL),(992,258,38,0.1500,NULL,NULL,NULL,NULL),(993,258,8,0.0070,NULL,NULL,NULL,NULL),(994,258,61,0.0070,NULL,NULL,NULL,NULL),(995,258,380,0.1000,NULL,NULL,NULL,NULL),(996,258,9,0.0150,NULL,NULL,NULL,NULL),(997,258,33,0.0700,NULL,NULL,NULL,NULL),(998,258,77,0.0050,NULL,NULL,NULL,NULL),(999,258,375,270.1400,NULL,NULL,NULL,NULL),(1000,98,16,0.0900,NULL,NULL,NULL,NULL),(1001,98,38,0.5000,NULL,NULL,NULL,NULL),(1002,98,60,0.0130,NULL,NULL,NULL,NULL),(1003,98,61,0.0130,NULL,NULL,NULL,NULL),(1004,98,26,0.0500,NULL,NULL,NULL,NULL),(1005,98,33,0.0100,NULL,NULL,NULL,NULL),(1006,98,381,0.0030,NULL,NULL,NULL,NULL),(1007,98,77,0.0150,NULL,NULL,NULL,NULL),(1008,98,375,230.1300,NULL,NULL,NULL,NULL),(1009,37,2,0.4500,NULL,NULL,NULL,NULL),(1010,37,160,0.0010,NULL,NULL,NULL,NULL),(1011,37,36,0.0120,NULL,NULL,NULL,NULL),(1012,37,246,0.0300,NULL,NULL,NULL,NULL),(1013,37,375,115.4000,NULL,NULL,NULL,NULL),(1014,46,74,0.0880,NULL,NULL,NULL,NULL),(1015,46,9,0.0250,NULL,NULL,NULL,NULL),(1016,46,8,0.0400,NULL,NULL,NULL,NULL),(1017,46,77,0.0160,NULL,NULL,NULL,NULL),(1018,46,375,95.3000,NULL,NULL,NULL,NULL),(1019,51,264,0.0220,NULL,NULL,NULL,NULL),(1020,51,89,0.0140,NULL,NULL,NULL,NULL),(1021,51,245,0.6500,NULL,NULL,NULL,NULL),(1022,51,81,0.0200,NULL,NULL,NULL,NULL),(1023,51,31,0.0030,NULL,NULL,NULL,NULL),(1024,51,52,0.0050,NULL,NULL,NULL,NULL),(1025,51,160,0.0010,NULL,NULL,NULL,NULL),(1026,51,77,0.0040,NULL,NULL,NULL,NULL),(1027,51,312,0.0550,NULL,NULL,NULL,NULL),(1028,51,273,0.0090,NULL,NULL,NULL,NULL),(1029,51,61,0.0040,NULL,NULL,NULL,NULL),(1030,51,87,0.0040,NULL,NULL,NULL,NULL),(1031,51,375,225.3000,NULL,NULL,NULL,NULL),(1032,385,194,0.0560,NULL,NULL,NULL,NULL),(1033,385,38,0.2300,NULL,NULL,NULL,NULL),(1034,385,26,0.1000,NULL,NULL,NULL,NULL),(1035,385,61,0.0170,NULL,NULL,NULL,NULL),(1036,385,77,0.0100,NULL,NULL,NULL,NULL),(1037,385,160,0.0010,NULL,NULL,NULL,NULL),(1038,385,375,105.7000,NULL,NULL,NULL,NULL),(1039,249,17,0.1200,NULL,NULL,NULL,NULL),(1040,249,60,0.0100,NULL,NULL,NULL,NULL),(1041,249,77,0.0500,NULL,NULL,NULL,NULL),(1042,249,160,0.0010,NULL,NULL,NULL,NULL),(1043,249,52,0.0100,NULL,NULL,NULL,NULL),(1044,72,11,0.4500,NULL,NULL,NULL,NULL),(1045,72,38,0.4000,NULL,NULL,NULL,NULL),(1046,72,77,0.0350,NULL,NULL,NULL,NULL),(1047,72,61,0.0250,NULL,NULL,NULL,NULL),(1048,72,381,0.0020,NULL,NULL,NULL,NULL),(1049,57,20,0.0300,NULL,NULL,NULL,NULL),(1050,57,3,0.0200,NULL,NULL,NULL,NULL),(1051,57,53,0.0200,NULL,NULL,NULL,NULL),(1052,57,31,0.0150,NULL,NULL,NULL,NULL),(1053,57,7,0.0200,NULL,NULL,NULL,NULL),(1054,57,60,0.0150,NULL,NULL,NULL,NULL),(1055,57,26,0.0050,NULL,NULL,NULL,NULL),(1056,57,381,0.0020,NULL,NULL,NULL,NULL),(1057,134,247,1.0000,NULL,NULL,NULL,NULL),(1058,480,155,1.0000,NULL,NULL,NULL,NULL),(1059,166,171,1.0000,NULL,NULL,NULL,NULL),(1060,505,377,1.0000,NULL,NULL,NULL,NULL),(1061,434,143,1.0000,NULL,NULL,NULL,NULL),(1062,444,16,0.9000,NULL,NULL,NULL,NULL),(1063,444,32,0.4000,NULL,NULL,NULL,NULL),(1064,444,61,0.0050,NULL,NULL,NULL,NULL),(1065,444,160,0.0050,NULL,NULL,NULL,NULL),(1066,531,157,0.0100,NULL,NULL,NULL,NULL),(1067,543,142,1.0000,NULL,NULL,NULL,NULL),(1068,433,376,1.0000,NULL,NULL,NULL,NULL),(1069,532,157,0.0100,NULL,NULL,NULL,NULL),(1070,114,155,1.0000,NULL,NULL,NULL,NULL),(1071,408,375,0.0250,NULL,NULL,NULL,NULL),(1072,322,291,0.0550,NULL,NULL,NULL,NULL),(1073,322,292,1.0000,NULL,NULL,NULL,NULL),(1074,138,111,1.0000,NULL,NULL,NULL,NULL),(1075,435,158,1.0000,NULL,NULL,NULL,NULL),(1076,165,164,1.0000,NULL,NULL,NULL,NULL),(1077,132,166,1.0000,NULL,NULL,NULL,NULL),(1078,436,165,1.0000,NULL,NULL,NULL,NULL),(1079,19,196,0.0300,NULL,NULL,NULL,NULL),(1080,19,3,0.0200,NULL,NULL,NULL,NULL),(1081,19,253,0.1500,NULL,NULL,NULL,NULL),(1082,19,76,0.0100,NULL,NULL,NULL,NULL),(1083,19,27,0.0200,NULL,NULL,NULL,NULL),(1084,19,60,0.0200,NULL,NULL,NULL,NULL),(1085,19,381,0.0020,NULL,NULL,NULL,NULL),(1086,19,26,0.0020,NULL,NULL,NULL,NULL),(1087,203,374,1.0000,NULL,NULL,NULL,NULL),(1088,393,194,1.0000,NULL,NULL,NULL,NULL),(1089,168,292,1.0000,NULL,NULL,NULL,NULL),(1090,168,291,0.0500,NULL,NULL,NULL,NULL),(1091,362,324,1.0000,NULL,NULL,NULL,NULL);
/*!40000 ALTER TABLE `d_recipes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_special`
--

DROP TABLE IF EXISTS `d_special`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_special` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_dish` int(11) DEFAULT NULL,
  `f_comment` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_special`
--

LOCK TABLES `d_special` WRITE;
/*!40000 ALTER TABLE `d_special` DISABLE KEYS */;
/*!40000 ALTER TABLE `d_special` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `d_translator`
--

DROP TABLE IF EXISTS `d_translator`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `d_translator` (
  `f_id` int(11) NOT NULL,
  `f_mode` smallint(6) DEFAULT NULL,
  `f_en` tinytext,
  `f_ru` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `d_translator`
--

LOCK TABLES `d_translator` WRITE;
/*!40000 ALTER TABLE `d_translator` DISABLE KEYS */;
/*!40000 ALTER TABLE `d_translator` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `droid_message`
--

DROP TABLE IF EXISTS `droid_message`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `droid_message` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `msg` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `droid_message`
--

LOCK TABLES `droid_message` WRITE;
/*!40000 ALTER TABLE `droid_message` DISABLE KEYS */;
/*!40000 ALTER TABLE `droid_message` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `e_cash`
--

DROP TABLE IF EXISTS `e_cash`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `e_cash` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_header` char(36) DEFAULT NULL,
  `f_cash` int(11) DEFAULT NULL,
  `f_sign` smallint(6) DEFAULT NULL,
  `f_remarks` varchar(1024) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_autoinput` smallint(6) DEFAULT '0',
  `f_oheader` char(36) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=30 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `e_cash`
--

LOCK TABLES `e_cash` WRITE;
/*!40000 ALTER TABLE `e_cash` DISABLE KEYS */;
INSERT INTO `e_cash` VALUES (29,'4f578133-682c-11ea-85d4-1078d2d2b808',1,1,'CASHINPUT 17.03.2020/B5',60.00,1,'4f4917b4-682c-11ea-85d4-1078d2d2b808');
/*!40000 ALTER TABLE `e_cash` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `e_cash_names`
--

DROP TABLE IF EXISTS `e_cash_names`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `e_cash_names` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `e_cash_names`
--

LOCK TABLES `e_cash_names` WRITE;
/*!40000 ALTER TABLE `e_cash_names` DISABLE KEYS */;
INSERT INTO `e_cash_names` VALUES (1,'Հիմնական'),(2,'Վաճառք, կանխիկ'),(3,'Վաճառք, անկանխիկ');
/*!40000 ALTER TABLE `e_cash_names` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `e_relation`
--

DROP TABLE IF EXISTS `e_relation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `e_relation` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `e_relation`
--

LOCK TABLES `e_relation` WRITE;
/*!40000 ALTER TABLE `e_relation` DISABLE KEYS */;
INSERT INTO `e_relation` VALUES (1,'Costumer debt paid');
/*!40000 ALTER TABLE `e_relation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `h_halls`
--

DROP TABLE IF EXISTS `h_halls`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `h_halls` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(64) DEFAULT NULL,
  `f_counter` int(11) DEFAULT '0',
  `f_prefix` char(8) NOT NULL,
  `f_settings` int(11) DEFAULT NULL,
  `f_counterhall` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `f_prefix_UNIQUE` (`f_prefix`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `h_halls`
--

LOCK TABLES `h_halls` WRITE;
/*!40000 ALTER TABLE `h_halls` DISABLE KEYS */;
INSERT INTO `h_halls` VALUES (1,'Bufet',235,'B',2,1);
/*!40000 ALTER TABLE `h_halls` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `h_tables`
--

DROP TABLE IF EXISTS `h_tables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `h_tables` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_hall` int(11) DEFAULT NULL,
  `f_name` varchar(32) DEFAULT NULL,
  `f_lock` smallint(6) DEFAULT '0',
  `f_lockSrc` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `fk_tables_hall_idx` (`f_hall`),
  CONSTRAINT `fk_tables_hall` FOREIGN KEY (`f_hall`) REFERENCES `h_halls` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `h_tables`
--

LOCK TABLES `h_tables` WRITE;
/*!40000 ALTER TABLE `h_tables` DISABLE KEYS */;
INSERT INTO `h_tables` VALUES (1,1,'Աէ',0,'');
/*!40000 ALTER TABLE `h_tables` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_additional`
--

DROP TABLE IF EXISTS `o_additional`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_additional` (
  `f_id` char(36) DEFAULT NULL,
  `f_coordinates` tinytext,
  `f_customer` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_additional`
--

LOCK TABLES `o_additional` WRITE;
/*!40000 ALTER TABLE `o_additional` DISABLE KEYS */;
INSERT INTO `o_additional` VALUES ('d78ee163-6e1f-11ea-85d4-1078d2d2b808','40.181230|44.523220',16),('05c618ef-6e22-11ea-85d4-1078d2d2b808','40.181230|44.523220',18),('a963bf09-6e68-11ea-85d4-1078d2d2b808','40.181230|44.523220',31),('f9cebbac-6e6b-11ea-85d4-1078d2d2b808','40.181230|44.523220',23),('b226ee2b-6e6c-11ea-85d4-1078d2d2b808','40.181230|44.523220',29),('f061d94e-6e6c-11ea-85d4-1078d2d2b808','40.181230|44.523220',29),('4772e5c9-6ea1-11ea-85d4-1078d2d2b808','40.181230|44.523220',19),('50207e94-6ecf-11ea-85d4-1078d2d2b808','40.181230|44.523220',20),('8088de2a-6f26-11ea-85d4-1078d2d2b808','40.181230|44.523220',16),('3a8e6c21-6f2d-11ea-85d4-1078d2d2b808','40.181230|44.523220',17),('c5113737-6f2d-11ea-85d4-1078d2d2b808','40.193337|44.473662',29);
/*!40000 ALTER TABLE `o_additional` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_body`
--

DROP TABLE IF EXISTS `o_body`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_body` (
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
  `f_print1` tinytext,
  `f_print2` tinytext,
  `f_comment` varchar(512) DEFAULT NULL,
  `f_remind` smallint(6) DEFAULT NULL,
  `f_adgcode` varchar(64) DEFAULT NULL,
  `f_removeReason` tinytext,
  `f_timeorder` smallint(6) DEFAULT NULL,
  `f_package` int(11) DEFAULT '0',
  PRIMARY KEY (`f_id`),
  KEY `fk_body_dish_idx` (`f_dish`),
  CONSTRAINT `fk_body_dish` FOREIGN KEY (`f_dish`) REFERENCES `d_dish` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_body`
--

LOCK TABLES `o_body` WRITE;
/*!40000 ALTER TABLE `o_body` DISABLE KEYS */;
INSERT INTO `o_body` VALUES ('02c0dfef-6697-11ea-85d4-1078d2d2b808','02bbf60c-6697-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('0ac33134-668d-11ea-85d4-1078d2d2b808','0abe2d78-668d-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('1424370a-668d-11ea-85d4-1078d2d2b808','14201146-668d-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('1b3a51ac-6a7f-11ea-85d4-1078d2d2b808','dece8ddc-6a7e-11ea-85d4-1078d2d2b808',1,202,1.00,1.00,100.00,0.00,0.00,100.00,2,'','','',0,'5910',NULL,1,0),('247c7aa9-6a80-11ea-85d4-1078d2d2b808','1f067601-6a80-11ea-85d4-1078d2d2b808',1,372,1.00,1.00,200.00,0.00,0.00,200.00,2,'','','',0,'5910',NULL,1,0),('2737c6d1-6697-11ea-85d4-1078d2d2b808','2733dc04-6697-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('30353b93-668c-11ea-85d4-1078d2d2b808','302ea57b-668c-11ea-85d4-1078d2d2b808',1,197,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('303972fd-668c-11ea-85d4-1078d2d2b808','302ea57b-668c-11ea-85d4-1078d2d2b808',1,532,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('303c6425-668c-11ea-85d4-1078d2d2b808','302ea57b-668c-11ea-85d4-1078d2d2b808',1,102,1.00,1.00,80.00,0.00,0.00,80.00,2,NULL,'','',0,'5910',NULL,NULL,0),('3e5d9e68-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,448,1.00,1.00,150.00,0.00,0.00,150.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e61234b-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,505,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e6394f5-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,421,1.00,1.00,200.00,0.00,0.00,200.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e65fa99-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,413,1.00,1.00,200.00,0.00,0.00,200.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e68afa6-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,218,1.00,1.00,300.00,0.00,0.00,300.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e6b3b39-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,261,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e6d451f-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,114,1.00,1.00,400.00,0.00,0.00,400.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e6f4c46-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,436,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e714881-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,189,1.00,1.00,150.00,0.00,0.00,150.00,2,NULL,'','',0,'5910',NULL,NULL,0),('3e73e48e-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,191,1.00,1.00,300.00,0.00,0.00,300.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e761396-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,411,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e7838b5-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,412,1.00,1.00,200.00,0.00,0.00,200.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e7a5dcd-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,169,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e7c7d10-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,165,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e7eac77-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,112,1.00,1.00,200.00,0.00,0.00,200.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e80de30-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,168,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('3e83a34e-668c-11ea-85d4-1078d2d2b808','3e598cd0-668c-11ea-85d4-1078d2d2b808',1,168,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('4f8ccaf9-668d-11ea-85d4-1078d2d2b808','4f87326a-668d-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('5240bb24-66aa-11ea-85d4-1078d2d2b808','1e84d0da-6698-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('5c169678-6a7f-11ea-85d4-1078d2d2b808','24bc9675-6a7f-11ea-85d4-1078d2d2b808',1,188,1.00,1.00,30.00,0.00,0.00,30.00,4,'','','',0,'5910',NULL,1,0),('5fb4a9cd-668c-11ea-85d4-1078d2d2b808','5faff9dc-668c-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('679fb757-6a7c-11ea-85d4-1078d2d2b808','233f4d1d-6a7c-11ea-85d4-1078d2d2b808',1,188,1.00,1.00,30.00,0.00,0.00,30.00,4,'','','',0,'5910',NULL,1,0),('740f18d5-6a7d-11ea-85d4-1078d2d2b808','740c0b24-6a7d-11ea-85d4-1078d2d2b808',1,397,1.00,1.00,300.00,0.00,0.00,300.00,2,'','','',0,'5910',NULL,1,0),('860280b7-6a7c-11ea-85d4-1078d2d2b808','83c9b806-6a7c-11ea-85d4-1078d2d2b808',1,636,1.00,1.00,600.00,0.00,0.00,600.00,4,'','','',0,'5910',NULL,1,0),('8c824630-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808',1,87,1.00,1.00,600.00,0.00,0.00,600.00,2,NULL,'','',0,'5910',NULL,NULL,0),('8c8621a4-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808',1,408,1.00,1.00,300.00,0.00,0.00,300.00,1,NULL,'','',0,'5910',NULL,NULL,0),('8c887699-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808',1,163,1.00,1.00,200.00,0.00,0.00,200.00,1,NULL,'','',0,'5910',NULL,NULL,0),('8c8ad39d-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808',1,191,1.00,1.00,300.00,0.00,0.00,300.00,1,NULL,'','',0,'5910',NULL,NULL,0),('9992cb02-6697-11ea-85d4-1078d2d2b808','998ee7fc-6697-11ea-85d4-1078d2d2b808',1,132,1.00,1.00,250.00,0.00,0.00,250.00,1,NULL,'','',0,'5910',NULL,NULL,0),('9f07874b-668c-11ea-85d4-1078d2d2b808','9f03336f-668c-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('9f17a75e-6697-11ea-85d4-1078d2d2b808','9f13c899-6697-11ea-85d4-1078d2d2b808',1,114,1.00,1.00,400.00,0.00,0.00,400.00,1,NULL,'','',0,'5910',NULL,NULL,0),('af2ebc0e-6a7e-11ea-85d4-1078d2d2b808','aa5ccdfd-6a7d-11ea-85d4-1078d2d2b808',1,188,1.00,1.00,30.00,0.00,0.00,30.00,4,'','','',0,'5910',NULL,1,0),('b6b0e9c8-66ac-11ea-85d4-1078d2d2b808','ae866ccf-66ac-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('bc474b25-6a7f-11ea-85d4-1078d2d2b808','bc4414d7-6a7f-11ea-85d4-1078d2d2b808',1,188,1.00,1.00,30.00,0.00,0.00,30.00,4,'','','',0,'5910',NULL,1,0),('c8f05b07-6696-11ea-85d4-1078d2d2b808','c8eb69d5-6696-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0),('d2484645-6696-11ea-85d4-1078d2d2b808','d24477c1-6696-11ea-85d4-1078d2d2b808',1,113,1.00,1.00,350.00,0.00,0.00,350.00,1,NULL,'','',0,'5910',NULL,NULL,0);
/*!40000 ALTER TABLE `o_body` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_body_state`
--

DROP TABLE IF EXISTS `o_body_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_body_state` (
  `f_id` int(11) NOT NULL,
  `f_name` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_body_state`
--

LOCK TABLES `o_body_state` WRITE;
/*!40000 ALTER TABLE `o_body_state` DISABLE KEYS */;
INSERT INTO `o_body_state` VALUES (1,'Նորմալ'),(2,'Սխալ'),(3,'Չեղակված');
/*!40000 ALTER TABLE `o_body_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_credit_card`
--

DROP TABLE IF EXISTS `o_credit_card`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_credit_card` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  `f_image` text,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_credit_card`
--

LOCK TABLES `o_credit_card` WRITE;
/*!40000 ALTER TABLE `o_credit_card` DISABLE KEYS */;
INSERT INTO `o_credit_card` VALUES (1,'VISA','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'),(2,'MAESTRO','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'),(3,'AMEX','iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAACXBIWXMAAAMJAAADCQEvBljhAAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAABnxJREFUeJztmm1sW1cZx3/nXufacRM7Lw40S9LaeWlTVdCUlcHG2iwr2jShbmWARumGmqxIfOHDJnUqSIh9QKC2gEDTSsvUboOqnZi2gSoGqJCktJ0oSCthpUmb1m7SUKeJkzh24vjt+vLhOm6yazvdBFxpuT/pKNK555z7PP/7f849vgpYWFhYWFhYWCxTBEDTifcaZC2zH0Q7aLVmB/W/RQRBO60K6blrOz5xQzQf66uXJPqAKrND+z8zmcmwQZIkDrD8kgeokiQOSLrtlyuiXfro13wxtFrJ7BDMxhLA7ADMxhLA7ADMxhLA7ADMxhLA7ADMxhLA7ADMxnZHgyRB96OtfLy0xHAtls7w4sVb7Gkr/JPiO+dHeMM/CcDrDzXzyWrnhwx3MX8JRvnm6euomsZ9K8s4tMWHXRYFxx8dGGffheCivjtywBdWVeRNHsBpk3hqjafo/N3rahDAppoV/7XkAbbUlvP9e+q552MrOLjFWzT5k9fD7H9f8nCHDuhaVwPAq5dD/ODdm7n+rfUuDm72stKpi/P2cJhnzg3nrlfZbXQ/1kqjy05HnYuvNOmfHc4Eo+zuDeTGuRSZv39pPQDb/zBI/9TckjHtaKnm+U11PN5YyWO+CmQh6JuI0dnjZzaVYW2Fg2Nbm3ApMmdHo+w9fwMtzzpLOuC+lWW0VjhQNY1fXgkBUCrr03r+HWEomiw4dzKR5vVruvWfa6ulo84FwCsDoSUTXIoTgxP8pG8UAFkIBsJxdvcGmE1laChTOPKAD5cic3Fyjm+dGSKdyZf+HTigq1V/+n8aiTAyoye7794Gnjk3nBVlnO/eXWeY1+Sycy2S4Gj/OF9rrsbnsgMwOB3n7Gh00Zh8bFtdwfc+bVxXzcCz7wxxbnSGw5fGcCkyW+tcdPX4iSRVPA4bRzsaqSktYSia4Bu9AWLpTMH8ijqgxe1gc205AK9c1p/a+qpSHm5w83CDG4A3/VNEkqph7rxwwViK314P5/rn11ldbufBelfBe6saJFTN0Nx2mRc3e2nz6HvJgX8EeeLUVSbiacpLZI50NLKqTCEUT9PVG2AykS6WYnEBOrNJvDcZ493xWQCebNE3vK+v1f/G0hl+nbX5Qu5dWcb6ylIAXuofI6PpJXHy+lR2bQ+yKLxpvT0c5nNvXTK0mZRKqU3iF+0+WtwOAKaTKnZZcKjdS2uFg5mUyu7eQM6xH0oAj8PGo94K4HbNykJglwV/HokwEU/jcegVdOxKCFVbXGOC25tnIJLg1Mg0JwYnSKgalXYbX/RVUjh9KJEELkU2tPk5qqaRWXBPmyRwZPemdAYSamHbL6TgHvDUGg8lkmA0luL3w9O5mz77zrBhbDCW4o83pg39j6xy8+M+hZuzSX7+rzHG51IA7GypxiFLRQXY5q3gh59pyHttOqmyq9vPtUgCWQjKFYlwQuXpXj/HP99Mk8vO0Y5GvnrqKqOxVLH88zugVJbY0VINwK+yT1cS0OZxGlqpTV/i5YEQ7zMBshB0Zkulf2qOUDyNXRbsXKOvTZESSKka0ZRqaGNzKTp7/FwOx5EE7PtsA68+2ER5iUw4odLZ7efmbJJaZwlHHvDhVuSiAuR1wOONlbgVmbkF9d1+l4tDW7yGsfsvBDkyMM4/J2JcCMUM17/cVMULF2/lNsrtvkqq7Pptizng5FCYk0PhIiPg+U31bMuW6eF2L109AW7NpdjVozuh2e3gcLuPzm4/cwVKwuAAScCu7Ob3ZuD2Dr8z64jhmSR/G5vlSjgO6AcSKZvJ/HF3IU6bxI7m6lzCnWtrcteKCbAUezfW8kSzfrBKZjTurlnBz+5fhSwEQ9EkT2dfixs9Tn6a7c+HWHO8b5FxH2pw88L9qwFIZzTmdVOyWc6f1FyKzJnt63DIEsk8hwxFEsykdPGcNpl0tj4USXAzluIuZwmqpqFqi9f/oPyoL0j/ZJzD7V5sRdb4TWCKvX81ngYNDph/f4O+syrZBnAhFMsdUyNJld9lLaosGLdwfFzVeMM/hSRY1H988PZbZWH/B+XgxVu8dGmcs6NRvn1+JO9Rd57tvkr2bDT+YDM4YLmx7L8HWAKYHYDZWAKYHYDZWAKYHYDZWAKYHYDZWAKYHYDZSPp/Ti5XRFAC7bTZYZiHdlqSNXUvYPyg99FnMpNhj9S/81NDsqZuAF5bHuUggsBrqpDarj65YcTsaCwsLCwsLCwsTOM/D0hr2RQ5pqcAAAAASUVORK5CYII='),(4,'MASTERCARD','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'),(5,'ARCA','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');
/*!40000 ALTER TABLE `o_credit_card` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_dish_remove_reason`
--

DROP TABLE IF EXISTS `o_dish_remove_reason`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_dish_remove_reason` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext CHARACTER SET utf8 COLLATE utf8_unicode_ci,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_dish_remove_reason`
--

LOCK TABLES `o_dish_remove_reason` WRITE;
/*!40000 ALTER TABLE `o_dish_remove_reason` DISABLE KEYS */;
/*!40000 ALTER TABLE `o_dish_remove_reason` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_goods`
--

DROP TABLE IF EXISTS `o_goods`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_goods` (
  `f_id` char(36) CHARACTER SET latin1 NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_body` char(36) DEFAULT NULL,
  `f_store` int(11) DEFAULT NULL,
  `f_goods` int(11) DEFAULT NULL,
  `f_qty` decimal(14,4) DEFAULT NULL,
  `f_price` decimal(14,2) DEFAULT NULL,
  `f_total` decimal(14,2) DEFAULT NULL,
  `f_tax` int(11) DEFAULT NULL,
  `f_sign` smallint(6) DEFAULT '1',
  `f_taxdept` varchar(2) DEFAULT NULL,
  `f_row` smallint(6) DEFAULT NULL,
  `f_storerec` char(36) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_goods`
--

LOCK TABLES `o_goods` WRITE;
/*!40000 ALTER TABLE `o_goods` DISABLE KEYS */;
INSERT INTO `o_goods` VALUES ('00a61a1b-6c4e-11ea-85d4-1078d2d2b808','009fee0f-6c4e-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('02c8e1dd-6697-11ea-85d4-1078d2d2b808','02bbf60c-6697-11ea-85d4-1078d2d2b808','02c0dfef-6697-11ea-85d4-1078d2d2b808',1,186,1.0000,0.00,0.00,0,1,'',1,'4f9d073a-668d-11ea-85d4-1078d2d2b808'),('056699a2-6c73-11ea-85d4-1078d2d2b808','a2084f11-6c72-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('057823b3-6c73-11ea-85d4-1078d2d2b808','05724423-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('05cc3860-6e22-11ea-85d4-1078d2d2b808','05c618ef-6e22-11ea-85d4-1078d2d2b808',NULL,6,14,2.0000,140.00,0.00,0,1,'0',1,'d79c96a7-6e1f-11ea-85d4-1078d2d2b808'),('063c06e7-6b97-11ea-85d4-1078d2d2b808','0632d916-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('068a6c21-6b97-11ea-85d4-1078d2d2b808','066a3453-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('068ee069-6b97-11ea-85d4-1078d2d2b808','066a3453-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('06a828b7-6b97-11ea-85d4-1078d2d2b808','066a3453-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('06fea97e-6b97-11ea-85d4-1078d2d2b808','06df2f69-6b97-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('0700ef27-6b97-11ea-85d4-1078d2d2b808','06df2f69-6b97-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('0702257b-6b97-11ea-85d4-1078d2d2b808','06df2f69-6b97-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('0736d759-6b97-11ea-85d4-1078d2d2b808','072a0540-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('0784be87-6b97-11ea-85d4-1078d2d2b808','076abe91-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('078983b7-6b97-11ea-85d4-1078d2d2b808','076abe91-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('0797c60a-6b97-11ea-85d4-1078d2d2b808','076abe91-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('07a19229-6b97-11ea-85d4-1078d2d2b808','076abe91-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('08070643-6b97-11ea-85d4-1078d2d2b808','07f804eb-6b97-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('086f90ce-6b97-11ea-85d4-1078d2d2b808','0841ccae-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('088141fc-6b97-11ea-85d4-1078d2d2b808','0841ccae-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('08833bba-6b97-11ea-85d4-1078d2d2b808','0841ccae-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('08868b76-6b97-11ea-85d4-1078d2d2b808','0841ccae-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('088f6d20-6b97-11ea-85d4-1078d2d2b808','0841ccae-6b97-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('08960486-6b97-11ea-85d4-1078d2d2b808','0841ccae-6b97-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('08e98264-6b97-11ea-85d4-1078d2d2b808','08d0bd4d-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('08f20e7a-6b97-11ea-85d4-1078d2d2b808','08d0bd4d-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('09070dbc-6b97-11ea-85d4-1078d2d2b808','08d0bd4d-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('090e9e4d-6b97-11ea-85d4-1078d2d2b808','08d0bd4d-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('09104ca6-6b97-11ea-85d4-1078d2d2b808','08d0bd4d-6b97-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('09182141-6b97-11ea-85d4-1078d2d2b808','08d0bd4d-6b97-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('0957a561-6b97-11ea-85d4-1078d2d2b808','0941758a-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('0991b766-6b97-11ea-85d4-1078d2d2b808','0983d660-6b97-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('09a2e034-6b97-11ea-85d4-1078d2d2b808','0983d660-6b97-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('09d0fc4d-6b97-11ea-85d4-1078d2d2b808','09c93cc9-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('0ef86058-6c4a-11ea-85d4-1078d2d2b808','0ef36296-6c4a-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('1a636cca-6a80-11ea-85d4-1078d2d2b808','bc4414d7-6a7f-11ea-85d4-1078d2d2b808','bc474b25-6a7f-11ea-85d4-1078d2d2b808',4,227,0.0630,0.00,0.00,0,1,'',1,'b38a65eb-6a7e-11ea-85d4-1078d2d2b808'),('1e7226e2-6e1f-11ea-85d4-1078d2d2b808','1e6be1a8-6e1f-11ea-85d4-1078d2d2b808',NULL,6,294,8.0000,2940.00,0.00,0,1,'0',1,'1e7a1bff-6e1f-11ea-85d4-1078d2d2b808'),('1ff9a8b9-6c7c-11ea-85d4-1078d2d2b808','1ff2cd86-6c7c-11ea-85d4-1078d2d2b808',NULL,6,14,2.0000,140.00,0.00,0,1,'0',1,'1ffdd359-6c7c-11ea-85d4-1078d2d2b808'),('20867bf1-6a7f-11ea-85d4-1078d2d2b808','dece8ddc-6a7e-11ea-85d4-1078d2d2b808','1b3a51ac-6a7f-11ea-85d4-1078d2d2b808',2,229,0.2500,0.00,0.00,0,1,'',1,'208a6093-6a7f-11ea-85d4-1078d2d2b808'),('23fa3b0a-6c66-11ea-85d4-1078d2d2b808','23f515e4-6c66-11ea-85d4-1078d2d2b808',NULL,6,380,2.0000,3800.00,0.00,0,1,'0',1,'74558ab8-6c15-11ea-85d4-1078d2d2b808'),('24bb9993-6c68-11ea-85d4-1078d2d2b808','24b4dc82-6c68-11ea-85d4-1078d2d2b808',NULL,6,4,20.0000,40.00,0.00,0,1,'0',1,'24c23b9a-6c68-11ea-85d4-1078d2d2b808'),('24bdc604-6c68-11ea-85d4-1078d2d2b808','24b4dc82-6c68-11ea-85d4-1078d2d2b808',NULL,6,276,1.0000,2760.00,0.00,0,1,'0',2,'a00706d3-6c5f-11ea-85d4-1078d2d2b808'),('25af3112-6c66-11ea-85d4-1078d2d2b808','25a9afde-6c66-11ea-85d4-1078d2d2b808',NULL,6,380,2.0000,3800.00,0.00,0,1,'0',1,'74558ab8-6c15-11ea-85d4-1078d2d2b808'),('25ba8dc0-6c66-11ea-85d4-1078d2d2b808','25b5dcc8-6c66-11ea-85d4-1078d2d2b808',NULL,6,380,2.0000,3800.00,0.00,0,1,'0',1,'74558ab8-6c15-11ea-85d4-1078d2d2b808'),('2648c3cd-6ce1-11ea-85d4-1078d2d2b808','26415a71-6ce1-11ea-85d4-1078d2d2b808',NULL,NULL,86,3.0000,860.00,0.00,0,1,'0',1,'264e6ee0-6ce1-11ea-85d4-1078d2d2b808'),('265c3b4e-6ce1-11ea-85d4-1078d2d2b808','26569800-6ce1-11ea-85d4-1078d2d2b808',NULL,NULL,6,35.0000,60.00,0.00,0,1,'0',1,'26600b01-6ce1-11ea-85d4-1078d2d2b808'),('273fa523-6697-11ea-85d4-1078d2d2b808','2733dc04-6697-11ea-85d4-1078d2d2b808','2737c6d1-6697-11ea-85d4-1078d2d2b808',1,186,1.0000,0.00,0.00,0,1,'',1,'4f9d073a-668d-11ea-85d4-1078d2d2b808'),('274f78c0-6c16-11ea-85d4-1078d2d2b808','274ac699-6c16-11ea-85d4-1078d2d2b808',NULL,6,385,1.0000,3850.00,0.00,0,1,'0',1,'2755958d-6c16-11ea-85d4-1078d2d2b808'),('2751b134-6c16-11ea-85d4-1078d2d2b808','274ac699-6c16-11ea-85d4-1078d2d2b808',NULL,6,290,2.0000,2900.00,0.00,0,1,'0',2,'2757c7da-6c16-11ea-85d4-1078d2d2b808'),('27625cd3-6c16-11ea-85d4-1078d2d2b808','275d9af9-6c16-11ea-85d4-1078d2d2b808',NULL,6,385,1.0000,3850.00,0.00,0,1,'0',1,'2755958d-6c16-11ea-85d4-1078d2d2b808'),('27645ef1-6c16-11ea-85d4-1078d2d2b808','275d9af9-6c16-11ea-85d4-1078d2d2b808',NULL,6,290,2.0000,2900.00,0.00,0,1,'0',2,'2757c7da-6c16-11ea-85d4-1078d2d2b808'),('28481120-6a80-11ea-85d4-1078d2d2b808','1f067601-6a80-11ea-85d4-1078d2d2b808','247c7aa9-6a80-11ea-85d4-1078d2d2b808',2,345,1.0000,0.00,0.00,0,1,'',1,'284ca5ce-6a80-11ea-85d4-1078d2d2b808'),('2dc85510-6b96-11ea-85d4-1078d2d2b808','2dc2c62b-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('2dd7948a-6b96-11ea-85d4-1078d2d2b808','2dd3752c-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('2dd95cd6-6b96-11ea-85d4-1078d2d2b808','2dd3752c-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('2ddaaa58-6b96-11ea-85d4-1078d2d2b808','2dd3752c-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('2dea843c-6b96-11ea-85d4-1078d2d2b808','2de66a89-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('2dec886f-6b96-11ea-85d4-1078d2d2b808','2de66a89-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('2dedd00b-6b96-11ea-85d4-1078d2d2b808','2de66a89-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('2dff03fe-6b96-11ea-85d4-1078d2d2b808','2dfa94c8-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('2e099367-6b96-11ea-85d4-1078d2d2b808','2e051c57-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('2e0c0ed0-6b96-11ea-85d4-1078d2d2b808','2e051c57-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('2e0d99f9-6b96-11ea-85d4-1078d2d2b808','2e051c57-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('2e0f5759-6b96-11ea-85d4-1078d2d2b808','2e051c57-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('2e1f934b-6b96-11ea-85d4-1078d2d2b808','2e1adaf3-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('2e2d5488-6b96-11ea-85d4-1078d2d2b808','2e28f84e-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('2e2f6285-6b96-11ea-85d4-1078d2d2b808','2e28f84e-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('2e30e420-6b96-11ea-85d4-1078d2d2b808','2e28f84e-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('2e3233fd-6b96-11ea-85d4-1078d2d2b808','2e28f84e-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('2e339e61-6b96-11ea-85d4-1078d2d2b808','2e28f84e-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('2e3512ba-6b96-11ea-85d4-1078d2d2b808','2e28f84e-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('2e4887f8-6b96-11ea-85d4-1078d2d2b808','2e43f0f1-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('2e4a82b2-6b96-11ea-85d4-1078d2d2b808','2e43f0f1-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('2e4bff81-6b96-11ea-85d4-1078d2d2b808','2e43f0f1-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('2e4db022-6b96-11ea-85d4-1078d2d2b808','2e43f0f1-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('2e4f6ab6-6b96-11ea-85d4-1078d2d2b808','2e43f0f1-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('2e51bbff-6b96-11ea-85d4-1078d2d2b808','2e43f0f1-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('2e6c600d-6b96-11ea-85d4-1078d2d2b808','2e6747da-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('2e7bf396-6b96-11ea-85d4-1078d2d2b808','2e76b17f-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('2e7e77d8-6b96-11ea-85d4-1078d2d2b808','2e76b17f-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('35b7c320-6b96-11ea-85d4-1078d2d2b808','35b35f65-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('35c1e592-6b96-11ea-85d4-1078d2d2b808','35bde553-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('35c39319-6b96-11ea-85d4-1078d2d2b808','35bde553-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('35c4d4ac-6b96-11ea-85d4-1078d2d2b808','35bde553-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('35d7ce2a-6b96-11ea-85d4-1078d2d2b808','35d35566-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('35d9d6c1-6b96-11ea-85d4-1078d2d2b808','35d35566-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('35db51d8-6b96-11ea-85d4-1078d2d2b808','35d35566-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('35e7d51e-6b96-11ea-85d4-1078d2d2b808','35e349db-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('35f2c948-6b96-11ea-85d4-1078d2d2b808','35ee60a0-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('35f48529-6b96-11ea-85d4-1078d2d2b808','35ee60a0-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('35f5c8dc-6b96-11ea-85d4-1078d2d2b808','35ee60a0-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('35f7311e-6b96-11ea-85d4-1078d2d2b808','35ee60a0-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('36088b8d-6b96-11ea-85d4-1078d2d2b808','360437c7-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('360b865f-682c-11ea-85d4-1078d2d2b808','3607020e-682c-11ea-85d4-1078d2d2b808',NULL,2,37,0.2000,0.00,0.00,0,1,'',1,'cd7c2879-682b-11ea-85d4-1078d2d2b808'),('3621b7f7-6b96-11ea-85d4-1078d2d2b808','361d6981-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('36237827-6b96-11ea-85d4-1078d2d2b808','361d6981-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('3625ce0d-6b96-11ea-85d4-1078d2d2b808','361d6981-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('3627204e-6b96-11ea-85d4-1078d2d2b808','361d6981-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('362951f1-6b96-11ea-85d4-1078d2d2b808','361d6981-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('362aac4d-6b96-11ea-85d4-1078d2d2b808','361d6981-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('36396021-6b96-11ea-85d4-1078d2d2b808','363532e4-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('363b08d0-6b96-11ea-85d4-1078d2d2b808','363532e4-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('363c7022-6b96-11ea-85d4-1078d2d2b808','363532e4-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('363dd4fa-6b96-11ea-85d4-1078d2d2b808','363532e4-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('363f13df-6b96-11ea-85d4-1078d2d2b808','363532e4-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('36406d76-6b96-11ea-85d4-1078d2d2b808','363532e4-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('3650a818-6b96-11ea-85d4-1078d2d2b808','364bfd5b-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('365a8a96-6b96-11ea-85d4-1078d2d2b808','3656741a-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('365c58d5-6b96-11ea-85d4-1078d2d2b808','3656741a-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('39a3adc9-6c58-11ea-85d4-1078d2d2b808','399f8340-6c58-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('3a5231e7-6c49-11ea-85d4-1078d2d2b808','3a47f05f-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('3a96250b-6f2d-11ea-85d4-1078d2d2b808','3a8e6c21-6f2d-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',1,'3a9caf4f-6f2d-11ea-85d4-1078d2d2b808'),('3a9864e5-6f2d-11ea-85d4-1078d2d2b808','3a8e6c21-6f2d-11ea-85d4-1078d2d2b808',NULL,6,4,15.5000,40.00,0.00,0,1,'0',2,'f9dc00c9-6e6b-11ea-85d4-1078d2d2b808'),('3b326512-6b96-11ea-85d4-1078d2d2b808','3b2e02f1-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('3b3a6c5a-6c74-11ea-85d4-1078d2d2b808','3b34db63-6c74-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('3b3c35f7-6b96-11ea-85d4-1078d2d2b808','3b384299-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('3b3dd976-6b96-11ea-85d4-1078d2d2b808','3b384299-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('3b3f0c95-6b96-11ea-85d4-1078d2d2b808','3b384299-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('3b4601aa-6c74-11ea-85d4-1078d2d2b808','3b4145ae-6c74-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('3b4a1cba-6b96-11ea-85d4-1078d2d2b808','3b46108c-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('3b4c30dd-6b96-11ea-85d4-1078d2d2b808','3b46108c-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('3b4d72d7-6b96-11ea-85d4-1078d2d2b808','3b46108c-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('3b5a6d8e-6b96-11ea-85d4-1078d2d2b808','3b560346-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('3b6545d1-6b96-11ea-85d4-1078d2d2b808','3b613c1c-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('3b66c4cb-6b96-11ea-85d4-1078d2d2b808','3b613c1c-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('3b683b2d-6b96-11ea-85d4-1078d2d2b808','3b613c1c-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('3b699459-6b96-11ea-85d4-1078d2d2b808','3b613c1c-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('3b761155-6b96-11ea-85d4-1078d2d2b808','3b71cf95-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('3b801a34-6b96-11ea-85d4-1078d2d2b808','3b7c0b1b-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('3b81df0a-6b96-11ea-85d4-1078d2d2b808','3b7c0b1b-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('3b832fd9-6b96-11ea-85d4-1078d2d2b808','3b7c0b1b-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('3b847a01-6b96-11ea-85d4-1078d2d2b808','3b7c0b1b-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('3b8614fd-6b96-11ea-85d4-1078d2d2b808','3b7c0b1b-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('3b87aefc-6b96-11ea-85d4-1078d2d2b808','3b7c0b1b-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('3b958718-6b96-11ea-85d4-1078d2d2b808','3b917999-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('3b971bdc-6b96-11ea-85d4-1078d2d2b808','3b917999-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('3b988e33-6b96-11ea-85d4-1078d2d2b808','3b917999-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('3b9a0e8d-6b96-11ea-85d4-1078d2d2b808','3b917999-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('3b9b59d8-6b96-11ea-85d4-1078d2d2b808','3b917999-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('3b9cb223-6b96-11ea-85d4-1078d2d2b808','3b917999-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('3baaf98c-6b96-11ea-85d4-1078d2d2b808','3ba69824-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('3bb57543-6b96-11ea-85d4-1078d2d2b808','3bb16e67-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('3bb7278c-6b96-11ea-85d4-1078d2d2b808','3bb16e67-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('3c2057ef-6e1d-11ea-85d4-1078d2d2b808','3c15fc26-6e1d-11ea-85d4-1078d2d2b808',NULL,6,313,6.0000,3130.00,0.00,0,1,'0',1,'b9a655db-6ce2-11ea-85d4-1078d2d2b808'),('3c3afcfd-6e1d-11ea-85d4-1078d2d2b808','3c334ac5-6e1d-11ea-85d4-1078d2d2b808',NULL,6,86,5.0000,860.00,0.00,0,1,'0',1,'3c40ca62-6e1d-11ea-85d4-1078d2d2b808'),('3c4eb124-6e1d-11ea-85d4-1078d2d2b808','3c47fd69-6e1d-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',1,'3c547d29-6e1d-11ea-85d4-1078d2d2b808'),('3c62e8e3-6e1d-11ea-85d4-1078d2d2b808','3c5b759f-6e1d-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'b9a655db-6ce2-11ea-85d4-1078d2d2b808'),('3face457-6c4b-11ea-85d4-1078d2d2b808','3fa55461-6c4b-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('3fbc4c00-6c4b-11ea-85d4-1078d2d2b808','3fb73769-6c4b-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('408bfaeb-6b96-11ea-85d4-1078d2d2b808','40874467-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('4095d6e3-6b96-11ea-85d4-1078d2d2b808','40918da5-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('409798e3-6b96-11ea-85d4-1078d2d2b808','40918da5-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('409937f7-6b96-11ea-85d4-1078d2d2b808','40918da5-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('40a582c8-6b96-11ea-85d4-1078d2d2b808','40a17b74-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('40a72c2c-6b96-11ea-85d4-1078d2d2b808','40a17b74-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('40a8945a-6b96-11ea-85d4-1078d2d2b808','40a17b74-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('40b451df-6b96-11ea-85d4-1078d2d2b808','40b057c2-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('40be9d36-6b96-11ea-85d4-1078d2d2b808','40ba716a-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('40c05f61-6b96-11ea-85d4-1078d2d2b808','40ba716a-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('40c1cb57-6b96-11ea-85d4-1078d2d2b808','40ba716a-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('40c30c26-6b96-11ea-85d4-1078d2d2b808','40ba716a-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('40cf2aff-6b96-11ea-85d4-1078d2d2b808','40cb1e04-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('40d947de-6b96-11ea-85d4-1078d2d2b808','40d53da2-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('40dafbd2-6b96-11ea-85d4-1078d2d2b808','40d53da2-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('40dc3f13-6b96-11ea-85d4-1078d2d2b808','40d53da2-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('40ddace1-6b96-11ea-85d4-1078d2d2b808','40d53da2-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('40df224a-6b96-11ea-85d4-1078d2d2b808','40d53da2-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('40e06aa5-6b96-11ea-85d4-1078d2d2b808','40d53da2-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('40ee4ec4-6b96-11ea-85d4-1078d2d2b808','40ea1c33-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('40f07d33-6b96-11ea-85d4-1078d2d2b808','40ea1c33-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('40f1e4c7-6b96-11ea-85d4-1078d2d2b808','40ea1c33-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('40f353dd-6b96-11ea-85d4-1078d2d2b808','40ea1c33-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('40f4bb70-6b96-11ea-85d4-1078d2d2b808','40ea1c33-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('40f63159-6b96-11ea-85d4-1078d2d2b808','40ea1c33-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('40fe244d-6c49-11ea-85d4-1078d2d2b808','40f99cde-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('41042724-6b96-11ea-85d4-1078d2d2b808','40ffeb6c-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('410e22f2-6b96-11ea-85d4-1078d2d2b808','4109fcd5-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('410fe9ed-6b96-11ea-85d4-1078d2d2b808','4109fcd5-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('41ddc085-6c74-11ea-85d4-1078d2d2b808','41d70665-6c74-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('43ed7a3d-6c5f-11ea-85d4-1078d2d2b808','43e8104a-6c5f-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',1,'43f6833a-6c5f-11ea-85d4-1078d2d2b808'),('43ef8951-6c5f-11ea-85d4-1078d2d2b808','43e8104a-6c5f-11ea-85d4-1078d2d2b808',NULL,6,380,10.0000,3800.00,0.00,0,1,'0',2,'74558ab8-6c15-11ea-85d4-1078d2d2b808'),('43f194b0-6c5f-11ea-85d4-1078d2d2b808','43e8104a-6c5f-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',3,'43f97277-6c5f-11ea-85d4-1078d2d2b808'),('45e1ffb1-6b96-11ea-85d4-1078d2d2b808','45dda76f-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('45eb1f32-6b96-11ea-85d4-1078d2d2b808','45e72908-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('45ecd075-6b96-11ea-85d4-1078d2d2b808','45e72908-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('45ee3772-6b96-11ea-85d4-1078d2d2b808','45e72908-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('45f91f8e-6b96-11ea-85d4-1078d2d2b808','45f509d6-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('45faaa62-6b96-11ea-85d4-1078d2d2b808','45f509d6-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('45fc0fd0-6b96-11ea-85d4-1078d2d2b808','45f509d6-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('46069e68-6b96-11ea-85d4-1078d2d2b808','4602be1b-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('46106df4-6b96-11ea-85d4-1078d2d2b808','460c4eed-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('461202a9-6b96-11ea-85d4-1078d2d2b808','460c4eed-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('46136c6d-6b96-11ea-85d4-1078d2d2b808','460c4eed-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('46152895-6b96-11ea-85d4-1078d2d2b808','460c4eed-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('4620f760-6b96-11ea-85d4-1078d2d2b808','461d02ba-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('462b515b-6b96-11ea-85d4-1078d2d2b808','46271393-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('462d5254-6b96-11ea-85d4-1078d2d2b808','46271393-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('462efefb-6b96-11ea-85d4-1078d2d2b808','46271393-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('46303c20-6b96-11ea-85d4-1078d2d2b808','46271393-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('4631a8ac-6b96-11ea-85d4-1078d2d2b808','46271393-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('46331862-6b96-11ea-85d4-1078d2d2b808','46271393-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('46408c24-6b96-11ea-85d4-1078d2d2b808','463c919f-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('4642447c-6b96-11ea-85d4-1078d2d2b808','463c919f-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('464381d0-6b96-11ea-85d4-1078d2d2b808','463c919f-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('4644f2f8-6b96-11ea-85d4-1078d2d2b808','463c919f-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('4646710c-6b96-11ea-85d4-1078d2d2b808','463c919f-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('4647b467-6b96-11ea-85d4-1078d2d2b808','463c919f-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('46560ab0-6b96-11ea-85d4-1078d2d2b808','4651e3f8-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('46609a95-6b96-11ea-85d4-1078d2d2b808','465c5594-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('46625d5a-6b96-11ea-85d4-1078d2d2b808','465c5594-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('47879ebe-6ea1-11ea-85d4-1078d2d2b808','4772e5c9-6ea1-11ea-85d4-1078d2d2b808',NULL,6,67,3.0000,670.00,0.00,0,1,'0',1,'47ab98f7-6ea1-11ea-85d4-1078d2d2b808'),('47944f67-6ea1-11ea-85d4-1078d2d2b808','4772e5c9-6ea1-11ea-85d4-1078d2d2b808',NULL,6,307,2.0000,3070.00,0.00,0,1,'0',2,'47afc0da-6ea1-11ea-85d4-1078d2d2b808'),('479ea4e7-6c49-11ea-85d4-1078d2d2b808','47988092-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('4d9834ae-6b96-11ea-85d4-1078d2d2b808','4d9369f0-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('4da2a623-6b96-11ea-85d4-1078d2d2b808','4d9e2e02-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('4da437e6-6b96-11ea-85d4-1078d2d2b808','4d9e2e02-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('4da5a4de-6b96-11ea-85d4-1078d2d2b808','4d9e2e02-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('4db1c16d-6b96-11ea-85d4-1078d2d2b808','4dad20b1-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('4db3c6e9-6b96-11ea-85d4-1078d2d2b808','4dad20b1-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('4db54c95-6b96-11ea-85d4-1078d2d2b808','4dad20b1-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('4dc10acf-6b96-11ea-85d4-1078d2d2b808','4dbcd506-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('4dd60877-6b96-11ea-85d4-1078d2d2b808','4dcf752b-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('4dd7da34-6b96-11ea-85d4-1078d2d2b808','4dcf752b-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('4dd91241-6b96-11ea-85d4-1078d2d2b808','4dcf752b-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('4dda56df-6b96-11ea-85d4-1078d2d2b808','4dcf752b-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('4de5e8e6-6b96-11ea-85d4-1078d2d2b808','4de2286b-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('4def78ed-6b96-11ea-85d4-1078d2d2b808','4deb8de5-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('4df13667-6b96-11ea-85d4-1078d2d2b808','4deb8de5-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('4df2819d-6b96-11ea-85d4-1078d2d2b808','4deb8de5-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('4df3ef29-6b96-11ea-85d4-1078d2d2b808','4deb8de5-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('4df55de6-6b96-11ea-85d4-1078d2d2b808','4deb8de5-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('4df6a538-6b96-11ea-85d4-1078d2d2b808','4deb8de5-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('4e0502c7-6b96-11ea-85d4-1078d2d2b808','4e00d4ac-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('4e06c008-6b96-11ea-85d4-1078d2d2b808','4e00d4ac-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('4e083d7e-6b96-11ea-85d4-1078d2d2b808','4e00d4ac-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('4e09a35d-6b96-11ea-85d4-1078d2d2b808','4e00d4ac-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('4e0aef5b-6b96-11ea-85d4-1078d2d2b808','4e00d4ac-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('4e0c6338-6b96-11ea-85d4-1078d2d2b808','4e00d4ac-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('4e1ff943-6b96-11ea-85d4-1078d2d2b808','4e1c3ad9-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('4e2960cd-6b96-11ea-85d4-1078d2d2b808','4e259ce3-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('4e2b1d09-6b96-11ea-85d4-1078d2d2b808','4e259ce3-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('4e3f45bc-6c49-11ea-85d4-1078d2d2b808','4e38e569-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('4f180fac-6c74-11ea-85d4-1078d2d2b808','4f12cbed-6c74-11ea-85d4-1078d2d2b808',NULL,6,4,23.0000,40.00,0.00,0,1,'0',1,'24c23b9a-6c68-11ea-85d4-1078d2d2b808'),('4f550272-682c-11ea-85d4-1078d2d2b808','4f4917b4-682c-11ea-85d4-1078d2d2b808',NULL,2,37,0.2000,300.00,60.00,77,1,'',1,'cd7c2879-682b-11ea-85d4-1078d2d2b808'),('4f97e9c6-668d-11ea-85d4-1078d2d2b808','4f87326a-668d-11ea-85d4-1078d2d2b808','4f8ccaf9-668d-11ea-85d4-1078d2d2b808',1,186,1.0000,0.00,0.00,0,1,'',1,'4f9d073a-668d-11ea-85d4-1078d2d2b808'),('50293d82-6ecf-11ea-85d4-1078d2d2b808','50207e94-6ecf-11ea-85d4-1078d2d2b808',NULL,6,294,1.5000,2940.00,0.00,0,1,'0',1,'1e7a1bff-6e1f-11ea-85d4-1078d2d2b808'),('524abe87-66aa-11ea-85d4-1078d2d2b808','1e84d0da-6698-11ea-85d4-1078d2d2b808','5240bb24-66aa-11ea-85d4-1078d2d2b808',1,186,1.0000,0.00,0.00,0,1,'',1,'4f9d073a-668d-11ea-85d4-1078d2d2b808'),('52fcbee8-6b96-11ea-85d4-1078d2d2b808','52f856b1-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('5308d82a-6b96-11ea-85d4-1078d2d2b808','5303113e-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('530abc33-6b96-11ea-85d4-1078d2d2b808','5303113e-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('530c6af0-6b96-11ea-85d4-1078d2d2b808','5303113e-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('53181afa-6b96-11ea-85d4-1078d2d2b808','5313f02f-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('5319a3fd-6b96-11ea-85d4-1078d2d2b808','5313f02f-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('531b0ef4-6b96-11ea-85d4-1078d2d2b808','5313f02f-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('5326de42-6b96-11ea-85d4-1078d2d2b808','5322c6d3-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5336af3a-6b96-11ea-85d4-1078d2d2b808','532ce872-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('5338f673-6b96-11ea-85d4-1078d2d2b808','532ce872-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('53453ba9-6b96-11ea-85d4-1078d2d2b808','532ce872-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('53548b10-6b96-11ea-85d4-1078d2d2b808','532ce872-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('5384d68e-6b96-11ea-85d4-1078d2d2b808','537b8d53-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('538e9e74-6b96-11ea-85d4-1078d2d2b808','538a90b9-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('53903796-6b96-11ea-85d4-1078d2d2b808','538a90b9-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5391a6e5-6b96-11ea-85d4-1078d2d2b808','538a90b9-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5392e687-6b96-11ea-85d4-1078d2d2b808','538a90b9-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('539412ef-6b96-11ea-85d4-1078d2d2b808','538a90b9-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('53958f9c-6b96-11ea-85d4-1078d2d2b808','538a90b9-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('53a2d95f-6b96-11ea-85d4-1078d2d2b808','539ec0a5-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('53a49711-6b96-11ea-85d4-1078d2d2b808','539ec0a5-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('53a6042e-6b96-11ea-85d4-1078d2d2b808','539ec0a5-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('53a7415b-6b96-11ea-85d4-1078d2d2b808','539ec0a5-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('53a8b45a-6b96-11ea-85d4-1078d2d2b808','539ec0a5-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('53aa2fdb-6b96-11ea-85d4-1078d2d2b808','539ec0a5-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('53b7dbca-6b96-11ea-85d4-1078d2d2b808','53b3c087-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('53c1f1f2-6b96-11ea-85d4-1078d2d2b808','53bde073-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('53c3ae05-6b96-11ea-85d4-1078d2d2b808','53bde073-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('54e0ca51-6c49-11ea-85d4-1078d2d2b808','54da0beb-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('5897b919-6b96-11ea-85d4-1078d2d2b808','58912099-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('58cebe05-6b96-11ea-85d4-1078d2d2b808','58c37983-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('58d0b1b6-6b96-11ea-85d4-1078d2d2b808','58c37983-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('58d253fc-6b96-11ea-85d4-1078d2d2b808','58c37983-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('58e041ba-6b96-11ea-85d4-1078d2d2b808','58dafb0b-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('58e1f645-6b96-11ea-85d4-1078d2d2b808','58dafb0b-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('58e3a298-6b96-11ea-85d4-1078d2d2b808','58dafb0b-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('59125ba1-6b96-11ea-85d4-1078d2d2b808','58fd95a6-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5922f095-6b96-11ea-85d4-1078d2d2b808','591efb9e-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('59249bbd-6b96-11ea-85d4-1078d2d2b808','591efb9e-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5925ca52-6b96-11ea-85d4-1078d2d2b808','591efb9e-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5926ee68-6b96-11ea-85d4-1078d2d2b808','591efb9e-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('59369ddb-6b96-11ea-85d4-1078d2d2b808','59327acc-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('5941405e-6b96-11ea-85d4-1078d2d2b808','593d28aa-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('59430144-6b96-11ea-85d4-1078d2d2b808','593d28aa-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('59444f7b-6b96-11ea-85d4-1078d2d2b808','593d28aa-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5945c4f3-6b96-11ea-85d4-1078d2d2b808','593d28aa-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('59472dc2-6b96-11ea-85d4-1078d2d2b808','593d28aa-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('59487b79-6b96-11ea-85d4-1078d2d2b808','593d28aa-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('5956b16f-6b96-11ea-85d4-1078d2d2b808','5952667d-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('59585eb6-6b96-11ea-85d4-1078d2d2b808','5952667d-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5959e2a5-6b96-11ea-85d4-1078d2d2b808','5952667d-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('595b2069-6b96-11ea-85d4-1078d2d2b808','5952667d-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('595c463e-6b96-11ea-85d4-1078d2d2b808','5952667d-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('595d94ab-6b96-11ea-85d4-1078d2d2b808','5952667d-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('5970ae53-6b96-11ea-85d4-1078d2d2b808','596c04f8-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('597a5f9f-6b96-11ea-85d4-1078d2d2b808','59766e96-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('597c1af6-6b96-11ea-85d4-1078d2d2b808','59766e96-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('5b7f072b-6c49-11ea-85d4-1078d2d2b808','5b79c05e-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('5e509299-6b96-11ea-85d4-1078d2d2b808','5e4bbcfd-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('5e5a4635-6b96-11ea-85d4-1078d2d2b808','5e566045-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5e5becae-6b96-11ea-85d4-1078d2d2b808','5e566045-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5e5d2c1a-6b96-11ea-85d4-1078d2d2b808','5e566045-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('5e68f324-6b96-11ea-85d4-1078d2d2b808','5e64a7a2-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('5e6aeefb-6b96-11ea-85d4-1078d2d2b808','5e64a7a2-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('5e6c4310-6b96-11ea-85d4-1078d2d2b808','5e64a7a2-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('5e775704-6b96-11ea-85d4-1078d2d2b808','5e73448f-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5e81792a-6b96-11ea-85d4-1078d2d2b808','5e7d505d-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('5e834342-6b96-11ea-85d4-1078d2d2b808','5e7d505d-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5e8494e6-6b96-11ea-85d4-1078d2d2b808','5e7d505d-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5e85d3a1-6b96-11ea-85d4-1078d2d2b808','5e7d505d-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('5e92180f-6b96-11ea-85d4-1078d2d2b808','5e8e021f-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('5e9c680f-6b96-11ea-85d4-1078d2d2b808','5e987660-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('5e9dfe90-6b96-11ea-85d4-1078d2d2b808','5e987660-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5e9f6da5-6b96-11ea-85d4-1078d2d2b808','5e987660-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5ea0e1ed-6b96-11ea-85d4-1078d2d2b808','5e987660-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('5ea22cb7-6b96-11ea-85d4-1078d2d2b808','5e987660-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('5ea3869e-6b96-11ea-85d4-1078d2d2b808','5e987660-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('5eb18620-6b96-11ea-85d4-1078d2d2b808','5ead81f7-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('5eb34cab-6b96-11ea-85d4-1078d2d2b808','5ead81f7-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5eb4ca4a-6b96-11ea-85d4-1078d2d2b808','5ead81f7-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('5eb62a58-6b96-11ea-85d4-1078d2d2b808','5ead81f7-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('5eb7e432-6b96-11ea-85d4-1078d2d2b808','5ead81f7-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('5eb96d1d-6b96-11ea-85d4-1078d2d2b808','5ead81f7-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('5ec67a1b-6b96-11ea-85d4-1078d2d2b808','5ec2d47e-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('5ed7782b-6b96-11ea-85d4-1078d2d2b808','5ed338fb-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('5ed94907-6b96-11ea-85d4-1078d2d2b808','5ed338fb-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('5ee4bfcd-6b96-11ea-85d4-1078d2d2b808','5ee0a8a9-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('62210946-6c49-11ea-85d4-1078d2d2b808','621a50df-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('628b2f5f-6c4d-11ea-85d4-1078d2d2b808','6282f3f2-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('63b613d4-6b96-11ea-85d4-1078d2d2b808','63b1fc71-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('63bf035e-6b96-11ea-85d4-1078d2d2b808','63bb3ad3-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('63c08196-6b96-11ea-85d4-1078d2d2b808','63bb3ad3-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('63c1e90a-6b96-11ea-85d4-1078d2d2b808','63bb3ad3-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('63cd1906-6b96-11ea-85d4-1078d2d2b808','63c922ef-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('63ceb347-6b96-11ea-85d4-1078d2d2b808','63c922ef-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('63d01942-6b96-11ea-85d4-1078d2d2b808','63c922ef-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('63db3771-6b96-11ea-85d4-1078d2d2b808','63d723ee-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('63e52b30-6b96-11ea-85d4-1078d2d2b808','63e1414f-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('63e6ce54-6b96-11ea-85d4-1078d2d2b808','63e1414f-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('63e87763-6b96-11ea-85d4-1078d2d2b808','63e1414f-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('63ea048d-6b96-11ea-85d4-1078d2d2b808','63e1414f-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('63f772d6-6b96-11ea-85d4-1078d2d2b808','63f36200-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('6401566b-6b96-11ea-85d4-1078d2d2b808','63fd7054-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('640312e5-6b96-11ea-85d4-1078d2d2b808','63fd7054-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('640457bb-6b96-11ea-85d4-1078d2d2b808','63fd7054-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6405c022-6b96-11ea-85d4-1078d2d2b808','63fd7054-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('640731ea-6b96-11ea-85d4-1078d2d2b808','63fd7054-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('64088247-6b96-11ea-85d4-1078d2d2b808','63fd7054-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('64160c9b-6b96-11ea-85d4-1078d2d2b808','64121980-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6417a664-6b96-11ea-85d4-1078d2d2b808','64121980-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('64190f9a-6b96-11ea-85d4-1078d2d2b808','64121980-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('641a7db3-6b96-11ea-85d4-1078d2d2b808','64121980-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('641bcb12-6b96-11ea-85d4-1078d2d2b808','64121980-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('641d067d-6b96-11ea-85d4-1078d2d2b808','64121980-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('642a99f3-6b96-11ea-85d4-1078d2d2b808','6426953a-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('6434ee18-6b96-11ea-85d4-1078d2d2b808','6430e392-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('64369d13-6b96-11ea-85d4-1078d2d2b808','6430e392-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('6444b568-6b96-11ea-85d4-1078d2d2b808','6440a223-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('64a6b10f-6ce4-11ea-85d4-1078d2d2b808','64a0581a-6ce4-11ea-85d4-1078d2d2b808',NULL,NULL,14,3.0000,140.00,0.00,0,1,'0',1,'64ab097a-6ce4-11ea-85d4-1078d2d2b808'),('670d26b2-6b97-11ea-85d4-1078d2d2b808','66febafb-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('674ce562-6b97-11ea-85d4-1078d2d2b808','673447fa-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('674fd2db-6b97-11ea-85d4-1078d2d2b808','673447fa-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6756a978-6b97-11ea-85d4-1078d2d2b808','673447fa-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('67921450-6b97-11ea-85d4-1078d2d2b808','677e1b89-6b97-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('679bd1bb-6b97-11ea-85d4-1078d2d2b808','677e1b89-6b97-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('67b0f594-6b97-11ea-85d4-1078d2d2b808','677e1b89-6b97-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('67fefdbd-6b97-11ea-85d4-1078d2d2b808','67ecabbd-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('68495270-6b97-11ea-85d4-1078d2d2b808','6824dad2-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6854aef4-6b97-11ea-85d4-1078d2d2b808','6824dad2-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('685cf2bf-6b97-11ea-85d4-1078d2d2b808','6824dad2-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('685e441c-6b97-11ea-85d4-1078d2d2b808','6824dad2-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('689715f9-6b97-11ea-85d4-1078d2d2b808','689115e9-6b97-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('68cdbee7-6b97-11ea-85d4-1078d2d2b808','68be745f-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('68d340e7-6b97-11ea-85d4-1078d2d2b808','68be745f-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('68d761f5-6b97-11ea-85d4-1078d2d2b808','68be745f-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('68d8d0f1-6b97-11ea-85d4-1078d2d2b808','68be745f-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('68da6135-6b97-11ea-85d4-1078d2d2b808','68be745f-6b97-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('68f01f19-6b97-11ea-85d4-1078d2d2b808','68be745f-6b97-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('693a2324-6b97-11ea-85d4-1078d2d2b808','692338f4-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('693d866f-6c4d-11ea-85d4-1078d2d2b808','69378c6d-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('69420e90-6b97-11ea-85d4-1078d2d2b808','692338f4-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('69476d83-6b97-11ea-85d4-1078d2d2b808','692338f4-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6954ce1d-6b97-11ea-85d4-1078d2d2b808','692338f4-6b97-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('69564a9e-6b97-11ea-85d4-1078d2d2b808','692338f4-6b97-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('6959260c-6b97-11ea-85d4-1078d2d2b808','692338f4-6b97-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('69923270-6b97-11ea-85d4-1078d2d2b808','6978e52e-6b97-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('69cfd81b-6b97-11ea-85d4-1078d2d2b808','69bbfba7-6b97-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('69df4d35-6b97-11ea-85d4-1078d2d2b808','69bbfba7-6b97-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('6a02bd97-6b97-11ea-85d4-1078d2d2b808','69f61e68-6b97-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6b7b4381-6b96-11ea-85d4-1078d2d2b808','6b773531-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6b84d66a-6b96-11ea-85d4-1078d2d2b808','6b80cd5c-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('6b867375-6b96-11ea-85d4-1078d2d2b808','6b80cd5c-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6b880394-6b96-11ea-85d4-1078d2d2b808','6b80cd5c-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('6b93bc31-6b96-11ea-85d4-1078d2d2b808','6b8faffe-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('6b95607d-6b96-11ea-85d4-1078d2d2b808','6b8faffe-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('6b96d86c-6b96-11ea-85d4-1078d2d2b808','6b8faffe-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('6ba26751-6b96-11ea-85d4-1078d2d2b808','6b9e3ca9-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6bac25f8-6b96-11ea-85d4-1078d2d2b808','6ba85daf-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6bad9f0a-6b96-11ea-85d4-1078d2d2b808','6ba85daf-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('6baf024c-6b96-11ea-85d4-1078d2d2b808','6ba85daf-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6bb05e38-6b96-11ea-85d4-1078d2d2b808','6ba85daf-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('6bbd3b2c-6b96-11ea-85d4-1078d2d2b808','6bb8a983-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('6bc746e7-6b96-11ea-85d4-1078d2d2b808','6bc39616-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6bc8e4ac-6b96-11ea-85d4-1078d2d2b808','6bc39616-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('6bca0d03-6b96-11ea-85d4-1078d2d2b808','6bc39616-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6bcb4fac-6b96-11ea-85d4-1078d2d2b808','6bc39616-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('6bccba23-6b96-11ea-85d4-1078d2d2b808','6bc39616-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('6bce1ffe-6b96-11ea-85d4-1078d2d2b808','6bc39616-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('6bdba74b-6b96-11ea-85d4-1078d2d2b808','6bd7d161-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6bdd5e37-6b96-11ea-85d4-1078d2d2b808','6bd7d161-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('6bde964d-6b96-11ea-85d4-1078d2d2b808','6bd7d161-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('6be014b9-6b96-11ea-85d4-1078d2d2b808','6bd7d161-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('6be17b60-6b96-11ea-85d4-1078d2d2b808','6bd7d161-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('6be2c1d5-6b96-11ea-85d4-1078d2d2b808','6bd7d161-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('6bf06bfa-6b96-11ea-85d4-1078d2d2b808','6bec55e1-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('6bfa702a-6b96-11ea-85d4-1078d2d2b808','6bf65583-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('6bfc0ebd-6b96-11ea-85d4-1078d2d2b808','6bf65583-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('6c06a0d1-6b96-11ea-85d4-1078d2d2b808','6c027557-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('6d1e7c37-6a7c-11ea-85d4-1078d2d2b808','233f4d1d-6a7c-11ea-85d4-1078d2d2b808','679fb757-6a7c-11ea-85d4-1078d2d2b808',4,227,0.0630,0.00,0.00,0,1,'',1,'6d2561eb-6a7c-11ea-85d4-1078d2d2b808'),('6e8cabfb-682c-11ea-85d4-1078d2d2b808','6e8809aa-682c-11ea-85d4-1078d2d2b808',NULL,2,37,0.5000,0.00,0.00,0,1,'',1,'6e927fca-682c-11ea-85d4-1078d2d2b808'),('6fdb8372-6c4d-11ea-85d4-1078d2d2b808','6fd6950e-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('70d8443f-6b96-11ea-85d4-1078d2d2b808','70d3cf5a-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,5.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('70e51968-6b96-11ea-85d4-1078d2d2b808','70e08b8c-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,10.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('70e6fbff-6b96-11ea-85d4-1078d2d2b808','70e08b8c-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('70e85ea4-6b96-11ea-85d4-1078d2d2b808','70e08b8c-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,2.0000,860.00,0.00,0,1,'0',3,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('70fb8a16-6b96-11ea-85d4-1078d2d2b808','70f65202-6b96-11ea-85d4-1078d2d2b808',NULL,6,6,20.0000,60.00,0.00,0,1,'0',1,'2df1cadf-6b96-11ea-85d4-1078d2d2b808'),('70fdf009-6b96-11ea-85d4-1078d2d2b808','70f65202-6b96-11ea-85d4-1078d2d2b808',NULL,6,43,520.0000,430.00,0.00,0,1,'0',2,'2df3a3b5-6b96-11ea-85d4-1078d2d2b808'),('70ff9929-6b96-11ea-85d4-1078d2d2b808','70f65202-6b96-11ea-85d4-1078d2d2b808',NULL,6,196,6.0000,1960.00,0.00,0,1,'0',3,'2df5761e-6b96-11ea-85d4-1078d2d2b808'),('710c332d-6b96-11ea-85d4-1078d2d2b808','71081cd8-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,2.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('71174949-6b96-11ea-85d4-1078d2d2b808','7112dda6-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('71191bba-6b96-11ea-85d4-1078d2d2b808','7112dda6-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,25.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('711adc9f-6b96-11ea-85d4-1078d2d2b808','7112dda6-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('711c6f6a-6b96-11ea-85d4-1078d2d2b808','7112dda6-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,10.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('712a1364-6b96-11ea-85d4-1078d2d2b808','7125d3fc-6b96-11ea-85d4-1078d2d2b808',NULL,6,4,145.0000,40.00,0.00,0,1,'0',1,'2e246460-6b96-11ea-85d4-1078d2d2b808'),('71345a58-6b96-11ea-85d4-1078d2d2b808','71304f4e-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('7135e597-6b96-11ea-85d4-1078d2d2b808','71304f4e-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('71374a03-6b96-11ea-85d4-1078d2d2b808','71304f4e-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('7138ab8a-6b96-11ea-85d4-1078d2d2b808','71304f4e-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('7139e89c-6b96-11ea-85d4-1078d2d2b808','71304f4e-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('713b5bc9-6b96-11ea-85d4-1078d2d2b808','71304f4e-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('714a5dfe-6b96-11ea-85d4-1078d2d2b808','7145cd4b-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('714c51b2-6b96-11ea-85d4-1078d2d2b808','7145cd4b-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,1.0000,140.00,0.00,0,1,'0',2,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('714da98a-6b96-11ea-85d4-1078d2d2b808','7145cd4b-6b96-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',3,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('714f2008-6b96-11ea-85d4-1078d2d2b808','7145cd4b-6b96-11ea-85d4-1078d2d2b808',NULL,6,86,1.0000,860.00,0.00,0,1,'0',4,'2de1dde8-6b96-11ea-85d4-1078d2d2b808'),('7150cc0b-6b96-11ea-85d4-1078d2d2b808','7145cd4b-6b96-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',5,'2e3ac5b5-6b96-11ea-85d4-1078d2d2b808'),('71521b64-6b96-11ea-85d4-1078d2d2b808','7145cd4b-6b96-11ea-85d4-1078d2d2b808',NULL,6,253,1.0000,2530.00,0.00,0,1,'0',6,'2e3c7ae9-6b96-11ea-85d4-1078d2d2b808'),('71626b54-6b96-11ea-85d4-1078d2d2b808','715cfc77-6b96-11ea-85d4-1078d2d2b808',NULL,6,14,55.0000,140.00,0.00,0,1,'0',1,'2dddf2f6-6b96-11ea-85d4-1078d2d2b808'),('716dd7fc-6b96-11ea-85d4-1078d2d2b808','7169806d-6b96-11ea-85d4-1078d2d2b808',NULL,6,3,25.0000,30.00,0.00,0,1,'0',1,'2e82109e-6b96-11ea-85d4-1078d2d2b808'),('716fbf0f-6b96-11ea-85d4-1078d2d2b808','7169806d-6b96-11ea-85d4-1078d2d2b808',NULL,6,347,1.0000,3470.00,0.00,0,1,'0',2,'2e842420-6b96-11ea-85d4-1078d2d2b808'),('717abcf8-6b96-11ea-85d4-1078d2d2b808','7176b1d8-6b96-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'2dcecdbe-6b96-11ea-85d4-1078d2d2b808'),('71c4d43b-6c77-11ea-85d4-1078d2d2b808','71be449e-6c77-11ea-85d4-1078d2d2b808',NULL,6,14,2.0000,140.00,0.00,0,1,'0',1,'74504eca-6c15-11ea-85d4-1078d2d2b808'),('72f615b8-6c7d-11ea-85d4-1078d2d2b808','72ef96de-6c7d-11ea-85d4-1078d2d2b808',NULL,NULL,380,4.0000,3800.00,0.00,0,1,'0',1,'72fbee1c-6c7d-11ea-85d4-1078d2d2b808'),('74436b72-6c15-11ea-85d4-1078d2d2b808','743d42d6-6c15-11ea-85d4-1078d2d2b808',NULL,6,14,6.0000,140.00,0.00,0,1,'0',1,'74504eca-6c15-11ea-85d4-1078d2d2b808'),('7446a12c-6c15-11ea-85d4-1078d2d2b808','743d42d6-6c15-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',2,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('74493e68-6c15-11ea-85d4-1078d2d2b808','743d42d6-6c15-11ea-85d4-1078d2d2b808',NULL,6,380,1.0000,3800.00,0.00,0,1,'0',3,'74558ab8-6c15-11ea-85d4-1078d2d2b808'),('767b63c5-6c4d-11ea-85d4-1078d2d2b808','7674663a-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('7d1ac78a-6c4d-11ea-85d4-1078d2d2b808','7d14bb06-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('808f803a-6f26-11ea-85d4-1078d2d2b808','8088de2a-6f26-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'1e7a1bff-6e1f-11ea-85d4-1078d2d2b808'),('838593b4-6a7d-11ea-85d4-1078d2d2b808','740c0b24-6a7d-11ea-85d4-1078d2d2b808','740f18d5-6a7d-11ea-85d4-1078d2d2b808',2,358,1.0000,0.00,0.00,0,1,'',1,'838b90cd-6a7d-11ea-85d4-1078d2d2b808'),('83bbc76b-6c4d-11ea-85d4-1078d2d2b808','83b4786e-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('8530c0a5-6c5d-11ea-85d4-1078d2d2b808','852a752d-6c5d-11ea-85d4-1078d2d2b808',NULL,6,352,1.0000,3520.00,0.00,0,1,'0',1,'8537a2d3-6c5d-11ea-85d4-1078d2d2b808'),('853284ad-6c5d-11ea-85d4-1078d2d2b808','852a752d-6c5d-11ea-85d4-1078d2d2b808',NULL,6,47,5.0000,470.00,0.00,0,1,'0',2,'8539d10e-6c5d-11ea-85d4-1078d2d2b808'),('8533d0b4-6c5d-11ea-85d4-1078d2d2b808','852a752d-6c5d-11ea-85d4-1078d2d2b808',NULL,6,138,2.0000,1380.00,0.00,0,1,'0',3,'853b96d6-6c5d-11ea-85d4-1078d2d2b808'),('87c44ae1-6c6b-11ea-85d4-1078d2d2b808','87b88ed6-6c6b-11ea-85d4-1078d2d2b808',NULL,6,6,22.0000,60.00,0.00,0,1,'0',1,'87c9eb9b-6c6b-11ea-85d4-1078d2d2b808'),('8a5a5452-6c4d-11ea-85d4-1078d2d2b808','8a549c3b-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('8c1a6954-6c77-11ea-85d4-1078d2d2b808','8c146c98-6c77-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'dfbd1ad5-6c66-11ea-85d4-1078d2d2b808'),('8c92816d-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808','8c824630-66ad-11ea-85d4-1078d2d2b808',2,98,0.0050,0.00,0.00,0,1,'',1,'8ca47586-66ad-11ea-85d4-1078d2d2b808'),('8c942dd7-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808','8c824630-66ad-11ea-85d4-1078d2d2b808',2,66,0.0100,0.00,0.00,0,1,'',2,'8ca663a0-66ad-11ea-85d4-1078d2d2b808'),('8c95868d-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808','8c8621a4-66ad-11ea-85d4-1078d2d2b808',1,375,0.0250,0.00,0.00,0,1,'',3,'8c9b0af0-66ad-11ea-85d4-1078d2d2b808'),('8c96dde9-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808','8c887699-66ad-11ea-85d4-1078d2d2b808',1,151,1.0000,0.00,0.00,0,1,'',4,'8c9cd27a-66ad-11ea-85d4-1078d2d2b808'),('8c983354-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808','8c8ad39d-66ad-11ea-85d4-1078d2d2b808',1,183,1.0000,0.00,0.00,0,1,'',5,'8c9e84d3-66ad-11ea-85d4-1078d2d2b808'),('90fc0764-6c4d-11ea-85d4-1078d2d2b808','90f42e3d-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('910a5eb1-6c4d-11ea-85d4-1078d2d2b808','9104d5c3-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('97a95201-6c4d-11ea-85d4-1078d2d2b808','97a3ca17-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('999bfe89-6697-11ea-85d4-1078d2d2b808','998ee7fc-6697-11ea-85d4-1078d2d2b808','9992cb02-6697-11ea-85d4-1078d2d2b808',1,166,1.0000,0.00,0.00,0,1,'',1,'999ec7cc-6697-11ea-85d4-1078d2d2b808'),('9e47c7a1-6c4d-11ea-85d4-1078d2d2b808','9e419131-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('9f1f0f26-6697-11ea-85d4-1078d2d2b808','9f13c899-6697-11ea-85d4-1078d2d2b808','9f17a75e-6697-11ea-85d4-1078d2d2b808',1,155,1.0000,0.00,0.00,0,1,'',1,'9f21f315-6697-11ea-85d4-1078d2d2b808'),('a002f98e-6c5f-11ea-85d4-1078d2d2b808','9ffced3f-6c5f-11ea-85d4-1078d2d2b808',NULL,6,276,5.0000,2760.00,0.00,0,1,'0',1,'a00706d3-6c5f-11ea-85d4-1078d2d2b808'),('a318ec25-6c73-11ea-85d4-1078d2d2b808','a31237ba-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('a327cc4b-6c73-11ea-85d4-1078d2d2b808','a3220985-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('a334cb11-6c73-11ea-85d4-1078d2d2b808','a32f86a4-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('a51084af-6c4d-11ea-85d4-1078d2d2b808','a5096b56-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('a8ef5f6c-6c15-11ea-85d4-1078d2d2b808','a8ea6fb8-6c15-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('a9686072-6e68-11ea-85d4-1078d2d2b808','a963bf09-6e68-11ea-85d4-1078d2d2b808',NULL,6,294,1.0000,2940.00,0.00,0,1,'0',1,'1e7a1bff-6e1f-11ea-85d4-1078d2d2b808'),('a96a23f6-6e68-11ea-85d4-1078d2d2b808','a963bf09-6e68-11ea-85d4-1078d2d2b808',NULL,6,14,52.0000,140.00,0.00,0,1,'0',2,'d79c96a7-6e1f-11ea-85d4-1078d2d2b808'),('a96b85b7-6e68-11ea-85d4-1078d2d2b808','a963bf09-6e68-11ea-85d4-1078d2d2b808',NULL,6,313,1.6000,3130.00,0.00,0,1,'0',3,'a96f56c8-6e68-11ea-85d4-1078d2d2b808'),('a9a4dd73-6c7b-11ea-85d4-1078d2d2b808','a99f6ad0-6c7b-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'a9ad3f14-6c7b-11ea-85d4-1078d2d2b808'),('a9c6a71a-6c73-11ea-85d4-1078d2d2b808','a9c1a295-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('a9d322e5-6c73-11ea-85d4-1078d2d2b808','a9ce2211-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('a9e1a384-6c73-11ea-85d4-1078d2d2b808','a9db8764-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('abb04688-6c4d-11ea-85d4-1078d2d2b808','abab208b-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('ad05e770-6b97-11ea-85d4-1078d2d2b808','ad01c290-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('b086f166-6c73-11ea-85d4-1078d2d2b808','b073902e-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('b0a6eed0-6c73-11ea-85d4-1078d2d2b808','b0a032ab-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('b22eb1cc-6e6c-11ea-85d4-1078d2d2b808','b226ee2b-6e6c-11ea-85d4-1078d2d2b808',NULL,6,313,1.6000,3130.00,0.00,0,1,'0',1,'a96f56c8-6e68-11ea-85d4-1078d2d2b808'),('b24e94d6-6c4d-11ea-85d4-1078d2d2b808','b24a497e-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('b3848876-6a7e-11ea-85d4-1078d2d2b808','aa5ccdfd-6a7d-11ea-85d4-1078d2d2b808','af2ebc0e-6a7e-11ea-85d4-1078d2d2b808',4,227,0.0630,0.00,0.00,0,1,'',1,'b38a65eb-6a7e-11ea-85d4-1078d2d2b808'),('b6b9e5d7-66ac-11ea-85d4-1078d2d2b808','ae866ccf-66ac-11ea-85d4-1078d2d2b808','b6b0e9c8-66ac-11ea-85d4-1078d2d2b808',1,186,1.0000,0.00,0.00,0,1,'',1,'4f9d073a-668d-11ea-85d4-1078d2d2b808'),('b74bbdeb-6c73-11ea-85d4-1078d2d2b808','b741d57c-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('b7594de6-6c73-11ea-85d4-1078d2d2b808','b75444f6-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('b7646c5e-6c73-11ea-85d4-1078d2d2b808','b75ff8ea-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('b8eacb6a-6c4d-11ea-85d4-1078d2d2b808','b8e6d8ad-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('b93eb2ab-6a7f-11ea-85d4-1078d2d2b808','24bc9675-6a7f-11ea-85d4-1078d2d2b808','5c169678-6a7f-11ea-85d4-1078d2d2b808',4,227,0.0630,0.00,0.00,0,1,'',1,'b38a65eb-6a7e-11ea-85d4-1078d2d2b808'),('b972dc86-6ce2-11ea-85d4-1078d2d2b808','b9664664-6ce2-11ea-85d4-1078d2d2b808',NULL,6,290,5.0000,2900.00,0.00,0,1,'0',1,'b97b2b72-6ce2-11ea-85d4-1078d2d2b808'),('b9756127-6ce2-11ea-85d4-1078d2d2b808','b9664664-6ce2-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',2,'b97dbb3a-6ce2-11ea-85d4-1078d2d2b808'),('b98fdb93-6ce2-11ea-85d4-1078d2d2b808','b98989a3-6ce2-11ea-85d4-1078d2d2b808',NULL,6,290,5.0000,2900.00,0.00,0,1,'0',1,'b97b2b72-6ce2-11ea-85d4-1078d2d2b808'),('b9921937-6ce2-11ea-85d4-1078d2d2b808','b98989a3-6ce2-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',2,'b97dbb3a-6ce2-11ea-85d4-1078d2d2b808'),('b9a1aaa2-6ce2-11ea-85d4-1078d2d2b808','b99c1369-6ce2-11ea-85d4-1078d2d2b808',NULL,6,313,4.0000,3130.00,0.00,0,1,'0',1,'b9a655db-6ce2-11ea-85d4-1078d2d2b808'),('ba49d045-6b97-11ea-85d4-1078d2d2b808','ba448fbd-6b97-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'2de0050b-6b96-11ea-85d4-1078d2d2b808'),('bdfa74bc-6c73-11ea-85d4-1078d2d2b808','bdf46036-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('be05d07a-6c73-11ea-85d4-1078d2d2b808','be010018-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('be11000a-6c73-11ea-85d4-1078d2d2b808','be0c4f03-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('bf85f9c6-6c4d-11ea-85d4-1078d2d2b808','bf820499-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('c361f5fe-6c49-11ea-85d4-1078d2d2b808','ba32318a-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('c4a3e8fd-6c73-11ea-85d4-1078d2d2b808','c49ec824-6c73-11ea-85d4-1078d2d2b808',NULL,6,130,2.0000,1300.00,0.00,0,1,'0',1,'056d6c5f-6c73-11ea-85d4-1078d2d2b808'),('c516a6f1-6f2d-11ea-85d4-1078d2d2b808','c5113737-6f2d-11ea-85d4-1078d2d2b808',NULL,6,14,15.0000,140.00,0.00,0,1,'0',1,'c51ea5a7-6f2d-11ea-85d4-1078d2d2b808'),('c62c0837-6c4d-11ea-85d4-1078d2d2b808','c6279bf9-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('c6709589-6c49-11ea-85d4-1078d2d2b808','c3684ba9-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('c8fa5b26-6696-11ea-85d4-1078d2d2b808','c8eb69d5-6696-11ea-85d4-1078d2d2b808','c8f05b07-6696-11ea-85d4-1078d2d2b808',1,186,1.0000,0.00,0.00,0,1,'',1,'4f9d073a-668d-11ea-85d4-1078d2d2b808'),('cc73cc11-6c49-11ea-85d4-1078d2d2b808','cc6fd770-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('cd7698a4-682b-11ea-85d4-1078d2d2b808','cd6a3090-682b-11ea-85d4-1078d2d2b808',NULL,2,37,0.2000,0.00,0.00,77,1,'',1,'cd7c2879-682b-11ea-85d4-1078d2d2b808'),('d042e71e-6c4d-11ea-85d4-1078d2d2b808','d03db85d-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('d04b55e9-6c57-11ea-85d4-1078d2d2b808','d046e0de-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,86,5.0000,860.00,0.00,0,1,'0',1,'d053feb4-6c57-11ea-85d4-1078d2d2b808'),('d04d60ff-6c57-11ea-85d4-1078d2d2b808','d046e0de-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,380,1.0000,3800.00,0.00,0,1,'0',2,'d056df5d-6c57-11ea-85d4-1078d2d2b808'),('d04ee2fe-6c57-11ea-85d4-1078d2d2b808','d046e0de-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,253,2.0000,2530.00,0.00,0,1,'0',3,'d058b598-6c57-11ea-85d4-1078d2d2b808'),('d050b668-6c57-11ea-85d4-1078d2d2b808','d046e0de-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,6,35.0000,60.00,0.00,0,1,'0',4,'d05ad454-6c57-11ea-85d4-1078d2d2b808'),('d0510023-6c4d-11ea-85d4-1078d2d2b808','d049634c-6c4d-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('d067a690-6c57-11ea-85d4-1078d2d2b808','d063e7bc-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,86,5.0000,860.00,0.00,0,1,'0',1,'d053feb4-6c57-11ea-85d4-1078d2d2b808'),('d069731e-6c57-11ea-85d4-1078d2d2b808','d063e7bc-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,380,1.0000,3800.00,0.00,0,1,'0',2,'d056df5d-6c57-11ea-85d4-1078d2d2b808'),('d06ac58a-6c57-11ea-85d4-1078d2d2b808','d063e7bc-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,253,2.0000,2530.00,0.00,0,1,'0',3,'d058b598-6c57-11ea-85d4-1078d2d2b808'),('d06c3688-6c57-11ea-85d4-1078d2d2b808','d063e7bc-6c57-11ea-85d4-1078d2d2b808',NULL,NULL,6,35.0000,60.00,0.00,0,1,'0',4,'d05ad454-6c57-11ea-85d4-1078d2d2b808'),('d255ba3a-6696-11ea-85d4-1078d2d2b808','d24477c1-6696-11ea-85d4-1078d2d2b808','d2484645-6696-11ea-85d4-1078d2d2b808',1,186,1.0000,0.00,0.00,0,1,'',1,'4f9d073a-668d-11ea-85d4-1078d2d2b808'),('d7975691-6e1f-11ea-85d4-1078d2d2b808','d78ee163-6e1f-11ea-85d4-1078d2d2b808',NULL,6,14,5.0000,140.00,0.00,0,1,'0',1,'d79c96a7-6e1f-11ea-85d4-1078d2d2b808'),('d883d22b-6c73-11ea-85d4-1078d2d2b808','d87f7450-6c73-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('daa80067-6c74-11ea-85d4-1078d2d2b808','daa1a167-6c74-11ea-85d4-1078d2d2b808',NULL,6,67,3.0000,670.00,0.00,0,1,'0',1,'daac2c09-6c74-11ea-85d4-1078d2d2b808'),('df1989a2-6c73-11ea-85d4-1078d2d2b808','df14fd9f-6c73-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('dfb85acf-6c66-11ea-85d4-1078d2d2b808','dfb11035-6c66-11ea-85d4-1078d2d2b808',NULL,6,294,2.0000,2940.00,0.00,0,1,'0',1,'dfbd1ad5-6c66-11ea-85d4-1078d2d2b808'),('e130bc26-6c77-11ea-85d4-1078d2d2b808','e12be07b-6c77-11ea-85d4-1078d2d2b808',NULL,6,6,3.0000,60.00,0.00,0,1,'0',1,'87c9eb9b-6c6b-11ea-85d4-1078d2d2b808'),('e5be41dd-6c49-11ea-85d4-1078d2d2b808','e5b9a241-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('e5be77be-6c73-11ea-85d4-1078d2d2b808','e5b869be-6c73-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('e5cccd02-6c73-11ea-85d4-1078d2d2b808','e5c6af23-6c73-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('e63e61dc-6c49-11ea-85d4-1078d2d2b808','e6396ee4-6c49-11ea-85d4-1078d2d2b808',NULL,NULL,294,1.0000,2940.00,0.00,0,1,'0',1,'3a5a539c-6c49-11ea-85d4-1078d2d2b808'),('ec60bd9e-6c73-11ea-85d4-1078d2d2b808','ec5a65f8-6c73-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('ec6ce7c7-6c73-11ea-85d4-1078d2d2b808','ec6801d1-6c73-11ea-85d4-1078d2d2b808',NULL,6,313,1.0000,3130.00,0.00,0,1,'0',1,'74537afc-6c15-11ea-85d4-1078d2d2b808'),('f06b51d4-6e6c-11ea-85d4-1078d2d2b808','f061d94e-6e6c-11ea-85d4-1078d2d2b808',NULL,6,380,2.9000,3800.00,0.00,0,1,'0',1,'f0703196-6e6c-11ea-85d4-1078d2d2b808'),('f2a9b1ff-682b-11ea-85d4-1078d2d2b808','f2a54324-682b-11ea-85d4-1078d2d2b808',NULL,2,37,0.2000,0.00,0.00,0,1,'',1,'cd7c2879-682b-11ea-85d4-1078d2d2b808'),('f9d653e7-6e6b-11ea-85d4-1078d2d2b808','f9cebbac-6e6b-11ea-85d4-1078d2d2b808',NULL,6,4,25.0000,40.00,0.00,0,1,'0',1,'f9dc00c9-6e6b-11ea-85d4-1078d2d2b808'),('fb140356-6c5c-11ea-85d4-1078d2d2b808','fb0afa80-6c5c-11ea-85d4-1078d2d2b808',NULL,6,14,52.0000,140.00,0.00,0,1,'0',1,'74504eca-6c15-11ea-85d4-1078d2d2b808'),('fb874a9b-6c77-11ea-85d4-1078d2d2b808','fb8212b9-6c77-11ea-85d4-1078d2d2b808',NULL,6,6,3.0000,60.00,0.00,0,1,'0',1,'87c9eb9b-6c6b-11ea-85d4-1078d2d2b808'),('fb944be2-6c77-11ea-85d4-1078d2d2b808','fb8fc826-6c77-11ea-85d4-1078d2d2b808',NULL,6,6,3.0000,60.00,0.00,0,1,'0',1,'87c9eb9b-6c6b-11ea-85d4-1078d2d2b808'),('fba43576-6c77-11ea-85d4-1078d2d2b808','fb9f0ca2-6c77-11ea-85d4-1078d2d2b808',NULL,6,6,3.0000,60.00,0.00,0,1,'0',1,'87c9eb9b-6c6b-11ea-85d4-1078d2d2b808'),('fc0f2623-6c5f-11ea-85d4-1078d2d2b808','fc081db9-6c5f-11ea-85d4-1078d2d2b808',NULL,6,371,6.0000,3710.00,0.00,0,1,'0',1,'fc13185a-6c5f-11ea-85d4-1078d2d2b808');
/*!40000 ALTER TABLE `o_goods` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_header`
--

DROP TABLE IF EXISTS `o_header`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_header` (
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
  `f_staff` int(11) DEFAULT '0',
  `f_comment` varchar(512) DEFAULT NULL,
  `f_print` int(11) DEFAULT '0',
  `f_amountTotal` decimal(14,2) DEFAULT NULL,
  `f_amountCash` decimal(14,2) DEFAULT '0.00',
  `f_amountCard` decimal(14,2) DEFAULT '0.00',
  `f_amountBank` decimal(14,2) DEFAULT '0.00',
  `f_amountOther` decimal(14,2) DEFAULT '0.00',
  `f_serviceMode` smallint(6) DEFAULT NULL,
  `f_amountService` decimal(14,2) DEFAULT NULL,
  `f_amountDiscount` decimal(14,2) DEFAULT NULL,
  `f_serviceFactor` decimal(4,2) DEFAULT NULL,
  `f_discountFactor` decimal(4,2) DEFAULT NULL,
  `f_creditCardId` int(11) DEFAULT NULL,
  `f_otherId` int(11) DEFAULT NULL,
  `f_shift` int(11) DEFAULT NULL,
  `f_source` smallint(6) DEFAULT '1',
  `f_cashdoc` char(36) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_header`
--

LOCK TABLES `o_header` WRITE;
/*!40000 ALTER TABLE `o_header` DISABLE KEYS */;
INSERT INTO `o_header` VALUES ('009fee0f-6c4e-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','19:01:22','19:01:22',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('02bbf60c-6697-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','12:28:56','12:28:56',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('05724423-6c73-11ea-85d4-1078d2d2b808',182,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('05c618ef-6e22-11ea-85d4-1078d2d2b808',226,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','02:16:48','02:16:48',4,'',1,280.00,0.00,0.00,280.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('0632d916-6b97-11ea-85d4-1078d2d2b808',142,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:34','21:11:34',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('066a3453-6b97-11ea-85d4-1078d2d2b808',143,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:34','21:11:34',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('06df2f69-6b97-11ea-85d4-1078d2d2b808',144,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:35','21:11:35',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('072a0540-6b97-11ea-85d4-1078d2d2b808',145,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:35','21:11:35',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('076abe91-6b97-11ea-85d4-1078d2d2b808',146,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:36','21:11:36',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('07f804eb-6b97-11ea-85d4-1078d2d2b808',147,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:37','21:11:37',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('0841ccae-6b97-11ea-85d4-1078d2d2b808',148,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:37','21:11:37',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('08d0bd4d-6b97-11ea-85d4-1078d2d2b808',149,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:38','21:11:38',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('0941758a-6b97-11ea-85d4-1078d2d2b808',150,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:39','21:11:39',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('0983d660-6b97-11ea-85d4-1078d2d2b808',151,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:39','21:11:39',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('09c93cc9-6b97-11ea-85d4-1078d2d2b808',152,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:11:40','21:11:40',4,'',1,5880.00,0.00,0.00,0.00,5880.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('0abe2d78-668d-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','11:17:34','11:17:34',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('0d76055c-6a79-11ea-85d4-1078d2d2b808',9,'B',4,1,1,'2020-03-20',NULL,NULL,'11:04:12',NULL,1,'',0,0.00,0.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,NULL,1,NULL),('0ef36296-6c4a-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:33:08','18:33:08',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('14201146-668d-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','11:17:50','11:17:50',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('1e6be1a8-6e1f-11ea-85d4-1078d2d2b808',224,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','02:16:48','02:16:48',4,'',1,23520.00,0.00,0.00,23520.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('1e84d0da-6698-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','14:47:10','14:47:10',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('1f067601-6a80-11ea-85d4-1078d2d2b808',17,'B',2,1,1,'2020-03-20','2020-03-20','2020-03-20','11:55:00','11:55:22',1,'',1,200.00,200.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('1ff2cd86-6c7c-11ea-85d4-1078d2d2b808',216,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','00:26:37','00:26:37',4,'',1,280.00,280.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('233f4d1d-6a7c-11ea-85d4-1078d2d2b808',10,'B',2,1,1,'2020-03-20','2020-03-20','2020-02-05','11:26:26','11:28:39',1,'',1,30.00,30.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('23f515e4-6c66-11ea-85d4-1078d2d2b808',175,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','21:54:09','21:54:09',4,'',1,7600.00,0.00,0.00,0.00,7600.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('24b4dc82-6c68-11ea-85d4-1078d2d2b808',179,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:00:07','22:00:07',4,'',1,3560.00,3560.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('24bc9675-6a7f-11ea-85d4-1078d2d2b808',15,'B',2,1,1,'2020-03-20','2020-03-20','2020-03-20','11:48:01','11:52:15',1,'',1,30.00,30.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('25a9afde-6c66-11ea-85d4-1078d2d2b808',176,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','21:54:12','21:54:12',4,'',1,7600.00,0.00,0.00,0.00,7600.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('25b5dcc8-6c66-11ea-85d4-1078d2d2b808',177,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','21:54:12','21:54:12',4,'',1,7600.00,0.00,0.00,0.00,7600.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('26415a71-6ce1-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-23','2020-03-23','2020-03-23','12:02:03','12:02:03',1,'',1,2580.00,0.00,0.00,2580.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('26569800-6ce1-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-23','2020-03-23','2020-03-23','12:02:03','12:02:03',1,'',1,2100.00,2100.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2733dc04-6697-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','12:29:57','12:29:57',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('274ac699-6c16-11ea-85d4-1078d2d2b808',168,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','12:21:35','12:21:35',4,'',1,9650.00,0.00,0.00,0.00,9650.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('275d9af9-6c16-11ea-85d4-1078d2d2b808',169,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','12:21:35','12:21:35',4,'',1,9650.00,0.00,0.00,0.00,9650.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2a25b520-6a77-11ea-85d4-1078d2d2b808',7,'B',4,1,1,'2020-03-20',NULL,NULL,'10:50:54',NULL,1,'',0,0.00,0.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,NULL,1,NULL),('2dc2c62b-6b96-11ea-85d4-1078d2d2b808',18,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2dd3752c-6b96-11ea-85d4-1078d2d2b808',19,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2de66a89-6b96-11ea-85d4-1078d2d2b808',20,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2dfa94c8-6b96-11ea-85d4-1078d2d2b808',21,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2e051c57-6b96-11ea-85d4-1078d2d2b808',22,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2e1adaf3-6b96-11ea-85d4-1078d2d2b808',23,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2e28f84e-6b96-11ea-85d4-1078d2d2b808',24,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2e43f0f1-6b96-11ea-85d4-1078d2d2b808',25,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:31','21:05:31',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2e6747da-6b96-11ea-85d4-1078d2d2b808',26,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:32','21:05:32',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('2e76b17f-6b96-11ea-85d4-1078d2d2b808',27,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:32','21:05:32',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('302ea57b-668c-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','11:11:28','11:11:28',1,'',1,580.00,580.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('35acbbe0-6a78-11ea-85d4-1078d2d2b808',8,'B',4,1,1,'2020-03-20',NULL,NULL,'10:58:27',NULL,1,'',0,0.00,0.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,NULL,1,NULL),('35b35f65-6b96-11ea-85d4-1078d2d2b808',28,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:44','21:05:44',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('35bde553-6b96-11ea-85d4-1078d2d2b808',29,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:44','21:05:44',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('35d35566-6b96-11ea-85d4-1078d2d2b808',30,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:44','21:05:44',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('35e349db-6b96-11ea-85d4-1078d2d2b808',31,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:44','21:05:44',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('35ee60a0-6b96-11ea-85d4-1078d2d2b808',32,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:44','21:05:44',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('360437c7-6b96-11ea-85d4-1078d2d2b808',33,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:44','21:05:44',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3607020e-682c-11ea-85d4-1078d2d2b808',4,'B',2,1,1,'2020-03-17','2020-03-17','2020-03-17','12:48:46','12:49:27',1,'',1,0.00,0.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('361d6981-6b96-11ea-85d4-1078d2d2b808',34,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:45','21:05:45',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('363532e4-6b96-11ea-85d4-1078d2d2b808',35,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:45','21:05:45',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('364bfd5b-6b96-11ea-85d4-1078d2d2b808',36,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:45','21:05:45',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3656741a-6b96-11ea-85d4-1078d2d2b808',37,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:45','21:05:45',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('399f8340-6c58-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','20:14:32','20:14:32',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3a47f05f-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:27:11','18:27:11',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3a8e6c21-6f2d-11ea-85d4-1078d2d2b808',234,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','11:46:36','11:46:36',4,'',1,4090.00,4090.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b2e02f1-6b96-11ea-85d4-1078d2d2b808',38,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:53','21:05:53',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b34db63-6c74-11ea-85d4-1078d2d2b808',204,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b384299-6b96-11ea-85d4-1078d2d2b808',39,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:53','21:05:53',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b4145ae-6c74-11ea-85d4-1078d2d2b808',205,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b46108c-6b96-11ea-85d4-1078d2d2b808',40,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:53','21:05:53',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b560346-6b96-11ea-85d4-1078d2d2b808',41,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:53','21:05:53',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b613c1c-6b96-11ea-85d4-1078d2d2b808',42,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:53','21:05:53',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b71cf95-6b96-11ea-85d4-1078d2d2b808',43,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:53','21:05:53',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b7c0b1b-6b96-11ea-85d4-1078d2d2b808',44,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:54','21:05:54',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3b917999-6b96-11ea-85d4-1078d2d2b808',45,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:54','21:05:54',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3ba69824-6b96-11ea-85d4-1078d2d2b808',46,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:54','21:05:54',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3bb16e67-6b96-11ea-85d4-1078d2d2b808',47,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:05:54','21:05:54',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3c15fc26-6e1d-11ea-85d4-1078d2d2b808',220,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','12:45:27','12:45:27',4,'',1,18780.00,0.00,0.00,0.00,18780.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3c334ac5-6e1d-11ea-85d4-1078d2d2b808',221,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','12:45:27','12:45:27',4,'',1,4300.00,0.00,0.00,4300.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3c47fd69-6e1d-11ea-85d4-1078d2d2b808',222,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','12:45:27','12:45:27',4,'',1,2530.00,2530.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3c5b759f-6e1d-11ea-85d4-1078d2d2b808',223,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','12:45:27','12:45:27',4,'',1,3130.00,3130.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3e598cd0-668c-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','11:11:51','11:11:51',1,'',1,4300.00,4300.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('3fa55461-6c4b-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:41:39','18:41:39',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('3fb73769-6c4b-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:41:39','18:41:39',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40874467-6b96-11ea-85d4-1078d2d2b808',48,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:02','21:06:02',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40918da5-6b96-11ea-85d4-1078d2d2b808',49,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:02','21:06:02',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40a17b74-6b96-11ea-85d4-1078d2d2b808',50,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:02','21:06:02',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40b057c2-6b96-11ea-85d4-1078d2d2b808',51,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:02','21:06:02',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40ba716a-6b96-11ea-85d4-1078d2d2b808',52,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:02','21:06:02',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40cb1e04-6b96-11ea-85d4-1078d2d2b808',53,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:02','21:06:02',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40d53da2-6b96-11ea-85d4-1078d2d2b808',54,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:03','21:06:03',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40ea1c33-6b96-11ea-85d4-1078d2d2b808',55,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:03','21:06:03',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40f99cde-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:27:22','18:27:22',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('40ffeb6c-6b96-11ea-85d4-1078d2d2b808',56,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:03','21:06:03',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4109fcd5-6b96-11ea-85d4-1078d2d2b808',57,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:03','21:06:03',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('41d70665-6c74-11ea-85d4-1078d2d2b808',206,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('43e8104a-6c5f-11ea-85d4-1078d2d2b808',172,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','21:04:56','21:04:56',4,'',1,39610.00,0.00,0.00,39610.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('45dda76f-6b96-11ea-85d4-1078d2d2b808',58,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:11','21:06:11',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('45e72908-6b96-11ea-85d4-1078d2d2b808',59,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:11','21:06:11',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('45f509d6-6b96-11ea-85d4-1078d2d2b808',60,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:11','21:06:11',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4602be1b-6b96-11ea-85d4-1078d2d2b808',61,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:11','21:06:11',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('460c4eed-6b96-11ea-85d4-1078d2d2b808',62,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:11','21:06:11',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('461d02ba-6b96-11ea-85d4-1078d2d2b808',63,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:11','21:06:11',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('46271393-6b96-11ea-85d4-1078d2d2b808',64,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:11','21:06:11',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('463c919f-6b96-11ea-85d4-1078d2d2b808',65,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:12','21:06:12',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4651e3f8-6b96-11ea-85d4-1078d2d2b808',66,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:12','21:06:12',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('465c5594-6b96-11ea-85d4-1078d2d2b808',67,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:12','21:06:12',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4772e5c9-6ea1-11ea-85d4-1078d2d2b808',231,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','11:46:36','11:46:36',4,'',1,8150.00,8150.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('47988092-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:27:33','18:27:33',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4d9369f0-6b96-11ea-85d4-1078d2d2b808',68,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:24','21:06:24',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4d9e2e02-6b96-11ea-85d4-1078d2d2b808',69,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:24','21:06:24',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4dad20b1-6b96-11ea-85d4-1078d2d2b808',70,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:24','21:06:24',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4dbcd506-6b96-11ea-85d4-1078d2d2b808',71,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:24','21:06:24',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4dcf752b-6b96-11ea-85d4-1078d2d2b808',72,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:24','21:06:24',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4de2286b-6b96-11ea-85d4-1078d2d2b808',73,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:24','21:06:24',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4deb8de5-6b96-11ea-85d4-1078d2d2b808',74,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:24','21:06:24',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4e00d4ac-6b96-11ea-85d4-1078d2d2b808',75,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:25','21:06:25',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4e1c3ad9-6b96-11ea-85d4-1078d2d2b808',76,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:25','21:06:25',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4e259ce3-6b96-11ea-85d4-1078d2d2b808',77,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:25','21:06:25',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4e38e569-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:27:44','18:27:44',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4f12cbed-6c74-11ea-85d4-1078d2d2b808',207,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,920.00,0.00,0.00,920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('4f4917b4-682c-11ea-85d4-1078d2d2b808',5,'B',2,1,1,'2020-03-17','2020-03-17','2020-03-17','12:49:27','12:50:09',1,'',1,60.00,60.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,'4f578133-682c-11ea-85d4-1078d2d2b808'),('4f87326a-668d-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','11:19:30','11:19:30',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('50207e94-6ecf-11ea-85d4-1078d2d2b808',232,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','11:46:36','11:46:36',4,'',1,4410.00,4410.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('52f856b1-6b96-11ea-85d4-1078d2d2b808',78,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:33','21:06:33',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5303113e-6b96-11ea-85d4-1078d2d2b808',79,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:33','21:06:33',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5313f02f-6b96-11ea-85d4-1078d2d2b808',80,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:33','21:06:33',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5322c6d3-6b96-11ea-85d4-1078d2d2b808',81,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:33','21:06:33',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('532ce872-6b96-11ea-85d4-1078d2d2b808',82,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:33','21:06:33',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('537b8d53-6b96-11ea-85d4-1078d2d2b808',83,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:34','21:06:34',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('538a90b9-6b96-11ea-85d4-1078d2d2b808',84,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:34','21:06:34',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('539ec0a5-6b96-11ea-85d4-1078d2d2b808',85,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:34','21:06:34',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('53b3c087-6b96-11ea-85d4-1078d2d2b808',86,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:34','21:06:34',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('53bde073-6b96-11ea-85d4-1078d2d2b808',87,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:34','21:06:34',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('54da0beb-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:27:56','18:27:56',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('58912099-6b96-11ea-85d4-1078d2d2b808',88,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:42','21:06:42',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('58c37983-6b96-11ea-85d4-1078d2d2b808',89,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:43','21:06:43',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('58dafb0b-6b96-11ea-85d4-1078d2d2b808',90,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:43','21:06:43',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('58fd95a6-6b96-11ea-85d4-1078d2d2b808',91,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:43','21:06:43',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('591efb9e-6b96-11ea-85d4-1078d2d2b808',92,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:43','21:06:43',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('59327acc-6b96-11ea-85d4-1078d2d2b808',93,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:43','21:06:43',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('593d28aa-6b96-11ea-85d4-1078d2d2b808',94,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:43','21:06:43',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5952667d-6b96-11ea-85d4-1078d2d2b808',95,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:44','21:06:44',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('596c04f8-6b96-11ea-85d4-1078d2d2b808',96,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:44','21:06:44',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('59766e96-6b96-11ea-85d4-1078d2d2b808',97,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:44','21:06:44',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5b79c05e-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:28:07','18:28:07',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5e4bbcfd-6b96-11ea-85d4-1078d2d2b808',98,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:52','21:06:52',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5e566045-6b96-11ea-85d4-1078d2d2b808',99,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:52','21:06:52',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5e64a7a2-6b96-11ea-85d4-1078d2d2b808',100,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:52','21:06:52',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5e73448f-6b96-11ea-85d4-1078d2d2b808',101,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:52','21:06:52',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5e7d505d-6b96-11ea-85d4-1078d2d2b808',102,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:52','21:06:52',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5e8e021f-6b96-11ea-85d4-1078d2d2b808',103,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:52','21:06:52',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5e987660-6b96-11ea-85d4-1078d2d2b808',104,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:52','21:06:52',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5ead81f7-6b96-11ea-85d4-1078d2d2b808',105,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:53','21:06:53',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5ec2d47e-6b96-11ea-85d4-1078d2d2b808',106,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:53','21:06:53',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5ed338fb-6b96-11ea-85d4-1078d2d2b808',107,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:53','21:06:53',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5ee0a8a9-6b96-11ea-85d4-1078d2d2b808',108,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:06:53','21:06:53',4,'',1,5880.00,0.00,0.00,0.00,5880.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('5faff9dc-668c-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','11:12:47','11:12:47',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('621a50df-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:28:18','18:28:18',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6282f3f2-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:56:56','18:56:56',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('63b1fc71-6b96-11ea-85d4-1078d2d2b808',109,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:01','21:07:01',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('63bb3ad3-6b96-11ea-85d4-1078d2d2b808',110,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:01','21:07:01',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('63c922ef-6b96-11ea-85d4-1078d2d2b808',111,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:01','21:07:01',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('63d723ee-6b96-11ea-85d4-1078d2d2b808',112,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:01','21:07:01',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('63e1414f-6b96-11ea-85d4-1078d2d2b808',113,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:01','21:07:01',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('63f36200-6b96-11ea-85d4-1078d2d2b808',114,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:01','21:07:01',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('63fd7054-6b96-11ea-85d4-1078d2d2b808',115,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:01','21:07:01',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('64121980-6b96-11ea-85d4-1078d2d2b808',116,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:02','21:07:02',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6426953a-6b96-11ea-85d4-1078d2d2b808',117,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:02','21:07:02',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6430e392-6b96-11ea-85d4-1078d2d2b808',118,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:02','21:07:02',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6440a223-6b96-11ea-85d4-1078d2d2b808',119,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:02','21:07:02',4,'',1,5880.00,0.00,0.00,0.00,5880.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('64a0581a-6ce4-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-23','2020-03-23','2020-03-23','12:57:53','12:57:53',1,'',1,420.00,420.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('66febafb-6b97-11ea-85d4-1078d2d2b808',153,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:16','21:14:16',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('673447fa-6b97-11ea-85d4-1078d2d2b808',154,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:16','21:14:16',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('677e1b89-6b97-11ea-85d4-1078d2d2b808',155,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:17','21:14:17',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('67ecabbd-6b97-11ea-85d4-1078d2d2b808',156,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:18','21:14:18',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6824dad2-6b97-11ea-85d4-1078d2d2b808',157,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:18','21:14:18',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('689115e9-6b97-11ea-85d4-1078d2d2b808',158,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:19','21:14:19',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('68be745f-6b97-11ea-85d4-1078d2d2b808',159,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:19','21:14:19',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('692338f4-6b97-11ea-85d4-1078d2d2b808',160,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:20','21:14:20',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('69378c6d-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:57:08','18:57:08',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6978e52e-6b97-11ea-85d4-1078d2d2b808',161,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:20','21:14:20',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('69bbfba7-6b97-11ea-85d4-1078d2d2b808',162,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:21','21:14:21',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('69f61e68-6b97-11ea-85d4-1078d2d2b808',163,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:14:21','21:14:21',4,'',1,5880.00,0.00,0.00,0.00,5880.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6b773531-6b96-11ea-85d4-1078d2d2b808',120,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:14','21:07:14',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6b80cd5c-6b96-11ea-85d4-1078d2d2b808',121,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:14','21:07:14',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6b8faffe-6b96-11ea-85d4-1078d2d2b808',122,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:14','21:07:14',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6b9e3ca9-6b96-11ea-85d4-1078d2d2b808',123,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:14','21:07:14',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6ba85daf-6b96-11ea-85d4-1078d2d2b808',124,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:14','21:07:14',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6bb8a983-6b96-11ea-85d4-1078d2d2b808',125,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:14','21:07:14',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6bc39616-6b96-11ea-85d4-1078d2d2b808',126,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:15','21:07:15',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6bd7d161-6b96-11ea-85d4-1078d2d2b808',127,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:15','21:07:15',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6bec55e1-6b96-11ea-85d4-1078d2d2b808',128,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:15','21:07:15',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6bf65583-6b96-11ea-85d4-1078d2d2b808',129,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:15','21:07:15',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6c027557-6b96-11ea-85d4-1078d2d2b808',130,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:15','21:07:15',4,'',1,5880.00,0.00,0.00,0.00,5880.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6e8809aa-682c-11ea-85d4-1078d2d2b808',6,'B',2,1,1,'2020-03-17','2020-03-17','2020-03-17','12:50:54','12:51:02',1,'',1,0.00,0.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('6fd6950e-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:57:19','18:57:19',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('70d3cf5a-6b96-11ea-85d4-1078d2d2b808',131,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:23','21:07:23',4,'',1,14700.00,14700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('70e08b8c-6b96-11ea-85d4-1078d2d2b808',132,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:23','21:07:23',4,'',1,6250.00,6250.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('70f65202-6b96-11ea-85d4-1078d2d2b808',133,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:23','21:07:23',4,'',1,471920.00,100000.00,0.00,371920.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('71081cd8-6b96-11ea-85d4-1078d2d2b808',134,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:23','21:07:23',4,'',1,6260.00,0.00,0.00,6260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('7112dda6-6b96-11ea-85d4-1078d2d2b808',135,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:23','21:07:23',4,'',1,18170.00,0.00,0.00,18170.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('7125d3fc-6b96-11ea-85d4-1078d2d2b808',136,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:24','21:07:24',4,'',1,5800.00,5800.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('71304f4e-6b96-11ea-85d4-1078d2d2b808',137,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:24','21:07:24',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('7145cd4b-6b96-11ea-85d4-1078d2d2b808',138,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:24','21:07:24',4,'',1,13400.00,13400.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('715cfc77-6b96-11ea-85d4-1078d2d2b808',139,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:24','21:07:24',4,'',1,7700.00,0.00,0.00,7700.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('7169806d-6b96-11ea-85d4-1078d2d2b808',140,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:24','21:07:24',4,'',1,4220.00,4220.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('7176b1d8-6b96-11ea-85d4-1078d2d2b808',141,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:07:24','21:07:24',4,'',1,5880.00,0.00,0.00,0.00,5880.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('71be449e-6c77-11ea-85d4-1078d2d2b808',209,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,280.00,0.00,0.00,280.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('72ef96de-6c7d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-23','2020-03-23','2020-03-23','00:26:37','00:26:37',1,'',1,15200.00,0.00,0.00,0.00,15200.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('740c0b24-6a7d-11ea-85d4-1078d2d2b808',12,'B',2,1,1,'2020-03-20','2020-03-20','2020-03-20','11:35:58','11:36:26',1,'',1,300.00,300.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('743d42d6-6c15-11ea-85d4-1078d2d2b808',166,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','12:16:35','12:16:35',4,'',1,7770.00,7770.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('7674663a-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:57:30','18:57:30',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('7d14bb06-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:57:41','18:57:41',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('8088de2a-6f26-11ea-85d4-1078d2d2b808',233,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','11:46:36','11:46:36',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('83b4786e-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:57:52','18:57:52',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('83c9b806-6a7c-11ea-85d4-1078d2d2b808',11,'B',2,1,1,'2020-03-20','2020-03-20','2020-02-05','11:28:48','11:29:59',1,'',1,600.00,600.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('852a752d-6c5d-11ea-85d4-1078d2d2b808',171,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','20:52:27','20:52:27',4,'',1,8630.00,0.00,0.00,0.00,8630.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('87b88ed6-6c6b-11ea-85d4-1078d2d2b808',180,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:00:07','22:00:07',4,'',1,1320.00,0.00,0.00,1320.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('8a549c3b-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:58:03','18:58:03',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('8c146c98-6c77-11ea-85d4-1078d2d2b808',210,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('8c7c3971-66ad-11ea-85d4-1078d2d2b808',2,'B',2,1,1,'2020-03-15','2020-03-15','2020-03-15','15:10:16','15:10:16',1,'',1,1400.00,1400.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('90f42e3d-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:58:14','18:58:14',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('9104d5c3-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:58:14','18:58:14',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('97a3ca17-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:58:26','18:58:26',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('998ee7fc-6697-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','12:33:09','12:33:09',1,'',1,250.00,250.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('9e419131-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:58:37','18:58:37',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('9f03336f-668c-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','11:14:33','11:14:33',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('9f13c899-6697-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','12:33:18','12:33:18',1,'',1,400.00,400.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('9ffced3f-6c5f-11ea-85d4-1078d2d2b808',173,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','21:07:31','21:07:31',4,'',1,13800.00,0.00,0.00,0.00,13800.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a2084f11-6c72-11ea-85d4-1078d2d2b808',181,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a31237ba-6c73-11ea-85d4-1078d2d2b808',183,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a3220985-6c73-11ea-85d4-1078d2d2b808',184,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a32f86a4-6c73-11ea-85d4-1078d2d2b808',185,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a5096b56-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:58:48','18:58:48',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a8ea6fb8-6c15-11ea-85d4-1078d2d2b808',167,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','12:18:03','12:18:03',4,'',1,3130.00,3130.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a963bf09-6e68-11ea-85d4-1078d2d2b808',227,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','02:16:48','02:16:48',4,'',1,15228.00,0.00,0.00,15228.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a99f6ad0-6c7b-11ea-85d4-1078d2d2b808',215,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','00:26:37','00:26:37',4,'',1,5880.00,5880.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a9c1a295-6c73-11ea-85d4-1078d2d2b808',186,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a9ce2211-6c73-11ea-85d4-1078d2d2b808',187,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('a9db8764-6c73-11ea-85d4-1078d2d2b808',188,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('aa5ccdfd-6a7d-11ea-85d4-1078d2d2b808',13,'B',2,1,1,'2020-03-20','2020-03-20','2020-03-20','11:37:27','11:44:56',1,'',1,30.00,30.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('abab208b-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:58:59','18:58:59',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('ad01c290-6b97-11ea-85d4-1078d2d2b808',164,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:16:13','21:16:13',4,'',1,3130.00,0.00,0.00,0.00,3130.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('ae866ccf-66ac-11ea-85d4-1078d2d2b808',2,'B',2,1,1,'2020-03-15','2020-03-15','2020-03-15','15:04:17','15:04:17',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('b073902e-6c73-11ea-85d4-1078d2d2b808',189,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b0a032ab-6c73-11ea-85d4-1078d2d2b808',190,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b226ee2b-6e6c-11ea-85d4-1078d2d2b808',229,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','11:44:52','11:44:52',4,'',1,5008.00,0.00,0.00,5008.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b24a497e-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:59:10','18:59:10',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b741d57c-6c73-11ea-85d4-1078d2d2b808',191,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b75444f6-6c73-11ea-85d4-1078d2d2b808',192,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b75ff8ea-6c73-11ea-85d4-1078d2d2b808',193,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b8e6d8ad-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:59:21','18:59:21',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b9664664-6ce2-11ea-85d4-1078d2d2b808',217,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','12:36:10','12:36:10',4,'',1,17100.00,0.00,0.00,0.00,17100.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b98989a3-6ce2-11ea-85d4-1078d2d2b808',218,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','12:36:10','12:36:10',4,'',1,17100.00,0.00,0.00,0.00,17100.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('b99c1369-6ce2-11ea-85d4-1078d2d2b808',219,'B',2,1,1,'2020-03-23','2020-03-23','2020-03-23','12:36:10','12:36:10',4,'',1,12520.00,0.00,0.00,12520.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('ba32318a-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:30:46','18:30:46',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('ba448fbd-6b97-11ea-85d4-1078d2d2b808',165,'B',2,1,1,'2020-03-21','2020-03-21','2020-03-21','21:16:36','21:16:36',4,'',1,3130.00,0.00,0.00,0.00,3130.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('bc4414d7-6a7f-11ea-85d4-1078d2d2b808',16,'B',2,1,1,'2020-03-20','2020-03-20','2020-03-20','11:52:17','11:54:58',1,'',1,30.00,30.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('bdf46036-6c73-11ea-85d4-1078d2d2b808',194,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('be010018-6c73-11ea-85d4-1078d2d2b808',195,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('be0c4f03-6c73-11ea-85d4-1078d2d2b808',196,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('bf820499-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:59:32','18:59:32',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('c3684ba9-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:31:01','18:31:01',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('c49ec824-6c73-11ea-85d4-1078d2d2b808',197,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','22:44:16','22:44:16',4,'',1,2600.00,2600.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('c5113737-6f2d-11ea-85d4-1078d2d2b808',235,'B',2,1,1,'2020-03-26','2020-03-26','2020-03-26','10:47:04','10:47:04',4,'',1,2100.00,2100.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('c6279bf9-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:59:44','18:59:44',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('c8eb69d5-6696-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','12:27:19','12:27:19',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('cc6fd770-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:31:16','18:31:16',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('cd6a3090-682b-11ea-85d4-1078d2d2b808',2,'B',2,1,1,'2020-03-17','2020-03-17','2020-03-17','12:45:43','12:46:31',1,'',1,0.00,0.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('d03db85d-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','19:00:01','19:00:01',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('d046e0de-6c57-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','20:11:36','20:11:36',4,'',1,15260.00,0.00,0.00,15260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('d049634c-6c4d-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','19:00:01','19:00:01',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('d063e7bc-6c57-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','20:11:36','20:11:36',4,'',1,15260.00,0.00,0.00,15260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('d24477c1-6696-11ea-85d4-1078d2d2b808',NULL,NULL,2,1,1,'2020-03-15','2020-03-15','2020-03-15','12:27:34','12:27:34',1,'',1,350.00,350.00,0.00,0.00,0.00,NULL,0.00,0.00,0.00,0.00,NULL,NULL,NULL,1,NULL),('d78ee163-6e1f-11ea-85d4-1078d2d2b808',225,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','02:16:48','02:16:48',4,'',1,700.00,700.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('d87f7450-6c73-11ea-85d4-1078d2d2b808',198,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('daa1a167-6c74-11ea-85d4-1078d2d2b808',208,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,2010.00,2010.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('dece8ddc-6a7e-11ea-85d4-1078d2d2b808',14,'B',2,1,1,'2020-03-20','2020-03-20','2020-03-20','11:45:51','11:47:59',1,'',1,100.00,100.00,0.00,0.00,0.00,2,0.00,0.00,0.00,0.00,0,0,1,1,NULL),('df14fd9f-6c73-11ea-85d4-1078d2d2b808',199,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('dfb11035-6c66-11ea-85d4-1078d2d2b808',178,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','20:48:17','20:48:17',4,'',1,5880.00,5880.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('e12be07b-6c77-11ea-85d4-1078d2d2b808',211,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:59:21','23:59:21',4,'',1,180.00,0.00,0.00,180.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('e5b869be-6c73-11ea-85d4-1078d2d2b808',200,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('e5b9a241-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:31:59','18:31:59',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('e5c6af23-6c73-11ea-85d4-1078d2d2b808',201,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('e6396ee4-6c49-11ea-85d4-1078d2d2b808',0,NULL,2,NULL,NULL,'2020-03-22','2020-03-22','2020-03-22','18:31:59','18:31:59',4,'',1,2940.00,2940.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('ec5a65f8-6c73-11ea-85d4-1078d2d2b808',202,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('ec6801d1-6c73-11ea-85d4-1078d2d2b808',203,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:23:16','23:23:16',4,'',1,3130.00,0.00,0.00,3130.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('f061d94e-6e6c-11ea-85d4-1078d2d2b808',230,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','11:46:36','11:46:36',4,'',1,11020.00,0.00,0.00,0.00,11020.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('f2a54324-682b-11ea-85d4-1078d2d2b808',3,'B',2,1,1,'2020-03-17','2020-03-17','2020-03-17','12:47:21','12:47:34',1,'',1,0.00,0.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('f9cebbac-6e6b-11ea-85d4-1078d2d2b808',228,'B',2,1,1,'2020-03-25','2020-03-25','2020-03-25','11:27:43','11:27:43',4,'',1,1000.00,0.00,0.00,1000.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('fb0afa80-6c5c-11ea-85d4-1078d2d2b808',170,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','20:48:35','20:48:35',4,'',1,7280.00,7280.00,0.00,0.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('fb8212b9-6c77-11ea-85d4-1078d2d2b808',212,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:59:21','23:59:21',4,'',1,180.00,0.00,0.00,180.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('fb8fc826-6c77-11ea-85d4-1078d2d2b808',213,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:59:21','23:59:21',4,'',1,180.00,0.00,0.00,180.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('fb9f0ca2-6c77-11ea-85d4-1078d2d2b808',214,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','23:59:21','23:59:21',4,'',1,180.00,0.00,0.00,180.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL),('fc081db9-6c5f-11ea-85d4-1078d2d2b808',174,'B',2,1,1,'2020-03-22','2020-03-22','2020-03-22','21:10:05','21:10:05',4,'',1,22260.00,0.00,0.00,22260.00,0.00,0,0.00,0.00,0.00,0.00,0,0,NULL,2,NULL);
/*!40000 ALTER TABLE `o_header` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_pay_cl`
--

DROP TABLE IF EXISTS `o_pay_cl`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_pay_cl` (
  `f_id` char(36) CHARACTER SET latin1 NOT NULL,
  `f_code` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_name` tinytext CHARACTER SET latin1,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_pay_cl`
--

LOCK TABLES `o_pay_cl` WRITE;
/*!40000 ALTER TABLE `o_pay_cl` DISABLE KEYS */;
/*!40000 ALTER TABLE `o_pay_cl` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_pay_other`
--

DROP TABLE IF EXISTS `o_pay_other`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_pay_other` (
  `f_id` int(11) NOT NULL,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_pay_other`
--

LOCK TABLES `o_pay_other` WRITE;
/*!40000 ALTER TABLE `o_pay_other` DISABLE KEYS */;
INSERT INTO `o_pay_other` VALUES (1,'Կցել սենյակին'),(2,'Հյուրասիրություն'),(3,'Գործընկերոչ պարտք'),(4,'Նախաճաշ'),(5,'Պարտք ');
/*!40000 ALTER TABLE `o_pay_other` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_pay_room`
--

DROP TABLE IF EXISTS `o_pay_room`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_pay_room` (
  `f_id` char(36) CHARACTER SET latin1 NOT NULL,
  `f_res` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_inv` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_room` varchar(16) CHARACTER SET latin1 DEFAULT NULL,
  `f_guest` varchar(128) CHARACTER SET latin1 DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_pay_room`
--

LOCK TABLES `o_pay_room` WRITE;
/*!40000 ALTER TABLE `o_pay_room` DISABLE KEYS */;
/*!40000 ALTER TABLE `o_pay_room` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_payment`
--

DROP TABLE IF EXISTS `o_payment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_payment` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext CHARACTER SET latin1,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_payment`
--

LOCK TABLES `o_payment` WRITE;
/*!40000 ALTER TABLE `o_payment` DISABLE KEYS */;
/*!40000 ALTER TABLE `o_payment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_state`
--

DROP TABLE IF EXISTS `o_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_state` (
  `f_id` int(11) NOT NULL,
  `f_name` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_state`
--

LOCK TABLES `o_state` WRITE;
/*!40000 ALTER TABLE `o_state` DISABLE KEYS */;
INSERT INTO `o_state` VALUES (0,'Անհայտ'),(1,'Բաց'),(2,'Փակ'),(3,'Չեղարկված'),(4,'Դատարկ');
/*!40000 ALTER TABLE `o_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_tax`
--

DROP TABLE IF EXISTS `o_tax`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_tax` (
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_tax`
--

LOCK TABLES `o_tax` WRITE;
/*!40000 ALTER TABLE `o_tax` DISABLE KEYS */;
INSERT INTO `o_tax` VALUES ('4f4917b4-682c-11ea-85d4-1078d2d2b808','','«Ռոգա էնդ կոպիտա ՍՊԸ»','Արշակունյանց 34','63219817','V98745506068','98198105','77','01588771','(Ֆ)','01.06.2018 15:46:53'),('8c7c3971-66ad-11ea-85d4-1078d2d2b808','','«Ռոգա էնդ կոպիտա ՍՊԸ»','Արշակունյանց 34','63219817','V98745506068','98198105','77','01588771','(Ֆ)','01.06.2018 15:46:53'),('9f13c899-6697-11ea-85d4-1078d2d2b808','','«Ռոգա էնդ կոպիտա ՍՊԸ»','Արշակունյանց 34','63219817','V98745506068','98198105','77','01588771','(Ֆ)','01.06.2018 15:46:53'),('ae866ccf-66ac-11ea-85d4-1078d2d2b808','','«Ռոգա էնդ կոպիտա ՍՊԸ»','Արշակունյանց 34','63219817','V98745506068','98198105','77','01588771','(Ֆ)','01.06.2018 15:46:53'),('cd6a3090-682b-11ea-85d4-1078d2d2b808','','«Ռոգա էնդ կոպիտա ՍՊԸ»','Արշակունյանց 34','63219817','V98745506068','98198105','77','01588771','(Ֆ)','01.06.2018 15:46:53');
/*!40000 ALTER TABLE `o_tax` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_tax_debug`
--

DROP TABLE IF EXISTS `o_tax_debug`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_tax_debug` (
  `f_id` char(36) NOT NULL,
  `f_date` date DEFAULT NULL,
  `f_time` time DEFAULT NULL,
  `f_taxid` varchar(36) DEFAULT NULL,
  `f_message` tinytext,
  `f_mark` tinytext,
  `f_elapsed` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_tax_debug`
--

LOCK TABLES `o_tax_debug` WRITE;
/*!40000 ALTER TABLE `o_tax_debug` DISABLE KEYS */;
INSERT INTO `o_tax_debug` VALUES ('4f518e9a-682c-11ea-85d4-1078d2d2b808','2020-03-17','12:50:09',NULL,'Constructor','4f4917b4-682c-11ea-85d4-1078d2d2b808',0),('4f51b65d-682c-11ea-85d4-1078d2d2b808','2020-03-17','12:50:09',NULL,'Print JSON, connect to host','4f4917b4-682c-11ea-85d4-1078d2d2b808',1),('4f51d71b-682c-11ea-85d4-1078d2d2b808','2020-03-17','12:50:09',NULL,'Print JSON, connection failed','4f4917b4-682c-11ea-85d4-1078d2d2b808',24),('cd72f3a7-682b-11ea-85d4-1078d2d2b808','2020-03-17','12:46:31',NULL,'Constructor','cd6a3090-682b-11ea-85d4-1078d2d2b808',0),('cd7329d1-682b-11ea-85d4-1078d2d2b808','2020-03-17','12:46:31',NULL,'Print JSON, connect to host','cd6a3090-682b-11ea-85d4-1078d2d2b808',1),('cd73445b-682b-11ea-85d4-1078d2d2b808','2020-03-17','12:46:31',NULL,'Print JSON, connection failed','cd6a3090-682b-11ea-85d4-1078d2d2b808',22);
/*!40000 ALTER TABLE `o_tax_debug` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `o_tax_log`
--

DROP TABLE IF EXISTS `o_tax_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `o_tax_log` (
  `f_id` char(36) NOT NULL,
  `f_order` char(36) DEFAULT NULL,
  `f_date` date DEFAULT NULL,
  `f_time` time DEFAULT NULL,
  `f_in` text,
  `f_out` text,
  `f_err` text,
  `f_result` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `o_tax_log`
--

LOCK TABLES `o_tax_log` WRITE;
/*!40000 ALTER TABLE `o_tax_log` DISABLE KEYS */;
INSERT INTO `o_tax_log` VALUES ('4f4e09fd-682c-11ea-85d4-1078d2d2b808','4f4917b4-682c-11ea-85d4-1078d2d2b808','2020-03-17','12:50:09','{\"seq\":1,\"paidAmount\":60, \"paidAmountCard\":0, \"partialAmount\":0, \"prePaymentAmount\":0, \"useExtPOS\":true, \"mode\":2,  \"items\":[{\"adgCode\":\"\",\"dep\":\"0\",\"price\":300,\"productCode\":\"37\",\"productName\":\"մածուն\",\"qty\":0.20,\"totalPrice\":60,\"unit\":\"հատ\"}]}','{\"rseq\":77,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1527853613000.0,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}','Host not found',0),('96d6043d-66ad-11ea-85d4-1078d2d2b808','8c7c3971-66ad-11ea-85d4-1078d2d2b808','2020-03-15','15:10:33','{\"seq\":1,\"paidAmount\":1400, \"paidAmountCard\":0, \"partialAmount\":0, \"prePaymentAmount\":0, \"useExtPOS\":, \"mode\":2,  \"items\":[{\"adgCode\":\"5910\",\"dep\":\"\",\"price\":600,\"productCode\":\"87\",\"productName\":\"Fresh բանան կիվի\",\"qty\":1,\"totalPrice\":600,\"unit\":\"հատ\"},{\"adgCode\":\"5910\",\"dep\":\"\",\"price\":300,\"productCode\":\"408\",\"productName\":\"Սառը թեյ\",\"qty\":1,\"totalPrice\":300,\"unit\":\"հատ\"},{\"adgCode\":\"5910\",\"dep\":\"\",\"price\":200,\"productCode\":\"163\",\"productName\":\"Orbit\",\"qty\":1,\"totalPrice\":200,\"unit\":\"հատ\"},{\"adgCode\":\"5910\",\"dep\":\"\",\"price\":300,\"productCode\":\"191\",\"productName\":\"Ջուր ապարան 1լ\",\"qty\":1,\"totalPrice\":300,\"unit\":\"հատ\"}]}','{\"rseq\":77,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1527853613000.0,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}','Host not found',0),('9fe19461-6697-11ea-85d4-1078d2d2b808','9f13c899-6697-11ea-85d4-1078d2d2b808','2020-03-15','12:33:19','{\"seq\":1,\"paidAmount\":400, \"paidAmountCard\":0, \"partialAmount\":0, \"prePaymentAmount\":0, \"useExtPOS\":, \"mode\":2,  \"items\":[{\"adgCode\":\"5910\",\"dep\":\"\",\"price\":400,\"productCode\":\"114\",\"productName\":\"Բոռժոմի\",\"qty\":1,\"totalPrice\":400,\"unit\":\"հատ\"}]}','{\"rseq\":77,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1527853613000.0,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}','Host not found',0),('b81a1768-66ac-11ea-85d4-1078d2d2b808','ae866ccf-66ac-11ea-85d4-1078d2d2b808','2020-03-15','15:04:19','{\"seq\":1,\"paidAmount\":350, \"paidAmountCard\":0, \"partialAmount\":0, \"prePaymentAmount\":0, \"useExtPOS\":, \"mode\":2,  \"items\":[{\"adgCode\":\"5910\",\"dep\":\"\",\"price\":350,\"productCode\":\"113\",\"productName\":\"Պեպսի 0.5\",\"qty\":1,\"totalPrice\":350,\"unit\":\"հատ\"}]}','{\"rseq\":77,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1527853613000.0,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}','Host not found',0),('cd6f1305-682b-11ea-85d4-1078d2d2b808','cd6a3090-682b-11ea-85d4-1078d2d2b808','2020-03-17','12:46:31','{\"seq\":1,\"paidAmount\":0, \"paidAmountCard\":0, \"partialAmount\":0, \"prePaymentAmount\":0, \"useExtPOS\":true, \"mode\":2,  \"items\":[{\"adgCode\":\"\",\"dep\":\"0\",\"price\":0,\"productCode\":\"37\",\"productName\":\"մածուն\",\"qty\":0.20,\"totalPrice\":0,\"unit\":\"հատ\"}]}','{\"rseq\":77,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1527853613000.0,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}','Host not found',0);
/*!40000 ALTER TABLE `o_tax_log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_app`
--

DROP TABLE IF EXISTS `s_app`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_app` (
  `f_app` varchar(32) NOT NULL,
  `f_version` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_app`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_app`
--

LOCK TABLES `s_app` WRITE;
/*!40000 ALTER TABLE `s_app` DISABLE KEYS */;
INSERT INTO `s_app` VALUES ('DB','16'),('FRONTDESK','1.1.2.44'),('LOCATION','1'),('menu','7'),('SHOP','1.1.1.26'),('WAITER','1.1.3.63');
/*!40000 ALTER TABLE `s_app` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_cache`
--

DROP TABLE IF EXISTS `s_cache`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_cache` (
  `f_id` int(11) NOT NULL,
  `f_version` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_cache`
--

LOCK TABLES `s_cache` WRITE;
/*!40000 ALTER TABLE `s_cache` DISABLE KEYS */;
INSERT INTO `s_cache` VALUES (1,1),(3,1),(4,0),(5,0),(6,0),(7,13),(8,1),(9,24),(14,2),(17,5),(18,0),(19,0),(20,0),(21,13),(23,0),(24,4),(25,0),(26,1),(27,0),(28,1),(29,1),(30,0),(31,0),(32,0),(33,0),(34,0),(35,0),(36,0),(37,0),(38,0),(39,0);
/*!40000 ALTER TABLE `s_cache` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_car`
--

DROP TABLE IF EXISTS `s_car`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_car` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=95 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_car`
--

LOCK TABLES `s_car` WRITE;
/*!40000 ALTER TABLE `s_car` DISABLE KEYS */;
INSERT INTO `s_car` VALUES (1,'ACURA'),(2,'ALFA ROMEO'),(3,'ASTON MARTIN'),(4,'AUDI'),(5,'BENTLEY'),(6,'BMW'),(7,'BRILLIANCE'),(8,'BUD'),(9,'BUGATTI'),(10,'BUICK'),(11,'BYD'),(12,'CADILLAC'),(13,'CHANGAN'),(14,'CHERY'),(15,'CHEVROLET'),(16,'CHRYSLER'),(17,'CITROEN'),(18,'DAEWOO'),(19,'DAIHATSU'),(20,'DATSUN'),(21,'DODGE'),(22,'DONGFENG'),(23,'DS'),(24,'FAW'),(25,'FERRARI'),(26,'FIAT'),(27,'FORD'),(28,'FOTON'),(29,'GAC'),(30,'GEELY'),(31,'GENESIS'),(32,'GMC'),(33,'GREAT HOVER'),(34,'GREAT WALL'),(35,'HAIMA'),(36,'HAVAL'),(37,'HONDA'),(38,'HUMMER'),(39,'HYUNDAI'),(40,'INFINITI'),(41,'ISUZU'),(42,'JAC'),(43,'JAGUAR'),(44,'JEEP'),(45,'KIA'),(46,'LADA (ВАЗ)'),(47,'LAMBORGHINI'),(48,'LANCIA'),(49,'LAND ROVER'),(50,'LEXUS'),(51,'LIFAN'),(52,'LINCOLN'),(53,'LUXGEN'),(54,'MASERATI'),(55,'MAYBACH'),(56,'MAZDA'),(57,'MERCEDES-BENZ'),(58,'MERCURY'),(59,'MINI'),(60,'MITSUBISHI'),(61,'NISSAN'),(62,'NIVA'),(63,'OKA'),(64,'OLDSMOBILE'),(65,'OPEL'),(66,'PEUGEOT'),(67,'PONTIAC'),(68,'PORSCHE'),(69,'RANGE ROVER'),(70,'RAVON'),(71,'RENAULT'),(72,'ROLLS-ROYCE'),(73,'ROVER'),(74,'SAAB'),(75,'SCION'),(76,'SEAT'),(77,'SKODA'),(78,'SKODA'),(79,'SMART'),(80,'SSANGYONG'),(81,'SUBARU'),(82,'SUZUKI'),(83,'TESLA'),(84,'TOYOTA'),(85,'VOLGA'),(86,'VOLKSWAGEN'),(87,'VOLVO'),(88,'ZAP'),(89,'ZAZ'),(90,'ZOTYE'),(91,'ГАЗ'),(92,'МОСКВИЧ'),(93,'ТАГАЗ'),(94,'УАЗ');
/*!40000 ALTER TABLE `s_car` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_db`
--

DROP TABLE IF EXISTS `s_db`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_db` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  `f_description` tinytext,
  `f_host` tinytext,
  `f_db` tinytext,
  `f_user` tinytext,
  `f_password` tinytext,
  `f_main` smallint(6) DEFAULT '0',
  `f_syncin` int(11) DEFAULT '0',
  `f_syncout` int(11) DEFAULT '0',
  `f_synctime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_db`
--

LOCK TABLES `s_db` WRITE;
/*!40000 ALTER TABLE `s_db` DISABLE KEYS */;
INSERT INTO `s_db` VALUES (1,'Bufet','Bufet','127.0.0.1','cafe5','root','osyolia',1,0,0,'2020-03-15 07:10:42');
/*!40000 ALTER TABLE `s_db` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_db_access`
--

DROP TABLE IF EXISTS `s_db_access`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_db_access` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_db` int(11) DEFAULT NULL,
  `f_user` int(11) DEFAULT NULL,
  `f_permit` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_db_access`
--

LOCK TABLES `s_db_access` WRITE;
/*!40000 ALTER TABLE `s_db_access` DISABLE KEYS */;
INSERT INTO `s_db_access` VALUES (1,1,1,1);
/*!40000 ALTER TABLE `s_db_access` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_login_session`
--

DROP TABLE IF EXISTS `s_login_session`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_login_session` (
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
) ENGINE=InnoDB AUTO_INCREMENT=488 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_login_session`
--

LOCK TABLES `s_login_session` WRITE;
/*!40000 ALTER TABLE `s_login_session` DISABLE KEYS */;
INSERT INTO `s_login_session` VALUES (1,_binary '\�\�Bh2\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','13:36:23','2020-03-17','13:45:41'),(2,_binary '!Y\�h4\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','13:45:41',NULL,NULL),(3,_binary 'Tʵ�h5\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','13:54:44',NULL,NULL),(4,_binary 'Z �th5\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','13:54:53',NULL,NULL),(5,_binary 'q&��h5\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','13:55:31','2020-03-17','13:59:08'),(6,_binary '�L�	h5\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','13:59:08','2020-03-17','16:57:10'),(7,_binary '\�/�dhN\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','16:57:10',NULL,NULL),(8,_binary '\��YhN\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','16:57:23','2020-03-17','16:59:27'),(9,_binary '\"\�\�hO\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','16:59:27','2020-03-17','18:42:36'),(10,_binary '��n3h]\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','18:42:36','2020-03-17','18:46:29'),(11,_binary 'w\0�h^\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','18:46:29',NULL,NULL),(12,_binary '\�1#�h`\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:06:06',NULL,NULL),(13,_binary 'ؙ�*h`\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:06:13',NULL,NULL),(14,_binary '\�]t�h`\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','19:06:31','2020-03-17','19:14:15'),(15,_binary '��x\�ha\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','19:14:15','2020-03-17','19:15:22'),(16,_binary 'j\�hb\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','19:15:22','2020-03-17','19:26:17'),(17,_binary '�S\\\"hc\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:26:17',NULL,NULL),(18,_binary '��_hc\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:26:43',NULL,NULL),(19,_binary '�.\�{hc\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:26:49',NULL,NULL),(20,_binary '�3�hc\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:26:54',NULL,NULL),(21,_binary '��&\�hc\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:26:58',NULL,NULL),(22,_binary 'Ę?�hc\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','19:27:08',NULL,NULL),(23,_binary '\�\�hc\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','19:27:17','2020-03-17','22:36:43'),(24,_binary '@��\�h~\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','22:36:43',NULL,NULL),(25,_binary 'H1\�Fh~\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','22:36:56','2020-03-17','22:51:09'),(26,_binary 'D�k\�h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','22:51:09','2020-03-17','22:52:00'),(27,_binary 'cWh�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','22:52:00','2020-03-17','22:53:09'),(28,_binary '�>\�h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','22:53:09','2020-03-17','22:54:42'),(29,_binary 'Ôd�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','22:54:42',NULL,NULL),(30,_binary '\�\�2uh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','22:54:57',NULL,NULL),(31,_binary '\�͡&h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','22:55:04','2020-03-17','23:01:54'),(32,_binary '\�AP�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:01:54',NULL,NULL),(33,_binary 'ҬpKh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:02:17',NULL,NULL),(34,_binary 'ة�qh�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:02:27','2020-03-17','23:06:25'),(35,_binary 'f���h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:06:25',NULL,NULL),(36,_binary '�\�th�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:07:34',NULL,NULL),(37,_binary '��	%h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:08:27',NULL,NULL),(38,_binary '�\r��h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:08:33',NULL,NULL),(39,_binary '�\�J\�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:08:38',NULL,NULL),(40,_binary '��\0�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:08:42',NULL,NULL),(41,_binary '�͏Hh�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:08:51','2020-03-17','23:11:13'),(42,_binary '��h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:11:13',NULL,NULL),(43,_binary '!D�\�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:11:38',NULL,NULL),(44,_binary '%\���h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:11:46',NULL,NULL),(45,_binary '-B�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:11:58',NULL,NULL),(46,_binary '0�7h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:03',NULL,NULL),(47,_binary '4��%h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:10',NULL,NULL),(48,_binary '7\�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:16',NULL,NULL),(49,_binary '9�A�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:19',NULL,NULL),(50,_binary '?5x�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:28',NULL,NULL),(51,_binary 'B�-h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:34',NULL,NULL),(52,_binary 'D\�m�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:38',NULL,NULL),(53,_binary 'F\�f�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:12:41',NULL,NULL),(54,_binary 'IL$#h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:12:45','2020-03-17','23:22:43'),(55,_binary '�\�U`h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:22:43',NULL,NULL),(56,_binary 'ئ\�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:23:55',NULL,NULL),(57,_binary '\�\���h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:24:00','2020-03-17','23:30:43'),(58,_binary '\�\�Pgh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:30:43',NULL,NULL),(59,_binary '\�D�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:30:55',NULL,NULL),(60,_binary '\�l�	h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:31:01',NULL,NULL),(61,_binary '\�\�#Jh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:31:05',NULL,NULL),(62,_binary '\�\�5�h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:31:10','2020-03-17','23:32:56'),(63,_binary '*\�Dh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:32:56',NULL,NULL),(64,_binary '!�`�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:33:08',NULL,NULL),(65,_binary '&\�A�h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:33:16','2020-03-17','23:45:46'),(66,_binary '\�>\�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:45:46',NULL,NULL),(67,_binary '��5[h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:46:09',NULL,NULL),(68,_binary '��Nnh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:46:15',NULL,NULL),(69,_binary '�\�R\�h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:46:23','2020-03-17','23:53:53'),(70,_binary 'KKuh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:53:53',NULL,NULL),(71,_binary '\�p\rh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:54:09',NULL,NULL),(72,_binary '\�zMh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:54:14',NULL,NULL),(73,_binary '��\�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:54:18',NULL,NULL),(74,_binary '\r��h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:54:21','2020-03-17','23:56:47'),(75,_binary 'pq\Zh�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:56:47',NULL,NULL),(76,_binary 'x\�h�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-17','23:57:02',NULL,NULL),(77,_binary '{\�\\:h�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-17','23:57:07','2020-03-18','22:54:45'),(78,_binary '�u\�\'h\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','11:33:25','2020-03-18','11:50:36'),(79,_binary '(!�Lh\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','11:50:36','2020-03-18','11:51:02'),(80,_binary '7l\�h\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','11:51:02','2020-03-18','11:53:09'),(81,_binary '����h\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','11:53:09','2020-03-18','11:53:35'),(82,_binary '�\�\�h\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','11:53:35','2020-03-18','11:55:30'),(83,_binary '׋��h\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','11:55:30','2020-03-18','11:56:47'),(84,_binary 'g8\�h\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','11:56:47','2020-03-18','12:03:20'),(85,_binary '\�\�[Kh\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','12:03:20','2020-03-18','12:15:05'),(86,_binary '��\�eh�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','12:15:05','2020-03-18','12:52:59'),(87,_binary '\�DZ h�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','12:52:59','2020-03-18','12:55:08'),(88,_binary ',\\fh�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','12:55:08','2020-03-18','12:59:08'),(89,_binary '�6��h�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','12:59:08','2020-03-18','13:03:10'),(90,_binary 'K)��h�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','13:03:10','2020-03-18','13:24:07'),(91,_binary '8\��h�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','13:24:07','2020-03-18','13:28:34'),(92,_binary '׷��h�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','13:28:34','2020-03-18','14:27:55'),(93,_binary '\"\�i\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','14:27:55','2020-03-18','14:50:56'),(94,_binary 'Y��Ji\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','14:50:56','2020-03-18','15:14:39'),(95,_binary '�\�-i	\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','15:14:39','2020-03-18','15:28:35'),(96,_binary '�s�ci\�\�x\�Ҹ',1,'10.1.0.253',NULL,'2020-03-18','15:28:33',NULL,NULL),(97,_binary '�\�i\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','15:28:35','2020-03-18','15:29:47'),(98,_binary '\��\"�i\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','15:29:47','2020-03-18','15:30:25'),(99,_binary '\�_Lhi\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-18','15:30:25','2020-03-23','12:51:15'),(100,_binary '�E�iI\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-18','22:54:46',NULL,NULL),(101,_binary '�9hjiJ\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-18','22:59:27','2020-03-18','23:06:29'),(102,_binary '�(\�\�iK\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-18','23:06:30',NULL,NULL),(103,_binary '��S\�iK\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-18','23:06:40','2020-03-19','00:44:32'),(104,_binary 'F���iY\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:44:32',NULL,NULL),(105,_binary 'R0��iY\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:44:52',NULL,NULL),(106,_binary 'V�#iY\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:45:00',NULL,NULL),(107,_binary 'Z�	\�iY\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:45:06',NULL,NULL),(108,_binary ']4m`iY\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:45:10',NULL,NULL),(109,_binary '_�,\�iY\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','00:45:15','2020-03-19','00:46:57'),(110,_binary '�\Z��iY\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:46:57',NULL,NULL),(111,_binary '��a\�iY\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:47:09',NULL,NULL),(112,_binary '���\�iY\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','00:47:13','2020-03-19','00:50:55'),(113,_binary '+\�biZ\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:50:56',NULL,NULL),(114,_binary 'F��iZ\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','00:51:42',NULL,NULL),(115,_binary 'J/\�\0iZ\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','00:51:48','2020-03-19','00:54:04'),(116,_binary '����iZ\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','00:54:04','2020-03-19','01:00:24'),(117,_binary '~#{i[\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','01:00:24',NULL,NULL),(118,_binary '�\'Hi\\\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','01:04:22',NULL,NULL),(119,_binary 'I!�i\\\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','01:04:33',NULL,NULL),(120,_binary '_\�mi\\\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','01:04:45','2020-03-19','01:05:34'),(121,_binary '6z\�\�i\\\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','01:05:34',NULL,NULL),(122,_binary '<���i\\\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','01:05:44','2020-03-19','14:30:19'),(123,_binary '�\�ni\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','14:30:19',NULL,NULL),(124,_binary '�\�\�i\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','14:30:32','2020-03-19','14:35:23'),(125,_binary 'X_\��i\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','14:35:23','2020-03-19','14:36:18'),(126,_binary 'x詶i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','14:36:18',NULL,NULL),(127,_binary '��\�pi\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','14:38:10','2020-03-19','15:02:29'),(128,_binary '!e\�i\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','15:02:29','2020-03-19','15:05:43'),(129,_binary '��\�-i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:05:43',NULL,NULL),(130,_binary '� \�i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:05:53',NULL,NULL),(131,_binary '�Gmi\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:06:02',NULL,NULL),(132,_binary '��M\Zi\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:06:20',NULL,NULL),(133,_binary '\�\��i\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','15:08:48','2020-03-19','15:09:24'),(134,_binary '\���i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:09:24',NULL,NULL),(135,_binary 'e��i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:11:34',NULL,NULL),(136,_binary 'j�\0i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:11:41',NULL,NULL),(137,_binary 'l���i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:11:45',NULL,NULL),(138,_binary 'r\�bi\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','15:11:55','2020-03-19','15:23:42'),(139,_binary '�Si\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:23:42',NULL,NULL),(140,_binary '\0�i\�i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:30:12',NULL,NULL),(141,_binary '��Ti\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','15:30:36','2020-03-19','15:34:46'),(142,_binary '��G\�i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:34:46',NULL,NULL),(143,_binary '�\�	i\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','15:35:01','2020-03-19','15:36:26'),(144,_binary '\�VZni\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:36:26',NULL,NULL),(145,_binary '\�IX\�i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:36:41',NULL,NULL),(146,_binary '\��_7i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:36:45',NULL,NULL),(147,_binary '\�k��i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:36:51',NULL,NULL),(148,_binary '�;��i\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','15:36:56',NULL,NULL),(149,_binary '�\�y�i\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','15:37:00','2020-03-19','16:48:49'),(150,_binary '�eki\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','16:48:49','2020-03-19','16:50:02'),(151,_binary '\'��Fi\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','16:50:02',NULL,NULL),(152,_binary '.�+\�i\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','16:50:14','2020-03-19','22:46:01'),(153,_binary '����j\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','22:37:45',NULL,NULL),(154,_binary '\�j\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','22:38:58',NULL,NULL),(155,_binary 'D\�l~j\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','22:41:36',NULL,NULL),(156,_binary '\�\�j\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','22:46:01',NULL,NULL),(157,_binary '0�/oj\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','22:48:12',NULL,NULL),(158,_binary '.yoj\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','22:55:18',NULL,NULL),(159,_binary '���vj\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','22:58:14','2020-03-19','22:59:27'),(160,_binary '\�W%Lj\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','22:59:27','2020-03-19','23:00:04'),(161,_binary '\�\�E~j\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','23:00:04','2020-03-19','23:03:03'),(162,_binary 'C��j\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','23:03:03','2020-03-19','23:04:10'),(163,_binary 'k�h�j\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','23:04:10',NULL,NULL),(164,_binary '�7\��j\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','23:13:11','2020-03-19','23:15:16'),(165,_binary '�x/�j\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','23:15:16',NULL,NULL),(166,_binary '!tj\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','23:30:43',NULL,NULL),(167,_binary 'o�cj\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-19','23:32:55',NULL,NULL),(168,_binary '��\�\�j\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','23:34:02','2020-03-19','23:34:39'),(169,_binary '�\�Ôj\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-19','23:34:39','2020-03-20','09:26:22'),(170,_binary 'W\�4jk\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','09:26:23','2020-03-20','09:29:19'),(171,_binary '�\��yjk\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','09:29:19','2020-03-20','09:41:06'),(172,_binary '\�1\�Cjl\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-20','09:36:46',NULL,NULL),(173,_binary 'f^ �jm\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','09:41:06','2020-03-20','09:41:36'),(174,_binary 'w�\n>jm\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','09:41:36','2020-03-20','09:52:05'),(175,_binary '\�z\�jn\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-20','09:52:05',NULL,NULL),(176,_binary '��)�j�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','13:31:26','2020-03-20','13:33:46'),(177,_binary '\�=\�j�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-20','13:33:44',NULL,NULL),(178,_binary '\��\�j�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-20','13:33:46',NULL,NULL),(179,_binary '�\�\�\�j�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','13:38:59','2020-03-20','13:39:54'),(180,_binary '\�\�j�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','13:39:54','2020-03-20','13:40:44'),(181,_binary '\�õ]j�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','13:40:45','2020-03-20','13:45:34'),(182,_binary '�`^j�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','13:45:34','2020-03-20','13:59:07'),(183,_binary 'q�*�j�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','13:59:07','2020-03-20','22:40:31'),(184,_binary 'I	w�j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','22:40:31','2020-03-20','22:53:15'),(185,_binary ']�\�j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','22:53:15','2020-03-20','22:56:45'),(186,_binary '�-x\�j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','22:56:45','2020-03-20','22:58:15'),(187,_binary '\�ɒKj\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','22:58:15','2020-03-20','23:05:02'),(188,_binary '�у\nj\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','23:05:02','2020-03-20','23:09:59'),(189,_binary 'f\���j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-20','23:09:59','2020-03-21','00:07:25'),(190,_binary 'l�*�j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-21','00:07:25','2020-03-21','00:10:36'),(191,_binary 'ޞމj\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-21','00:10:36','2020-03-21','00:13:15'),(192,_binary '=w\�\�j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-21','00:13:15','2020-03-21','00:19:45'),(193,_binary '%�\�j\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-21','00:19:45',NULL,NULL),(194,_binary 'I\Z�pj\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-21','00:27:54','2020-03-21','00:32:47'),(195,_binary '����j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-21','00:32:47','2020-03-21','00:39:45'),(196,_binary '�1\�j\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-21','00:39:45',NULL,NULL),(197,_binary '-J�j\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-21','00:41:26','2020-03-21','10:59:27'),(198,_binary '�YhUkA\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-21','10:59:27',NULL,NULL),(199,_binary '\�1n\'kg\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-21','15:33:30',NULL,NULL),(200,_binary '�>Eks\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','16:57:20','2020-03-21','17:07:42'),(201,_binary '�>Eks\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','17:07:42',NULL,NULL),(202,_binary '}�\�ikx\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','17:33:00','2020-03-21','17:35:55'),(203,_binary '}�\�ikx\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','17:35:55',NULL,NULL),(204,_binary 'Ԑ�\�k|\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:04:03','2020-03-21','18:07:48'),(205,_binary 'Z\Z��k}\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:07:48',NULL,NULL),(206,_binary 'Ԑ�\�k|\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:07:48',NULL,NULL),(207,_binary '�xC|k}\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:09:05','2020-03-21','18:16:41'),(208,_binary '�\�\�k}\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:09:35',NULL,NULL),(209,_binary '�|k~\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:16:41',NULL,NULL),(210,_binary '�xC|k}\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:16:41',NULL,NULL),(211,_binary '�\�Rk\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:20:08','2020-03-21','18:20:52'),(212,_binary '-\�^\�k\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:20:52',NULL,NULL),(213,_binary '�\�Rk\�\�x\�Ҹ',1,NULL,NULL,'2020-03-21','18:20:52',NULL,NULL),(214,_binary '|t��k\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:23:04',NULL,NULL),(215,_binary '�\� ik�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(216,_binary '�\�1vk�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(217,_binary '�䠊k�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(218,_binary '�\��k�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(219,_binary '�\�\�k�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(220,_binary '��k�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(221,_binary '��*k�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(222,_binary '���ek�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(223,_binary '��\�\Zk�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(224,_binary '�\0h�k�\�\�x\�Ҹ',4,NULL,NULL,'2020-03-21','18:31:43',NULL,NULL),(225,_binary '\�V\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:39',NULL,NULL),(226,_binary '\�Ok�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:39',NULL,NULL),(227,_binary '\�\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:39',NULL,NULL),(228,_binary '\�$2#k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:39',NULL,NULL),(229,_binary '\�)9zk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:40',NULL,NULL),(230,_binary '\�-\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:40',NULL,NULL),(231,_binary '\�1\�_k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:40',NULL,NULL),(232,_binary '\�5�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:40',NULL,NULL),(233,_binary '\�9\�0k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:40',NULL,NULL),(234,_binary '\�=��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:48:40',NULL,NULL),(235,_binary '\�_�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:25',NULL,NULL),(236,_binary '��lk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:33',NULL,NULL),(237,_binary '��zk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:33',NULL,NULL),(238,_binary '!�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:33',NULL,NULL),(239,_binary 's\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:33',NULL,NULL),(240,_binary '&/k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:33',NULL,NULL),(241,_binary '@\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:33',NULL,NULL),(242,_binary '�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:34',NULL,NULL),(243,_binary 's�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:50:34',NULL,NULL),(244,_binary '&\���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:00',NULL,NULL),(245,_binary '+\��$k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:08',NULL,NULL),(246,_binary '+\�\�\0k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:08',NULL,NULL),(247,_binary '+\�\�9k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:08',NULL,NULL),(248,_binary '+\�xk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:08',NULL,NULL),(249,_binary '+\�\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:08',NULL,NULL),(250,_binary '+�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:08',NULL,NULL),(251,_binary '+���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:09',NULL,NULL),(252,_binary '+�W�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:09',NULL,NULL),(253,_binary '3t\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(254,_binary '3x��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(255,_binary '3|L\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(256,_binary '3���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(257,_binary '3��Kk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(258,_binary '3�Fk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(259,_binary '3�	�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(260,_binary '3�5�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:21',NULL,NULL),(261,_binary ';�Ok�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(262,_binary ';�bk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(263,_binary ';.�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(264,_binary ';@�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(265,_binary ';#.\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(266,_binary ';\'��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(267,_binary ';.a�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(268,_binary ';2=\nk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:34',NULL,NULL),(269,_binary 'B�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(270,_binary 'B���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(271,_binary 'B�lk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(272,_binary 'Bçok�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(273,_binary 'B\�\�]k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(274,_binary 'B\�r�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(275,_binary 'B\�\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(276,_binary 'B\�Ƕk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:47',NULL,NULL),(277,_binary 'JT3k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:59',NULL,NULL),(278,_binary 'JX\�8k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:51:59',NULL,NULL),(279,_binary 'J]L�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:00',NULL,NULL),(280,_binary 'Ja�Gk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:00',NULL,NULL),(281,_binary 'Jf*sk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:00',NULL,NULL),(282,_binary 'Jj0�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:00',NULL,NULL),(283,_binary 'Jn\nk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:00',NULL,NULL),(284,_binary 'Jy�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:00',NULL,NULL),(285,_binary 'Q�;�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:12',NULL,NULL),(286,_binary 'R��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:12',NULL,NULL),(287,_binary 'R	�\rk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:12',NULL,NULL),(288,_binary 'R�1k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:12',NULL,NULL),(289,_binary 'R2k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:12',NULL,NULL),(290,_binary 'R�:k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:12',NULL,NULL),(291,_binary 'R��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:13',NULL,NULL),(292,_binary 'R ��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:13',NULL,NULL),(293,_binary 'Y�T0k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(294,_binary 'Y�xFk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(295,_binary 'Y���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(296,_binary 'Y��\"k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(297,_binary 'Y��\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(298,_binary 'Y�ySk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(299,_binary 'Y��\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(300,_binary 'Y\�qsk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:25',NULL,NULL),(301,_binary 'aE\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(302,_binary 'aI?�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(303,_binary 'aMSk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(304,_binary 'aS5k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(305,_binary 'aW��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(306,_binary 'a\\rk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(307,_binary 'aaMRk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(308,_binary 'ae�Kk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:38',NULL,NULL),(309,_binary 'h\�.k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(310,_binary 'h\�R/k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(311,_binary 'h\�Uk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(312,_binary 'h\�\�Rk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(313,_binary 'h�\0\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(314,_binary 'h�B1k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(315,_binary 'h�G\Zk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(316,_binary 'i��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:52:51',NULL,NULL),(317,_binary 'p~�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:03',NULL,NULL),(318,_binary 'p�7k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:04',NULL,NULL),(319,_binary 'p��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:04',NULL,NULL),(320,_binary 'p�H\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:04',NULL,NULL),(321,_binary 'p�\�6k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:04',NULL,NULL),(322,_binary 'p�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:04',NULL,NULL),(323,_binary 'p���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:04',NULL,NULL),(324,_binary 'p�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:04',NULL,NULL),(325,_binary 'x$\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:16',NULL,NULL),(326,_binary 'x+��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:16',NULL,NULL),(327,_binary 'x1-8k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:16',NULL,NULL),(328,_binary 'x5<}k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:16',NULL,NULL),(329,_binary 'x9�	k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:16',NULL,NULL),(330,_binary 'x=\�}k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:16',NULL,NULL),(331,_binary 'xB\�Rk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:17',NULL,NULL),(332,_binary 'xG\�Nk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:17',NULL,NULL),(333,_binary '�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(334,_binary '\�tUk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(335,_binary '\�U\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(336,_binary '\�5k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(337,_binary '\�\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(338,_binary 'ҁ�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(339,_binary 'ւok�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(340,_binary '\�{1k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:53:29',NULL,NULL),(341,_binary '\�\��Ck�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(342,_binary '\�\�\��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(343,_binary '\�\�Ƞk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(344,_binary '\�\�dwk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(345,_binary '\�\�mk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(346,_binary '\��\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(347,_binary '\���Yk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(348,_binary '\���$k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:00',NULL,NULL),(349,_binary '\�\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:01',NULL,NULL),(350,_binary '\�\n�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:56:01',NULL,NULL),(351,_binary '���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:57:41',NULL,NULL),(352,_binary '2��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(353,_binary '2n2k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(354,_binary '2Ck�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(355,_binary '2\Z#\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(356,_binary '2k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(357,_binary '2\"`�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(358,_binary '2&v�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(359,_binary '2*�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(360,_binary '2/k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(361,_binary '23s1k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','20:58:28',NULL,NULL),(362,_binary '����k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(363,_binary '���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(364,_binary '��ݿk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(365,_binary '��e\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(366,_binary '��1k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(367,_binary '���Yk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(368,_binary '��\�1k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(369,_binary '��e\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(370,_binary '��	�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(371,_binary '�©|k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:27',NULL,NULL),(372,_binary '���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(373,_binary '�\Zy�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(374,_binary '��ik�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(375,_binary '�\"ٰk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(376,_binary '�)4\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(377,_binary '�-sgk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(378,_binary '�1�Xk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(379,_binary '�5�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(380,_binary '�:!k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:46',NULL,NULL),(381,_binary '�@\r�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:47',NULL,NULL),(382,_binary '�E\�}k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:47',NULL,NULL),(383,_binary '�Кnk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(384,_binary '�\�\�2k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(385,_binary '�؍6k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(386,_binary '�\�_�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(387,_binary '�\��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(388,_binary '�\�\�[k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(389,_binary '�\��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(390,_binary '�\�Uk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:01:59',NULL,NULL),(391,_binary '(\�Z\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(392,_binary '(\�e�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(393,_binary '(\�\�&k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(394,_binary '(ߏk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(395,_binary '(\�#�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(396,_binary '(\�\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(397,_binary '(\�)k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(398,_binary '(\�\�\0k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(399,_binary '(�\r\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(400,_binary '(���k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:05:22',NULL,NULL),(401,_binary '�N@k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(402,_binary '�SCk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(403,_binary '�\\xk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(404,_binary '�bWok�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(405,_binary '�f�Hk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(406,_binary '�k=bk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(407,_binary '�o�Wk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(408,_binary '�s�gk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(409,_binary '�xU|k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(410,_binary '�|�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(411,_binary '��%k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:09',NULL,NULL),(412,_binary '�\�Bkk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(413,_binary '�\�C�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(414,_binary '�\�3\rk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(415,_binary '�\�`�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(416,_binary '�⧞k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(417,_binary '�\��k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(418,_binary '�\�&\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(419,_binary '��q0k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(420,_binary '��\�\�k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:21',NULL,NULL),(421,_binary '���Fk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:11:22',NULL,NULL),(422,_binary '�\�k�\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-21','21:11:33','2020-03-21','21:14:15'),(423,_binary 'f:\�.k�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:14:15',NULL,NULL),(424,_binary 'fN��k�\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-21','21:14:15','2020-03-21','21:15:06'),(425,_binary '�\�\nk�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-21','21:15:06',NULL,NULL),(426,_binary '���\�k�\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-21','21:15:06','2020-03-22','13:53:41'),(427,_binary '4\�\�l#\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','13:53:41','2020-03-22','18:23:41'),(428,_binary '��lH\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:23:41','2020-03-22','18:30:45'),(429,_binary '���{lI\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:30:45','2020-03-22','18:32:53'),(430,_binary 'Lj7lJ\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:32:48','2020-03-22','18:32:56'),(431,_binary 'e\"lJ\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-22','18:32:53',NULL,NULL),(432,_binary '��\�lJ\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:32:56','2020-03-22','18:38:00'),(433,_binary '�ۓ\�lJ\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:38:00','2020-03-22','18:39:44'),(434,_binary '�4X3lJ\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:39:44','2020-03-22','18:41:04'),(435,_binary '*�X|lK\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:41:04','2020-03-22','18:42:06'),(436,_binary 'O~�JlK\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:42:06','2020-03-22','18:53:24'),(437,_binary '\�o�lL\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:53:24','2020-03-22','18:56:55'),(438,_binary 'alqlM\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','18:56:55','2020-03-22','19:01:21'),(439,_binary '\0�lN\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','19:01:21','2020-03-22','20:08:16'),(440,_binary 'YA2@lW\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','20:08:16','2020-03-22','20:48:34'),(441,_binary '�qwsl\\\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','20:48:34','2020-03-22','21:59:12'),(442,_binary '؀lf\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','21:59:12','2020-03-22','22:00:23'),(443,_binary '�Flg\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','22:00:23','2020-03-22','22:32:31'),(444,_binary '�O5�lk\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','22:32:31','2020-03-22','22:44:33'),(445,_binary '.<\\�lm\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','22:44:33','2020-03-22','22:49:52'),(446,_binary '\�\��lm\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','22:49:52','2020-03-22','22:53:54'),(447,_binary '}#z\�ln\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','22:53:54','2020-03-22','23:04:51'),(448,_binary 'u\�lp\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','23:04:51','2020-03-22','23:15:56'),(449,_binary '��O`lq\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','23:15:56','2020-03-22','23:23:34'),(450,_binary '����lr\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','23:23:34','2020-03-22','23:59:38'),(451,_binary '�윽lw\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-22','23:59:38','2020-03-23','00:26:54'),(452,_binary 'z�\�l{\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-23','00:26:54',NULL,NULL),(453,_binary 'e\'��l}\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-23','00:40:37','2020-03-23','00:42:00'),(454,_binary '����l}\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-23','00:42:00',NULL,NULL),(455,_binary '�\�\�l}\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-23','00:42:00','2020-03-23','00:51:42'),(456,_binary '�T�l~\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-23','00:51:42',NULL,NULL),(457,_binary '�A�l~\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-23','00:51:42',NULL,NULL),(458,_binary '\"�b\�l\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-23','00:53:04',NULL,NULL),(459,_binary '��\�l�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-23','01:03:18',NULL,NULL),(460,_binary '\�\��l�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-23','01:05:11',NULL,NULL),(461,_binary 'S�zl\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-23','12:28:47','2020-03-23','12:34:28'),(462,_binary '�U�l\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-23','12:34:28',NULL,NULL),(463,_binary '�fl\�\�\�x\�Ҹ',1,'10.1.0.2','10.1.0.2','2020-03-23','12:34:28','2020-03-23','12:36:27'),(464,_binary 'e{�l\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-23','12:36:26',NULL,NULL),(465,_binary 'e�U�l\�\�\�x\�Ҹ',1,'10.1.0.2',NULL,'2020-03-23','12:36:27',NULL,NULL),(466,_binary '\�\nTl\�\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-23','12:39:57','2020-03-23','12:45:44'),(467,_binary '�qk\rl\�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-23','12:45:43',NULL,NULL),(468,_binary '���l\�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-23','12:45:44',NULL,NULL),(469,_binary 'w,�l\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-23','12:51:15','2020-03-23','12:52:44'),(470,_binary '�\'\�\�l\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-23','12:52:44','2020-03-23','12:55:13'),(471,_binary '\�9Vl\�\�\�x\�Ҹ',1,'10.1.0.253','10.1.0.253','2020-03-23','12:55:13','2020-03-23','12:55:52'),(472,_binary '�IWl\�\�\�x\�Ҹ',1,'10.1.0.253',NULL,'2020-03-23','12:55:52',NULL,NULL),(473,_binary 'E�N\�l\�\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-23','13:39:58',NULL,NULL),(474,_binary '\�\�\�n\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-25','02:14:59','2020-03-25','02:17:05'),(475,_binary '4�\�?n\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','02:17:04',NULL,NULL),(476,_binary '4��n\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-25','02:17:05','2020-03-25','11:28:00'),(477,_binary '��\�\�n\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','02:29:40',NULL,NULL),(478,_binary '\�ί�n\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','02:35:24',NULL,NULL),(479,_binary '+\�|nj\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','11:28:00',NULL,NULL),(480,_binary '+�\"nj\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-25','11:28:00','2020-03-25','11:43:37'),(481,_binary '�ȰQnk\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','11:40:56',NULL,NULL),(482,_binary 'Y���nl\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','11:43:37',NULL,NULL),(483,_binary 'Y�V^nl\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-25','11:43:37','2020-03-25','11:44:58'),(484,_binary '�\�#nl\�\�x\�Ҹ',4,'10.1.0.2','10.1.0.2','2020-03-25','11:44:58','2020-03-25','11:46:53'),(485,_binary '��u�nl\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','11:44:59',NULL,NULL),(486,_binary '\�\�qbnl\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','11:46:53',NULL,NULL),(487,_binary '\�\�\�nl\�\�x\�Ҹ',4,'10.1.0.2',NULL,'2020-03-25','11:46:53',NULL,NULL);
/*!40000 ALTER TABLE `s_login_session` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_receipt_print`
--

DROP TABLE IF EXISTS `s_receipt_print`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_receipt_print` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_station` varchar(64) DEFAULT NULL,
  `f_ip` varchar(64) DEFAULT NULL,
  `f_printer` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `f_station` (`f_station`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_receipt_print`
--

LOCK TABLES `s_receipt_print` WRITE;
/*!40000 ALTER TABLE `s_receipt_print` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_receipt_print` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_salary_account`
--

DROP TABLE IF EXISTS `s_salary_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_salary_account` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_position` int(11) DEFAULT NULL,
  `f_shift` int(11) DEFAULT NULL,
  `f_individual` smallint(6) DEFAULT NULL,
  `f_percent` decimal(9,3) DEFAULT NULL,
  `f_round` smallint(6) DEFAULT NULL,
  `f_raw` text,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_salary_account`
--

LOCK TABLES `s_salary_account` WRITE;
/*!40000 ALTER TABLE `s_salary_account` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_salary_account` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_salary_body`
--

DROP TABLE IF EXISTS `s_salary_body`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_salary_body` (
  `f_id` char(36) NOT NULL,
  `f_header` char(36) DEFAULT NULL,
  `f_user` int(11) DEFAULT NULL,
  `f_position` int(11) DEFAULT NULL,
  `f_amount` decimal(14,2) DEFAULT NULL,
  `f_row` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_salary_body`
--

LOCK TABLES `s_salary_body` WRITE;
/*!40000 ALTER TABLE `s_salary_body` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_salary_body` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_salary_options`
--

DROP TABLE IF EXISTS `s_salary_options`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_salary_options` (
  `f_id` char(36) NOT NULL,
  `f_cashdoc` char(36) DEFAULT NULL,
  `f_shift` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_salary_options`
--

LOCK TABLES `s_salary_options` WRITE;
/*!40000 ALTER TABLE `s_salary_options` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_salary_options` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_salary_shift`
--

DROP TABLE IF EXISTS `s_salary_shift`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_salary_shift` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  `f_number` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_salary_shift`
--

LOCK TABLES `s_salary_shift` WRITE;
/*!40000 ALTER TABLE `s_salary_shift` DISABLE KEYS */;
INSERT INTO `s_salary_shift` VALUES (1,'I հերթ',1),(2,'II հերթ',2);
/*!40000 ALTER TABLE `s_salary_shift` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_salary_shift_time`
--

DROP TABLE IF EXISTS `s_salary_shift_time`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_salary_shift_time` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_shift` int(11) DEFAULT NULL,
  `f_start` time DEFAULT NULL,
  `f_end` time DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_salary_shift_time`
--

LOCK TABLES `s_salary_shift_time` WRITE;
/*!40000 ALTER TABLE `s_salary_shift_time` DISABLE KEYS */;
INSERT INTO `s_salary_shift_time` VALUES (1,1,'10:00:00','22:00:00'),(2,2,'22:00:01','23:59:59'),(3,2,'00:00:00','09:59:50');
/*!40000 ALTER TABLE `s_salary_shift_time` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_settings_names`
--

DROP TABLE IF EXISTS `s_settings_names`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_settings_names` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(64) DEFAULT NULL,
  `f_description` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_settings_names`
--

LOCK TABLES `s_settings_names` WRITE;
/*!40000 ALTER TABLE `s_settings_names` DISABLE KEYS */;
INSERT INTO `s_settings_names` VALUES (1,'Main',''),(2,'Smart',''),(3,'Shop',''),(4,'Waiter',''),(5,'Vano','');
/*!40000 ALTER TABLE `s_settings_names` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_settings_values`
--

DROP TABLE IF EXISTS `s_settings_values`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_settings_values` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_settings` int(11) DEFAULT NULL,
  `f_key` int(11) DEFAULT NULL,
  `f_value` tinytext,
  PRIMARY KEY (`f_id`),
  KEY `fk_settings_values_s_idx` (`f_settings`),
  CONSTRAINT `fk_settings_values_s` FOREIGN KEY (`f_settings`) REFERENCES `s_settings_names` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=469 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_settings_values`
--

LOCK TABLES `s_settings_values` WRITE;
/*!40000 ALTER TABLE `s_settings_values` DISABLE KEYS */;
INSERT INTO `s_settings_values` VALUES (181,2,1,'local'),(182,2,2,''),(183,2,3,''),(184,2,4,''),(185,2,5,''),(186,2,6,''),(187,2,7,''),(188,2,8,''),(189,2,9,'1'),(190,2,10,'A'),(191,2,11,'1'),(192,2,12,'1'),(193,2,13,''),(194,2,14,NULL),(195,2,15,NULL),(196,2,17,''),(197,2,18,'2'),(198,2,21,''),(199,2,22,''),(200,2,23,''),(201,2,24,'0'),(202,2,25,'0'),(203,2,26,'::'),(204,2,27,'0'),(205,2,28,''),(206,2,30,'0'),(207,2,32,''),(208,2,33,NULL),(209,2,34,'0'),(210,2,35,''),(211,2,36,''),(212,2,37,''),(213,2,38,'0'),(214,2,39,''),(215,2,40,''),(216,2,41,'0'),(253,3,1,'local'),(254,3,2,'0.1'),(255,3,3,''),(256,3,4,''),(257,3,5,''),(258,3,6,''),(259,3,7,''),(260,3,8,''),(261,3,9,'1'),(262,3,10,'SHOP'),(263,3,11,'1'),(264,3,12,'1'),(265,3,13,''),(266,3,14,'2'),(267,3,15,'true'),(268,3,17,''),(269,3,18,'2'),(270,3,21,''),(271,3,22,''),(272,3,23,''),(273,3,24,'0'),(274,3,25,'0'),(275,3,26,'::'),(276,3,27,'0'),(277,3,28,''),(278,3,30,'0'),(279,3,32,'1'),(280,3,33,NULL),(281,3,34,'0'),(282,3,35,'2'),(283,3,36,'CASHINPUT'),(284,3,37,'CARDINPUT'),(285,3,38,'1'),(286,3,39,''),(287,3,40,''),(288,3,41,'1'),(325,4,1,'local'),(326,4,2,'0.1'),(327,4,3,''),(328,4,4,''),(329,4,5,''),(330,4,6,''),(331,4,7,''),(332,4,8,''),(333,4,9,'1'),(334,4,10,'W'),(335,4,11,'1'),(336,4,12,'1'),(337,4,13,''),(338,4,14,'2'),(339,4,15,NULL),(340,4,17,''),(341,4,18,'2'),(342,4,21,''),(343,4,22,''),(344,4,23,''),(345,4,24,'0'),(346,4,25,'0'),(347,4,26,'::'),(348,4,27,'0'),(349,4,28,''),(350,4,30,'1'),(351,4,32,'1'),(352,4,33,NULL),(353,4,34,'0'),(354,4,35,'2'),(355,4,36,'CASHINPUT'),(356,4,37,'CARDINPUT'),(357,4,38,'1'),(358,4,39,''),(359,4,40,''),(360,4,41,'0'),(361,1,1,''),(362,1,2,''),(363,1,3,''),(364,1,4,''),(365,1,5,''),(366,1,6,''),(367,1,7,''),(368,1,8,''),(369,1,9,NULL),(370,1,10,''),(371,1,11,''),(372,1,12,''),(373,1,13,''),(374,1,14,NULL),(375,1,15,NULL),(376,1,17,''),(377,1,18,NULL),(378,1,21,''),(379,1,22,''),(380,1,23,''),(381,1,24,'0'),(382,1,25,'0'),(383,1,26,'::'),(384,1,27,'0'),(385,1,28,'11'),(386,1,30,'1'),(387,1,32,''),(388,1,33,'0'),(389,1,34,'0'),(390,1,35,''),(391,1,36,''),(392,1,37,''),(393,1,38,'0'),(394,1,39,''),(395,1,40,''),(396,1,41,'0'),(433,5,1,''),(434,5,2,''),(435,5,3,''),(436,5,4,''),(437,5,5,''),(438,5,6,''),(439,5,7,''),(440,5,8,''),(441,5,9,NULL),(442,5,10,''),(443,5,11,'1'),(444,5,12,'1'),(445,5,13,''),(446,5,14,'6'),(447,5,15,NULL),(448,5,17,''),(449,5,18,'2'),(450,5,21,''),(451,5,22,''),(452,5,23,''),(453,5,24,'0'),(454,5,25,'0'),(455,5,26,'::'),(456,5,27,'0'),(457,5,28,''),(458,5,30,'0'),(459,5,32,''),(460,5,33,NULL),(461,5,34,'0'),(462,5,35,''),(463,5,36,''),(464,5,37,''),(465,5,38,'0'),(466,5,39,''),(467,5,40,''),(468,5,41,'0');
/*!40000 ALTER TABLE `s_settings_values` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_station_conf`
--

DROP TABLE IF EXISTS `s_station_conf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_station_conf` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_station` varchar(64) DEFAULT NULL,
  `f_ip` varchar(64) DEFAULT NULL,
  `f_conf` int(11) DEFAULT NULL,
  `f_dateCash` date DEFAULT NULL,
  `f_dateCashAuto` smallint(6) DEFAULT '1',
  `f_shift` smallint(6) DEFAULT '1',
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `f_station` (`f_station`)
) ENGINE=InnoDB AUTO_INCREMENT=447 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_station_conf`
--

LOCK TABLES `s_station_conf` WRITE;
/*!40000 ALTER TABLE `s_station_conf` DISABLE KEYS */;
INSERT INTO `s_station_conf` VALUES (418,'hotel','127.0.0.1',55,'2020-03-20',0,1),(419,'bufet','192.168.2.21',1,'2020-03-20',0,1),(420,'loby-bar','192.168.2.20',1,'2020-03-20',0,1),(446,'sevak-pc','192.168.2.31',1,'2020-03-20',0,1);
/*!40000 ALTER TABLE `s_station_conf` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_stranslator`
--

DROP TABLE IF EXISTS `s_stranslator`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_stranslator` (
  `f_name` tinytext,
  `f_en` tinytext,
  `f_ru` tinytext
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_stranslator`
--

LOCK TABLES `s_stranslator` WRITE;
/*!40000 ALTER TABLE `s_stranslator` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_stranslator` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_syncronize`
--

DROP TABLE IF EXISTS `s_syncronize`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_syncronize` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_db` int(11) DEFAULT NULL,
  `f_table` tinytext,
  `f_recid` char(36) DEFAULT NULL,
  `f_op` smallint(6) DEFAULT NULL,
  `f_date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_syncronize`
--

LOCK TABLES `s_syncronize` WRITE;
/*!40000 ALTER TABLE `s_syncronize` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_syncronize` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_syncronize_in`
--

DROP TABLE IF EXISTS `s_syncronize_in`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_syncronize_in` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_table` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_syncronize_in`
--

LOCK TABLES `s_syncronize_in` WRITE;
/*!40000 ALTER TABLE `s_syncronize_in` DISABLE KEYS */;
INSERT INTO `s_syncronize_in` VALUES (1,'o_tax_log'),(2,'o_tax'),(3,'o_tax_debug'),(4,'o_header'),(5,'o_goods'),(6,'a_store_draft');
/*!40000 ALTER TABLE `s_syncronize_in` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_translator`
--

DROP TABLE IF EXISTS `s_translator`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_translator` (
  `f_text` varchar(128) NOT NULL,
  `f_en` tinytext,
  `f_ru` tinytext,
  PRIMARY KEY (`f_text`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_translator`
--

LOCK TABLES `s_translator` WRITE;
/*!40000 ALTER TABLE `s_translator` DISABLE KEYS */;
INSERT INTO `s_translator` VALUES ('(F)','',''),('(Ֆ)','(F)',''),('Class','',''),('Class | Name | Qty | Price | Total','',''),('Complimentary','',''),('Date','',''),('Department','',''),('Device number','',''),('Fiscal','',''),('Need to pay','',''),('Payment, cash','',''),('Printed','',''),('Receipt #','',''),('Receipt number','',''),('Serial','',''),('Signature','',''),('Staff','',''),('Table','',''),('Tax number','',''),('Thank you for visit!','',''),('Total','',''),('ԱՀ','ԱՀ',''),('Ամսաթիվ','Date',''),('Այդ թվում սպասարկում','Service fee included',''),('Բաժին','Բաժին',''),('ԳՀ','ԳՀ',''),('Դաս','Class',''),('Դաս | Անվանում | Քանակ | Գին | Ընդամենը','Class | Name | Qty | Price | Total',''),('Դեբիտորական պարտք','City Ledger',''),('Ենթակա է վճարման','Need to pay',''),('Ընդամենը','Total',''),('ԿՀ','ԿՀ',''),('Կցել սենյակի հաշվին','Transfer to room',''),('Հաշիվ №','Receipt #',''),('Հյուրասիրություն','Complimentary',''),('ՀՎՀՀ','ՀՎՀՀ',''),('Նախաճաշ','Breakfast',''),('Շնորհակալություն այցելության համար','Thank For Your Visit',''),('Սեղան','Table',''),('Սպասարկող','Staff',''),('Սպասարկում','Service',''),('Ստորագրություն','Signature',''),('Վերջնահաշիվ','Total',''),('Վճարումը անկանխիկ','Payment, card',''),('Վճարումը առնձերն','Payment, cash',''),('Տպված','Printed',''),('Փոխանցում բանկով',NULL,NULL),('Ֆիսկալ','Ֆիսկալ','');
/*!40000 ALTER TABLE `s_translator` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_user`
--

DROP TABLE IF EXISTS `s_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_user` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_group` int(11) DEFAULT NULL,
  `f_state` smallint(6) DEFAULT NULL,
  `f_first` tinytext,
  `f_last` tinytext,
  `f_login` tinytext,
  `f_password` tinytext,
  `f_altPassword` tinytext,
  `f_config` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_group` (`f_group`),
  KEY `fk_user_state_idx` (`f_state`),
  CONSTRAINT `fk_user_group` FOREIGN KEY (`f_group`) REFERENCES `s_user_group` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_user_state` FOREIGN KEY (`f_state`) REFERENCES `s_user_state` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_user`
--

LOCK TABLES `s_user` WRITE;
/*!40000 ALTER TABLE `s_user` DISABLE KEYS */;
INSERT INTO `s_user` VALUES (1,1,1,'Admin','','admin','4a7d1ed414474e4033ac29ccb8653d9b','a35fe7f7fe8217b4369a0af4244d1fca',NULL),(4,4,1,'Վանիկ','Մխիթարյան','vano','81dc9bdb52d04dc20036dbd8313ed055',NULL,5);
/*!40000 ALTER TABLE `s_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_user_access`
--

DROP TABLE IF EXISTS `s_user_access`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_user_access` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_group` int(11) DEFAULT NULL,
  `f_key` int(11) DEFAULT NULL,
  `f_value` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_id`),
  KEY `idx_group` (`f_group`),
  KEY `idx_value` (`f_value`),
  CONSTRAINT `fk_user_access_group` FOREIGN KEY (`f_group`) REFERENCES `s_user_group` (`f_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_user_access`
--

LOCK TABLES `s_user_access` WRITE;
/*!40000 ALTER TABLE `s_user_access` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_user_access` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_user_group`
--

DROP TABLE IF EXISTS `s_user_group`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_user_group` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_user_group`
--

LOCK TABLES `s_user_group` WRITE;
/*!40000 ALTER TABLE `s_user_group` DISABLE KEYS */;
INSERT INTO `s_user_group` VALUES (1,'Administrator'),(4,'Առաքիչ');
/*!40000 ALTER TABLE `s_user_group` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_user_state`
--

DROP TABLE IF EXISTS `s_user_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_user_state` (
  `f_id` smallint(6) NOT NULL,
  `f_name` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_user_state`
--

LOCK TABLES `s_user_state` WRITE;
/*!40000 ALTER TABLE `s_user_state` DISABLE KEYS */;
INSERT INTO `s_user_state` VALUES (1,'Աշխատող'),(2,'Հեռացված');
/*!40000 ALTER TABLE `s_user_state` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `s_waiter_reports`
--

DROP TABLE IF EXISTS `s_waiter_reports`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `s_waiter_reports` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` tinytext,
  `f_sql` text,
  `f_logo` text,
  `f_subtotal` tinytext,
  `f_total` tinytext,
  PRIMARY KEY (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `s_waiter_reports`
--

LOCK TABLES `s_waiter_reports` WRITE;
/*!40000 ALTER TABLE `s_waiter_reports` DISABLE KEYS */;
/*!40000 ALTER TABLE `s_waiter_reports` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2020-04-10 12:31:04
