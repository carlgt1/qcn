create table qcn_scedc
(db varchar(10), 
triggerid int(11), 
hostid int(11), 
time_trig_utc datetime, 
time_trig_micros int(7),
time_received datetime, 
time_sync datetime, sync_offset float,
magnitude float, significance float, latitude float, longitude float, 
file varchar(255), 
numreset int(7), dt float, trigger_type varchar(2), hostipaddrid int(11), geoipaddrid int(11),
sensor varchar(128), alignment varchar(64), 
level_value float, level_type varchar(64), usgs_quake_time datetime, 
quake_depth_km float, quake_lat float, quake_lon float, quake_mag float, quake_id int(11), quake_desc varchar(128)
);


in a database named "sensor" where the above qcn_scedc exists & resides, import using:

/usr/local/mysql/bin/mysqlimport -h db-private -u root -p \
  --fields-enclosed-by='"' --fields-terminated-by=',' --local --ignore-lines=2 \
  sensor qcn_scedc.csv



I found some extra fields I'm rerunning so the csv/excel for mat will be like this, which can be "mysqlimport"-ed into a mysql table on scedc's end very easily:

Query Used: Date Range: 2010-04-01 - 2010-05-31   LatMin = 31.5  LatMax = 37.5  LngMin = -121.0  LngMax = -114.0
db, triggerid, hostid, time_trig_utc, time_trig_micros, time_received, time_sync, sync_offset, magnitude, significance, latitude, longitude, file, numreset, dt, trig type, hostipaddrid, geoipaddrid, sensor, alignment, level_value, level_type, usgs_quake_time, quake_depth_km, quake_lat, quake_lon, quake_mag, quake_id, quake_desc
"C","1183493","82","2010-04-01 00:00:00","17646.0","2010-04-01 00:00:04","2010-03-31 23:56:35","0.009192","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270080000.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",
"C","1183599","82","2010-04-01 00:10:00","17901.0","2010-04-01 00:10:01","2010-03-31 23:56:35","0.009192","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270080600.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",
"C","1183728","82","2010-04-01 00:20:00","15630.0","2010-04-01 00:20:04","2010-04-01 00:11:35","0.009117","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270081200.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",
"C","1183845","82","2010-04-01 00:30:00","5188.0","2010-04-01 00:30:04","2010-04-01 00:26:36","0.008506","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270081800.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",
"C","1183925","82","2010-04-01 00:40:00","3651.0","2010-04-01 00:40:01","2010-04-01 00:26:36","0.008506","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270082400.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",
"C","1184106","82","2010-04-01 00:50:00","3327.0","2010-04-01 00:50:05","2010-04-01 00:41:36","0.008798","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270083000.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",
"C","1184215","82","2010-04-01 01:00:00","4028.0","2010-04-01 01:00:05","2010-04-01 00:56:36","0.008337","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270083600.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",
"C","1184263","82","2010-04-01 01:10:00","2861.0","2010-04-01 01:10:01","2010-04-01 00:56:36","0.008337","0.0","0.0","33.9744306385","-117.325004339","continual_sc300_sta300_006091_000000_1270084200.zip","0","0.02","C","208","0","JoyWarrior 24F8 USB","Unaligned","","","","","","","","",


