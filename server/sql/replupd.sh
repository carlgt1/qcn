imply sends the last continual trigger sync time so know if replication bombed out
#!/bin/bash
/usr/local/mysql/bin/mysql -u qcnsql -pqcnsql continual -e 'select abs(max(time_received)-unix_timestamp()) from continual.qcn_trigger'  > /tmp/max.txt
/usr/bin/scp -i /Users/carlgt1/.ssh/id_rsa /tmp/max.txt carlgt1@qcn-web:/var/www/boinc/sensor/html/user/
rm -f /tmp/max.txt
