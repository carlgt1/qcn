#! /usr/bin/env python
# break down a big trigger zip directory to subdirs based on trigger time
# CMC note -- need to install 3rd party MySQLdb libraries for python
import traceback, sys, os, time, tempfile, string, MySQLdb, shutil, zipfile
from datetime import datetime

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

  #dirArchive = "/home/boinc/test/"
  dirArchive = "/data/QCN/trigger/continual/"
  now = time.time()

  for f in os.listdir(dirArchive):
    if not f.endswith(".zip"):
      continue

    dzip = f.find(".zip")
    dund = f.rfind("_")
    errLevel = 0
    if dzip>0 and dund>0 and dund<dzip:
       # found zip & underscore
       dtime = f[dund+1:dzip]
       dbin = f[dund+1:dund+7]
       if dbin.endswith(".zip"):
         dbin = dtime

       #print dtime + "  " + dbin

       #make directory
       fullpath = os.path.join(dirArchive, dbin)
       if not os.path.isdir(fullpath):
         os.mkdir(fullpath)

       fullpath = os.path.join(fullpath, f)

       #move file over to fullpath
       fpath = os.path.join(dirArchive, f)
       try:
          shutil.move(fpath, fullpath)
       except:  # not a fatal error but weird so may want to go back and remove file
          traceback.print_exc()

    else:
       print "Invalid file name " + f + "  dzip = " + dzip + "   dund = " + dund

def main():
   #archiveFilesPath(False)
   archiveFilesPath(True)

if __name__ == '__main__':
   main()

                                        
