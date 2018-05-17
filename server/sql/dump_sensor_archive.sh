nohup /usr/local/mysql/bin/mysqldump -h localhost -u root -p --database sensor_archive --tables qcn_trigger --where="time_trigger < unix_timestamp('2014-01-01 00:00:00')" 1>dump_sensor_archive_before_year.sql 2>dump_sensor_archive.err &


