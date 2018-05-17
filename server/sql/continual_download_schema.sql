drop database if exists continual_download;
create database continual_download;

use continual_download;

drop table if exists user;

create table user (
id int(11) primary key not null auto_increment, 
create_time int(11),
name varchar(256), 
password varchar(256),
affiliation varchar(256),
address varchar(256),
countryid varchar(4),
level smallint, 
dl_bytes_limit int(11) default 1e9, 
dl_bytes_actual int(11), 
cookie varchar(256)
);
alter table user add index user_name (name);
alter table user add index user_create_time (create_time);

drop table if exists job;

create table job (
id int(11) primary key not null auto_increment,
userid int(11),
create_time int(11),
finish_time int(11),
size int(11),
priority int(6),
url varchar(256),
local_path varchar(256),
list_triggerid mediumblob
);
alter table job add index job_userid (userid);
alter table job add index job_create_time (create_time);
alter table job add index job_finish_time (finish_time);


source /home/boinc/projects/qcn/server/sql/country.sql;


