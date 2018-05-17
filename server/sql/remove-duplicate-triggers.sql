# obviously use with caution!
# but this was necessary one-time due to the problem with the multiple 
# trickles sent when the auto-earthquake matching script sent too big
# file requests to clients, so some hosts sent hundreds of thousands of triggers!


drop table qcn_trigger_backup;
create table qcn_trigger_backup like qcn_trigger;
insert into qcn_trigger_backup select * from qcn_trigger;

drop table qcn_trigger_distinct;
create table qcn_trigger_distinct like qcn_trigger;

insert into qcn_trigger_distinct 
(hostid, ipaddr, result_name, time_trigger,
 time_received, time_sync,  sync_offset,
 significance, magnitude, latitude,
 longitude, depth_km, file, dt,
 numreset, qcn_sensorid, sw_version, qcn_quakeid,
 time_filereq, received_file, runtime_clock, runtime_cpu)
select distinct hostid, ipaddr, result_name, time_trigger,
 time_trigger, time_sync,  sync_offset,
 significance, magnitude, latitude,
 longitude, depth_km, file, dt,
 numreset, qcn_sensorid, sw_version, qcn_quakeid,
 time_filereq, received_file, runtime_clock, runtime_cpu
from qcn_trigger_backup;

drop table qcn_trigger;
create table qcn_trigger like qcn_trigger_backup;

insert into qcn_trigger
(hostid, ipaddr, result_name, time_trigger,
 time_received, time_sync,  sync_offset,
 significance, magnitude, latitude,
 longitude, depth_km, file, dt,
 numreset, qcn_sensorid, sw_version, qcn_quakeid,
 time_filereq, received_file, runtime_clock, runtime_cpu)
select hostid, ipaddr, result_name, time_trigger,
 time_received, time_sync,  sync_offset,
 significance, magnitude, latitude,
 longitude, depth_km, file, dt,
 numreset, qcn_sensorid, sw_version, qcn_quakeid,
 time_filereq, received_file, runtime_clock, runtime_cpu
from qcn_trigger_distinct;


