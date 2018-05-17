select count(*) sample_size,
           min(time_received-time_trigger) time_min, 
           max(time_received-time_trigger) time_max, 
           avg(time_received-time_trigger) time_mean, 
           stddev(time_received-time_trigger) time_stddev 
from qcn_trigger 
where sync_offset!=0 and 
(time_received-time_trigger)>0 
and (time_received-time_trigger<30);



