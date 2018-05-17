select hostid from sensor.qcn_trigger where time_trigger>unix_timestamp()-(3600*24*14) group by hostid having count(*)>10000;
