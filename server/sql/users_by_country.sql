select country,count(*) from user 
   where id in 
     (select distinct userid from host where rpc_time>unix_timestamp()-(3600*24*180))
   group by country;
