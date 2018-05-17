select 
  hostid, 
   s.description sensor, 
   concat(year(from_unixtime(time_trigger)), '_', lpad(dayofyear(from_unixtime(time_trigger)), 3, '0')) year_day,
   count(*) trig_per_day
into outfile 'trig_by_day.csv'
from qcn_trigger t, qcn_sensor s
   where t.qcn_sensorid = s.id and varietyid=0 and time_trigger>10000 and qcn_sensorid>0
 group by hostid, sensor, year_day;

