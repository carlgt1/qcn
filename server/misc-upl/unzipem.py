#! /usr/bin/env python
# archive triggers greater than 3 months into a zip file by trigger time / 10000 (roughly a day)
# CMC note -- need to install 3rd party MySQLdb libraries for python
import traceback, sys, os, time, tempfile, string, MySQLdb, shutil, zipfile
from datetime import datetime

CMD_UNZIP = "/usr/bin/unzip"

# trigger file download URL base
URL_DOWNLOAD_BASE = "http://qcn-upl.stanford.edu/trigger/"

# CMC note -- make sure these paths exist, or they will be created!
TMP_DIR = "/tmp"
# use the old directory
UPLOAD_WEB_DIR = "/data/QCN/trigger/"
UPLOAD_CONTINUAL_WEB_DIR = "/data/QCN/trigger/continual/"
#UPLOAD_WEB_DIR = "/var/www/trigger/"
#UPLOAD_CONTINUAL_WEB_DIR = "/var/www/trigger/continual/"

#ARCHIVE_DIR = "/var/www/trigger/archive/"
#ARCHIVE_CONTINUAL_DIR = "/var/www/trigger/archive/continual/"
ARCHIVE_DIR = "/data/QCN/trigger/archive/"
ARCHIVE_CONTINUAL_DIR = "/data/QCN/trigger/archive/continual/"

DBNAME = "sensor"
DBHOST = "db-private"
DBUSER = "qcn"
DBPASSWD = ""

# archive older than 3 months (3600 seconds/hr * 24 hrs/day * 90 days)
ARCHIVE_TIME = 1

# archive old trigger files
#file names are like:
#  continual_sc300_sta200_025914_000000_1288257600.zip  qcnk_sc300_sta100_105297_000135_1288592406.zip
#  continual_sc300_sta100_025293_000014_1288177200.zip  continual_sc300_sta200_025914_000000_1288260600.zip  qcnk_sc300_sta200_092450_000208_1287194026.zip
# basically take the unix time between the last _ and .zip -- divide by 10K, and make a zip file with that name
def archiveFilesPath(bContinual):
  if bContinual:
    dirOrig = UPLOAD_CONTINUAL_WEB_DIR
    dirArchive = ARCHIVE_CONTINUAL_DIR
  else: 
    dirOrig = UPLOAD_WEB_DIR
    dirArchive = ARCHIVE_DIR

  dirArchive = "/home/boinc/continual_backup/"
  now = time.time()
  print dirArchive
  for f in os.listdir(dirArchive):
    fname = os.path.join(dirArchive, f)
    dzip = f.rfind(".zip")
    errLevel = 0
    if dzip>0:
       # found zip & hash, not
       dtime = f[0:dzip]
       print f + " - " + dtime
       fdir = os.path.join(dirArchive, dtime)
       res = os.system(CMD_UNZIP + " -o " + fname + " -d " + fdir)
       if res == 0:
          os.remove(fname)
    else:
       print "Error - " + f
 
def main():
   #archiveFilesPath(False)
   archiveFilesPath(True)

if __name__ == '__main__':
   main()

                                        
