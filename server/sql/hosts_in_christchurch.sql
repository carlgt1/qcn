select sum
(
 select count(distinct hostid) 
  from sensor.qcn_trigger
   where latitude between -45 and -42
    and longitude between 170 and 175
union
 select count(distinct hostid) 
  from continual.qcn_trigger
   where latitude between -45 and -42
    and longitude between 170 and 175
);

