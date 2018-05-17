drop database trigmem_test;
create database trigmem_test;
create table trigmem_test.qcn_trigger_memory
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

alter table trigmem_test.qcn_trigger_memory ADD PRIMARY KEY (db_name, triggerid);
create index qcn_trigger_memory_time on trigmem_test.qcn_trigger_memory (time_trigger desc, varietyid asc);
create index qcn_trigger_memory_hostid on trigmem_test.qcn_trigger_memory(hostid, time_trigger, qcn_quakeid, varietyid, posted);
create index qcn_trigger_memory_latlng on trigmem_test.qcn_trigger_memory(latitude, longitude, qcn_quakeid, varietyid, posted);
create index qcn_trigger_memory_file on trigmem_test.qcn_trigger_memory(file);

