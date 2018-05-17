select from_unixtime(t.time_trigger), u.id, u.name, u.email_addr, h.id, h.domain_name,
  t.time_trigger, t.time_received, t.time_sync,t.significance, t.magnitude,
   t.latitude, t.longitude, t.file, t.dt, t.numreset, t.qcn_sensorid, t.sw_version
from qcn_trigger t, user u, host h
where t.hostid=h.id and h.userid=u.id
  and t.latitude between 39.0 and 40.0
  and t.longitude between -120.5 and -119.5
  and t.time_trigger between 
             unix_timestamp('2008-04-25 23:30')
   AND unix_timestamp('2008-04-26 02:30')
order by t.time_trigger
; 

