// build a list of all files for a resultname & host
insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
values(
unix_timestamp(),
570,
'filelist',
0,
'<trickle_down>
<result_name>qcne_002040_0</result_name>
<filelist>
<sendme>qcne_002040_000243_1209189699.zip</sendme> 
<sendme>qcne_002040_000244_1209190557.zip</sendme> 
<sendme>qcne_002040_000245_1209190604.zip</sendme> 
<sendme>qcne_002040_000246_1209190674.zip</sendme> 
<sendme>qcne_002040_000247_1209191502.zip</sendme> 
<sendme>qcne_002040_000248_1209192013.zip</sendme> 
<sendme>qcne_002040_000249_1209192092.zip</sendme> 
<sendme>qcne_002040_000250_1209192116.zip</sendme> 
<sendme>qcne_002040_000251_1209192303.zip</sendme> 
<sendme>qcne_002040_000279_1209198103.zip</sendme> 
</filelist>
</trickle_down>\n'
);

// get list for above query based on hostid
select
concat('<sendme>',t.file,'</sendme>')
from qcn_trigger t
where hostid=570
  and t.latitude between 39.0 and 40.0
  and t.longitude between -120.5 and -119.5
  and t.time_trigger between
             unix_timestamp('2008-04-25 23:00')
   AND unix_timestamp('2008-04-26 03:00')
;


// uses latest result name
insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
select
unix_timestamp(), 
t.hostid,
'filelist',
0,
concat(
'<trickle_down>
<result_name>', r.name, '</result_name>
<filelist>
<sendme>', t.file, '</sendme>
</filelist>
</trickle_down>\n')
from qcn_trigger t, result r
where t.hostid=r.hostid
  and r.sent_time=(select max(rr.sent_time) from result rr where rr.hostid=r.hostid)
  and t.latitude between 39.0 and 40.0
  and t.longitude between -120.5 and -119.5
  and t.time_trigger between 
             unix_timestamp('2008-04-25 23:30')
   AND unix_timestamp('2008-04-26 02:30')
;


// uses result name from trigger
insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
select
unix_timestamp(), 
t.hostid,
'filelist',
0,
concat(
'<trickle_down>
<result_name>', result_name, '</result_name>
<filelist>
<sendme>', t.file, '</sendme>
</filelist>
</trickle_down>\n')
from qcn_trigger t, user u, host h
where t.hostid=h.id and h.userid=u.id
  and t.latitude between 39.0 and 40.0
  and t.longitude between -120.5 and -119.5
  and t.time_trigger between 
             unix_timestamp('2008-04-25 23:30')
   AND unix_timestamp('2008-04-26 02:30')
;

insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
values 
(unix_timestamp(),
27,
'filelist',0,
'<trickle_down>
<result_name>qcne_000066_0</result_name>
<filelist>
<sendme>qcne_000066_000072_1201982200.zip</sendme>
<sendme>qcne_000066_000141_1202139034.zip</sendme>
<sendme>qcne_000066_000145_1202146211.zip</sendme>
<sendme>qcne_000066_000131_1202112524.zip</sendme>
<sendme>qcne_000066_000051_1201964379.zip</sendme>
<sendme>qcne_000066_000043_1201957531.zip</sendme>
<sendme>qcne_000066_000029_1201934523.zip</sendme>
</filelist>
</trickle_down>\n');
