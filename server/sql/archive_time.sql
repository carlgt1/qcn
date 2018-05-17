select received_file,file, FROM_UNIXTIME(CONCAT(SUBSTRING(file, INSTR(file, '.zip')-10,5), '00000')) from continual_archive.qcn_trigger where received_file>10 order by time_trigger desc limit 30000;

