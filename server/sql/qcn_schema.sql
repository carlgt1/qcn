drop table if exists qcn_host_ipaddr;
drop table if exists qcn_trigger;
drop table if exists qcn_trigger_followup;
drop table if exists trigmem.qcn_trigger_memory;
drop table if exists trigmem.qcn_trigger_followup;
drop database trigmem;
drop table if exists qcn_geo_ipaddr;
drop table if exists qcn_quake;
drop table if exists qcn_sensor;
drop table if exists qcn_level;
drop table if exists qcn_align;
drop table if exists qcn_variety;
drop table if exists qcn_dluser;
drop table if exists qcn_post;
drop table if exists qcn_ramp_participant;
drop table if exists qcn_ramp_coordinator;
drop table if exists qcn_constant;
drop table if exists qcn_kml_region;
drop table if exists qcn_trigsummary;

create table qcn_constant (
    id int not null primary key auto_increment,
    description varchar(64) not null,
    value_int int,
    value_text varchar(64),
    value_float float
);
create index qcn_constant_description on qcn_constant(description);
insert into qcn_constant (description, value_int) values ('ArchiveTime', unix_timestamp());

create table qcn_sensor (id smallint not null primary key, is_usb boolean not null default 0, 
    is_gps boolean not null default 0, description varchar(64));
insert into qcn_sensor values (0   , 0, 0, 'Not Found');
insert into qcn_sensor values (1   , 0, 0, 'Mac PPC 1');
insert into qcn_sensor values (2   , 0, 0, 'Mac PPC 2');
insert into qcn_sensor values (3   , 0, 0, 'Mac PPC 3');
insert into qcn_sensor values (4   , 0, 0, 'Mac Intel');
insert into qcn_sensor values (5   , 0, 0, 'Lenovo Thinkpad');
insert into qcn_sensor values (6   , 0, 0, 'HP Laptop');
insert into qcn_sensor values (7, 0, 0, 'Android Device');
insert into qcn_sensor values (100 , 1, 0, 'JoyWarrior 24F8 USB');
insert into qcn_sensor values (101 , 1, 0, 'MotionNode Accel USB');
insert into qcn_sensor values (102 , 1, 0, 'ONavi 1 USB');
insert into qcn_sensor values (103 , 1, 0, 'JoyWarrior 24F14 USB');
insert into qcn_sensor values (104 , 1, 0, 'ONavi A 12-bit USB');
insert into qcn_sensor values (105 , 1, 0, 'ONavi B 16-bit USB');
insert into qcn_sensor values (106 , 1, 0, 'ONavi C 24-bit USB');
insert into qcn_sensor values (107 , 1, 0, 'Phidgets 1056 228ug USB');
insert into qcn_sensor values (108 , 1, 0, 'Phidgets 1042 976ug USB');
insert into qcn_sensor values (109 , 1, 0, 'Phidgets 1044 76ug USB');
insert into qcn_sensor values (110 , 1, 0, 'Phidgets 1041 976ug USB');
insert into qcn_sensor values (111 , 1, 0, 'Phidgets 1043 76ug USB');
insert into qcn_sensor values (1000, 1, 1, 'Phidgets 1040 GPS USB');

create table qcn_level (id smallint not null primary key, description varchar(64));
insert into qcn_level values (0, 'N/A');
insert into qcn_level values (1, 'Floor (+/- above/below surface)');
insert into qcn_level values (2, 'Meters (above/below surface)');
insert into qcn_level values (3, 'Feet (above/below surface)');
insert into qcn_level values (4, 'Elevation - meters above sea level');
insert into qcn_level values (5, 'Elevation - feet above sea level');

create table qcn_align (id smallint not null primary key default 0, description varchar(64));
insert into qcn_align values (0, 'Unaligned');
insert into qcn_align values (1, 'Magnetic North');
insert into qcn_align values (2, 'South');
insert into qcn_align values (3, 'East');
insert into qcn_align values (4, 'West');
insert into qcn_align values (5, 'Wall');
insert into qcn_align values (6, 'True North');

create table qcn_variety (id smallint not null primary key default 0, description varchar(64));
insert into qcn_variety values (-2, 'Final Stats Trigger');
insert into qcn_variety values (-1, 'Quakelist Trigger');
insert into qcn_variety values (0, 'Normal Trigger');
insert into qcn_variety values (1, 'Ping Trigger');
insert into qcn_variety values (2, 'Continual Trigger');

create table qcn_dluser (userid int not null primary key);

create table qcn_ramp_coordinator (
    id int not null primary key,
    userid int not null,
    receive_distribute boolean,
    help_troubleshoot boolean,
    enlist_volunteers boolean,
    how_many int,
    active boolean not null default 1,
    comments varchar(255),
    time_edit int 
);
create unique index qcn_ramp_coordinator_userid on qcn_ramp_coordinator(userid);

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
  `quake_damage` varchar(5) NULL DEFAULT '',
  `liquefaction` boolean NULL DEFAULT '0',
  `time_added` int(11) NULL DEFAULT '0',
  `completed` boolean NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `qcn_ramp_participant_userid` (`userid`),
  KEY `qcn_ramp_participant_kml_regionid` (`kml_regionid`)
) ENGINE=InnoDB AUTO_INCREMENT=112 DEFAULT CHARSET=latin1;

create index qcn_ramp_participant_lname on qcn_ramp_participant(lname);
create index qcn_ramp_participant_country on qcn_ramp_participant(country);
create index qcn_ramp_participant_time_added on qcn_ramp_participant(time_added);
create index qcn_ramp_participant_time_completed on qcn_ramp_participant(time_completed);
create index qcn_ramp_participant_completed on qcn_ramp_participant(completed);

create table qcn_kml_region
(
  id int primary key auto_increment,
  name varchar(64),
  filename varchar(255),
  data blob
);

insert into qcn_kml_region values (1, 'Anchorage Alaska RAMP', 'Anchorage_RAMP.kml', null);
insert into qcn_kml_region values (2, 'California Metro RAMP', 'CA_Metro_RAMP.kml', null);
insert into qcn_kml_region values (3, 'Hayward RAMP', 'Hayward_RAMP.kml', null);
insert into qcn_kml_region values (4, 'Istanbul RAMP', 'Istanbul_RAMP.kml', null);
insert into qcn_kml_region values (5, 'New Madrid Fault RAMP (MO,TN,AR,KY)', 'NM_RAMP.kml', null);
insert into qcn_kml_region values (6, 'Oregon Metro RAMP', 'OR_Metro_RAMP.kml', null);
insert into qcn_kml_region values (7, 'Pacific Northwest Coastal RAMP', 'PNW_Coast_RAMP.kml', null);
insert into qcn_kml_region values (8, 'San Andreas Fault North RAMP', 'SAF_N_RAMP.kml', null);
insert into qcn_kml_region values (9, 'San Francisco RAMP', 'SAF_RAMP.kml', null);
insert into qcn_kml_region values (10, 'San Andreas Fault South RAMP', 'SAF_S_RAMP.kml', null);
insert into qcn_kml_region values (11, 'Washington State Metro RAMP', 'WA_Metro_RAMP.kml', null);
insert into qcn_kml_region values (12, 'Wasatch (SLC Utah) RAMP', 'Wasatch_RAMP.kml', null);

create table qcn_reds (
    id int not null primary key auto_increment,
    userid int not null,
    name varchar(64),
    email_addr varchar(100),
    magnitude_min_global double,
    magnitude_min_local double,
    active boolean not null default 1,
    time_edit int
);
create unique index qcn_reds_userid on qcn_reds(userid);

create table qcn_post (
    id int not null primary key,
    where_clause varchar(255) not null,
    url varchar(255) not null,
    active boolean not null default 1,
    contact_name varchar(255),
    contact_email varchar(255),
    contact_address varchar(255)
);

insert into qcn_post values (1, 
  'latitude BETWEEN -15 AND -13 AND longitude BETWEEN -173 AND -169', 
  'http://qcn-upl.stanford.edu/carlc/test-post.php',
  1,
  'Mihai Tarabuta',
  'mihai.tarabuta@mobilis.com',
  ''
);

create table qcn_host_ipaddr
(
id int(11) not null primary key auto_increment,
hostid int(11) not null,
ipaddr varchar(32) not null default '',
location varchar(128) not null default '',
latitude double,
longitude double,
levelvalue float,
levelid smallint,
alignid smallint,
geoipaddrid int(11) not null default 0
);

create unique index qcn_host_ipaddr_id on qcn_host_ipaddr (hostid,ipaddr,geoipaddrid);

create table qcn_geo_ipaddr
(
id int(11) not null primary key auto_increment,
ipaddr varchar(32) not null,
time_lookup double,
country varchar(32),
region  varchar(32),
city    varchar(32),
latitude double,
longitude double
);

create unique index qcn_geo_ipaddr_ipaddr on qcn_geo_ipaddr (ipaddr);

create table qcn_quake
(
id int(11) not null primary key auto_increment,
time_utc double,
magnitude double,
depth_km double,
latitude double,
longitude double,
description varchar(256),
processed bool,
url varchar(256),
guid varchar(256)
);

create unique index qcn_quake_guid on qcn_quake (guid);
alter table qcn_quake add index qcn_quake_magnitude (magnitude);
alter table qcn_quake add index qcn_time_utc (time_utc);
alter table qcn_quake add index qcn_latitude (latitude);
alter table qcn_quake add index qcn_longitude (longitude);

create table qcn_trigger
(
id int(11) not null primary key auto_increment,
hostid int(11) not null,
ipaddr varchar(32) not null,
result_name varchar(64) not null,
time_trigger double,
time_received double,
time_sync double,
sync_offset double,
significance double,
magnitude double,
mxy1p float null,
mz1p float null,
mxy1a float null,
mz1a float null,
mxy2a float null,
mz2a float null,
mxy4a float null,
mz4a float null,
latitude double,
longitude double,
levelvalue float,
levelid smallint,
alignid smallint,
file varchar(64),
dt float,
numreset int(6),
qcn_sensorid int(3),
sw_version varchar(8),
os_type varchar(8),
qcn_quakeid int(11),
time_filereq double,
received_file tinyint(1),
runtime_clock double,
runtime_cpu double,
varietyid smallint not null default 0,
flag boolean not null default 0,
hostipaddrid int(11) not null default 0,
geoipaddrid int(11) not null default 0
);

create index qcn_trigger_file on qcn_trigger(file);
create index qcn_trigger_sensorid on qcn_trigger(qcn_sensorid);
create index qcn_trigger_quakeid on qcn_trigger(quakeid);
create index qcn_trigger_flag on qcn_trigger(flag);

create index qcn_trigger_time_trigger on qcn_trigger(time_trigger);
create index qcn_trigger_hostid on qcn_trigger(hostid);
create index qcn_trigger_time_filereq on qcn_trigger(time_filereq);
create index qcn_trigger_received_file on qcn_trigger(received_file);
create index qcn_trigger_varietyid on qcn_trigger(varietyid);
create index qcn_trigger_latitude on qcn_trigger(latitude);
create index qcn_trigger_longitude on qcn_trigger(longitude);
create index qcn_trigger_result_name on qcn_trigger(result_name);


create table qcn_remote
(
   id int(11) not null primary key,
   script_name varchar(64) not null,
   name varchar(64) not null,
   url varchar(128) not null,
   country varchar(32),
   trigger_url varchar(128) not null,
   download_url varchar(128) not null,
   password varchar(32) not null,
   contact_email_1 varchar(128) not null,
   contact_email_2 varchar(128),
   contact_email_3 varchar(128)
);

create index qcn_script_name on qcn_remote(script_name);
create index qcn_remote_name on qcn_remote(name);
create index qcn_remote_url on qcn_remote(url);

insert into qcn_remote values (1, 'st_sensor_rep.php', 'QCN Stanford Sensor', 'http://qcn.stanford.edu/sensor', 'US',
    'http://qcn.stanford.edu/sensor', 'http://qcn-upl.stanford.edu/trigger', '', 'carlgt1@yahoo.com', 'jflawrence@stanford.edu', 'escochran@gmail.com');
insert into qcn_remote values (2, 'st_continual_rep.php', 'QCN Stanford Continual', 'http://qcn.stanford.edu/continual', 'US',
    'http://qcn.stanford.edu/continual', 'http://qcn-upl.stanford.edu/trigger/continual', '', 'carlgt1@yahoo.com', 'jflawrence@stanford.edu', 'escochran@gmail.com');
insert into qcn_remote values (3, 'tw_sensor_rep.php', 'QCN Taiwan Sensor', 'http://qcn.twgrid.org/sensor', 'TW',
    'http://qcn.stanford.edu/sensor/tw_sensor_rep.php', 'http://qcn.twgrid.org/trigger', '', 'waynesan@twgrid.org', 'eric.yen@twgrid.org', 'ychen@twgrid.org');
insert into qcn_remote values (4, 'tw_continual_rep.php', 'QCN Taiwan Continual', 'http://qcn.twgrid.org/continual', 'TW',
    'http://qcn.stanford.edu/sensor/tw_continual_rep.php', 'http://qcn.twgrid.org/trigger/continual', '', 'waynesan@twgrid.org', 'eric.yen@twgrid.org', 'ychen@twgrid.org');
insert into qcn_remote values (5, 'mx_sensor_rep.php', 'QCN Mexico Sensor', 'http://www.ras.unam.mx/sensor', 'MX',
    'http://qcn.stanford.edu/sensor/mx_sensor_rep.php', 'http://www.ras.unam.mx/trigger', '', 'ladominguez@geofisica.unam.mx', 'uskerhay@gmail.com', '');
insert into qcn_remote values (6, 'mx_continual_rep.php', 'QCN Mexico Continual', 'http://www.ras.unam.mx/continual', 'MX',
    'http://qcn.stanford.edu/sensor/mx_continual_rep.php', 'http://www.ras.unam.mx/trigger/continual', '', 'ladominguez@geofisica.unam.mx', 'uskerhay@gmail.com', '');
insert into qcn_remote values (7, 'fr_sensor_rep.php', 'QCN France Sensor', 'http://emsc-csem.org/sensor', 'FR',
    'http://qcn.stanford.edu/sensor/fr_sensor_rep.php', 'http://emsc-csem.org/trigger', '', 'mazet@emsc-csem.org', '', '');
insert into qcn_remote values (8, 'fr_continual_rep.php', 'QCN France Continual', 'http://emsc-csem.org/continual', 'FR',
    'http://qcn.stanford.edu/sensor/fr_continual_rep.php', 'http://emsc-csem.org/trigger/continual', '', 'mazet@emsc-csem.org', '', '');



create table qcn_trigger_remote
(
id int(11) not null primary key auto_increment,
qcn_remoteid int(11) not null,
hostid int(11) not null,
ipaddr varchar(32) not null,
result_name varchar(64) not null,
time_trigger double,
time_received double,
time_sync double,
sync_offset double,
significance double,
magnitude double,
mxy1p float null,
mz1p float null,
mxy1a float null,
mz1a float null,
mxy2a float null,
mz2a float null,
mxy4a float null,
mz4a float null,
latitude double,
longitude double,
levelvalue float,
levelid smallint,
alignid smallint,
file varchar(64),
dt float,
numreset int(6),
qcn_sensorid int(3),
sw_version varchar(8),
os_type varchar(8),
qcn_quakeid int(11),
time_filereq double,
received_file tinyint(1),
runtime_clock double,
runtime_cpu double,
varietyid smallint not null default 0,
flag boolean not null default 0,
hostipaddrid int(11) not null default 0,
geoipaddrid int(11) not null default 0
);

create index qcn_trigger_remote_remoteid on qcn_trigger_remote(qcn_remoteid);
create index qcn_trigger_remote_file on qcn_trigger_remote(file);
create index qcn_trigger_remote_sensorid on qcn_trigger_remote(qcn_sensorid);
create index qcn_trigger_remote_quakeid on qcn_trigger_remote(qcn_quakeid);
create index qcn_trigger_remote_flag on qcn_trigger_remote(flag);

create index qcn_trigger_remote_time_trigger on qcn_trigger_remote(time_trigger);
create index qcn_trigger_remote_hostid on qcn_trigger_remote(hostid);
create index qcn_trigger_remote_time_filereq on qcn_trigger_remote(time_filereq);
create index qcn_trigger_remote_received_file on qcn_trigger_remote(received_file);
create index qcn_trigger_remote_varietyid on qcn_trigger_remote(varietyid);
create index qcn_trigger_remote_latitude on qcn_trigger_remote(latitude);
create index qcn_trigger_remote_longitude on qcn_trigger_remote(longitude);
create index qcn_trigger_remote_result_name on qcn_trigger_remote(result_name);


/* temp tables for stats */
CREATE TABLE qcn_recalcresult (resultid int(11) NOT NULL PRIMARY KEY, weight double, total_credit double, time_received double);
ALTER TABLE qcn_recalcresult ADD INDEX recalc_result (resultid);

CREATE TABLE qcn_stats
         (userid int(11), hostid int(11), teamid int(11), 
             result_name varchar(254), total_credit double, weight double, expavg_time double);
        
ALTER TABLE qcn_stats ADD INDEX qcn_stats_userid (userid);
ALTER TABLE qcn_stats ADD INDEX qcn_stats_hostid (hostid);
ALTER TABLE qcn_stats ADD INDEX qcn_stats_teamid (teamid);
          
CREATE TABLE qcn_trigsummary
(id integer not null auto_increment primary key,       
userid int(11), hostid int(11), teamid int(11),
result_name varchar(254),
total_credit double, weight double,
time_received double, is_archive boolean
);

CREATE INDEX qcn_trigsummary_result on qcn_trigsummary(result_name, total_credit);
CREATE INDEX qcn_trigsummary_archive on qcn_trigsummary(is_archive);

CREATE TABLE qcn_finalstats
(
    id int(11) auto_increment NOT NULL PRIMARY KEY,
    resultid int(11) NOT NULL,
    time_received double NOT NULL DEFAULT 0,
    runtime_clock double NOT NULL DEFAULT 0,
    runtime_cpu double NOT NULL DEFAULT 0
);

ALTER TABLE qcn_finalstats ADD UNIQUE INDEX qcn_finalstats_resultid (resultid);

/* make the index for result(random) */
ALTER TABLE result ADD INDEX result_random (random);

create table qcn_showhostlocation (hostid int(11) primary key not null);

create database trigmem;
create table trigmem.qcn_trigger_memory 
(
db_name varchar(16) not null,
triggerid int(11) not null,
hostid int(11) not null,
ipaddr varchar(32) not null,
result_name varchar(64) not null,
time_trigger double,
time_received double,
time_sync double,
sync_offset double,
significance double,
magnitude double,
mxy1p float null,
mz1p float null,
mxy1a float null,
mz1a float null,
mxy2a float null,
mz2a float null,
mxy4a float null,
mz4a float null,
latitude double,
longitude double,
levelvalue float,
levelid smallint,
alignid smallint,
file varchar(64),
dt float,
numreset int(6),
qcn_sensorid int(3),
varietyid smallint not null default 0,
qcn_quakeid int not null default 0,
hostipaddrid int(11) not null default 0,
geoipaddrid int(11) not null default 0,
posted boolean not null default false
) ENGINE = MEMORY;

alter table trigmem.qcn_trigger_memory ADD PRIMARY KEY (db_name, triggerid);
create index qcn_trigger_memory_time on trigmem.qcn_trigger_memory (time_trigger desc, varietyid asc);
create index qcn_trigger_memory_hostid on trigmem.qcn_trigger_memory(hostid, time_trigger, qcn_quakeid, varietyid, posted);
create index qcn_trigger_memory_latlng on trigmem.qcn_trigger_memory(latitude, longitude, qcn_quakeid, varietyid, posted);
create index qcn_trigger_memory_file on trigmem.qcn_trigger_memory(file);

/*

insert into trigmem.qcn_trigger_memory
   select 'sensor',id,hostid,ipaddr,result_name,time_trigger,time_received,time_sync,sync_offset,
      significance,magnitude,latitude,longitude,
      levelvalue, levelid, alignid, dt, numreset, qcn_sensorid, varietyid
         from qcn_trigger where time_sync>1e6 order by time_trigger desc limit 10000;


 select round(time_trigger,0) tt, round(latitude,1) lat, round(longitude,1) lng, count(*) 
    from trigmem.qcn_trigger_memory  group by tt, lat,lng;

select from_Unixtime(min(time_trigger)) min_time, from_Unixtime(max(time_trigger)) max_time
   from trigmem.qcn_trigger_memory;

*/

SOURCE qcn_country_latlng.sql

/* Now generate the stored procedures */

SOURCE do_final_trigger.sql;

SOURCE do_stats.sql;


