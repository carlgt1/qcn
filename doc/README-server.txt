The latest QCN/BOINC server-side setup is as follows:

1) Web and data server:  This will have the Apache HTTPD web server instance and appropriate libraries ie PHP, mySQL etc
2) Database server:  This will contain the mySQL database server 


The web server (qcn.usc.edu) should be available to the public for HTTP port 80.  The main website is currently (as of 9/2016) WordPress but that will change when IRIS takes over and puts up their own website.

The WordPress (main) website is under /var/www/qcnwp   (the mySQL database for WordPress is "qcnwp")

The BOINC parts (ie what the client software connects to) of the website are undr /var/www/boinc -- there are further subdrectories "sensor" and "continual" for the respective parts of the project (ie the "continual" polling of a clients sensor; versus the usual "trigger only" polling which is called the "sensor" project).

At this point it may be useful to look at the BOINC project server setup (ie the generic open-source software that runs much of QCN):

https://boinc.berkeley.edu/trac/wiki/ServerIntro


1) web / data server — this should be a fairly powerful server (8-16 CPU, 32-64GB RAM, 1TB+ disk space) with a fast and stable Internet connection.  

Main programs:  
  1) Apache httpd — installed in the /usr/local/apache2 tree
  2) PHP - config file in https://github.com/carlgt1/qcn/blob/master/server/config/phpconfig
      ./configure LDFLAGS=-ldl --enable-dav --enable-ssl --with-apr=/usr/local/apr --with-ssl=/usr/local/ssl --enable-so --enable-rewrite --enable-cgi --enable-cache --enable-deflate

  3) Python — various libraries also needed such as MySqlDb for python

Data drive mounted at /data  (4TB)

build script for Apache http in github:

https://github.com/carlgt1/qcn/blob/master/server/config/httpdconfig

./configure LDFLAGS=-ldl --enable-dav --enable-ssl --with-apr=/usr/local/apr --with-ssl=/usr/local/ssl --enable-so --enable-rewrite --enable-cgi --enable-cache --enable-deflate

Apache httpd configuration file is in /usr/local/apache2/conf/httpd.conf

Account “boinc” is the user account for httpd and crontab scripts

crontab -l

0,5,10,15,20,25,30,35,40,45,50,55 * * * * /var/www/boinc/sensor/bin/start --cron
6,12,18,24,30,36,42,48,54 * * * * /var/www/boinc/continual/bin/start --cron
12 7 * * 0 /var/www/boinc/sensor/bin/clean_tmpdata.sh
#QCN upload server routines follow this line
# note don't put it on an even 10 minute boundary as files are coming in from continual!
15,45 * * * * /usr/bin/python /home/boinc/upload/procupload.py 1>/home/boinc/upload/procupload.log 2>/home/boinc/upload/procupload.err
04 * * * * /usr/bin/python /home/boinc/upload/procdownloadjob.py C 1>/home/boinc/upload/procdownloadjobC.log 2>/home/boinc/upload/procdownloadjobC.err
34 * * * * /usr/bin/python /home/boinc/upload/procdownloadjob.py S 1>/home/boinc/upload/procdownloadjobS.log 2>/home/boinc/upload/procdownloadjobS.err


the main web directory is /var/www/qcnwp for the WordPress site (which will probably change for the new IRIS website)

BOINC server programs and website is under /var/www/boinc

The BOINC daemons are invoked by the top two lines in the crontab (i.e. BOINC has it’s own “pseudo-crontab” of tasks per the BOINC server info above)

Various python scripts seen in the crontab process uploaded trigger files from the clients, data download requests etc

The boinc installation is customized for QCN by the files in:
https://github.com/carlgt1/qcn/tree/master/server/boincmods

The root account on qcn-web (qcn.usc.edu) basically backs up the website and database every few days to /data/QCN

Note that in addition to the “public” ethernet connection for httpd (currently resolving to 128.125.230.8) there is a “private” ethernet cable connection setup so that the web/data server is web-private (10.1.1.2) and the mysql database server described below is db-private (10.1.1.1)

2) mysql database server - setup as a private IP with access only from the web server on a 2nd private cable connection.  Server should have a lot of memory (64GB or so) and a fair amount of fast disk space (1TB+) as the database is large after 9 years!

The main program/process is of course the mySQL database engine located at /usr/local/mysql

Current mysql version is 5.7.12-linux-glibc2.5-x86_64

The main QCN database is “sensor” — there is also a “continual” database for a little-used sub-project (monitoring the sensors continually i.e. dumping data around the clock from each client)

The mySQL database also holds various other databases such as an archive of >1 year old triggers (continual_archive and sensor_archive) as well as the WordPress mysql site (qcnwp).

Basically everything is run under the “mysql” user account; with appropriate startup scripts to launch the mysql-safe daemon.

Database is backed up every few days with a script run as root from the web server (i.e. it backs up a mysqldump .sql file, gzipped, to /data/QCN/qcn-backup.sql.gz
(currently an 11GB sized file)




