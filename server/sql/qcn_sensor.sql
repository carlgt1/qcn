-- MySQL dump 10.13  Distrib 5.5.16, for linux2.6 (x86_64)
--
-- Host: db-private    Database: sensor
-- ------------------------------------------------------
-- Server version	5.5.16-log

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
-- Table structure for table `qcn_sensor`
--

DROP TABLE IF EXISTS `qcn_sensor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_sensor` (
  `id` smallint(6) NOT NULL,
  `is_usb` tinyint(1) NOT NULL DEFAULT '0',
  `is_gps` tinyint(1) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `qcn_sensor`
--

LOCK TABLES `qcn_sensor` WRITE;
/*!40000 ALTER TABLE `qcn_sensor` DISABLE KEYS */;
INSERT INTO `qcn_sensor` VALUES (0,0,0,'Not Found'),(1,0,0,'Mac PPC 1'),(2,0,0,'Mac PPC 2'),(3,0,0,'Mac PPC 3'),(4,0,0,'Mac Intel'),(5,0,0,'Lenovo Thinkpad'),(6,0,0,'HP Laptop'),(100,1,0,'JoyWarrior 24F8 USB'),(101,1,0,'MotionNode Accel USB'),(102,1,0,'ONavi 1 USB'),(103,1,0,'JoyWarrior 24F14 USB'),(104,1,0,'ONavi A 12-bit USB'),(105,1,0,'ONavi B 16-bit USB'),(106,1,0,'ONavi C 24-bit USB'),(107,1,0,'Phidgets 1056 228ug USB'),(108,1,0,'Phidgets 1042 976ug USB'),(109,1,0,'Phidgets 1044 76ug USB'),(110,1,0,'Phidgets 1041 976ug USB'),(111,1,0,'Phidgets 1043 76ug USB'),(1000,1,1,'Phidgets 1040 GPS USB');
/*!40000 ALTER TABLE `qcn_sensor` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-03-25  8:30:22
