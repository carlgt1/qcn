-- MySQL dump 10.13  Distrib 5.5.16, for linux2.6 (x86_64)
--  mysqldump -u root -p --no-data sensor > qcn_schema_mysqldump.sql
-- Host: localhost    Database: sensor
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
-- Table structure for table `app`
--

DROP TABLE IF EXISTS `app`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `app` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `min_version` int(11) NOT NULL DEFAULT '0',
  `deprecated` smallint(6) NOT NULL DEFAULT '0',
  `user_friendly_name` varchar(254) NOT NULL,
  `homogeneous_redundancy` smallint(6) NOT NULL DEFAULT '0',
  `weight` double NOT NULL DEFAULT '1',
  `beta` smallint(6) NOT NULL DEFAULT '0',
  `target_nresults` smallint(6) NOT NULL DEFAULT '0',
  `min_avg_pfc` double NOT NULL DEFAULT '1',
  `host_scale_check` tinyint(4) NOT NULL,
  `homogeneous_app_version` tinyint(4) NOT NULL DEFAULT '0',
  `non_cpu_intensive` tinyint(4) NOT NULL DEFAULT '0',
  `locality_scheduling` int(11) NOT NULL DEFAULT '0',
  `n_size_classes` smallint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `app_version`
--

DROP TABLE IF EXISTS `app_version`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `app_version` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `appid` int(11) NOT NULL,
  `version_num` int(11) NOT NULL,
  `platformid` int(11) NOT NULL,
  `xml_doc` mediumblob,
  `min_core_version` int(11) NOT NULL DEFAULT '0',
  `max_core_version` int(11) NOT NULL DEFAULT '0',
  `deprecated` tinyint(4) NOT NULL DEFAULT '0',
  `plan_class` varchar(254) NOT NULL DEFAULT '',
  `pfc_n` double NOT NULL DEFAULT '0',
  `pfc_avg` double NOT NULL DEFAULT '0',
  `pfc_scale` double NOT NULL DEFAULT '0',
  `expavg_credit` double NOT NULL DEFAULT '0',
  `expavg_time` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `apvp` (`appid`,`platformid`,`version_num`,`plan_class`)
) ENGINE=InnoDB AUTO_INCREMENT=978 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `app_version_backup`
--

DROP TABLE IF EXISTS `app_version_backup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `app_version_backup` (
  `id` int(11) NOT NULL DEFAULT '0',
  `create_time` int(11) NOT NULL,
  `appid` int(11) NOT NULL,
  `version_num` int(11) NOT NULL,
  `platformid` int(11) NOT NULL,
  `xml_doc` mediumblob,
  `min_core_version` int(11) NOT NULL DEFAULT '0',
  `max_core_version` int(11) NOT NULL DEFAULT '0',
  `deprecated` tinyint(4) NOT NULL DEFAULT '0',
  `plan_class` varchar(254) NOT NULL DEFAULT '',
  `pfc_n` double NOT NULL DEFAULT '0',
  `pfc_avg` double NOT NULL DEFAULT '0',
  `pfc_scale` double NOT NULL DEFAULT '0',
  `expavg_credit` double NOT NULL DEFAULT '0',
  `expavg_time` double NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `app_version_bak`
--

DROP TABLE IF EXISTS `app_version_bak`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `app_version_bak` (
  `id` int(11) NOT NULL DEFAULT '0',
  `create_time` int(11) NOT NULL,
  `appid` int(11) NOT NULL,
  `version_num` int(11) NOT NULL,
  `platformid` int(11) NOT NULL,
  `xml_doc` mediumblob,
  `min_core_version` int(11) NOT NULL DEFAULT '0',
  `max_core_version` int(11) NOT NULL DEFAULT '0',
  `deprecated` tinyint(4) NOT NULL DEFAULT '0',
  `plan_class` varchar(254) NOT NULL DEFAULT '',
  `pfc_n` double NOT NULL DEFAULT '0',
  `pfc_avg` double NOT NULL DEFAULT '0',
  `pfc_scale` double NOT NULL DEFAULT '0',
  `expavg_credit` double NOT NULL DEFAULT '0',
  `expavg_time` double NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `assignment`
--

DROP TABLE IF EXISTS `assignment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `assignment` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `target_id` int(11) NOT NULL,
  `target_type` int(11) NOT NULL,
  `multi` tinyint(4) NOT NULL,
  `workunitid` int(11) NOT NULL,
  `resultid` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `asgn_target` (`target_type`,`target_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `badge`
--

DROP TABLE IF EXISTS `badge`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `badge` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `create_time` double NOT NULL,
  `type` tinyint(4) NOT NULL,
  `name` varchar(255) NOT NULL,
  `title` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL,
  `image_url` varchar(255) NOT NULL,
  `level` varchar(255) NOT NULL,
  `tags` varchar(255) NOT NULL,
  `sql_rule` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `badge_team`
--

DROP TABLE IF EXISTS `badge_team`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `badge_team` (
  `badge_id` int(11) NOT NULL,
  `team_id` int(11) NOT NULL,
  `create_time` double NOT NULL,
  `reassign_time` double NOT NULL,
  UNIQUE KEY `team_id` (`team_id`,`badge_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `badge_user`
--

DROP TABLE IF EXISTS `badge_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `badge_user` (
  `badge_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `create_time` double NOT NULL,
  `reassign_time` double NOT NULL,
  UNIQUE KEY `user_id` (`user_id`,`badge_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `banishment_vote`
--

DROP TABLE IF EXISTS `banishment_vote`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `banishment_vote` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `modid` int(11) NOT NULL,
  `start_time` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `banishment_votes`
--

DROP TABLE IF EXISTS `banishment_votes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `banishment_votes` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `voteid` int(11) NOT NULL,
  `modid` int(11) NOT NULL,
  `time` int(11) NOT NULL,
  `yes` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `batch`
--

DROP TABLE IF EXISTS `batch`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `batch` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `logical_start_time` double NOT NULL,
  `logical_end_time` double NOT NULL,
  `est_completion_time` double NOT NULL,
  `njobs` int(11) NOT NULL,
  `fraction_done` double NOT NULL,
  `nerror_jobs` int(11) NOT NULL,
  `state` int(11) NOT NULL,
  `completion_time` double NOT NULL,
  `credit_estimate` double NOT NULL,
  `credit_canonical` double NOT NULL,
  `credit_total` double NOT NULL,
  `name` varchar(255) NOT NULL,
  `app_id` int(11) NOT NULL,
  `project_state` int(11) NOT NULL,
  `description` varchar(255) NOT NULL,
  `expire_time` double NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_course`
--

DROP TABLE IF EXISTS `bolt_course`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_course` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `short_name` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `description` text NOT NULL,
  `hidden` tinyint(4) NOT NULL,
  `bossa_app_id` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_enrollment`
--

DROP TABLE IF EXISTS `bolt_enrollment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_enrollment` (
  `create_time` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `last_view_id` int(11) NOT NULL,
  `mastery` double NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_question`
--

DROP TABLE IF EXISTS `bolt_question`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_question` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `name` varchar(255) NOT NULL,
  `question` text NOT NULL,
  `state` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_refresh`
--

DROP TABLE IF EXISTS `bolt_refresh`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_refresh` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `name` varchar(255) NOT NULL,
  `last_view_id` int(11) NOT NULL,
  `xset_result_id` int(11) NOT NULL,
  `due_time` int(11) NOT NULL,
  `count` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_result`
--

DROP TABLE IF EXISTS `bolt_result`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_result` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `view_id` int(11) NOT NULL,
  `item_name` varchar(255) NOT NULL,
  `score` double NOT NULL,
  `response` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=33 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_select_finished`
--

DROP TABLE IF EXISTS `bolt_select_finished`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_select_finished` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  `name` varchar(255) NOT NULL,
  `selected_unit` varchar(255) NOT NULL,
  `view_id` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_user`
--

DROP TABLE IF EXISTS `bolt_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_user` (
  `user_id` int(11) NOT NULL,
  `birth_year` int(11) NOT NULL,
  `sex` tinyint(4) NOT NULL,
  `flags` int(11) NOT NULL,
  `attrs` text NOT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_view`
--

DROP TABLE IF EXISTS `bolt_view`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_view` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `item_name` varchar(255) NOT NULL,
  `state` text NOT NULL,
  `mode` int(11) NOT NULL,
  `phase` int(11) NOT NULL,
  `action` int(11) NOT NULL,
  `start_time` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  `prev_view_id` int(11) NOT NULL,
  `fraction_done` double NOT NULL,
  `result_id` int(11) NOT NULL,
  `refresh_id` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=165 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bolt_xset_result`
--

DROP TABLE IF EXISTS `bolt_xset_result`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `bolt_xset_result` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `start_time` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  `name` varchar(255) NOT NULL,
  `score` double NOT NULL,
  `view_id` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `category`
--

DROP TABLE IF EXISTS `category`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `category` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `orderID` int(11) NOT NULL,
  `lang` int(11) NOT NULL,
  `name` varchar(254) CHARACTER SET latin1 COLLATE latin1_bin DEFAULT NULL,
  `is_helpdesk` smallint(6) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `cat1` (`name`,`is_helpdesk`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `credit_multiplier`
--

DROP TABLE IF EXISTS `credit_multiplier`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `credit_multiplier` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `appid` int(11) NOT NULL,
  `time` int(11) NOT NULL,
  `multiplier` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `credited_job`
--

DROP TABLE IF EXISTS `credited_job`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `credited_job` (
  `userid` int(11) NOT NULL,
  `workunitid` bigint(20) NOT NULL,
  UNIQUE KEY `credited_job_user_wu` (`userid`,`workunitid`),
  KEY `credited_job_user` (`userid`),
  KEY `credited_job_wu` (`workunitid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `donation_items`
--

DROP TABLE IF EXISTS `donation_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `donation_items` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `item_name` varchar(32) NOT NULL,
  `title` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL,
  `required` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `donation_paypal`
--

DROP TABLE IF EXISTS `donation_paypal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `donation_paypal` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `order_time` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `email_addr` varchar(255) NOT NULL,
  `order_amount` double(6,2) NOT NULL,
  `processed` tinyint(4) NOT NULL DEFAULT '0',
  `payment_time` int(11) NOT NULL,
  `item_name` varchar(255) NOT NULL,
  `item_number` varchar(255) NOT NULL,
  `payment_status` varchar(255) NOT NULL,
  `payment_amount` double(6,2) NOT NULL,
  `payment_fee` double(5,2) DEFAULT NULL,
  `payment_currency` varchar(255) NOT NULL,
  `txn_id` varchar(255) NOT NULL,
  `receiver_email` varchar(255) NOT NULL,
  `payer_email` varchar(255) NOT NULL,
  `payer_name` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `forum`
--

DROP TABLE IF EXISTS `forum`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `forum` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `category` int(11) NOT NULL,
  `orderID` int(11) NOT NULL,
  `title` varchar(254) NOT NULL,
  `description` varchar(254) NOT NULL,
  `timestamp` int(11) NOT NULL,
  `threads` int(11) NOT NULL,
  `posts` int(11) NOT NULL,
  `rate_min_expavg_credit` int(11) NOT NULL,
  `rate_min_total_credit` int(11) NOT NULL,
  `post_min_interval` int(11) NOT NULL,
  `post_min_expavg_credit` int(11) NOT NULL,
  `post_min_total_credit` int(11) NOT NULL,
  `is_dev_blog` tinyint(4) NOT NULL DEFAULT '0',
  `parent_type` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `pct` (`parent_type`,`category`,`title`)
) ENGINE=InnoDB AUTO_INCREMENT=32 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `forum_logging`
--

DROP TABLE IF EXISTS `forum_logging`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `forum_logging` (
  `userid` int(11) NOT NULL DEFAULT '0',
  `threadid` int(11) NOT NULL DEFAULT '0',
  `timestamp` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`userid`,`threadid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `forum_preferences`
--

DROP TABLE IF EXISTS `forum_preferences`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `forum_preferences` (
  `userid` int(11) NOT NULL DEFAULT '0',
  `signature` varchar(254) NOT NULL DEFAULT '',
  `posts` int(11) NOT NULL DEFAULT '0',
  `last_post` int(11) NOT NULL,
  `avatar` varchar(254) NOT NULL DEFAULT '',
  `hide_avatars` tinyint(4) NOT NULL DEFAULT '0',
  `forum_sorting` int(11) NOT NULL,
  `thread_sorting` int(11) NOT NULL,
  `no_signature_by_default` tinyint(4) NOT NULL DEFAULT '1',
  `images_as_links` tinyint(4) NOT NULL DEFAULT '0',
  `link_popup` tinyint(4) NOT NULL DEFAULT '0',
  `mark_as_read_timestamp` int(11) NOT NULL DEFAULT '0',
  `special_user` char(12) NOT NULL DEFAULT '0',
  `jump_to_unread` tinyint(4) NOT NULL DEFAULT '1',
  `hide_signatures` tinyint(4) NOT NULL DEFAULT '0',
  `rated_posts` varchar(254) NOT NULL,
  `low_rating_threshold` int(11) NOT NULL DEFAULT '-25',
  `high_rating_threshold` int(11) NOT NULL DEFAULT '5',
  `minimum_wrap_postcount` int(11) NOT NULL DEFAULT '100',
  `display_wrap_postcount` int(11) NOT NULL DEFAULT '75',
  `ignorelist` varchar(254) NOT NULL,
  `ignore_sticky_posts` tinyint(4) NOT NULL DEFAULT '0',
  `banished_until` int(11) NOT NULL DEFAULT '0',
  `pm_notification` tinyint(4) NOT NULL DEFAULT '0',
  `highlight_special` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `friend`
--

DROP TABLE IF EXISTS `friend`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `friend` (
  `user_src` int(11) NOT NULL,
  `user_dest` int(11) NOT NULL,
  `message` varchar(255) NOT NULL,
  `create_time` int(11) NOT NULL,
  `reciprocated` tinyint(4) NOT NULL,
  UNIQUE KEY `friend_u` (`user_src`,`user_dest`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `host`
--

DROP TABLE IF EXISTS `host`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `host` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `rpc_seqno` int(11) NOT NULL,
  `rpc_time` int(11) NOT NULL,
  `total_credit` double NOT NULL,
  `expavg_credit` double NOT NULL,
  `expavg_time` double NOT NULL,
  `timezone` int(11) NOT NULL,
  `domain_name` varchar(254) DEFAULT NULL,
  `serialnum` varchar(254) DEFAULT NULL,
  `last_ip_addr` varchar(254) DEFAULT NULL,
  `nsame_ip_addr` int(11) NOT NULL,
  `on_frac` double NOT NULL,
  `connected_frac` double NOT NULL,
  `active_frac` double NOT NULL,
  `cpu_efficiency` double NOT NULL,
  `duration_correction_factor` double NOT NULL,
  `p_ncpus` int(11) NOT NULL,
  `p_vendor` varchar(254) DEFAULT NULL,
  `p_model` varchar(254) DEFAULT NULL,
  `p_fpops` double NOT NULL,
  `p_iops` double NOT NULL,
  `p_membw` double NOT NULL,
  `os_name` varchar(254) DEFAULT NULL,
  `os_version` varchar(254) DEFAULT NULL,
  `m_nbytes` double NOT NULL,
  `m_cache` double NOT NULL,
  `m_swap` double NOT NULL,
  `d_total` double NOT NULL,
  `d_free` double NOT NULL,
  `d_boinc_used_total` double NOT NULL,
  `d_boinc_used_project` double NOT NULL,
  `d_boinc_max` double NOT NULL,
  `n_bwup` double NOT NULL,
  `n_bwdown` double NOT NULL,
  `credit_per_cpu_sec` double NOT NULL,
  `venue` varchar(254) NOT NULL,
  `nresults_today` int(11) NOT NULL,
  `avg_turnaround` double NOT NULL,
  `host_cpid` varchar(254) DEFAULT NULL,
  `external_ip_addr` varchar(254) DEFAULT NULL,
  `max_results_day` int(11) NOT NULL,
  `error_rate` double NOT NULL DEFAULT '0',
  `product_name` varchar(254) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `host_user` (`userid`),
  KEY `host_avg` (`expavg_credit`),
  KEY `host_tot` (`total_credit`)
) ENGINE=InnoDB AUTO_INCREMENT=49724 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `host_app_version`
--

DROP TABLE IF EXISTS `host_app_version`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `host_app_version` (
  `host_id` int(11) NOT NULL,
  `app_version_id` int(11) NOT NULL,
  `pfc_n` double NOT NULL,
  `pfc_avg` double NOT NULL,
  `et_n` double NOT NULL,
  `et_avg` double NOT NULL,
  `et_var` double NOT NULL,
  `et_q` double NOT NULL,
  `max_jobs_per_day` int(11) NOT NULL,
  `n_jobs_today` int(11) NOT NULL,
  `turnaround_n` double NOT NULL,
  `turnaround_avg` double NOT NULL,
  `turnaround_var` double NOT NULL,
  `turnaround_q` double NOT NULL,
  `consecutive_valid` int(11) NOT NULL,
  UNIQUE KEY `hap` (`host_id`,`app_version_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `host_ip`
--

DROP TABLE IF EXISTS `host_ip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `host_ip` (
  `id` int(11) NOT NULL,
  `hostid` int(11) DEFAULT NULL,
  `ipaddr` varchar(32) DEFAULT NULL,
  `location` varchar(32) DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `host_ip_id` (`hostid`,`ipaddr`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `job_file`
--

DROP TABLE IF EXISTS `job_file`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `job_file` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `md5` char(64) NOT NULL,
  `create_time` double NOT NULL,
  `delete_time` double NOT NULL,
  PRIMARY KEY (`id`),
  KEY `md5` (`md5`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `msg_from_host`
--

DROP TABLE IF EXISTS `msg_from_host`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `msg_from_host` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `variety` varchar(254) NOT NULL,
  `handled` smallint(6) NOT NULL,
  `xml` mediumtext,
  PRIMARY KEY (`id`),
  KEY `message_handled` (`handled`)
) ENGINE=InnoDB AUTO_INCREMENT=959836 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `msg_to_host`
--

DROP TABLE IF EXISTS `msg_to_host`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `msg_to_host` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `variety` varchar(254) NOT NULL,
  `handled` smallint(6) NOT NULL,
  `xml` mediumtext,
  PRIMARY KEY (`id`),
  KEY `msg_to_host` (`hostid`,`handled`)
) ENGINE=InnoDB AUTO_INCREMENT=5919 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `notify`
--

DROP TABLE IF EXISTS `notify`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `notify` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `opaque` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `notify_un` (`userid`,`type`,`opaque`)
) ENGINE=InnoDB AUTO_INCREMENT=4677 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `platform`
--

DROP TABLE IF EXISTS `platform`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `platform` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `user_friendly_name` varchar(254) NOT NULL,
  `deprecated` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `post`
--

DROP TABLE IF EXISTS `post`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `post` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `thread` int(11) NOT NULL,
  `user` int(11) NOT NULL,
  `timestamp` int(11) NOT NULL,
  `content` text NOT NULL,
  `modified` int(11) NOT NULL,
  `parent_post` int(11) NOT NULL,
  `score` double NOT NULL,
  `votes` int(11) NOT NULL,
  `signature` tinyint(4) NOT NULL DEFAULT '0',
  `hidden` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `post_user` (`user`),
  KEY `post_thread` (`thread`),
  FULLTEXT KEY `post_content` (`content`)
) ENGINE=MyISAM AUTO_INCREMENT=5766 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `post_ratings`
--

DROP TABLE IF EXISTS `post_ratings`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `post_ratings` (
  `post` int(11) NOT NULL,
  `user` int(11) NOT NULL,
  `rating` tinyint(4) NOT NULL,
  PRIMARY KEY (`post`,`user`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `private_messages`
--

DROP TABLE IF EXISTS `private_messages`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `private_messages` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `senderid` int(11) NOT NULL,
  `date` int(11) NOT NULL,
  `opened` tinyint(4) NOT NULL DEFAULT '0',
  `subject` varchar(255) NOT NULL,
  `content` text NOT NULL,
  PRIMARY KEY (`id`),
  KEY `userid` (`userid`)
) ENGINE=MyISAM AUTO_INCREMENT=1439 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `profile`
--

DROP TABLE IF EXISTS `profile`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `profile` (
  `userid` int(11) NOT NULL,
  `language` varchar(254) DEFAULT NULL,
  `response1` text,
  `response2` text,
  `has_picture` smallint(6) NOT NULL,
  `recommend` int(11) NOT NULL,
  `reject` int(11) NOT NULL,
  `posts` int(11) NOT NULL,
  `uotd_time` int(11) DEFAULT NULL,
  `verification` int(11) NOT NULL,
  PRIMARY KEY (`userid`),
  UNIQUE KEY `profile_userid` (`userid`),
  KEY `pro_uotd` (`uotd_time`),
  FULLTEXT KEY `profile_reponse` (`response1`,`response2`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `profile_bak`
--

DROP TABLE IF EXISTS `profile_bak`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `profile_bak` (
  `userid` int(11) NOT NULL,
  `language` varchar(254) DEFAULT NULL,
  `response1` text,
  `response2` text,
  `has_picture` smallint(6) NOT NULL,
  `recommend` int(11) NOT NULL,
  `reject` int(11) NOT NULL,
  `posts` int(11) NOT NULL,
  `uotd_time` int(11) DEFAULT NULL,
  `verification` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

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
) ENGINE=InnoDB AUTO_INCREMENT=857378 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_finalstats_bak`
--

DROP TABLE IF EXISTS `qcn_finalstats_bak`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_finalstats_bak` (
  `id` int(11) NOT NULL DEFAULT '0',
  `hostid` int(11) NOT NULL,
  `resultid` int(11) NOT NULL,
  `time_received` double NOT NULL DEFAULT '0',
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_finalstats_nohost`
--

DROP TABLE IF EXISTS `qcn_finalstats_nohost`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_finalstats_nohost` (
  `id` int(11) NOT NULL DEFAULT '0',
  `resultid` int(11) NOT NULL,
  `time_received` double NOT NULL DEFAULT '0',
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
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
) ENGINE=InnoDB AUTO_INCREMENT=133809 DEFAULT CHARSET=latin1;
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
) ENGINE=InnoDB AUTO_INCREMENT=230700 DEFAULT CHARSET=latin1;
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
) ENGINE=InnoDB AUTO_INCREMENT=98063 DEFAULT CHARSET=latin1;
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
) ENGINE=InnoDB AUTO_INCREMENT=2976 DEFAULT CHARSET=latin1;
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
-- Table structure for table `qcn_recalcresult_archive`
--

DROP TABLE IF EXISTS `qcn_recalcresult_archive`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_recalcresult_archive` (
  `resultid` int(11) NOT NULL,
  `weight` double DEFAULT NULL,
  `total_credit` double DEFAULT NULL,
  `time_received` double DEFAULT NULL
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
-- Table structure for table `qcn_remote`
--

DROP TABLE IF EXISTS `qcn_remote`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_remote` (
  `id` int(11) NOT NULL,
  `script_name` varchar(64) NOT NULL,
  `name` varchar(64) NOT NULL,
  `url` varchar(128) NOT NULL,
  `country` varchar(32) DEFAULT NULL,
  `trigger_url` varchar(128) NOT NULL,
  `download_url` varchar(128) NOT NULL,
  `password` varchar(32) NOT NULL,
  `contact_email_1` varchar(128) NOT NULL,
  `contact_email_2` varchar(128) DEFAULT NULL,
  `contact_email_3` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `qcn_script_name` (`script_name`),
  KEY `qcn_remote_name` (`name`),
  KEY `qcn_remote_url` (`url`)
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
  `is_gps` tinyint(1) NOT NULL DEFAULT '0',
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
  `mxy1p` float DEFAULT NULL,
  `mz1p` float DEFAULT NULL,
  `mxy1a` float DEFAULT NULL,
  `mz1a` float DEFAULT NULL,
  `mxy2a` float DEFAULT NULL,
  `mz2a` float DEFAULT NULL,
  `mxy4a` float DEFAULT NULL,
  `mz4a` float DEFAULT NULL,
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
) ENGINE=InnoDB AUTO_INCREMENT=507855012 DEFAULT CHARSET=latin1;
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
-- Table structure for table `qcn_trigger_remote`
--

DROP TABLE IF EXISTS `qcn_trigger_remote`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_trigger_remote` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `qcn_remoteid` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `ipaddr` varchar(32) NOT NULL,
  `result_name` varchar(64) NOT NULL,
  `time_trigger` double DEFAULT NULL,
  `time_received` double DEFAULT NULL,
  `time_sync` double DEFAULT NULL,
  `sync_offset` double DEFAULT NULL,
  `significance` double DEFAULT NULL,
  `magnitude` double DEFAULT NULL,
  `mxy1p` float DEFAULT NULL,
  `mz1p` float DEFAULT NULL,
  `mxy1a` float DEFAULT NULL,
  `mz1a` float DEFAULT NULL,
  `mxy2a` float DEFAULT NULL,
  `mz2a` float DEFAULT NULL,
  `mxy4a` float DEFAULT NULL,
  `mz4a` float DEFAULT NULL,
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
  `runtime_clock` double DEFAULT NULL,
  `runtime_cpu` double DEFAULT NULL,
  `varietyid` smallint(6) NOT NULL DEFAULT '0',
  `flag` tinyint(1) NOT NULL DEFAULT '0',
  `hostipaddrid` int(11) NOT NULL DEFAULT '0',
  `geoipaddrid` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `qcn_trigger_remote_remoteid` (`qcn_remoteid`),
  KEY `qcn_trigger_remote_file` (`file`),
  KEY `qcn_trigger_remote_sensorid` (`qcn_sensorid`),
  KEY `qcn_trigger_remote_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_remote_flag` (`flag`),
  KEY `qcn_trigger_remote_time_trigger` (`time_trigger`),
  KEY `qcn_trigger_remote_hostid` (`hostid`),
  KEY `qcn_trigger_remote_time_filereq` (`time_filereq`),
  KEY `qcn_trigger_remote_received_file` (`received_file`),
  KEY `qcn_trigger_remote_varietyid` (`varietyid`),
  KEY `qcn_trigger_remote_latitude` (`latitude`),
  KEY `qcn_trigger_remote_longitude` (`longitude`),
  KEY `qcn_trigger_remote_result_name` (`result_name`)
) ENGINE=InnoDB AUTO_INCREMENT=324952 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `qcn_trigger_remote_xml`
--

DROP TABLE IF EXISTS `qcn_trigger_remote_xml`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `qcn_trigger_remote_xml` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `qcn_remoteid` int(11) DEFAULT NULL,
  `hostid` int(11) DEFAULT NULL,
  `valid` tinyint(1) DEFAULT NULL,
  `variety` varchar(32) DEFAULT NULL,
  `result_name` varchar(64) DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `xml` blob,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
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

--
-- Table structure for table `result`
--

DROP TABLE IF EXISTS `result`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `result` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `workunitid` int(11) NOT NULL,
  `server_state` int(11) NOT NULL,
  `outcome` int(11) NOT NULL,
  `client_state` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `report_deadline` int(11) NOT NULL,
  `sent_time` int(11) NOT NULL,
  `received_time` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `cpu_time` double NOT NULL,
  `xml_doc_in` blob,
  `xml_doc_out` blob,
  `stderr_out` blob,
  `batch` int(11) NOT NULL,
  `file_delete_state` int(11) NOT NULL,
  `validate_state` int(11) NOT NULL,
  `claimed_credit` double NOT NULL,
  `granted_credit` double NOT NULL,
  `opaque` double NOT NULL,
  `random` int(11) NOT NULL,
  `app_version_num` int(11) NOT NULL,
  `appid` int(11) NOT NULL,
  `exit_status` int(11) NOT NULL,
  `teamid` int(11) NOT NULL,
  `priority` int(11) NOT NULL,
  `mod_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `elapsed_time` double NOT NULL,
  `flops_estimate` double NOT NULL,
  `app_version_id` int(11) NOT NULL,
  `runtime_outlier` tinyint(4) NOT NULL,
  `size_class` smallint(6) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `res_wuid` (`workunitid`),
  KEY `ind_res_st` (`server_state`,`priority`),
  KEY `res_app_state` (`appid`,`server_state`),
  KEY `res_filedel` (`file_delete_state`),
  KEY `res_userid_id` (`userid`,`id`),
  KEY `res_userid_val` (`userid`,`validate_state`),
  KEY `res_hostid_id` (`hostid`,`id`),
  KEY `res_wu_user` (`workunitid`,`userid`),
  KEY `res_host_state` (`hostid`,`server_state`),
  KEY `result_random` (`random`)
) ENGINE=InnoDB AUTO_INCREMENT=3349944 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sent_email`
--

DROP TABLE IF EXISTS `sent_email`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sent_email` (
  `userid` int(11) NOT NULL,
  `time_sent` int(11) NOT NULL,
  `email_type` smallint(6) NOT NULL,
  PRIMARY KEY (`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spammer`
--

DROP TABLE IF EXISTS `spammer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `spammer` (
  `userid` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `state_counts`
--

DROP TABLE IF EXISTS `state_counts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `state_counts` (
  `appid` int(11) NOT NULL,
  `last_update_time` int(11) NOT NULL,
  `result_server_state_2` int(11) NOT NULL,
  `result_server_state_4` int(11) NOT NULL,
  `result_file_delete_state_1` int(11) NOT NULL,
  `result_file_delete_state_2` int(11) NOT NULL,
  `result_server_state_5_and_file_delete_state_0` int(11) NOT NULL,
  `workunit_need_validate_1` int(11) NOT NULL,
  `workunit_assimilate_state_1` int(11) NOT NULL,
  `workunit_file_delete_state_1` int(11) NOT NULL,
  `workunit_file_delete_state_2` int(11) NOT NULL,
  PRIMARY KEY (`appid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `subscriptions`
--

DROP TABLE IF EXISTS `subscriptions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `subscriptions` (
  `userid` int(11) NOT NULL,
  `threadid` int(11) NOT NULL,
  `notified_time` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `sub_unique` (`userid`,`threadid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `team`
--

DROP TABLE IF EXISTS `team`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `team` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `name_lc` varchar(254) DEFAULT NULL,
  `url` varchar(254) DEFAULT NULL,
  `type` int(11) NOT NULL,
  `name_html` varchar(254) DEFAULT NULL,
  `description` text,
  `nusers` int(11) NOT NULL,
  `country` varchar(254) DEFAULT NULL,
  `total_credit` double NOT NULL,
  `expavg_credit` double NOT NULL,
  `expavg_time` double NOT NULL,
  `seti_id` int(11) NOT NULL,
  `ping_user` int(11) NOT NULL DEFAULT '0',
  `ping_time` int(10) unsigned NOT NULL DEFAULT '0',
  `joinable` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `team_avg` (`expavg_credit`),
  KEY `team_tot` (`total_credit`),
  KEY `team_userid` (`userid`),
  FULLTEXT KEY `team_name_desc` (`name`,`description`),
  FULLTEXT KEY `team_name` (`name`)
) ENGINE=MyISAM AUTO_INCREMENT=6434 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `team_admin`
--

DROP TABLE IF EXISTS `team_admin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `team_admin` (
  `teamid` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `rights` int(11) NOT NULL,
  UNIQUE KEY `teamid` (`teamid`,`userid`),
  UNIQUE KEY `teamid_2` (`teamid`,`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `team_delta`
--

DROP TABLE IF EXISTS `team_delta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `team_delta` (
  `userid` int(11) NOT NULL,
  `teamid` int(11) NOT NULL,
  `timestamp` int(11) NOT NULL,
  `joining` tinyint(4) NOT NULL,
  `total_credit` double NOT NULL,
  KEY `team_delta_teamid` (`teamid`,`timestamp`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `temp_del_hostid`
--

DROP TABLE IF EXISTS `temp_del_hostid`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `temp_del_hostid` (
  `hostid` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `thread`
--

DROP TABLE IF EXISTS `thread`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `thread` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `forum` int(11) NOT NULL,
  `owner` int(11) NOT NULL,
  `status` int(11) NOT NULL,
  `title` varchar(254) NOT NULL,
  `timestamp` int(11) NOT NULL,
  `views` int(11) NOT NULL,
  `replies` int(11) NOT NULL,
  `activity` double NOT NULL,
  `sufferers` int(11) NOT NULL,
  `score` double NOT NULL,
  `votes` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `hidden` int(11) NOT NULL,
  `sticky` tinyint(4) NOT NULL DEFAULT '0',
  `locked` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  FULLTEXT KEY `thread_title` (`title`)
) ENGINE=MyISAM AUTO_INCREMENT=1230 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `email_addr` varchar(254) NOT NULL,
  `name` varchar(254) DEFAULT NULL,
  `authenticator` varchar(254) DEFAULT NULL,
  `country` varchar(254) DEFAULT NULL,
  `postal_code` varchar(254) DEFAULT NULL,
  `total_credit` double NOT NULL,
  `expavg_credit` double NOT NULL,
  `expavg_time` double NOT NULL,
  `global_prefs` blob,
  `project_prefs` blob,
  `teamid` int(11) NOT NULL,
  `venue` varchar(254) NOT NULL,
  `url` varchar(254) DEFAULT NULL,
  `send_email` smallint(6) NOT NULL,
  `show_hosts` smallint(6) NOT NULL,
  `posts` smallint(6) NOT NULL,
  `seti_id` int(11) NOT NULL,
  `seti_nresults` int(11) NOT NULL,
  `seti_last_result_time` int(11) NOT NULL,
  `seti_total_cpu` double NOT NULL,
  `signature` varchar(254) DEFAULT NULL,
  `has_profile` smallint(6) NOT NULL,
  `cross_project_id` varchar(254) NOT NULL,
  `passwd_hash` varchar(254) NOT NULL,
  `email_validated` smallint(6) NOT NULL,
  `donated` smallint(6) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `email_addr` (`email_addr`),
  UNIQUE KEY `authenticator` (`authenticator`),
  KEY `ind_tid` (`teamid`),
  KEY `user_name` (`name`),
  KEY `user_tot` (`total_credit`),
  KEY `user_avg` (`expavg_credit`)
) ENGINE=InnoDB AUTO_INCREMENT=118301 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `user_backup`
--

DROP TABLE IF EXISTS `user_backup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_backup` (
  `id` int(11) NOT NULL DEFAULT '0',
  `create_time` int(11) NOT NULL,
  `email_addr` varchar(254) NOT NULL,
  `name` varchar(254) DEFAULT NULL,
  `authenticator` varchar(254) DEFAULT NULL,
  `country` varchar(254) DEFAULT NULL,
  `postal_code` varchar(254) DEFAULT NULL,
  `total_credit` double NOT NULL,
  `expavg_credit` double NOT NULL,
  `expavg_time` double NOT NULL,
  `global_prefs` blob,
  `project_prefs` blob,
  `teamid` int(11) NOT NULL,
  `venue` varchar(254) NOT NULL,
  `url` varchar(254) DEFAULT NULL,
  `send_email` smallint(6) NOT NULL,
  `show_hosts` smallint(6) NOT NULL,
  `posts` smallint(6) NOT NULL,
  `seti_id` int(11) NOT NULL,
  `seti_nresults` int(11) NOT NULL,
  `seti_last_result_time` int(11) NOT NULL,
  `seti_total_cpu` double NOT NULL,
  `signature` varchar(254) DEFAULT NULL,
  `has_profile` smallint(6) NOT NULL,
  `cross_project_id` varchar(254) NOT NULL,
  `passwd_hash` varchar(254) NOT NULL,
  `email_validated` smallint(6) NOT NULL,
  `donated` smallint(6) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `user_spammer`
--

DROP TABLE IF EXISTS `user_spammer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_spammer` (
  `id` int(11) NOT NULL DEFAULT '0',
  `create_time` int(11) NOT NULL,
  `email_addr` varchar(254) NOT NULL,
  `name` varchar(254) DEFAULT NULL,
  `authenticator` varchar(254) DEFAULT NULL,
  `country` varchar(254) DEFAULT NULL,
  `postal_code` varchar(254) DEFAULT NULL,
  `total_credit` double NOT NULL,
  `expavg_credit` double NOT NULL,
  `expavg_time` double NOT NULL,
  `global_prefs` blob,
  `project_prefs` blob,
  `teamid` int(11) NOT NULL,
  `venue` varchar(254) NOT NULL,
  `url` varchar(254) DEFAULT NULL,
  `send_email` smallint(6) NOT NULL,
  `show_hosts` smallint(6) NOT NULL,
  `posts` smallint(6) NOT NULL,
  `seti_id` int(11) NOT NULL,
  `seti_nresults` int(11) NOT NULL,
  `seti_last_result_time` int(11) NOT NULL,
  `seti_total_cpu` double NOT NULL,
  `signature` varchar(254) DEFAULT NULL,
  `has_profile` smallint(6) NOT NULL,
  `cross_project_id` varchar(254) NOT NULL,
  `passwd_hash` varchar(254) NOT NULL,
  `email_validated` smallint(6) NOT NULL,
  `donated` smallint(6) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `user_submit`
--

DROP TABLE IF EXISTS `user_submit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_submit` (
  `user_id` int(11) NOT NULL,
  `quota` double NOT NULL,
  `logical_start_time` double NOT NULL,
  `submit_all` tinyint(4) NOT NULL,
  `manage_all` tinyint(4) NOT NULL,
  `max_jobs_in_progress` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `user_submit_app`
--

DROP TABLE IF EXISTS `user_submit_app`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_submit_app` (
  `user_id` int(11) NOT NULL,
  `app_id` int(11) NOT NULL,
  `manage` tinyint(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `usgs_quake`
--

DROP TABLE IF EXISTS `usgs_quake`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `usgs_quake` (
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
  UNIQUE KEY `usgs_quake_guid` (`guid`),
  KEY `usgs_quake_magnitude` (`magnitude`),
  KEY `usgs_time_utc` (`time_utc`),
  KEY `usgs_latitude` (`latitude`),
  KEY `usgs_longitude` (`longitude`)
) ENGINE=InnoDB AUTO_INCREMENT=26201 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `workseq`
--

DROP TABLE IF EXISTS `workseq`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `workseq` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `state` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `wuid_last_done` int(11) NOT NULL,
  `wuid_last_sent` int(11) NOT NULL,
  `workseqid_master` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `workunit`
--

DROP TABLE IF EXISTS `workunit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `workunit` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `appid` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `xml_doc` blob,
  `batch` int(11) NOT NULL,
  `rsc_fpops_est` double NOT NULL,
  `rsc_fpops_bound` double NOT NULL,
  `rsc_memory_bound` double NOT NULL,
  `rsc_disk_bound` double NOT NULL,
  `need_validate` smallint(6) NOT NULL,
  `canonical_resultid` int(11) NOT NULL,
  `canonical_credit` double NOT NULL,
  `transition_time` int(11) NOT NULL,
  `delay_bound` int(11) NOT NULL,
  `error_mask` int(11) NOT NULL,
  `file_delete_state` int(11) NOT NULL,
  `assimilate_state` int(11) NOT NULL,
  `hr_class` int(11) NOT NULL,
  `opaque` double NOT NULL,
  `min_quorum` int(11) NOT NULL,
  `target_nresults` int(11) NOT NULL,
  `max_error_results` int(11) NOT NULL,
  `max_total_results` int(11) NOT NULL,
  `max_success_results` int(11) NOT NULL,
  `result_template_file` varchar(63) NOT NULL,
  `priority` int(11) NOT NULL,
  `mod_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `rsc_bandwidth_bound` double NOT NULL,
  `fileset_id` int(11) NOT NULL,
  `app_version_id` int(11) NOT NULL DEFAULT '0',
  `transitioner_flags` tinyint(4) NOT NULL,
  `size_class` smallint(6) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `wu_val` (`appid`,`need_validate`),
  KEY `wu_timeout` (`transition_time`),
  KEY `wu_filedel` (`file_delete_state`),
  KEY `wu_assim` (`appid`,`assimilate_state`)
) ENGINE=InnoDB AUTO_INCREMENT=3356791 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-05-22 10:02:27
