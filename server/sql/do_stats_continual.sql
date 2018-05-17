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

    /* first off run the do_final_trigger() stored procedure to process any finished workunit trigger stats */
    CALL do_final_trigger();

    /* make a subset of result table */
    TRUNCATE TABLE qcn_recalcresult;

    /* first insert the final result records (final stats trickles) */
    INSERT INTO qcn_recalcresult
      (SELECT resultid,
       exp(-(abs(unix_timestamp()-time_received))*0.69314718/604800.0) weight,
         (50.0*IF(runtime_clock>100000.0,100000.0,runtime_clock))/86400.0 total_credit,
           time_received
         FROM qcn_finalstats);

    /* note the left join below to ignore triggers for resultid's that we received a final stats trickle for above */
    INSERT INTO qcn_recalcresult
      (SELECT r.id resultid,                              
       exp(-(abs(unix_timestamp()-max(t.time_received))*0.69314718/604800.0)) weight,
         (50.0*IF(MAX(t.runtime_clock)>100000.0,100000.0,MAX(t.runtime_clock)))/86400.0 total_credit,
           max(t.time_received)
         FROM result r
           LEFT JOIN qcn_finalstats f ON r.id=f.resultid 
           JOIN qcn_trigger t ON r.name=t.result_name
         WHERE f.resultid IS NULL and t.runtime_clock>0
                   GROUP BY r.id);


    INSERT INTO qcn_recalcresult
       SELECT * FROM qcn_recalcresult_archive q 
           WHERE q.resultid NOT IN (SELECT resultid FROM qcn_recalcresult);

    /* archived triggers 
    DROP TABLE qcn_recalcresult_archive;
    CREATE TABLE qcn_recalcresult_archive
      (SELECT r.id resultid,                              
       exp(-(abs(unix_timestamp()-max(t.time_received))*0.69314718/604800.0)) weight,
         (50.0*IF(MAX(t.runtime_clock)>100000.0,100000.0,MAX(t.runtime_clock)))/86400.0 total_credit,
           max(t.time_received)
         FROM result r
           LEFT JOIN qcn_finalstats f ON r.id=f.resultid 
           LEFT JOIN qcn_trigger q    ON r.name=q.result_name
           JOIN continual_archive.qcn_trigger t ON r.name=t.result_name
         WHERE f.resultid IS NULL AND q.result_name IS NULL AND t.runtime_clock>0
                   GROUP BY r.id);
     */

    TRUNCATE TABLE qcn_stats;
    INSERT INTO qcn_stats 
       SELECT r.userid,r.hostid,u.teamid,r.id,c.total_credit,c.weight,c.time_received
        FROM result r, user u, 
         qcn_recalcresult c 
       WHERE r.id=c.resultid AND r.userid=u.id;

    UPDATE result u, qcn_recalcresult rs 
       SET u.granted_credit=
          ROUND(IFNULL(total_credit,0),3), u.claimed_credit=u.granted_credit, u.validate_state=3
           WHERE u.id=rs.resultid;

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

