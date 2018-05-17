/*
    this sends a 'killer trickle' for a single host/result
*/
insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from result where name='continual_sc300_sta300_000047_0'
;


/*
    this sends a 'killer trickle' for hosts which have triggered in the past week
*/
insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  result_name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from qcn_trigger where time_trigger>=unix_timestamp()-(86400*7)
    group by hostid,result_name
;


/*
    this sends a 'killer trickle' for android hosts
*/
insert into continual.msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  result_name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from continual.qcn_trigger where qcn_sensorid=7 group by hostid;
;



/* killer trickle for hosts with no sensor that are ping triggering too much (>100) */

insert into msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  result_name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from qcn_trigger 
     where varietyid=1 and time_trigger>unix_timestamp()-259200 
       and qcn_sensorid=0 group by hostid having count(*)>100;


insert into msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  result_name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from qcn_trigger 
     where varietyid=1 and qcn_sensorid=0 group by hostid having count(*)>300 order by count(*);



/* killer trickle for hosts that are resending huge lists of triggers (>1000)

insert into continual.msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  result_name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from continual.qcn_trigger
 group by hostid having count(*)>1000;



/*
    this sends a 'killer trickle' for hosts running an older version than we'd like who triggered in the past two weeks
*/
insert into sensor.msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  result_name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from qcn_trigger where time_trigger>=unix_timestamp()-(86400*14) 
    and sw_version<4.2
    group by hostid,result_name
;

/*
   this sends a 'killer trickle' to a new version (made a two day ago) that has bugs
*/

insert into msg_to_host
(create_time,hostid,variety,handled,xml)
select unix_timestamp(), hostid, 'abort', 0,
concat('<trickle_down>\n<result_name>',
  result_name,
   '</result_name>\n<abort></abort>\n</trickle_down>\n')
  from qcn_trigger where time_trigger>=unix_timestamp()-(86400*2)
    and sw_version='7.10'
    group by hostid,result_name
;


