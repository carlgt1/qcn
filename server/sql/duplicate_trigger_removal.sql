/*

use to remove duplicate triggers from the archive table

*/

use continual_archive;

lock tables qcn_trigger write;

delete from qcn_trigger where varietyid=1 and qcn_sensorid=0;

create temporary table qcn_trigger_duplicate 
   (id int, hostid int, varietyid smallint, time_trigger double, result_name varchar(64));

create unique index qtdi on qcn_trigger_duplicate(id);

insert into qcn_trigger_duplicate
  (select min(id), hostid, varietyid, time_trigger, result_name
     from qcn_trigger
       group by hostid, varietyid, time_trigger, result_name);


delete from qcn_trigger where id not in (select id from qcn_trigger_duplicate);

optimize table qcn_trigger;

drop table qcn_trigger_duplicate;

unlock tables;

