select 
  concat(m.mydb, ',' , m.id, ',', m.hostid, ',', 
    from_unixtime(m.time_trigger), ',', FLOOR(ROUND((m.time_trigger-FLOOR(m.time_trigger)), 6) * 1e6),
     ',',
    m.magnitude, ',', m.significance, ',',
    m.latitude, ',', m.longitude, ',', m.file,',', m.numreset, ',',
    s.description, ',', IFNULL(a.description,''), ',' , IFNULL(m.levelvalue,''), ',', IFNULL(l.description,'')
  )
from
(
select 'Q' mydb, t.*
from sensor.qcn_trigger t
where time_trigger between unix_timestamp('2010-04-04 00:00:00') and unix_timestamp('2010-04-07 00:00:00') 
and time_sync>0
and varietyid in (0,2)
and received_file=100
and latitude between 31.5 and 37.5 and longitude between -121 and -114

UNION

select 'C' mydb, tt.*
from continual.qcn_trigger tt
where time_trigger between unix_timestamp('2010-04-04 00:00:00') and unix_timestamp('2010-04-07 00:00:00') 
and time_sync>0
and varietyid in (0,2)
and received_file=100
and latitude between 31.5 and 37.5 and longitude between -121 and -114
) m
LEFT JOIN sensor.qcn_sensor s ON m.qcn_sensorid = s.id
LEFT OUTER JOIN sensor.qcn_align a ON m.alignid = a.id
LEFT OUTER JOIN sensor.qcn_level l ON m.levelid = l.id
where m.qcn_sensorid=s.id
order by time_trigger,hostid
;
