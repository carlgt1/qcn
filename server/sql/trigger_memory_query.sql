select db_name,triggerid,
from_unixtime(time_trigger),
from_unixtime(time_received),
time_received-time_trigger timediff, 
mxy1p,mz1p,mxy1a,mz1a,mxy2a,mz2a,mxy4a,mz4a 
from qcn_trigger_memory order by timediff;

select avg(time_received-time_trigger) from qcn_trigger_memory;

