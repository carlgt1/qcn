select db,hostid,latitude,longitude, soh_24hrs, real_24hrs from
(select 'Q' db,hostid,latitude,longitude,
    sum( (if(varietyid=1, 1, 0 )) ) soh_24hrs,
    sum( (if(varietyid!=1, 1, 0 )) ) real_24hrs
from sensor.qcn_trigger 
where time_trigger > unix_timestamp() - (3600*24) group by 'Q', hostid, latitude, longitude
UNION
select 'C' db,hostid,latitude,longitude, 
    sum( (if(varietyid=2, 1, 0 )) ) soh_24hrs,
    sum( (if(varietyid=0, 1, 0 )) ) real_24hrs
from continual.qcn_trigger 
where time_trigger > unix_timestamp() - (3600*24) group by 'C', hostid, latitude, longitude
) qcn_24hrs
order by db, hostid, latitude, longitude;
