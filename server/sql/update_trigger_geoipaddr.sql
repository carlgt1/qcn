update qcn_trigger t
set hostipaddrid=
IFNULL((select id from qcn_host_ipaddr i 
   where i.hostid=t.hostid
     and round(i.latitude,3)=round(t.latitude,3) 
    and round(i.longitude,3)=round(t.longitude,3) LIMIT 1), 0)
, geoipaddrid=
IFNULL((select geoipaddrid from qcn_host_ipaddr i 
   where i.hostid=t.hostid
     and round(i.latitude,3)=round(t.latitude,3) 
    and round(i.longitude,3)=round(t.longitude,3) LIMIT 1), 0)
where t.hostipaddrid = 0;


