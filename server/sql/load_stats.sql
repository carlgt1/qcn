
use sensor;
/*
set autocommit=1;
         INSERT INTO sensor_archive.qcn_trigsummary (userid, hostid, teamid, result_name, total_credit, weight, time_received, is_archive) 
            SELECT h.userid, hostid, u.teamid, result_name, 
               IF(runtime_clock>86400.0, 50.0, CEIL(runtime_clock * 0.0005787)) total_credit,
               EXP(-(ABS(unix_timestamp()-time_received))*0.69314718/604800.0) weight, 
               time_received, 
               1 is_archive
            FROM sensor_archive.qcn_trigger t, host h, user u
            WHERE hostid=h.id AND h.userid=u.id AND runtime_clock>3600 ORDER BY time_trigger;


use sensor_archive;

CREATE TABLE qcn_trigsummary_tmp ( userid int(11), hostid int(11), teamid int(11), result_name varchar(254), total_credit double, weight double, time_received double, is_archive boolean );

CREATE INDEX qcn_trigsummary_tmp_result on qcn_trigsummary_tmp(result_name, total_credit);  

CREATE INDEX qcn_trigsummary_tmp_archive on qcn_trigsummary_tmp(is_archive);

LOAD DATA INFILE '/home/mysql/sensor_qcn_trigsummary.csv' 
   INTO TABLE sensor_archive.qcn_trigsummary_tmp 
       FIELDS TERMINATED BY ',' ENCLOSED BY '"';
 

*/

         SELECT h.userid, hostid, u.teamid, result_name, 
               IF(runtime_clock>86400.0, 50.0, CEIL(runtime_clock * 0.0005787)) total_credit,
               EXP(-(ABS(unix_timestamp()-time_received))*0.69314718/604800.0) weight, 
               time_received, 
               1 is_archive
            FROM sensor_archive.qcn_trigger t, host h, user u
            WHERE hostid=h.id AND h.userid=u.id AND runtime_clock>3600 ORDER BY time_trigger
          INTO OUTFILE '/home/mysql/sensor_qcn_trigsummary.csv'
            FIELDS TERMINATED BY ','
            ENCLOSED BY '"'
          LINES TERMINATED BY '\n';


