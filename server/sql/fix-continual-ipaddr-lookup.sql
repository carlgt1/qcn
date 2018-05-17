// update host ipaddr's to just use one lat/lng per host

create temporary table tmp_setip 
   (select id from continual.qcn_host_ipaddr where (geoipaddrid is null or geoipaddrid=0) and ipaddr is not null and ipaddr!=''
        and hostid not in (select hostid from continual.qcn_host_ipaddr where ipaddr is null or ipaddr='')
        and hostid not in (select hostid from continual.qcn_host_ipaddr where (geoipaddrid is null or geoipaddrid=0) group by hostid having count(*)>1)
);

update continual.qcn_host_ipaddr set ipaddr='' where id in (select id from tmp_setip);

// update trigger table with "sole location" hosts

update continual.qcn_trigger t, continual.qcn_host_ipaddr i set t.latitude=i.latitude,t.longitude=i.longitude where t.hostid=i.hostid and i.ipaddr='' and i.geoipaddrid=0;

// query to see which hosts in the trigger table have geoipaddr records

select t.hostid, t.ipaddr, count(*) 
   from continual.qcn_trigger t, continual.qcn_host_ipaddr i
   where t.hostid=i.hostid and t.ipaddr=i.ipaddr and i.geoipaddrid>0 
     and round(t.latitude,5)=round(i.latitude,5) 
     and round(t.longitude,5)=round(i.longitude,5)
  group by t.hostid, t.ipaddr;


select t.hostid, count(*)    
   from continual.qcn_trigger t, continual.qcn_host_ipaddr i    
   where t.hostid=i.hostid and t.ipaddr=i.ipaddr and i.geoipaddrid>0
     and round(t.latitude,5)=round(i.latitude,5)
     and round(t.longitude,5)=round(i.longitude,5)
   group by t.hostid;

+--------+----------+
| hostid | count(*) |
+--------+----------+
|      4 |     1286 | 
|      5 |        4 | 
|      8 |       39 | 
|     21 |    33475 | 
|     28 |       21 | 
|     47 |       15 | 
|     70 |        1 | 
|     87 |        7 | 
|     88 |     5087 | 
|     94 |     5532 | 
|     96 |     2082 | 
|    106 |        1 | 
|    110 |      132 | 
|    126 |     2147 | 
|    128 |       67 | 
|    143 |      610 | 
|    155 |      477 | 
|    158 |    16386 | 
|    174 |     1353 | 
|    182 |       48 | 
|    183 |        9 | 
|    184 |     1445 | 
|    186 |        1 | 
|    216 |      864 | 
|    224 |      742 | 
|    225 |        2 | 
+--------+----------+


