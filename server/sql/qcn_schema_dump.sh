#!/bin/bash
/usr/local/mysql/bin/mysqldump -h db-private -u root -pPWD --triggers --routines -d sensor > sensor.sql
#/usr/local/mysql/bin/mysqldump -h db-private -u root -pPWD -d --functions --databases sensor continual continual_archive sensor_archive qcnwp sensor_download continual_download > qcn_all_schema.sql
