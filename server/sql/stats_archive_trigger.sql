/* for qcn_recalcresult table in do_stats ie to get archive numbers */

  USE sensor;
/*
    TRUNCATE TABLE qcn_recalcresult;
    CREATE TABLE qcn_recalcresult_archive SELECT * FROM qcn_recalcresult;
 */
   LOCK TABLES qcn_recalcresult_archive WRITE, result AS r READ, qcn_finalstats AS f READ, 
     qcn_trigger AS q READ, sensor_archive.qcn_trigger AS t READ;
   TRUNCATE TABLE qcn_recalcresult_archive;
   INSERT INTO qcn_recalcresult_archive
      (SELECT r.id resultid,
       exp(-(abs(unix_timestamp()-max(t.time_received))*0.69314718/604800.0)) weight,
         (50.0*IF(MAX(t.runtime_clock)>100000.0,100000.0,MAX(t.runtime_clock)))/86400.0 total_credit,
           max(t.time_received)
         FROM result r
           LEFT JOIN qcn_finalstats f ON r.id=f.resultid
           LEFT JOIN qcn_trigger q    ON r.name=q.result_name
           JOIN sensor_archive.qcn_trigger t ON r.name=t.result_name
         WHERE f.resultid IS NULL AND q.result_name IS NULL AND t.runtime_clock>0
                   GROUP BY r.id);
   UNLOCK TABLES;

   USE continual;
   LOCK TABLES qcn_recalcresult_archive WRITE, result AS r READ, qcn_finalstats AS f READ, 
     qcn_trigger AS q READ, continual_archive.qcn_trigger AS t READ;
    TRUNCATE TABLE qcn_recalcresult_archive;
    INSERT INTO qcn_recalcresult_archive
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
   UNLOCK TABLES;


