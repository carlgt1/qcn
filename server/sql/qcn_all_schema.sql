-- MySQL dump 10.11
--
-- Host: db-private    Database: sensor
-- ------------------------------------------------------
-- Server version	6.0.6-alpha-log

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
-- Current Database: `sensor`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `sensor` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `sensor`;

--
-- Table structure for table `app`
--

DROP TABLE IF EXISTS `app`;
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
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

--
-- Table structure for table `app_version`
--

DROP TABLE IF EXISTS `app_version`;
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
) ENGINE=InnoDB AUTO_INCREMENT=866 DEFAULT CHARSET=latin1;

--
-- Table structure for table `assignment`
--

DROP TABLE IF EXISTS `assignment`;
CREATE TABLE `assignment` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `target_id` int(11) NOT NULL,
  `target_type` int(11) NOT NULL,
  `multi` tinyint(4) NOT NULL,
  `workunitid` int(11) NOT NULL,
  `resultid` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `banishment_vote`
--

DROP TABLE IF EXISTS `banishment_vote`;
CREATE TABLE `banishment_vote` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `modid` int(11) NOT NULL,
  `start_time` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `banishment_votes`
--

DROP TABLE IF EXISTS `banishment_votes`;
CREATE TABLE `banishment_votes` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `voteid` int(11) NOT NULL,
  `modid` int(11) NOT NULL,
  `time` int(11) NOT NULL,
  `yes` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `bolt_course`
--

DROP TABLE IF EXISTS `bolt_course`;
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

--
-- Table structure for table `bolt_enrollment`
--

DROP TABLE IF EXISTS `bolt_enrollment`;
CREATE TABLE `bolt_enrollment` (
  `create_time` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `course_id` int(11) NOT NULL,
  `last_view_id` int(11) NOT NULL,
  `mastery` double NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `bolt_question`
--

DROP TABLE IF EXISTS `bolt_question`;
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

--
-- Table structure for table `bolt_refresh`
--

DROP TABLE IF EXISTS `bolt_refresh`;
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

--
-- Table structure for table `bolt_result`
--

DROP TABLE IF EXISTS `bolt_result`;
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

--
-- Table structure for table `bolt_select_finished`
--

DROP TABLE IF EXISTS `bolt_select_finished`;
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

--
-- Table structure for table `bolt_user`
--

DROP TABLE IF EXISTS `bolt_user`;
CREATE TABLE `bolt_user` (
  `user_id` int(11) NOT NULL,
  `birth_year` int(11) NOT NULL,
  `sex` tinyint(4) NOT NULL,
  `flags` int(11) NOT NULL,
  `attrs` text NOT NULL,
  PRIMARY KEY (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `bolt_view`
--

DROP TABLE IF EXISTS `bolt_view`;
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

--
-- Table structure for table `bolt_xset_result`
--

DROP TABLE IF EXISTS `bolt_xset_result`;
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

--
-- Table structure for table `category`
--

DROP TABLE IF EXISTS `category`;
CREATE TABLE `category` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `orderID` int(11) NOT NULL,
  `lang` int(11) NOT NULL,
  `name` varchar(254) CHARACTER SET latin1 COLLATE latin1_bin DEFAULT NULL,
  `is_helpdesk` smallint(6) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `cat1` (`name`,`is_helpdesk`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

--
-- Table structure for table `credit_multiplier`
--

DROP TABLE IF EXISTS `credit_multiplier`;
CREATE TABLE `credit_multiplier` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `appid` int(11) NOT NULL,
  `time` int(11) NOT NULL,
  `multiplier` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `credited_job`
--

DROP TABLE IF EXISTS `credited_job`;
CREATE TABLE `credited_job` (
  `userid` int(11) NOT NULL,
  `workunitid` bigint(20) NOT NULL,
  UNIQUE KEY `credited_job_user_wu` (`userid`,`workunitid`),
  KEY `credited_job_user` (`userid`),
  KEY `credited_job_wu` (`workunitid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `donation_items`
--

DROP TABLE IF EXISTS `donation_items`;
CREATE TABLE `donation_items` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `item_name` varchar(32) NOT NULL,
  `title` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL,
  `required` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `donation_paypal`
--

DROP TABLE IF EXISTS `donation_paypal`;
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

--
-- Table structure for table `forum`
--

DROP TABLE IF EXISTS `forum`;
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
) ENGINE=InnoDB AUTO_INCREMENT=27 DEFAULT CHARSET=latin1;

--
-- Table structure for table `forum_logging`
--

DROP TABLE IF EXISTS `forum_logging`;
CREATE TABLE `forum_logging` (
  `userid` int(11) NOT NULL DEFAULT '0',
  `threadid` int(11) NOT NULL DEFAULT '0',
  `timestamp` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`userid`,`threadid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `forum_preferences`
--

DROP TABLE IF EXISTS `forum_preferences`;
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

--
-- Table structure for table `friend`
--

DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `user_src` int(11) NOT NULL,
  `user_dest` int(11) NOT NULL,
  `message` varchar(255) NOT NULL,
  `create_time` int(11) NOT NULL,
  `reciprocated` tinyint(4) NOT NULL,
  UNIQUE KEY `friend_u` (`user_src`,`user_dest`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `host`
--

DROP TABLE IF EXISTS `host`;
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
  PRIMARY KEY (`id`),
  KEY `host_user` (`userid`),
  KEY `host_avg` (`expavg_credit`),
  KEY `host_tot` (`total_credit`)
) ENGINE=InnoDB AUTO_INCREMENT=16355 DEFAULT CHARSET=latin1;

--
-- Table structure for table `host_app_version`
--

DROP TABLE IF EXISTS `host_app_version`;
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

--
-- Table structure for table `host_ip`
--

DROP TABLE IF EXISTS `host_ip`;
CREATE TABLE `host_ip` (
  `id` int(11) NOT NULL,
  `hostid` int(11) DEFAULT NULL,
  `ipaddr` varchar(32) DEFAULT NULL,
  `location` varchar(128) DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `host_ip_id` (`hostid`,`ipaddr`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `msg_from_host`
--

DROP TABLE IF EXISTS `msg_from_host`;
CREATE TABLE `msg_from_host` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `variety` varchar(254) NOT NULL,
  `handled` smallint(6) NOT NULL,
  `xml` mediumtext,
  PRIMARY KEY (`id`),
  KEY `message_handled` (`handled`)
) ENGINE=InnoDB AUTO_INCREMENT=388212 DEFAULT CHARSET=latin1;

--
-- Table structure for table `msg_to_host`
--

DROP TABLE IF EXISTS `msg_to_host`;
CREATE TABLE `msg_to_host` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `variety` varchar(254) NOT NULL,
  `handled` smallint(6) NOT NULL,
  `xml` mediumtext,
  PRIMARY KEY (`id`),
  KEY `msg_to_host` (`hostid`,`handled`)
) ENGINE=InnoDB AUTO_INCREMENT=228 DEFAULT CHARSET=latin1;

--
-- Table structure for table `notify`
--

DROP TABLE IF EXISTS `notify`;
CREATE TABLE `notify` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `opaque` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `notify_un` (`userid`,`type`,`opaque`)
) ENGINE=InnoDB AUTO_INCREMENT=2445 DEFAULT CHARSET=latin1;

--
-- Table structure for table `platform`
--

DROP TABLE IF EXISTS `platform`;
CREATE TABLE `platform` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `user_friendly_name` varchar(254) NOT NULL,
  `deprecated` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;

--
-- Table structure for table `post`
--

DROP TABLE IF EXISTS `post`;
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
) ENGINE=MyISAM AUTO_INCREMENT=3283 DEFAULT CHARSET=latin1;

--
-- Table structure for table `post_ratings`
--

DROP TABLE IF EXISTS `post_ratings`;
CREATE TABLE `post_ratings` (
  `post` int(11) NOT NULL,
  `user` int(11) NOT NULL,
  `rating` tinyint(4) NOT NULL,
  PRIMARY KEY (`post`,`user`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `private_messages`
--

DROP TABLE IF EXISTS `private_messages`;
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
) ENGINE=MyISAM AUTO_INCREMENT=391 DEFAULT CHARSET=latin1;

--
-- Table structure for table `profile`
--

DROP TABLE IF EXISTS `profile`;
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

--
-- Table structure for table `profile_bak`
--

DROP TABLE IF EXISTS `profile_bak`;
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

--
-- Table structure for table `qcn_align`
--

DROP TABLE IF EXISTS `qcn_align`;
CREATE TABLE `qcn_align` (
  `id` smallint(6) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_constant`
--

DROP TABLE IF EXISTS `qcn_constant`;
CREATE TABLE `qcn_constant` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `description` varchar(64) NOT NULL,
  `value_int` int(11) DEFAULT NULL,
  `value_text` varchar(64) DEFAULT NULL,
  `value_float` float DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `qcn_constant_description` (`description`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_continual_filename`
--

DROP TABLE IF EXISTS `qcn_continual_filename`;
CREATE TABLE `qcn_continual_filename` (
  `filename` varchar(255) DEFAULT NULL,
  KEY `list_continual_filename` (`filename`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_country_latlng`
--

DROP TABLE IF EXISTS `qcn_country_latlng`;
CREATE TABLE `qcn_country_latlng` (
  `id` varchar(4) NOT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_finalstats`
--

DROP TABLE IF EXISTS `qcn_finalstats`;
CREATE TABLE `qcn_finalstats` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `resultid` int(11) NOT NULL,
  `time_received` double NOT NULL DEFAULT '0',
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_finalstats_resultid` (`resultid`)
) ENGINE=InnoDB AUTO_INCREMENT=386568 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_finalstats_bak`
--

DROP TABLE IF EXISTS `qcn_finalstats_bak`;
CREATE TABLE `qcn_finalstats_bak` (
  `id` int(11) NOT NULL DEFAULT '0',
  `hostid` int(11) NOT NULL,
  `resultid` int(11) NOT NULL,
  `time_received` double NOT NULL DEFAULT '0',
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_finalstats_nohost`
--

DROP TABLE IF EXISTS `qcn_finalstats_nohost`;
CREATE TABLE `qcn_finalstats_nohost` (
  `id` int(11) NOT NULL DEFAULT '0',
  `resultid` int(11) NOT NULL,
  `time_received` double NOT NULL DEFAULT '0',
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_geo_ipaddr`
--

DROP TABLE IF EXISTS `qcn_geo_ipaddr`;
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
) ENGINE=InnoDB AUTO_INCREMENT=79173 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_host_ipaddr`
--

DROP TABLE IF EXISTS `qcn_host_ipaddr`;
CREATE TABLE `qcn_host_ipaddr` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `hostid` int(11) NOT NULL,
  `ipaddr` varchar(32) NOT NULL DEFAULT '',
  `location` varchar(128) NOT NULL DEFAULT '',
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `levelvalue` float DEFAULT NULL,
  `levelid` smallint(6) DEFAULT NULL,
  `alignid` smallint(6) DEFAULT NULL,
  `geoipaddrid` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_host_ipaddr_id` (`hostid`,`ipaddr`,`geoipaddrid`)
) ENGINE=InnoDB AUTO_INCREMENT=125188 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_kml_region`
--

DROP TABLE IF EXISTS `qcn_kml_region`;
CREATE TABLE `qcn_kml_region` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(64) DEFAULT NULL,
  `filename` varchar(255) DEFAULT NULL,
  `data` blob,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_level`
--

DROP TABLE IF EXISTS `qcn_level`;
CREATE TABLE `qcn_level` (
  `id` smallint(6) NOT NULL,
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_post`
--

DROP TABLE IF EXISTS `qcn_post`;
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

--
-- Table structure for table `qcn_quake`
--

DROP TABLE IF EXISTS `qcn_quake`;
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
) ENGINE=InnoDB AUTO_INCREMENT=47056 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_ramp_coordinator`
--

DROP TABLE IF EXISTS `qcn_ramp_coordinator`;
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

--
-- Table structure for table `qcn_ramp_participant`
--

DROP TABLE IF EXISTS `qcn_ramp_participant`;
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
  `regional` tinyint(1) NOT NULL DEFAULT '0',
  `kml_regionid` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_ramp_participant_userid` (`userid`),
  KEY `qcn_ramp_participant_kml_regionid` (`kml_regionid`)
) ENGINE=InnoDB AUTO_INCREMENT=797 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_recalcresult`
--

DROP TABLE IF EXISTS `qcn_recalcresult`;
CREATE TABLE `qcn_recalcresult` (
  `resultid` int(11) NOT NULL,
  `weight` double DEFAULT NULL,
  `total_credit` double DEFAULT NULL,
  `time_received` double DEFAULT NULL,
  PRIMARY KEY (`resultid`),
  KEY `recalc_result` (`resultid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_reds`
--

DROP TABLE IF EXISTS `qcn_reds`;
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

--
-- Table structure for table `qcn_sensor`
--

DROP TABLE IF EXISTS `qcn_sensor`;
CREATE TABLE `qcn_sensor` (
  `id` smallint(6) NOT NULL,
  `is_usb` tinyint(1) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_showhostlocation`
--

DROP TABLE IF EXISTS `qcn_showhostlocation`;
CREATE TABLE `qcn_showhostlocation` (
  `hostid` int(11) NOT NULL,
  PRIMARY KEY (`hostid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_stats`
--

DROP TABLE IF EXISTS `qcn_stats`;
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

--
-- Table structure for table `qcn_trigger`
--

DROP TABLE IF EXISTS `qcn_trigger`;
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
  PRIMARY KEY (`id`),
  KEY `qcn_trigger_hostid_filereq` (`hostid`,`time_filereq`,`received_file`),
  KEY `qcn_trigger_file` (`file`),
  KEY `qcn_trigger_type_sensor` (`qcn_sensorid`),
  KEY `qcn_trigger_usgs_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_flag` (`flag`),
  KEY `qcn_trigger_time` (`time_trigger`,`varietyid`),
  KEY `qcn_trigger_hostid` (`hostid`,`time_trigger`,`varietyid`),
  KEY `qcn_trigger_timelatlng` (`time_trigger`,`latitude`,`longitude`,`varietyid`),
  KEY `qcn_trigger_result_name` (`result_name`,`id`,`varietyid`)
) ENGINE=InnoDB AUTO_INCREMENT=56574588 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_trigger_followup`
--

DROP TABLE IF EXISTS `qcn_trigger_followup`;
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

--
-- Table structure for table `qcn_variety`
--

DROP TABLE IF EXISTS `qcn_variety`;
CREATE TABLE `qcn_variety` (
  `id` smallint(6) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `result`
--

DROP TABLE IF EXISTS `result`;
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
) ENGINE=InnoDB AUTO_INCREMENT=1119937 DEFAULT CHARSET=latin1;

--
-- Table structure for table `sent_email`
--

DROP TABLE IF EXISTS `sent_email`;
CREATE TABLE `sent_email` (
  `userid` int(11) NOT NULL,
  `time_sent` int(11) NOT NULL,
  `email_type` smallint(6) NOT NULL,
  PRIMARY KEY (`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `spammer`
--

DROP TABLE IF EXISTS `spammer`;
CREATE TABLE `spammer` (
  `userid` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `state_counts`
--

DROP TABLE IF EXISTS `state_counts`;
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

--
-- Table structure for table `subscriptions`
--

DROP TABLE IF EXISTS `subscriptions`;
CREATE TABLE `subscriptions` (
  `userid` int(11) NOT NULL,
  `threadid` int(11) NOT NULL,
  `notified_time` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `sub_unique` (`userid`,`threadid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `team`
--

DROP TABLE IF EXISTS `team`;
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
) ENGINE=MyISAM AUTO_INCREMENT=391 DEFAULT CHARSET=latin1;

--
-- Table structure for table `team_admin`
--

DROP TABLE IF EXISTS `team_admin`;
CREATE TABLE `team_admin` (
  `teamid` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `rights` int(11) NOT NULL,
  UNIQUE KEY `teamid` (`teamid`,`userid`),
  UNIQUE KEY `teamid_2` (`teamid`,`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `team_delta`
--

DROP TABLE IF EXISTS `team_delta`;
CREATE TABLE `team_delta` (
  `userid` int(11) NOT NULL,
  `teamid` int(11) NOT NULL,
  `timestamp` int(11) NOT NULL,
  `joining` tinyint(4) NOT NULL,
  `total_credit` double NOT NULL,
  KEY `team_delta_teamid` (`teamid`,`timestamp`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `temp_del_hostid`
--

DROP TABLE IF EXISTS `temp_del_hostid`;
CREATE TABLE `temp_del_hostid` (
  `hostid` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `thread`
--

DROP TABLE IF EXISTS `thread`;
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
) ENGINE=MyISAM AUTO_INCREMENT=704 DEFAULT CHARSET=latin1;

--
-- Table structure for table `unique_latlon`
--

DROP TABLE IF EXISTS `unique_latlon`;
CREATE TABLE `unique_latlon` (
  `country` char(0) NOT NULL DEFAULT '',
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `ctr` bigint(21) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
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
) ENGINE=InnoDB AUTO_INCREMENT=30510 DEFAULT CHARSET=latin1;

--
-- Table structure for table `usgs_quake`
--

DROP TABLE IF EXISTS `usgs_quake`;
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

--
-- Table structure for table `workseq`
--

DROP TABLE IF EXISTS `workseq`;
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

--
-- Table structure for table `workunit`
--

DROP TABLE IF EXISTS `workunit`;
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
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `wu_val` (`appid`,`need_validate`),
  KEY `wu_timeout` (`transition_time`),
  KEY `wu_filedel` (`file_delete_state`),
  KEY `wu_assim` (`appid`,`assimilate_state`)
) ENGINE=InnoDB AUTO_INCREMENT=1123935 DEFAULT CHARSET=latin1;

--
-- Table structure for table `workunit_backup`
--

DROP TABLE IF EXISTS `workunit_backup`;
CREATE TABLE `workunit_backup` (
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
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `wu_val` (`appid`,`need_validate`),
  KEY `wu_timeout` (`transition_time`),
  KEY `wu_filedel` (`file_delete_state`),
  KEY `wu_assim` (`appid`,`assimilate_state`)
) ENGINE=InnoDB AUTO_INCREMENT=132763 DEFAULT CHARSET=latin1;

--
-- Current Database: `continual`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `continual` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `continual`;

--
-- Table structure for table `app`
--

DROP TABLE IF EXISTS `app`;
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
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Table structure for table `app_version`
--

DROP TABLE IF EXISTS `app_version`;
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
) ENGINE=InnoDB AUTO_INCREMENT=375 DEFAULT CHARSET=latin1;

--
-- Table structure for table `assignment`
--

DROP TABLE IF EXISTS `assignment`;
CREATE TABLE `assignment` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `target_id` int(11) NOT NULL,
  `target_type` int(11) NOT NULL,
  `multi` tinyint(4) NOT NULL,
  `workunitid` int(11) NOT NULL,
  `resultid` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `banishment_vote`
--

DROP TABLE IF EXISTS `banishment_vote`;
CREATE TABLE `banishment_vote` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `modid` int(11) NOT NULL,
  `start_time` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `banishment_votes`
--

DROP TABLE IF EXISTS `banishment_votes`;
CREATE TABLE `banishment_votes` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `voteid` int(11) NOT NULL,
  `modid` int(11) NOT NULL,
  `time` int(11) NOT NULL,
  `yes` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `carl_tmp`
--

DROP TABLE IF EXISTS `carl_tmp`;
CREATE TABLE `carl_tmp` (
  `id` int(11) NOT NULL DEFAULT '0',
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
  `type_sensor` int(2) DEFAULT NULL,
  `sw_version` varchar(8) DEFAULT NULL,
  `os_type` varchar(8) DEFAULT NULL,
  `usgs_quakeid` int(11) DEFAULT NULL,
  `time_filereq` double DEFAULT NULL,
  `received_file` tinyint(1) DEFAULT NULL,
  `file_url` varchar(128) DEFAULT NULL,
  `runtime_clock` double DEFAULT NULL,
  `runtime_cpu` double DEFAULT NULL,
  `varietyid` smallint(6) NOT NULL DEFAULT '0',
  `flag` tinyint(1) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `category`
--

DROP TABLE IF EXISTS `category`;
CREATE TABLE `category` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `orderID` int(11) NOT NULL,
  `lang` int(11) NOT NULL,
  `name` varchar(254) CHARACTER SET latin1 COLLATE latin1_bin DEFAULT NULL,
  `is_helpdesk` smallint(6) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `cat1` (`name`,`is_helpdesk`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

--
-- Table structure for table `credit_multiplier`
--

DROP TABLE IF EXISTS `credit_multiplier`;
CREATE TABLE `credit_multiplier` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `appid` int(11) NOT NULL,
  `time` int(11) NOT NULL,
  `multiplier` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `credited_job`
--

DROP TABLE IF EXISTS `credited_job`;
CREATE TABLE `credited_job` (
  `userid` int(11) NOT NULL,
  `workunitid` bigint(20) NOT NULL,
  UNIQUE KEY `credited_job_user_wu` (`userid`,`workunitid`),
  KEY `credited_job_user` (`userid`),
  KEY `credited_job_wu` (`workunitid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `donation_items`
--

DROP TABLE IF EXISTS `donation_items`;
CREATE TABLE `donation_items` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `item_name` varchar(32) NOT NULL,
  `title` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL,
  `required` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `donation_paypal`
--

DROP TABLE IF EXISTS `donation_paypal`;
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

--
-- Table structure for table `forum`
--

DROP TABLE IF EXISTS `forum`;
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
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;

--
-- Table structure for table `forum_logging`
--

DROP TABLE IF EXISTS `forum_logging`;
CREATE TABLE `forum_logging` (
  `userid` int(11) NOT NULL DEFAULT '0',
  `threadid` int(11) NOT NULL DEFAULT '0',
  `timestamp` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`userid`,`threadid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `forum_preferences`
--

DROP TABLE IF EXISTS `forum_preferences`;
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

--
-- Table structure for table `friend`
--

DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `user_src` int(11) NOT NULL,
  `user_dest` int(11) NOT NULL,
  `message` varchar(255) NOT NULL,
  `create_time` int(11) NOT NULL,
  `reciprocated` tinyint(4) NOT NULL,
  UNIQUE KEY `friend_u` (`user_src`,`user_dest`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `host`
--

DROP TABLE IF EXISTS `host`;
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
  PRIMARY KEY (`id`),
  KEY `host_user` (`userid`),
  KEY `host_avg` (`expavg_credit`),
  KEY `host_tot` (`total_credit`)
) ENGINE=InnoDB AUTO_INCREMENT=673 DEFAULT CHARSET=latin1;

--
-- Table structure for table `host_app_version`
--

DROP TABLE IF EXISTS `host_app_version`;
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

--
-- Table structure for table `msg_from_host`
--

DROP TABLE IF EXISTS `msg_from_host`;
CREATE TABLE `msg_from_host` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `variety` varchar(254) NOT NULL,
  `handled` smallint(6) NOT NULL,
  `xml` mediumtext,
  PRIMARY KEY (`id`),
  KEY `message_handled` (`handled`)
) ENGINE=InnoDB AUTO_INCREMENT=47254 DEFAULT CHARSET=latin1;

--
-- Table structure for table `msg_to_host`
--

DROP TABLE IF EXISTS `msg_to_host`;
CREATE TABLE `msg_to_host` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `hostid` int(11) NOT NULL,
  `variety` varchar(254) NOT NULL,
  `handled` smallint(6) NOT NULL,
  `xml` mediumtext,
  PRIMARY KEY (`id`),
  KEY `msg_to_host` (`hostid`,`handled`)
) ENGINE=InnoDB AUTO_INCREMENT=447 DEFAULT CHARSET=latin1;

--
-- Table structure for table `notify`
--

DROP TABLE IF EXISTS `notify`;
CREATE TABLE `notify` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `userid` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `opaque` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `notify_un` (`userid`,`type`,`opaque`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Table structure for table `platform`
--

DROP TABLE IF EXISTS `platform`;
CREATE TABLE `platform` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) NOT NULL,
  `name` varchar(254) NOT NULL,
  `user_friendly_name` varchar(254) NOT NULL,
  `deprecated` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=latin1;

--
-- Table structure for table `post`
--

DROP TABLE IF EXISTS `post`;
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
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;

--
-- Table structure for table `post_ratings`
--

DROP TABLE IF EXISTS `post_ratings`;
CREATE TABLE `post_ratings` (
  `post` int(11) NOT NULL,
  `user` int(11) NOT NULL,
  `rating` tinyint(4) NOT NULL,
  PRIMARY KEY (`post`,`user`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `private_messages`
--

DROP TABLE IF EXISTS `private_messages`;
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
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Table structure for table `profile`
--

DROP TABLE IF EXISTS `profile`;
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

--
-- Table structure for table `qcn_align`
--

DROP TABLE IF EXISTS `qcn_align`;
CREATE TABLE `qcn_align` (
  `id` smallint(6) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_constant`
--

DROP TABLE IF EXISTS `qcn_constant`;
CREATE TABLE `qcn_constant` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `description` varchar(64) NOT NULL,
  `value_int` int(11) DEFAULT NULL,
  `value_text` varchar(64) DEFAULT NULL,
  `value_float` float DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `qcn_constant_description` (`description`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_finalstats`
--

DROP TABLE IF EXISTS `qcn_finalstats`;
CREATE TABLE `qcn_finalstats` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `resultid` int(11) NOT NULL,
  `time_received` double NOT NULL DEFAULT '0',
  `runtime_clock` double NOT NULL DEFAULT '0',
  `runtime_cpu` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_finalstats_resultid` (`resultid`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_geo_ipaddr`
--

DROP TABLE IF EXISTS `qcn_geo_ipaddr`;
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
) ENGINE=InnoDB AUTO_INCREMENT=1955 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_host_ipaddr`
--

DROP TABLE IF EXISTS `qcn_host_ipaddr`;
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
) ENGINE=InnoDB AUTO_INCREMENT=3708 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_host_ipaddr_backup`
--

DROP TABLE IF EXISTS `qcn_host_ipaddr_backup`;
CREATE TABLE `qcn_host_ipaddr_backup` (
  `id` int(11) NOT NULL DEFAULT '0',
  `hostid` int(11) NOT NULL,
  `ipaddr` varchar(32) NOT NULL DEFAULT '',
  `location` varchar(32) NOT NULL DEFAULT '',
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `levelvalue` float DEFAULT NULL,
  `levelid` smallint(6) DEFAULT NULL,
  `alignid` smallint(6) DEFAULT NULL,
  `geoipaddrid` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_level`
--

DROP TABLE IF EXISTS `qcn_level`;
CREATE TABLE `qcn_level` (
  `id` smallint(6) NOT NULL,
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_quake`
--

DROP TABLE IF EXISTS `qcn_quake`;
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
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_recalcresult`
--

DROP TABLE IF EXISTS `qcn_recalcresult`;
CREATE TABLE `qcn_recalcresult` (
  `resultid` int(11) NOT NULL,
  `weight` double DEFAULT NULL,
  `total_credit` double DEFAULT NULL,
  `time_received` double DEFAULT NULL,
  PRIMARY KEY (`resultid`),
  KEY `recalc_result` (`resultid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_sensor`
--

DROP TABLE IF EXISTS `qcn_sensor`;
CREATE TABLE `qcn_sensor` (
  `id` smallint(6) NOT NULL,
  `is_usb` tinyint(1) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_showhostlocation`
--

DROP TABLE IF EXISTS `qcn_showhostlocation`;
CREATE TABLE `qcn_showhostlocation` (
  `hostid` int(11) NOT NULL,
  PRIMARY KEY (`hostid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_stats`
--

DROP TABLE IF EXISTS `qcn_stats`;
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

--
-- Table structure for table `qcn_trigger`
--

DROP TABLE IF EXISTS `qcn_trigger`;
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
  `runtime_clock` double DEFAULT NULL,
  `runtime_cpu` double DEFAULT NULL,
  `varietyid` smallint(6) NOT NULL DEFAULT '0',
  `flag` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `qcn_trigger_hostid_filereq` (`hostid`,`time_filereq`,`received_file`),
  KEY `qcn_trigger_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_file` (`file`),
  KEY `qcn_trigger_flag` (`flag`),
  KEY `qcn_trigger_type_sensor` (`qcn_sensorid`),
  KEY `qcn_trigger_time` (`time_trigger`,`varietyid`),
  KEY `qcn_trigger_hostid` (`hostid`,`time_trigger`,`varietyid`),
  KEY `qcn_trigger_timelatlng` (`time_trigger`,`latitude`,`longitude`,`varietyid`),
  KEY `qcn_trigger_result_name` (`result_name`,`id`,`varietyid`)
) ENGINE=InnoDB AUTO_INCREMENT=8962797 DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_trigger_bak`
--

DROP TABLE IF EXISTS `qcn_trigger_bak`;
CREATE TABLE `qcn_trigger_bak` (
  `id` int(11) NOT NULL DEFAULT '0',
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
  `type_sensor` int(2) DEFAULT NULL,
  `sw_version` varchar(8) DEFAULT NULL,
  `os_type` varchar(8) DEFAULT NULL,
  `usgs_quakeid` int(11) DEFAULT NULL,
  `time_filereq` double DEFAULT NULL,
  `received_file` tinyint(1) DEFAULT NULL,
  `file_url` varchar(128) DEFAULT NULL,
  `runtime_clock` double DEFAULT NULL,
  `runtime_cpu` double DEFAULT NULL,
  `varietyid` smallint(6) NOT NULL DEFAULT '0',
  `flag` tinyint(1) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `qcn_variety`
--

DROP TABLE IF EXISTS `qcn_variety`;
CREATE TABLE `qcn_variety` (
  `id` smallint(6) NOT NULL DEFAULT '0',
  `description` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `result`
--

DROP TABLE IF EXISTS `result`;
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
  KEY `result_random` (`random`)
) ENGINE=InnoDB AUTO_INCREMENT=158113 DEFAULT CHARSET=latin1;

--
-- Table structure for table `sent_email`
--

DROP TABLE IF EXISTS `sent_email`;
CREATE TABLE `sent_email` (
  `userid` int(11) NOT NULL,
  `time_sent` int(11) NOT NULL,
  `email_type` smallint(6) NOT NULL,
  PRIMARY KEY (`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `state_counts`
--

DROP TABLE IF EXISTS `state_counts`;
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

--
-- Table structure for table `subscriptions`
--

DROP TABLE IF EXISTS `subscriptions`;
CREATE TABLE `subscriptions` (
  `userid` int(11) NOT NULL,
  `threadid` int(11) NOT NULL,
  `notified_time` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `sub_unique` (`userid`,`threadid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `team`
--

DROP TABLE IF EXISTS `team`;
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
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

--
-- Table structure for table `team_admin`
--

DROP TABLE IF EXISTS `team_admin`;
CREATE TABLE `team_admin` (
  `teamid` int(11) NOT NULL,
  `userid` int(11) NOT NULL,
  `create_time` int(11) NOT NULL,
  `rights` int(11) NOT NULL,
  UNIQUE KEY `teamid` (`teamid`,`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `team_delta`
--

DROP TABLE IF EXISTS `team_delta`;
CREATE TABLE `team_delta` (
  `userid` int(11) NOT NULL,
  `teamid` int(11) NOT NULL,
  `timestamp` int(11) NOT NULL,
  `joining` tinyint(4) NOT NULL,
  `total_credit` double NOT NULL,
  KEY `team_delta_teamid` (`teamid`,`timestamp`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `thread`
--

DROP TABLE IF EXISTS `thread`;
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
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
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
) ENGINE=InnoDB AUTO_INCREMENT=27494 DEFAULT CHARSET=latin1;

--
-- Table structure for table `usgs_quake`
--

DROP TABLE IF EXISTS `usgs_quake`;
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
  UNIQUE KEY `usgs_quake_guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `workunit`
--

DROP TABLE IF EXISTS `workunit`;
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
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `wu_val` (`appid`,`need_validate`),
  KEY `wu_timeout` (`transition_time`),
  KEY `wu_filedel` (`file_delete_state`),
  KEY `wu_assim` (`appid`,`assimilate_state`)
) ENGINE=InnoDB AUTO_INCREMENT=150001 DEFAULT CHARSET=latin1;

--
-- Current Database: `continual_archive`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `continual_archive` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `continual_archive`;

--
-- Table structure for table `qcn_trigger`
--

DROP TABLE IF EXISTS `qcn_trigger`;
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
  `runtime_clock` double DEFAULT NULL,
  `runtime_cpu` double DEFAULT NULL,
  `varietyid` smallint(6) NOT NULL DEFAULT '0',
  `flag` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `qcn_trigger_time` (`time_trigger`,`varietyid`),
  KEY `qcn_trigger_hostid` (`hostid`,`time_trigger`,`varietyid`),
  KEY `qcn_trigger_timelatlng` (`time_trigger`,`latitude`,`longitude`,`varietyid`),
  KEY `qcn_trigger_result_name` (`result_name`,`id`,`varietyid`),
  KEY `qcn_trigger_hostid_filereq` (`hostid`,`time_filereq`,`received_file`),
  KEY `qcn_trigger_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_file` (`file`),
  KEY `qcn_trigger_type_sensor` (`qcn_sensorid`),
  KEY `qcn_trigger_qcn_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_flag` (`flag`)
) ENGINE=InnoDB AUTO_INCREMENT=8682390 DEFAULT CHARSET=latin1;

--
-- Current Database: `sensor_archive`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `sensor_archive` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `sensor_archive`;

--
-- Table structure for table `qcn_trigger`
--

DROP TABLE IF EXISTS `qcn_trigger`;
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
  `runtime_clock` double DEFAULT NULL,
  `runtime_cpu` double DEFAULT NULL,
  `varietyid` smallint(6) NOT NULL DEFAULT '0',
  `flag` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `qcn_trigger_hostid_filereq` (`hostid`,`time_filereq`,`received_file`),
  KEY `qcn_trigger_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_file` (`file`),
  KEY `qcn_trigger_type_sensor` (`qcn_sensorid`),
  KEY `qcn_trigger_usgs_quakeid` (`qcn_quakeid`),
  KEY `qcn_trigger_time` (`time_trigger`,`varietyid`),
  KEY `qcn_trigger_hostid` (`hostid`,`time_trigger`,`varietyid`),
  KEY `qcn_trigger_timelatlng` (`time_trigger`,`latitude`,`longitude`,`varietyid`),
  KEY `qcn_trigger_result_name` (`result_name`,`id`,`varietyid`)
) ENGINE=InnoDB AUTO_INCREMENT=55869992 DEFAULT CHARSET=latin1;

--
-- Current Database: `qcnwp`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `qcnwp` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `qcnwp`;

--
-- Table structure for table `wp_commentmeta`
--

DROP TABLE IF EXISTS `wp_commentmeta`;
CREATE TABLE `wp_commentmeta` (
  `meta_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `comment_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `meta_key` varchar(255) DEFAULT NULL,
  `meta_value` longtext,
  PRIMARY KEY (`meta_id`),
  KEY `comment_id` (`comment_id`),
  KEY `meta_key` (`meta_key`(191))
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_comments`
--

DROP TABLE IF EXISTS `wp_comments`;
CREATE TABLE `wp_comments` (
  `comment_ID` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `comment_post_ID` bigint(20) unsigned NOT NULL DEFAULT '0',
  `comment_author` tinytext NOT NULL,
  `comment_author_email` varchar(100) NOT NULL DEFAULT '',
  `comment_author_url` varchar(200) NOT NULL DEFAULT '',
  `comment_author_IP` varchar(100) NOT NULL DEFAULT '',
  `comment_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `comment_date_gmt` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `comment_content` text NOT NULL,
  `comment_karma` int(11) NOT NULL DEFAULT '0',
  `comment_approved` varchar(20) NOT NULL DEFAULT '1',
  `comment_agent` varchar(255) NOT NULL DEFAULT '',
  `comment_type` varchar(20) NOT NULL DEFAULT '',
  `comment_parent` bigint(20) unsigned NOT NULL DEFAULT '0',
  `user_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`comment_ID`),
  KEY `comment_approved` (`comment_approved`),
  KEY `comment_post_ID` (`comment_post_ID`),
  KEY `comment_approved_date_gmt` (`comment_approved`,`comment_date_gmt`),
  KEY `comment_date_gmt` (`comment_date_gmt`),
  KEY `comment_parent` (`comment_parent`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_duplicator`
--

DROP TABLE IF EXISTS `wp_duplicator`;
CREATE TABLE `wp_duplicator` (
  `bid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `zipname` varchar(250) NOT NULL,
  `zipsize` int(11) DEFAULT NULL,
  `created` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `owner` varchar(60) NOT NULL,
  `type` varchar(30) NOT NULL,
  `status` varchar(30) NOT NULL,
  `ver_plug` varchar(10) NOT NULL,
  `ver_db` varchar(10) NOT NULL,
  PRIMARY KEY (`bid`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_fbthumbnails`
--

DROP TABLE IF EXISTS `wp_fbthumbnails`;
CREATE TABLE `wp_fbthumbnails` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `thumbnail` varchar(150) NOT NULL,
  `path` text NOT NULL,
  `url` text NOT NULL,
  `active` int(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `wp_links`
--

DROP TABLE IF EXISTS `wp_links`;
CREATE TABLE `wp_links` (
  `link_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `link_url` varchar(255) NOT NULL DEFAULT '',
  `link_name` varchar(255) NOT NULL DEFAULT '',
  `link_image` varchar(255) NOT NULL DEFAULT '',
  `link_target` varchar(25) NOT NULL DEFAULT '',
  `link_description` varchar(255) NOT NULL DEFAULT '',
  `link_visible` varchar(20) NOT NULL DEFAULT 'Y',
  `link_owner` bigint(20) unsigned NOT NULL DEFAULT '1',
  `link_rating` int(11) NOT NULL DEFAULT '0',
  `link_updated` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `link_rel` varchar(255) NOT NULL DEFAULT '',
  `link_notes` mediumtext NOT NULL,
  `link_rss` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`link_id`),
  KEY `link_visible` (`link_visible`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_options`
--

DROP TABLE IF EXISTS `wp_options`;
CREATE TABLE `wp_options` (
  `option_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `blog_id` int(11) NOT NULL DEFAULT '0',
  `option_name` varchar(64) NOT NULL DEFAULT '',
  `option_value` longtext NOT NULL,
  `autoload` varchar(20) NOT NULL DEFAULT 'yes',
  PRIMARY KEY (`option_id`),
  UNIQUE KEY `option_name` (`option_name`)
) ENGINE=InnoDB AUTO_INCREMENT=2276 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_postmeta`
--

DROP TABLE IF EXISTS `wp_postmeta`;
CREATE TABLE `wp_postmeta` (
  `meta_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `post_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `meta_key` varchar(255) DEFAULT NULL,
  `meta_value` longtext,
  PRIMARY KEY (`meta_id`),
  KEY `post_id` (`post_id`),
  KEY `meta_key` (`meta_key`(191))
) ENGINE=InnoDB AUTO_INCREMENT=3643 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_posts`
--

DROP TABLE IF EXISTS `wp_posts`;
CREATE TABLE `wp_posts` (
  `ID` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `post_author` bigint(20) unsigned NOT NULL DEFAULT '0',
  `post_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_date_gmt` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_content` longtext NOT NULL,
  `post_title` text NOT NULL,
  `post_excerpt` text NOT NULL,
  `post_status` varchar(20) NOT NULL DEFAULT 'publish',
  `comment_status` varchar(20) NOT NULL DEFAULT 'open',
  `ping_status` varchar(20) NOT NULL DEFAULT 'open',
  `post_password` varchar(20) NOT NULL DEFAULT '',
  `post_name` varchar(200) NOT NULL DEFAULT '',
  `to_ping` text NOT NULL,
  `pinged` text NOT NULL,
  `post_modified` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_modified_gmt` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_content_filtered` text NOT NULL,
  `post_parent` bigint(20) unsigned NOT NULL DEFAULT '0',
  `guid` varchar(255) NOT NULL DEFAULT '',
  `menu_order` int(11) NOT NULL DEFAULT '0',
  `post_type` varchar(20) NOT NULL DEFAULT 'post',
  `post_mime_type` varchar(100) NOT NULL DEFAULT '',
  `comment_count` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  KEY `post_name` (`post_name`(191)),
  KEY `type_status_date` (`post_type`,`post_status`,`post_date`,`ID`),
  KEY `post_parent` (`post_parent`),
  KEY `post_author` (`post_author`)
) ENGINE=InnoDB AUTO_INCREMENT=1319 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_term_relationships`
--

DROP TABLE IF EXISTS `wp_term_relationships`;
CREATE TABLE `wp_term_relationships` (
  `object_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `term_taxonomy_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `term_order` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`object_id`,`term_taxonomy_id`),
  KEY `term_taxonomy_id` (`term_taxonomy_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_term_taxonomy`
--

DROP TABLE IF EXISTS `wp_term_taxonomy`;
CREATE TABLE `wp_term_taxonomy` (
  `term_taxonomy_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `term_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `taxonomy` varchar(32) NOT NULL DEFAULT '',
  `description` longtext NOT NULL,
  `parent` bigint(20) unsigned NOT NULL DEFAULT '0',
  `count` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`term_taxonomy_id`),
  UNIQUE KEY `term_id_taxonomy` (`term_id`,`taxonomy`),
  KEY `taxonomy` (`taxonomy`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_terms`
--

DROP TABLE IF EXISTS `wp_terms`;
CREATE TABLE `wp_terms` (
  `term_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL DEFAULT '',
  `slug` varchar(200) NOT NULL DEFAULT '',
  `term_group` bigint(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`term_id`),
  UNIQUE KEY `slug` (`slug`),
  KEY `name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=latin1;

--
-- Table structure for table `wp_translations`
--

DROP TABLE IF EXISTS `wp_translations`;
CREATE TABLE `wp_translations` (
  `original` text COLLATE utf8_bin NOT NULL,
  `lang` char(5) COLLATE utf8_bin NOT NULL,
  `translated` text COLLATE utf8_bin,
  `source` tinyint(4) NOT NULL,
  KEY `original` (`original`(6),`lang`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

--
-- Table structure for table `wp_translations_log`
--

DROP TABLE IF EXISTS `wp_translations_log`;
CREATE TABLE `wp_translations_log` (
  `original` text COLLATE utf8_bin NOT NULL,
  `lang` char(5) COLLATE utf8_bin NOT NULL,
  `translated` text COLLATE utf8_bin,
  `translated_by` varchar(15) COLLATE utf8_bin DEFAULT NULL,
  `source` tinyint(4) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  KEY `original` (`original`(6),`lang`,`timestamp`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;

--
-- Table structure for table `wp_usermeta`
--

DROP TABLE IF EXISTS `wp_usermeta`;
CREATE TABLE `wp_usermeta` (
  `umeta_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `meta_key` varchar(255) DEFAULT NULL,
  `meta_value` longtext,
  PRIMARY KEY (`umeta_id`),
  KEY `user_id` (`user_id`),
  KEY `meta_key` (`meta_key`(191))
) ENGINE=InnoDB AUTO_INCREMENT=1402 DEFAULT CHARSET=utf8;

--
-- Table structure for table `wp_users`
--

DROP TABLE IF EXISTS `wp_users`;
CREATE TABLE `wp_users` (
  `ID` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `user_login` varchar(60) NOT NULL DEFAULT '',
  `user_pass` varchar(64) NOT NULL DEFAULT '',
  `user_nicename` varchar(50) NOT NULL DEFAULT '',
  `user_email` varchar(100) NOT NULL DEFAULT '',
  `user_url` varchar(100) NOT NULL DEFAULT '',
  `user_registered` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `user_activation_key` varchar(60) NOT NULL DEFAULT '',
  `user_status` int(11) NOT NULL DEFAULT '0',
  `display_name` varchar(250) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`),
  KEY `user_login_key` (`user_login`),
  KEY `user_nicename` (`user_nicename`)
) ENGINE=InnoDB AUTO_INCREMENT=86 DEFAULT CHARSET=utf8;

--
-- Current Database: `sensor_download`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `sensor_download` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `sensor_download`;

--
-- Table structure for table `country`
--

DROP TABLE IF EXISTS `country`;
CREATE TABLE `country` (
  `id` varchar(4) NOT NULL DEFAULT '',
  `name` varchar(64) DEFAULT NULL,
  `population` int(11) DEFAULT NULL,
  UNIQUE KEY `country_id_u` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `job`
--

DROP TABLE IF EXISTS `job`;
CREATE TABLE `job` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) DEFAULT NULL,
  `create_time` int(11) DEFAULT NULL,
  `finish_time` int(11) DEFAULT NULL,
  `size` int(11) DEFAULT NULL,
  `priority` int(6) DEFAULT NULL,
  `url` varchar(256) DEFAULT NULL,
  `local_path` varchar(256) DEFAULT NULL,
  `list_triggerid` mediumblob,
  PRIMARY KEY (`id`),
  KEY `job_userid` (`userid`),
  KEY `job_create_time` (`create_time`),
  KEY `job_finish_time` (`finish_time`)
) ENGINE=InnoDB AUTO_INCREMENT=36 DEFAULT CHARSET=latin1;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) DEFAULT NULL,
  `name` varchar(256) DEFAULT NULL,
  `password` varchar(256) DEFAULT NULL,
  `affiliation` varchar(256) DEFAULT NULL,
  `address` varchar(256) DEFAULT NULL,
  `countryid` varchar(4) DEFAULT NULL,
  `level` smallint(6) DEFAULT NULL,
  `dl_bytes_limit` int(11) DEFAULT '1000000000',
  `dl_bytes_actual` int(11) DEFAULT NULL,
  `cookie` varchar(256) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `user_name` (`name`),
  KEY `user_create_time` (`create_time`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Current Database: `continual_download`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `continual_download` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `continual_download`;

--
-- Table structure for table `country`
--

DROP TABLE IF EXISTS `country`;
CREATE TABLE `country` (
  `id` varchar(4) NOT NULL DEFAULT '',
  `name` varchar(64) DEFAULT NULL,
  `population` int(11) DEFAULT NULL,
  UNIQUE KEY `country_id_u` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `job`
--

DROP TABLE IF EXISTS `job`;
CREATE TABLE `job` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` int(11) DEFAULT NULL,
  `create_time` int(11) DEFAULT NULL,
  `finish_time` int(11) DEFAULT NULL,
  `size` int(11) DEFAULT NULL,
  `priority` int(6) DEFAULT NULL,
  `url` varchar(256) DEFAULT NULL,
  `local_path` varchar(256) DEFAULT NULL,
  `list_triggerid` mediumblob,
  PRIMARY KEY (`id`),
  KEY `job_userid` (`userid`),
  KEY `job_create_time` (`create_time`),
  KEY `job_finish_time` (`finish_time`)
) ENGINE=InnoDB AUTO_INCREMENT=1532 DEFAULT CHARSET=latin1;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `create_time` int(11) DEFAULT NULL,
  `name` varchar(256) DEFAULT NULL,
  `password` varchar(256) DEFAULT NULL,
  `affiliation` varchar(256) DEFAULT NULL,
  `address` varchar(256) DEFAULT NULL,
  `countryid` varchar(4) DEFAULT NULL,
  `level` smallint(6) DEFAULT NULL,
  `dl_bytes_limit` int(11) DEFAULT '1000000000',
  `dl_bytes_actual` int(11) DEFAULT NULL,
  `cookie` varchar(256) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `user_name` (`name`),
  KEY `user_create_time` (`create_time`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-09-20 19:06:17
