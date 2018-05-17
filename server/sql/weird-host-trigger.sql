select id,hostid,from_unixtime(time_received),from_unixtime(time_trigger),from_unixtime(time_sync),sync_offset from qcn_trigger where time_received+100<time_trigger and time_sync>1000 order by hostid,time_received;

