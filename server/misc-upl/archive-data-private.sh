#!/bin/bash
#archive.sql is stored in svn:/qcn/server/sql/archive.sql
FILE_BACKUP=/data/QCN/qcn-archive-data-private-backup.sql
FILE_BACKUP_GZ=/data/QCN/qcn-archive-data-private-backup.sql.gz
FILE_SENSOR_CONTINUAL=/data/QCN/sensor_continual.sql
DIR_QCN_DATA=/data/QCN/
DB=data-private
#first set the constant for the archive time i.e. current time - two months
/usr/local/mysql/bin/mysql -h $DB -u root -pPWD sensor -e "UPDATE sensor.qcn_constant SET value_int=unix_timestamp()-(3600*24*60) WHERE description='ArchiveTime'"
/usr/local/mysql/bin/mysql -h $DB -u root -pPWD sensor < /home/boinc/archive-data-private.sql
# now that the archive is done, make a copy of it, so we don't have to dump this all out every night
/bin/rm -f $FILE_SENSOR_CONTINUAL
/bin/rm -f $FILE_BACKUP
/bin/nice -n19 /usr/local/mysql/bin/mysqldump -h $DB -u root -pPWD --databases sensor continual > $FILE_SENSOR_CONTINUAL
/bin/nice -n19 /usr/local/mysql/bin/mysqldump -h $DB -u root -pPWD --databases sensor_archive continual_archive > $FILE_BACKUP
#/bin/nice -n19 /bin/gzip -f $FILE_BACKUP
/usr/bin/python /home/boinc/trigger_data-private_archive.py
