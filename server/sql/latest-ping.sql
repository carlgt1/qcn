select count(distinct hostid) from qcn_trigger where ping=1 and time_received>unix_timestamp()-(3600*4);
