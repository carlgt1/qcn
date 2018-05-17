/* CMC -- this is the old do_stats python moved to a stored procedure
   to hopefully run faster!

   One "wall-clock day" of QCN is worth 50 cobblestones per Dave Anderson suggestion

   also check msg_from_host table for variety "finalstats" and handled=0 for this xml:

*/

DELIMITER //

DROP PROCEDURE IF EXISTS do_stats
//

CREATE PROCEDURE do_stats()
BEGIN

    SET AUTOCOMMIT=1;

    /* make a subset of result table - truncate old recalc table, just live recs of new table */

    /* rebuild trigsummary for archive records

         INSERT INTO qcn_trigsummary (userid, hostid, teamid, result_name, total_credit, weight, time_received, is_archive) 
            SELECT h.userid, hostid, u.teamid, result_name, 
               IF(runtime_clock>86400.0, 50.0, CEIL(runtime_clock * 0.0005787)) total_credit,
               EXP(-(ABS(unix_timestamp()-time_received))*0.69314718/604800.0) weight, 
               time_received, 
               1 is_archive
            FROM sensor_archive.qcn_trigger t, host h, user u
            WHERE hostid=h.id AND h.userid=u.id AND total_credit>0 ORDER BY time_trigger;


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
 

    INSERT INTO sensor.qcn_trigsummary 
             (userid, hostid, teamid, result_name, total_credit, weight, time_received, is_archive)
       SELECT userid, hostid, teamid, result_name, total_credit, weight, time_received, is_archive 
          FROM sensor_archive.qcn_trigsummary_tmp;
*/

     /* the following line returns trigger counts by year-month
       select concat(year(from_unixtime(time_received)), lpad(month(from_unixtime(time_received)), 2, '0')) ym, 
              count(*) from continual_archive.qcn_trigger group by ym;
     */

    /* get latest credit-worthy triggers into trigsummary */
    DELETE FROM qcn_trigsummary WHERE is_archive=0;
    INSERT INTO qcn_trigsummary (userid, hostid, teamid, result_name, total_credit, weight, time_received, is_archive) 
            SELECT h.userid, hostid, u.teamid, result_name, 
               IF(runtime_clock>86400.0, 50.0, CEIL(runtime_clock * 0.0005787)) total_credit,
               EXP(-(ABS(unix_timestamp()-time_received))*0.69314718/604800.0) weight, 
               time_received, 
               0 is_archive
            FROM qcn_trigger t, host h, user u
            WHERE hostid=h.id AND h.userid=u.id AND runtime_clock>3600 ORDER BY time_trigger;

    TRUNCATE TABLE qcn_stats;
    INSERT INTO qcn_stats
        SELECT 
          userid, hostid, teamid, result_name, total_credit, weight, total_credit*weight
        FROM qcn_trigsummary
        WHERE id IN (SELECT MAX(id) FROM qcn_trigsummary GROUP BY result_name);

    UPDATE result u, qcn_stats s
       SET u.granted_credit=
          IFNULL(total_credit,0), u.claimed_credit=u.granted_credit, u.validate_state=3
           WHERE u.name=s.result_name;

    UPDATE user u SET total_credit=IFNULL((select sum(total_credit) from 
          qcn_stats r WHERE r.userid=u.id),0),
             expavg_credit=IFNULL((SELECT SUM(weight*total_credit) FROM 
               qcn_stats rs WHERE rs.userid=u.id),0),
             expavg_time=(SELECT IFNULL(AVG(rrs.expavg_time),0) FROM qcn_stats rrs WHERE rrs.userid=u.id)
       WHERE u.id IN (SELECT userid FROM qcn_stats);
 
    UPDATE host u SET total_credit=IFNULL((select sum(total_credit) from 
          qcn_stats r WHERE r.hostid=u.id),0),
             expavg_credit=IFNULL((SELECT SUM(weight*total_credit) FROM 
               qcn_stats rs WHERE rs.hostid=u.id),0),
             expavg_time=(SELECT IFNULL(AVG(rrs.expavg_time),0) FROM qcn_stats rrs WHERE rrs.hostid=u.id)
       WHERE u.id IN (SELECT hostid FROM qcn_stats);

    UPDATE team u SET total_credit=IFNULL((select sum(total_credit) from 
          qcn_stats r WHERE r.teamid=u.id),0),
             expavg_credit=IFNULL((SELECT SUM(weight*total_credit) FROM 
               qcn_stats rs WHERE rs.teamid=u.id),0),
             expavg_time=(SELECT IFNULL(AVG(rrs.expavg_time),0) FROM qcn_stats rrs WHERE rrs.teamid=u.id)
       WHERE u.id IN (SELECT teamid FROM qcn_stats);

    COMMIT;
END
//

DELIMITER ;

