select concat('S,',hostid, ',', latitude, ',', longitude, ',' , count(*)) from sensor.qcn_trigger group by hostid,latitude,longitude
union
select concat('C,',hostid, ',', latitude, ',', longitude, ',' , count(*)) from continual.qcn_trigger group by hostid,latitude,longitude
;
