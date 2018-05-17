/* CMC -- this takes the final QCN trigger and updates the latest trigger (or makes a new 'fake' trigger)
    for the purposes of granting credit in the do_stats() procedure

   check msg_from_host table for variety "finalstats" and handled=0 for this xml:

<result_name>qcng_003347_0</result_name>
      <time>1225420058</time>
<finalstats>
<trigcnt>4</trigcnt>
<uplcnt>0</uplcnt>
<rstcnt>0</rstcnt>
<wct>5974.57</wct>
<cpt>91.14</cpt>
</finalstats>

*/


DELIMITER //

DROP PROCEDURE IF EXISTS do_final_trigger
//

CREATE PROCEDURE do_final_trigger()
BEGIN

 DECLARE l_msgid INT(11);
 DECLARE l_resultid INT(11); 
 DECLARE l_triggerid INT(11);
 DECLARE l_finalid INT(11);
 DECLARE l_count INT(11);
 DECLARE l_result_name varchar(254);
 DECLARE l_runtime_clock double;
 DECLARE l_runtime_cpu double;
 DECLARE l_done_msg INT DEFAULT 0;

 /* now get a cursor resultset with the latest msg_from_host (trickle) records */
 DECLARE curNew CURSOR FOR 
   SELECT m.id, r.id resultid,
       SUBSTRING(xml, INSTR(xml, '<result_name>') + LENGTH('<result_name>'), 
                 INSTR(xml, '</result_name>') 
                    - INSTR(xml, '<result_name>') 
                    - LENGTH('<result_name>')) result_name,
        SUBSTRING(xml, INSTR(xml, '<wct>') + LENGTH('<wct>'), 
                 INSTR(xml, '</wct>') 
                    - INSTR(xml, '<wct>') 
                    - LENGTH('<wct>')) runtime_clock,
        SUBSTRING(xml, INSTR(xml, '<cpt>') + LENGTH('<cpt>'), 
                 INSTR(xml, '</cpt>') 
                    - INSTR(xml, '<cpt>') 
                    - LENGTH('<cpt>')) runtime_cpu
   FROM msg_from_host m, result r
   WHERE variety='finalstats' 
     AND handled=0 
     AND r.name=
        SUBSTRING(xml, INSTR(xml, '<result_name>') + LENGTH('<result_name>'), 
            INSTR(xml, '</result_name>') 
              - INSTR(xml, '<result_name>') 
              - LENGTH('<result_name>')) 
 ;
 DECLARE CONTINUE HANDLER FOR NOT FOUND SET l_done_msg=1;

 SET AUTOCOMMIT=1;

 /* first off, delete handled msg_from_host records older than a month */
 DELETE FROM msg_from_host WHERE handled=1 AND create_time < UNIX_TIMESTAMP()-(3600*24*30);

 /* now use the cursor and loop through for final trickles */
 OPEN curNew;
 
 trickle_loop: LOOP
    /* get a row from our big sql statement above */
    FETCH curNew INTO l_msgid, l_resultid, l_result_name, l_runtime_clock, l_runtime_cpu;

    IF l_done_msg=1 THEN
       LEAVE trickle_loop;
    END IF;

    /* -- probably safer and faster to just insert a "final trigger" record for the stats */
    SELECT COUNT(id) INTO l_count FROM qcn_finalstats WHERE resultid=l_resultid;
    IF l_count=0 THEN
       INSERT INTO qcn_finalstats (resultid, time_received, runtime_clock, runtime_cpu)
          VALUES (l_resultid, UNIX_TIMESTAMP(), 
              IF(l_runtime_clock>100000,100000,l_runtime_clock), IF(l_runtime_cpu>100000,100000,l_runtime_cpu));
    ELSE
        SELECT IFNULL(id,0) INTO l_finalid FROM qcn_finalstats 
            WHERE resultid=l_resultid;
        IF l_finalid>0 THEN
           UPDATE qcn_finalstats 
             SET time_received=UNIX_TIMESTAMP(), runtime_clock=IF(l_runtime_clock>100000,100000,l_runtime_clock), 
                 runtime_cpu=IF(l_runtime_cpu>100000,100000,l_runtime_cpu)
              WHERE id=l_finalid;
        ELSE  /* probably should never get here since the l_count above catches it */
           INSERT INTO qcn_finalstats (resultid, time_received, runtime_clock, runtime_cpu)
               VALUES (l_resultid, UNIX_TIMESTAMP(), 
                  IF(l_runtime_clock>100000,100000,l_runtime_clock), IF(l_runtime_cpu>100000,100000,l_runtime_cpu));
        END IF;
    END IF;
  
    /* don't forget to update msg_from_host and set this record to 1 / handled ! */
    UPDATE msg_from_host SET handled=1 WHERE id=l_msgid; 
  
 END LOOP trickle_loop;

 CLOSE curNew;

 COMMIT;

END
//

DELIMITER ;

