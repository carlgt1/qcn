select h.os_name, h.os_version, count(distinct hostid) 
from sensor_archive.qcn_trigger t, host h 
where t.hostid=h.id 
 and varietyid=0 
 group by os_name,os_version
;
