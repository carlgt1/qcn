#! /usr/bin/env python

# this program will bundle up files from a continual_download.job request
# into a single file for downloading

# this will of course be run on the upload server as a cron task, mysqldb for
# python should be installed on the server as well as a mysql acct from the
# upload server to the database server

# CMC note -- need to install 3rd party MySQLdb libraries for python

# the layout of the program is to unzip each requested zip, insert metadata into the unzipped 
# SAC files (from the qcn_trigger record), and then after all requested zip files are processed, zip them all up and 
# move to a download location, with an email to the user of the download link


import math, time, tempfile, smtplib, traceback, sys, os, tempfile, string, MySQLdb, shutil, zipfile, lockfile
from datetime import datetime
from zipfile import ZIP_STORED
from time import strptime, mktime
from qcnutil import getSACMetadata
from qcnutil import getFanoutDirFromZip

global DBHOST 
global DBUSER
global DBPASSWD
global SMTPS_HOST, SMTPS_PORT, SMTPS_LOCAL_HOSTNAME, SMTPS_KEYFILE, SMTPS_CERTFILE, SMTPS_TIMEOUT

os.environ["HOME"] = "/home/boinc/upload"

DBHOST = "db-private"
DBUSER = "qcn"
DBPASSWD = "PWD"

SMTPS_HOST = "smtp.usc.edu"
SMTPS_PORT = 25
SMTPS_LOCAL_HOSTNAME = "qcn.usc.edu"
SMTPS_KEYFILE = "/etc/sslcerts/server.key"
SMTPS_CERTFILE = "/etc/sslcerts/server.crt"
SMTPS_TIMEOUT = 60

# the next 5 globals will be set by the appropriate run type in SetRunType()
global URL_DOWNLOAD_BASE
global UPLOAD_WEB_DIR
global ARCHIVE_WEB_DIR
global DOWNLOAD_WEB_DIR
global DBNAME
global DBNAME_ARCHIVE
global DBNAME_JOB

# use fast zip -1 compression as the files are already compressed
#global ZIP_CMD
#ZIP_CMD  = "/usr/bin/zip -1 "
#global UNZIP_CMD
global typeRunning
typeRunning = ""

# delete old job files > 7 days old
def delFilesPath(path):
  now = time.time()
  for f in os.listdir(path):
    fname = os.path.join(path, f)
    if os.stat(fname).st_mtime < now - (7 * 86400):
      if os.path.isfile(fname): # and f.find("qcn_scedc") == -1:
        os.remove(fname)


def SetRunType():
  global URL_DOWNLOAD_BASE
  global UPLOAD_WEB_DIR
  global ARCHIVE_WEB_DIR
  global DOWNLOAD_WEB_DIR
  global DBNAME
  global DBNAME_JOB
  global typeRunning
  global DBNAME, DBNAME_ARCHIVE
  icnt = 0
  typeRunning = ""
  for arg in sys.argv:
    if icnt == 1:
      typeRunning = arg
    icnt = icnt + 1

  if typeRunning != "C" and typeRunning != "S":
    print "Must pass in C for Continual jobs, S for regular sensor DB jobs"
    sys.exit(3)

  if typeRunning == "C":  # continual
    URL_DOWNLOAD_BASE = "http://qcn-upl.stanford.edu/trigger/continual/job/"
    # CMC note -- make sure these paths exist
    UPLOAD_WEB_DIR = "/data/QCN/trigger/continual/"
    ARCHIVE_WEB_DIR = "/data/QCN/trigger/archive/continual/"
    DOWNLOAD_WEB_DIR = "/data/QCN/trigger/continual/job/"
    DBNAME = "continual"
    DBNAME_ARCHIVE = "continual_archive"
    DBNAME_JOB = "continual_download"
  else:   #sensor database
    URL_DOWNLOAD_BASE = "http://qcn-upl.stanford.edu/trigger/job/"
    UPLOAD_WEB_DIR = "/data/QCN/trigger/"
    ARCHIVE_WEB_DIR = "/data/QCN/trigger/archive/"
    DOWNLOAD_WEB_DIR = "/data/QCN/trigger/job/"
    DBNAME = "sensor"
    DBNAME_ARCHIVE = "sensor_archive"
    DBNAME_JOB = "sensor_download"

  #UNZIP_CMD = "/usr/bin/unzip -o -d " + UPLOAD_WEB_DIR + " "

def procDownloadRequest(dbconn, outfilename, url, jobid, userid, trigidlist):
 global DBNAME, DBNAME_ARCHIVE
 # check for empty set of trig id
 if trigidlist == "()":
   return 0

 tmpdir = tempfile.mkdtemp()
 myCursor = dbconn.cursor()
 # need to join with archive table 
 query = "SELECT t.id,t.hostid,t.latitude,t.longitude,t.levelvalue,t.levelid,t.file, " +\
            "t.qcn_quakeid, q.time_utc quake_time, q.depth_km quake_depth_km, " +\
            "q.latitude quake_lat, q.longitude quake_lon, q.magnitude quake_mag, 0 is_archive, t.time_trigger " +\
              "FROM " + DBNAME + ".qcn_trigger t " +\
              "LEFT OUTER JOIN sensor.qcn_quake q ON q.id = t.qcn_quakeid " +\
              "WHERE t.received_file=100 AND t.id IN " + trigidlist + " UNION " +\
        "SELECT t.id,t.hostid,t.latitude,t.longitude,t.levelvalue,t.levelid,t.file, " +\
            "t.qcn_quakeid, q.time_utc quake_time, q.depth_km quake_depth_km, " +\
            "q.latitude quake_lat, q.longitude quake_lon, q.magnitude quake_mag, 1 is_archive, t.time_trigger " +\
              "FROM " + DBNAME_ARCHIVE + ".qcn_trigger t " +\
              "LEFT OUTER JOIN sensor.qcn_quake q ON q.id = t.qcn_quakeid " +\
              "WHERE t.received_file=100 AND t.id IN " + trigidlist

 try:
   myCursor.execute(query)
 except:
   print "Error in query"
   print query
   traceback.print_exc()
   sys.exit(3)


 zipoutpath = os.path.join(DOWNLOAD_WEB_DIR, outfilename)
 zipinpath = ""

 # get the resultset as a tuple
 result = myCursor.fetchall()
 numbyte = 0
 myzipout = None
 errlevel = 0

 try:
 
   # open a zip output file - allow zip64 compression for large (>2GB) files
   errlevel = 1
   #print zipoutpath, "  ", 'w', "  ", str(ZIP_STORED), "  ", str(True)
   myzipout = zipfile.ZipFile(zipoutpath, "w", ZIP_STORED, True)

   # iterate through resultset
   curdir = os.getcwd()   # save current directory and go to the temp dir (so paths aren't stored in zip's)
   os.chdir(tmpdir)
   for rec in result:
      if rec[13] == 1:  # archive - need to get fanout dir name from file name rec[6]
         fandir, dtime = getFanoutDirFromZip(rec[6])
         fullpath = os.path.join(ARCHIVE_WEB_DIR, fandir)
         zipinpath = os.path.join(fullpath, rec[6])
      else:
         zipinpath = os.path.join(UPLOAD_WEB_DIR, rec[6])

      errlevel = 2
      #print "    ", rec[0] , "  ", rec[1], "  ", rec[2], "  ", rec[3], "  ", rec[4], "  ", rec[5], "  ", rec[6]

      #host id is rec[1], time_trigger is rec[14]
      # test for valid zip file
      try:
        myzipin = zipfile.ZipFile(zipinpath, "r")
        if os.path.isfile(zipinpath) and myzipin.testzip() == None:
           errlevel = 3
           # valid zip file so add to myzipout, first close
           zipinlist = myzipin.namelist()
           myzipin.extractall(tmpdir)
           myzipin.close()
           for zipinname in zipinlist:
             errlevel = 4
             #zipinpath = os.path.join(tmpdir, zipinname)
             # OK - at this point the zip file requested has been unzipped, so we need to process metadata here
             getSACMetadata(zipinname, rec[1], rec[2], rec[3], rec[4], rec[5], rec[7], rec[8], rec[9], rec[10], rec[11], rec[12])

             # OK - at this point prepend hostid ID & trigger time so it will be sorted OK
             nicefilename = "%09d_%d_%s" % (rec[1], rec[14], zipinname)
             os.rename(zipinname, nicefilename)
             myzipout.write(nicefilename)
             os.remove(nicefilename)
        else:
          print "Invalid or missing file " + zipinpath   

      except:
        print "Error " + str(errlevel) + " in myzipin " + zipinpath
        continue

   os.chdir(curdir)   # go back to regular directory so tmpdir can be erased
   myzipout.close() 
   numbyte = os.path.getsize(zipoutpath)
   shutil.rmtree(tmpdir)    # remove temp directory
   myCursor.close();
   return numbyte

 except zipfile.error:
   print "Error " + str(errlevel) + " in " + zipoutpath + " or " + zipinpath +\
        " is an invalid zip file (tmpdir=" + tmpdir + ")"
   #dbconn.rollback()
   traceback.print_exc()
   shutil.rmtree(tmpdir)    # remove temp directory
   myCursor.close();
   if (myzipout != None):
      myzipout.close() 
      os.remove(zipoutpath)
   return 0
 except:
   print "Error " + str(errlevel) + " in " + zipoutpath + " or " + zipinpath + " (tmpdir=" + tmpdir + ")"
   #dbconn.rollback()
   traceback.print_exc()
   shutil.rmtree(tmpdir)    # remove temp directory
   myCursor.close();
   if (myzipout != None):
      myzipout.close() 
      os.remove(zipoutpath)
   return 0

def sendEmail(Username, ToEmailAddr, DLURL, NumMB):
  global SMTPS_HOST, SMTPS_PORT, SMTPS_LOCAL_HOSTNAME, SMTPS_KEYFILE, SMTPS_CERTFILE, SMTPS_TIMEOUT
  # sends email that job is done
  FromEmailAddr = "noreply@qcn.stanford.edu"
  #server=smtplib.SMTP(SMTPS_HOST, SMTPS_PORT, SMTPS_LOCAL_HOSTNAME, SMTPS_KEYFILE, SMTPS_CERTFILE, SMTPS_TIMEOUT)
  server=smtplib.SMTP(SMTPS_HOST, SMTPS_PORT, SMTPS_LOCAL_HOSTNAME)
  msg = "Hello " + Username + ":\n\n" + "Your requested files are available for download " +\
    "over the next 24 hours from the following URL:\n\n" + DLURL +\
    "\n\nThe file size to download is approximately " + str(NumMB) + " megabytes." +\
    "\n\nNote that this email is automated - please do not reply!"
  if typeRunning == "C":
    subj = "QCN Continual Download Archive Completed"
  else:
    subj = "QCN Sensor Download Archive Completed"

  MessageText = """\
From: %s
To: %s
Subject: %s

%s
""" % (FromEmailAddr, ToEmailAddr, subj, msg)

  server.sendmail(FromEmailAddr, ToEmailAddr, MessageText)
  server.quit()

def updateRequest(dbconn, jobid, numbyte, outfilename, url):
   myCursor = dbconn.cursor()
   query = "UPDATE " + DBNAME_JOB + ".job SET finish_time=unix_timestamp(), " +\
                "url='" + url + "', local_path='" + outfilename + "', size=" + str(numbyte) +\
                " WHERE id=" + str(jobid)
   #print query
   myCursor.execute(query)
   dbconn.commit();
   myCursor.close();


def processContinualJobs(dbconn):
   # read the DBNAME_JOB table for unfinished jobs, then process the upload files into a single bundle
   myCursor = dbconn.cursor()
   query = "SELECT j.id, j.userid, u.name, u.email_addr, j.create_time, j.list_triggerid " +\
      "FROM " + DBNAME_JOB + ".job j, " + DBNAME + ".user u " +\
      "WHERE j.userid=u.id AND finish_time IS NULL"

   myCursor.execute(query)

   # get the resultset as a tuple
   result = myCursor.fetchall()
   totalmb = 0

   # iterate through resultset
   for rec in result:
      outfilename = "u" + str(rec[1]) + "_j" + str(rec[0]) + ".zip"
      url = URL_DOWNLOAD_BASE + outfilename
      print rec[0] , "  ", rec[1], "  ", rec[2], "  ", rec[3]
      numbyte = procDownloadRequest(dbconn, outfilename, url, rec[0], rec[1], rec[5])
      if (numbyte > 0):
         nummb = math.floor(numbyte/(1024*1024))
         totalmb += nummb
         sendEmail(rec[2], rec[3], url, nummb)
      
      updateRequest(dbconn, rec[0], numbyte, outfilename, url)

   myCursor.close();
   return math.ceil(totalmb)

# makes sure that the necessary paths are in place as defined above
def checkPaths():
   global UPLOAD_WEB_DIR
   global ARCHIVE_WEB_DIR
   global DOWNLOAD_WEB_DIR
   if not os.access(UPLOAD_WEB_DIR, os.F_OK | os.W_OK):
      print UPLOAD_WEB_DIR + " directory for UPLOAD_WEB_DIR does not exist or not writable!"
      return 1
   
   if not os.access(ARCHIVE_WEB_DIR, os.F_OK | os.W_OK):
      print ARCHIVE_WEB_DIR + " directory for ARCHIVE_WEB_DIR does not exist or not writable!"
      return 1
   
   if not os.access(DOWNLOAD_WEB_DIR, os.F_OK | os.W_OK):
      print DOWNLOAD_WEB_DIR + " directory for DOWNLOAD_WEB_DIR does not exist or not writable!"
      return 1
   
   return 0
      
def main():
   global typeRunning
   try:
      # set appropriate global vars for run type (i.e. continual or sensor)
      SetRunType() 

      # first make sure all the necessary paths are in place
      if (checkPaths() != 0):
         sys.exit(2)

      lock = lockfile.FileLock("/tmp/procdownloadjob" + typeRunning)
      while not lock.i_am_locking():
        try:
          lock.acquire(timeout=0)  
        except:
          raise Exception("FileLock")

      delFilesPath(DOWNLOAD_WEB_DIR)

      dbconn = MySQLdb.connect (host = DBHOST,
                           user = DBUSER,
                           passwd = DBPASSWD,
                           db = DBNAME)

      totalmb = processContinualJobs(dbconn)

      print str(totalmb) + " MB of zip files processed"

      dbconn.close()
      lock.release()

   except:
      traceback.print_exc()
      lock.release()
      sys.exit(1)

if __name__ == '__main__':
    main()


