#! /usr/local/bin/python
#// #! /usr/bin/env python
# archive triggers greater than 3 months into a zip file by trigger time / 10000 (roughly a day)
# CMC note -- need to install 3rd party MySQLdb libraries for python
import traceback, sys, os, time, tempfile, string, MySQLdb, shutil, zipfile, qcnutil
from datetime import datetime
from qcnutil import getFanoutDirFromZip
from qcnutil import getSACMetadata
from qcnutil import makeFanoutDir 

# trigger file download URL base
URL_DOWNLOAD_BASE = "http://qcn-upl.stanford.edu/trigger/"

# CMC note -- make sure these paths exist, or they will be created!
TMP_DIR = "/tmp"
# use the old directory
UPLOAD_WEB_DIR = "/data/QCN/trigger/"
UPLOAD_CONTINUAL_WEB_DIR = "/data/QCN/trigger/continual/"

ARCHIVE_DIR = "/data/QCN/trigger/archive/"
ARCHIVE_CONTINUAL_DIR = "/data/QCN/trigger/archive/continual/"

DBNAME = "sensor"
DBHOST = "db-private"
DBUSER = "qcn"
DBPASSWD = "PWD"

# we'll get the archive time from sensor.qcn_constant table for row ArchiveTime
ARCHIVE_TIME = 1e9

# get archive constant from database
def getArchiveTime(dbconn):
  global ARCHIVE_TIME
  query = "SELECT value_int FROM sensor.qcn_constant WHERE description='ArchiveTime'"
  crs = dbconn.cursor()
  crs.execute(query)
  rowTime = crs.fetchone()
  if rowTime == None:
     return
  ARCHIVE_TIME = rowTime[0]

# archive old trigger files
#file names are like:
#  continual_sc300_sta200_025914_000000_1288257600.zip  qcnk_sc300_sta100_105297_000135_1288592406.zip
#  continual_sc300_sta100_025293_000014_1288177200.zip  continual_sc300_sta200_025914_000000_1288260600.zip  qcnk_sc300_sta200_092450_000208_1287194026.zip
# basically take the unix time between the last _ and .zip -- divide by 10K, and make a zip file with that name
def archiveFilesPath(bContinual):
  global ARCHIVE_TIME
  if bContinual:
    dirOrig = UPLOAD_CONTINUAL_WEB_DIR
    dirArchive = ARCHIVE_CONTINUAL_DIR
  else: 
    dirOrig = UPLOAD_WEB_DIR
    dirArchive = ARCHIVE_DIR

  now = time.time()
  ctr = 0
  for f in os.listdir(dirOrig):
    if not f.endswith(".zip"):
      #os.remove(dirOrig + fname)
      continue

    #if ctr > 5:
    #   print "That's enough..."
    #   break

    fname = os.path.join(dirOrig, f)
    dtime = 0
    (dbin, dtime) = getFanoutDirFromZip(f)
    #print str(dbin) + " - " + str(dtime)
    if long(dtime) > 0 and long(dtime) <= ARCHIVE_TIME:
         ctr = ctr + 1
         #just move file to archive dir but use fanout dir name
         fullarchivedir = makeFanoutDir(dirArchive, dbin)
         fullarchivepath = os.path.join(fullarchivedir, f)

         # old file to archive
         # look for zip file with this bin
         #fullzippath = dirArchive + dbin + ".zip"
         errLevel = 0
         try:  # catch zipfile exceptions if any - open for append, no compression (since already a zip), and allow > 2GB
           #myzip = zipfile.ZipFile(fullzippath, "a", zipfile.ZIP_STORED, True)
           # test for valid zip file
           #if myzip.testzip() != None:  # file corrupt write a new file
           #   myzip = zipfile.ZipFile(fullzippath, "w", zipfile.ZIP_STORED, True)
           
           # we just want to add fname to this zip file
           #myzip.write(fname, f, zipfile.ZIP_STORED)
           #myzip.close()

           # if we made it here then it was archived and can remove the original
           #os.remove(fname)
           shutil.move(fname, fullarchivepath)
           print "Successfully archived " + fname + " to " + fullarchivepath

         except:
           print "Error in moving archive zip file: " + f + " to  " + fullarchivepath
           traceback.print_exc()
           print ""
           errLevel = 1

#    else:
#      print "Invalid file name?  " + fname   

# makes sure that the necessary paths are in place as defined above
def checkPaths():
   if not os.access(UPLOAD_WEB_DIR, os.F_OK | os.W_OK):
      print UPLOAD_WEB_DIR + " directory for UPLOAD_WEB_DIR does not exist or not writable!"
      return 1

   if not os.access(UPLOAD_CONTINUAL_WEB_DIR, os.F_OK | os.W_OK):
      print UPLOAD_CONTINUAL_WEB_DIR + " directory for UPLOAD_CONTINUAL_WEB_DIR does not exist or not writable!"
      return 1

   if not os.access(ARCHIVE_DIR, os.F_OK | os.W_OK):
      print ARCHIVE_DIR + " directory for ARCHIVE_DIR does not exist or not writable!"
      return 1

   if not os.access(ARCHIVE_CONTINUAL_DIR, os.F_OK | os.W_OK):
      print ARCHIVE_CONTINUAL_DIR + " directory for ARCHIVE_CONTINUAL_DIR does not exist or not writable!"
      return 1

   #if not os.access(UPLOAD_BACKUP_DIR, os.F_OK):
   #   print UPLOAD_BACKUP_DIR + " directory for UPLOAD_BACKUP_DIR does not exist, creating!"
   #   if os.mkdir(UPLOAD_BACKUP_DIR):
   #      print "Could not create UPLOAD_BACKUP_DIR=" + UPLOAD_BACKUP_DIR
   #      return 1

   return 0


def main():
   try:

      # first make sure all the necessary paths are in place
      if (checkPaths() != 0):
         sys.exit(2)

      dbconn = MySQLdb.connect (host = DBHOST,
                           user = DBUSER,
                           passwd = DBPASSWD,
                           db = DBNAME)

      getArchiveTime(dbconn)
      dbconn.close()

      print "Using archive time = " + str(ARCHIVE_TIME)

      archiveFilesPath(False)
      archiveFilesPath(True)

      # basically we need to go to each 
      #processUploadZIPFiles(dbconn)


   except:
      traceback.print_exc()
      sys.exit(1)

if __name__ == '__main__':
    main()

                                        
