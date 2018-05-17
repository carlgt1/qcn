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
-- Table structure for table `qcn_align`
--

DROP TABLE IF EXISTS `qcn_align`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_align` (
  `id` smallint(6) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_constant`
--

DROP TABLE IF EXISTS `qcn_constant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_constant` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `description` varchar(64) NOT NULL,
  `value_int` int(11) DEFAULT NULL,
  `value_text` varchar(64) DEFAULT NULL,
  `value_float` float DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `qcn_constant_description` (`description`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_continual_filename`
--

DROP TABLE IF EXISTS `qcn_continual_filename`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_continual_filename` (
  `filename` varchar(255) DEFAULT NULL,
  KEY `list_continual_filename` (`filename`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_country_latlng`
--

DROP TABLE IF EXISTS `qcn_country_latlng`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_country_latlng` (
  `id` varchar(4) NOT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_finalstats`
--

DROP TABLE IF EXISTS `qcn_finalstats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_finalstats` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `resultid` int(11) NOT NULL,
  `time_received` double NOT NULL DEFAULT '0',
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_finalstats_resultid` (`resultid`)
) ENGINE=InnoDB AUTO_INCREMENT=686234 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_geo_ipaddr`
--

DROP TABLE IF EXISTS `qcn_geo_ipaddr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_geo_ipaddr` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ipaddr` varchar(32) NOT NULL,
  `time_lookup` double DEFAULT NULL,
  `country` varchar(32) DEFAULT NULL,
  `region` varchar(32) DEFAULT NULL,
  `city` varchar(32) DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_geo_ipaddr_ipaddr` (`ipaddr`)
) ENGINE=InnoDB AUTO_INCREMENT=103615 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_host_ipaddr`
--

DROP TABLE IF EXISTS `qcn_host_ipaddr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_host_ipaddr` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `hostid` int(11) NOT NULL,
  `ipaddr` varchar(32) NOT NULL DEFAULT '',
  `location` varchar(32) NOT NULL DEFAULT '',
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `levelvalue` float DEFAULT NULL,
  `levelid` smallint(6) DEFAULT NULL,
  `alignid` smallint(6) DEFAULT NULL,
  `geoipaddrid` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_host_ipaddr_id` (`hostid`,`ipaddr`,`geoipaddrid`)
) ENGINE=InnoDB AUTO_INCREMENT=177124 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_kml_region`
--

DROP TABLE IF EXISTS `qcn_kml_region`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_kml_region` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(64) DEFAULT NULL,
  `filename` varchar(255) DEFAULT NULL,
  `data` blob,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_level`
--

DROP TABLE IF EXISTS `qcn_level`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_level` (
  `id` smallint(6) NOT NULL,
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_post`
--

DROP TABLE IF EXISTS `qcn_post`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_post` (
  `id` int(11) NOT NULL,
  `where_clause` varchar(255) NOT NULL,
  `url` varchar(255) NOT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '1',
  `contact_name` varchar(255) DEFAULT NULL,
  `contact_email` varchar(255) DEFAULT NULL,
  `contact_address` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_quake`
--

DROP TABLE IF EXISTS `qcn_quake`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_quake` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time_utc` double DEFAULT NULL,
  `magnitude` double DEFAULT NULL,
  `depth_km` double DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `description` varchar(256) DEFAULT NULL,
  `processed` tinyint(1) DEFAULT NULL,
  `url` varchar(256) DEFAULT NULL,
  `guid` varchar(256) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_quake_guid` (`guid`),
  KEY `qcn_quake_magnitude` (`magnitude`),
  KEY `qcn_time_utc` (`time_utc`),
  KEY `qcn_latitude` (`latitude`),
  KEY `qcn_longitude` (`longitude`)
) ENGINE=InnoDB AUTO_INCREMENT=64911 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_ramp_coordinator`
--

DROP TABLE IF EXISTS `qcn_ramp_coordinator`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_ramp_coordinator` (
  `id` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `receive_distribute` tinyint(1) DEFAULT NULL,
  `help_troubleshoot` tinyint(1) DEFAULT NULL,
  `enlist_volunteers` tinyint(1) DEFAULT NULL,
  `how_many` int(11) DEFAULT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '1',
  `comments` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_ramp_coordinator_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_ramp_participant`
--

DROP TABLE IF EXISTS `qcn_ramp_participant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_ramp_participant` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `qcn_ramp_coordinator_id` int(11) DEFAULT NULL,
  `fname` varchar(64) DEFAULT NULL,
  `lname` varchar(64) DEFAULT NULL,
  `email_addr` varchar(100) DEFAULT NULL,
  `addr1` varchar(64) DEFAULT NULL,
  `addr2` varchar(64) DEFAULT NULL,
  `city` varchar(64) DEFAULT NULL,
  `region` varchar(64) DEFAULT NULL,
  `postcode` varchar(20) DEFAULT NULL,
  `country` varchar(64) DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `gmap_placename` varchar(64) DEFAULT NULL,
  `gmap_view_level` int(11) DEFAULT NULL,
  `gmap_view_type` int(11) DEFAULT NULL,
  `phone` varchar(64) DEFAULT NULL,
  `fax` varchar(64) DEFAULT NULL,
  `bshare_coord` tinyint(1) DEFAULT NULL,
  `bshare_map` tinyint(1) DEFAULT NULL,
  `bshare_ups` tinyint(1) DEFAULT NULL,
  `cpu_type` varchar(20) DEFAULT NULL,
  `cpu_os` varchar(20) DEFAULT NULL,
  `cpu_age` int(11) DEFAULT NULL,
  `cpu_floor` int(11) DEFAULT NULL,
  `cpu_admin` tinyint(1) DEFAULT NULL,
  `cpu_permission` tinyint(1) DEFAULT NULL,
  `cpu_firewall` tinyint(1) DEFAULT NULL,
  `cpu_proxy` tinyint(1) DEFAULT NULL,
  `cpu_internet` tinyint(1) DEFAULT NULL,
  `cpu_unint_power` tinyint(1) DEFAULT NULL,
  `sensor_distribute` tinyint(1) DEFAULT NULL,
  `comments` blob,
  `active` tinyint(1) NOT NULL DEFAULT '1',
  `time_edit` int(11) DEFAULT NULL,
  `loc_home` tinyint(1) NOT NULL DEFAULT '0',
  `loc_business` tinyint(1) NOT NULL DEFAULT '0',
  `loc_affix_perm` tinyint(1) NOT NULL DEFAULT '0',
  `loc_self_install` tinyint(1) NOT NULL DEFAULT '0',
  `loc_day_install_sunday` tinyint(1) DEFAULT '0',
  `loc_time_install_sunday` varchar(20) DEFAULT '',
  `loc_day_install_monday` tinyint(1) DEFAULT '0',
  `loc_time_install_monday` varchar(20) DEFAULT '',
  `loc_day_install_tuesday` tinyint(1) DEFAULT '0',
  `loc_time_install_tuesday` varchar(20) DEFAULT '',
  `loc_day_install_wednesday` tinyint(1) DEFAULT '0',
  `loc_time_install_wednesday` varchar(20) DEFAULT '',
  `loc_day_install_thursday` tinyint(1) DEFAULT '0',
  `loc_time_install_thursday` varchar(20) DEFAULT '',
  `loc_day_install_friday` tinyint(1) DEFAULT '0',
  `loc_time_install_friday` varchar(20) DEFAULT '',
  `loc_day_install_saturday` tinyint(1) DEFAULT '0',
  `loc_time_install_saturday` varchar(20) DEFAULT '',
  `loc_years_host` smallint(6) DEFAULT NULL,
  `ramp_type` varchar(5) NOT NULL DEFAULT '',
  `kml_regionid` int(11) NOT NULL DEFAULT '0',
  `quake_damage` varchar(5) DEFAULT '',
  `liquefaction` tinyint(1) DEFAULT '0',
  `time_added` int(11) DEFAULT NULL,
  `completed` tinyint(1) DEFAULT '0',
  `time_completed` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_ramp_participant_userid` (`userid`),
  KEY `qcn_ramp_participant_kml_regionid` (`kml_regionid`),
  KEY `qcn_ramp_participant_country` (`country`),
  KEY `qcn_ramp_participant_time_added` (`time_added`),
  KEY `qcn_ramp_participant_time_completed` (`time_completed`),
  KEY `qcn_ramp_participant_completed` (`completed`),
  KEY `qcn_ramp_participant_lname` (`lname`)
) ENGINE=InnoDB AUTO_INCREMENT=2260 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_recalcresult`
--

DROP TABLE IF EXISTS `qcn_recalcresult`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_recalcresult` (
  `resultid` int(11) NOT NULL,
  `weight` double DEFAULT NULL,
  `total_credit` double DEFAULT NULL,
  `time_received` double DEFAULT NULL,
  PRIMARY KEY (`resultid`),
  KEY `recalc_result` (`resultid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_reds`
--

DROP TABLE IF EXISTS `qcn_reds`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_reds` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `name` varchar(64) DEFAULT NULL,
  `email_addr` varchar(100) DEFAULT NULL,
  `magnitude_min_global` double DEFAULT NULL,
  `magnitude_min_local` double DEFAULT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '1',
  `time_edit` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_reds_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_sensor`
--

DROP TABLE IF EXISTS `qcn_sensor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_sensor` (
  `id` smallint(6) NOT NULL,
  `is_usb` tinyint(1) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_showhostlocation`
--

DROP TABLE IF EXISTS `qcn_showhostlocation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_showhostlocation` (
  `hostid` int(11) NOT NULL,
  PRIMARY KEY (`hostid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_stats`
--

DROP TABLE IF EXISTS `qcn_stats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_stats` (
  `userid` int(11) DEFAULT NULL,
  `hostid` int(11) DEFAULT NULL,
  `teamid` int(11) DEFAULT NULL,
  `resultid` int(11) DEFAULT NULL,
  `total_credit` double DEFAULT NULL,
  `weight` double DEFAULT NULL,
  `expavg_time` double DEFAULT NULL,
  KEY `qcn_stats_userid` (`userid`),
  KEY `qcn_stats_hostid` (`hostid`),
  KEY `qcn_stats_teamid` (`teamid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_trigger`
--

DROP TABLE IF EXISTS `qcn_trigger`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_trigger` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `hostid` int(11) NOT NULL,
  `ipaddr` varchar(32) NOT NULL,
  `result_name` varchar(64) NOT NULL,
  `time_trigger` double DEFAULT NULL,
  `time_received` double DEFAULT NULL,
  `time_sync` double DEFAULT NULL,
  `sync_offset` double DEFAULT NULL,
  `significance` double DEFAULT NULL,
  `magnitude` double DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `levelvalue` float DEFAULT NULL,
  `levelid` smallint(6) DEFAULT NULL,
  `alignid` smallint(6) DEFAULT NULL,
  `file` varchar(64) DEFAULT NULL,
  `dt` float DEFAULT NULL,
  `numreset` int(6) DEFAULT NULL,
  `qcn_sensorid` int(3) DEFAULT NULL,
  `sw_version` varchar(8) DEFAULT NULL,
  `os_type` varchar(8) DEFAULT NULL,
  `qcn_quakeid` int(11) DEFAULT NULL,
  `time_filereq` double DEFAULT NULL,
  `received_file` tinyint(1) DEFAULT NULL,
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0',
  `varietyid` smallint(6) NOT NULL DEFAULT '0',
  `flag` tinyint(1) NOT NULL DEFAULT '0',
  `hostipaddrid` int(11) NOT NULL DEFAULT '0',
  `geoipaddrid` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `qcn_trigger_file` (`file`),
  KEY `qcn_trigger_type_sensor` (`qcn_sensorid`),
  KEY `qcn_trigger_usgs_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_flag` (`flag`),
  KEY `qcn_trigger_time_trigger` (`time_trigger`),
  KEY `qcn_trigger_hostid` (`hostid`),
  KEY `qcn_trigger_time_filereq` (`time_filereq`),
  KEY `qcn_trigger_received_file` (`received_file`),
  KEY `qcn_trigger_varietyid` (`varietyid`),
  KEY `qcn_trigger_latitude` (`latitude`),
  KEY `qcn_trigger_longitude` (`longitude`),
  KEY `qcn_trigger_result_name` (`result_name`)
) ENGINE=InnoDB AUTO_INCREMENT=84135310 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_trigger_followup`
--

DROP TABLE IF EXISTS `qcn_trigger_followup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_trigger_followup` (
  `id` int(11) NOT NULL,
  `mxy1p` float DEFAULT NULL,
  `mz1p` float DEFAULT NULL,
  `mxy1a` float DEFAULT NULL,
  `mz1a` float DEFAULT NULL,
  `mxy2a` float DEFAULT NULL,
  `mz2a` float DEFAULT NULL,
  `mxy4a` float DEFAULT NULL,
  `mz4a` float DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_variety`
--

DROP TABLE IF EXISTS `qcn_variety`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_variety` (
  `id` smallint(6) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-01-08 22:42:32
