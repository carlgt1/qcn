select db_hostid, max_trigid, latitude, longitude from 
(
select concat('Q', hostid) db_hostid , max(id) max_trigid, latitude,longitude 
from sensor.qcn_trigger where time_trigger > unix_timestamp('2010-01-01')
group by db_hostid
union 
select concat('Q', hostid) db_hostid, max(id) max_trigid, latitude,longitude 
from sensor_archive.qcn_trigger where time_trigger > unix_timestamp('2010-01-01')
group by db_hostid
union
select concat('C', hostid) db_hostid, max(id) max_trigid, latitude,longitude 
from continual.qcn_trigger where time_trigger > unix_timestamp('2010-01-01')
group by db_hostid
union
select concat('C', hostid) db_hostid, max(id) max_trigid, latitude,longitude 
from continual_archive.qcn_trigger where time_trigger > unix_timestamp('2010-01-01')
group by db_hostid
) b
group by db_hostid;
