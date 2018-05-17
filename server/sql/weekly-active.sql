select yearweek(from_unixtime(time_received)) yw, count(distinct hostid) ctr
from qcn_trigger
where yearweek(from_unixtime(time_received))
  between 
      yearweek(date(date_sub(date(date_sub(now(), interval 180 day)), 
        interval dayofweek(date(date_sub(now(), interval 180 day))) day) )  )
    and 
      yearweek(date(date_sub(now(), interval dayofweek(now()) day)))
group by yearweek(from_unixtime(time_received)) 
order by yearweek(from_unixtime(time_received))
;
