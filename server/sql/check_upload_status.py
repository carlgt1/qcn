#! /usr/bin/env python

# this file will unzip the files in the upload directory
# and move them to an http download directory for delivery via apache httpd

# it will also update the database qcn_trigger table to reflect that the files
# have been uploaded for a particular trigger and are available at the URL
# define below with other important vars

# this will of course be run on the upload server as a cron task, mysqldb for
# python should be installed on the server as well as a mysql acct from the
# upload server to the database server

# CMC note -- need to install 3rd party MySQLdb libraries for python
import traceback, sys, os, time, tempfile, string, MySQLdb, shutil, zipfile
from datetime import datetime
from qcnutil import getSACMetadata


# CMC note -- make sure these paths exist, or they will be created!
FINAL_UPLOAD_DIR_SENSOR    = "/var/www/trigger/"
FINAL_UPLOAD_DIR_CONTINUAL = "/home/boinc/web/trigger/continual/"

TMP_DIR = "/tmp"
UPLOAD_WEB_DIR = "/home/boinc/web/trigger/"
UPLOAD_USB_WEB_DIR = "/home/boinc/web/trigger/usb/"
UPLOAD_CONTINUAL_WEB_DIR = "/home/boinc/web/trigger/continual/"
UPLOAD_BOINC_DIR = "/home/boinc/web/boinc/sensor/upload"
#not saving original file, moves to /cees2
#UPLOAD_BACKUP_DIR = "/home/boinc/upload_backup/"
UNZIP_CMD = "/usr/bin/unzip -o -d " + UPLOAD_WEB_DIR + " " 

#UPLOAD_WEB_DIR = "c:\\temp\\web\\"
#UPLOAD_BOINC_DIR = "c:\\temp\\upload\\"
#UPLOAD_BACKUP_DIR = "c:\\temp\\uplbak\\"
#UNZIP_CMD = "c:\\program files\\7-zip\\7z.exe e -y -o" + UPLOAD_WEB_DIR + " "

DBNAME = "sensor"
DBHOST = "db-private"
DBUSER = "qcn"
DBPASSWD = ""

QUERY = "SELECT t.id,t.hostid,t.latitude,t.longitude,t.levelvalue,t.levelid,t.file, " +\
            "t.qcn_quakeid, q.time_utc quake_time, q.depth_km quake_depth_km, " +\
            "q.latitude quake_lat, q.longitude quake_lon, q.magnitude quake_mag " +\
              "FROM " + DBNAME + ".qcn_trigger t " +\
              "LEFT OUTER JOIN sensor.qcn_quake q ON q.id = t.qcn_quakeid " +\
              "WHERE t.received_file=100 AND t.id="

# delete old invalid zip files > 14 days old, they'll never get uploaded or fixed
def delFilesPath(path):
  now = time.time()
  for f in os.listdir(path):
    fname = os.path.join(path, f)
    if os.stat(fname).st_mtime < now - (14 * 86400):
      if os.path.isfile(fname) and f.endswith(".zip"):
        os.remove(fname)

   
def removeTempDir(tmpdir):
   # first see if this tmpdir exists!
   if not os.access(tmpdir, os.F_OK | os.W_OK):
       return
      
   # delete all tmp files & dirs in and below tmpdir
   for root, dirs, files in os.walk(tmpdir, topdown=False):
       for name in files:
           os.remove(os.path.join(root, name))
       for name in dirs:
           os.rmdir(os.path.join(root, name))
   # now delete parent tmpdir directory
   os.rmdir(tmpdir)

def checkSingleZipFile(dbconn, dirupl, myzipfile):
   # process a single zip file, i.e. test, unzip into myTempDir, list all the filenames,
   # test the zips within the zip file perhaps?, and of course update the qcn_trigger
   # table that we have received this zipfile
   fullzippath = os.path.join(dirupl, myzipfile)
   errLevel = 0
   try:  # catch zipfile exceptions if any
      myCursor = dbconn.cursor()
      
      myzip = zipfile.ZipFile(fullzippath, "r")
      # test for valid zip file
      if myzip.testzip() != None:
         # move out invalid zip file by raising the zipfile.error exception (caught below)
         raise zipfile.error

      if myzipfile.endswith("_usb.zip"):
          # this is an upload from a usb test zip file
          newfile = os.path.join(UPLOAD_USB_WEB_DIR, myzipfile)
          # move the file over to our disk archive as appropriate for the trigger type
      elif myzipfile.startswith("continual_"):
          # now update the qcn_trigger table!
          strSQL = "UPDATE continual.qcn_trigger SET received_file=100 " +\
                        "WHERE file='" + myzipfile + "'"
          myCursor.execute(strSQL)
          dbconn.commit()
      else: 
          # now update the qcn_trigger table!
          myCursor.execute("UPDATE sensor.qcn_trigger SET received_file=100 " +\
                        "WHERE file='" + myzipfile + "'")
          dbconn.commit()

        # end of for loop of files within the zip archive

      myzip.close()
      print "Successfully processed " + fullzippath
     
   # note error zip files ages over 30 days will get deleted by delFilesPath, so don't delete here
   except zipfile.error:
      errLevel = 1
      dbconn.rollback()
      if myzip != None:
         myzip.close()
      print "Error 1 in " + fullzippath + "(invalid zip file)"
      traceback.print_exc()
   except:
      errLevel = 2
      dbconn.rollback()
      if myzip != None:
         myzip.close()
      print "Error 2 in " + fullzippath
      traceback.print_exc()
   
def checkUploadZIPFiles(dbconn, dirupl):
   # the uploads are actually 1-to-many qcn_trigger zip files within a "parent" zip file
   # (i.e. a single upload from a machine could have multiple trigger files in it)
   
   # this proc will unzip the files in UPLOAD_BOINC_DIR to UPLOAD_WEB_DIR and move the "parent"
   # zip file to UPLOAD_BACKUP_DIR

   # each file in the parent zip needs to be updated in the qcn_trigger table (field: file)
   # with the time received (calling the updateTriggerFile() proc)

   # first off, get a temp directory to work in, as we need it to be empty each time
   # so as to easily get the file list within each "parent" zip file
   # CMC NOTE: tmpdir not needed as zipfile obj can test & get names from zip file
   #myTempDir = tempfile.mkdtemp("qcn")
   #if myTempDir == "":
   #   print "Could not make temp directory, quitting..."
   #   return

   # now we need to enumerate a list of files in UPLOAD_BOINC_DIR
   # go through one by one unzipping to tempdir, updating database etc

   newzips = os.listdir(dirupl)
   for myzipfile in newzips:
      fullzippath = os.path.join(dirupl, myzipfile)
      if myzipfile[-3:] == "zip" \
        and os.path.isfile(fullzippath) \
        and zipfile.is_zipfile(fullzippath):
         # if here then this is a zipfile, so process
         checkSingleZipFile(dbconn, dirupl, myzipfile)

   # don't forget to remove the temp directory!
   #removeTempDir(myTempDir)

def updateTriggerFile(filename, dbconn):
   # generate the trickle down request for a trigger that may have occurred near a USGS event
   try:
      cMain = dbconn.cursor()
      cMain.execute(QUERY_TRIGGER_HOST_LIST)
      i = 0
      while (1):
         rowTrig = cMain.fetchone()
         if rowTrig == None:
            break

         # for each "unprocessed" quake, set matches in trigger table
         print "Requesting files for host # " + str(rowTrig[0])
         cTrig = dbconn.cursor()
         cTrig.execute("SELECT concat('<sendme>',t.file,'</sendme>\n') from qcn_trigger t " +\
            "where hostid=" + str(rowTrig[0]) + " and t.qcn_quakeid>0 " +\
            "and (time_filereq is null or time_filereq=0)" )

         strSendMe = ""
         while (1):
            rowTrickle = cTrig.fetchone()
            if rowTrickle == None:
               break
            strSendMe += rowTrickle[0]
            # check that string isn't too big, 256K is max msg_to_host size, 64K should be plenty
            if len(strSendMe) > 65536:
               break

         # now make the full trickle down insert
         if len(strSendMe) > 0:
             cTrig.execute("insert into msg_to_host " +\
                "(create_time,hostid,variety,handled,xml) " +\
                "SELECT unix_timestamp(), " + str(rowTrig[0]) + ", 'filelist', 0, " +\
                "concat('<trickle_down>\n<result_name>', r.name, '</result_name>\n<filelist>\n" +\
                strSendMe + "</filelist>\n</trickle_down>\n') " +\
                "from result r " +\
                "where r.hostid=" + str(rowTrig[0])  +\
                "  and r.sent_time=(SELECT max(rr.sent_time) from result rr where rr.hostid=r.hostid) " )

         cTrig.execute("update qcn_trigger set time_filereq=unix_timestamp() " +\
                       "where hostid=" + str(rowTrig[0]) +\
                       " and qcn_quakeid>0 and " +\
                         "(time_filereq is null or time_filereq=0)")
         
         dbconn.commit()
         cTrig.close()
         i = i + 1
         
      cMain.close()
      print str(i) + " hosts sent file requests (trickle down)"
   except:
      dbconn.rollback()
      traceback.print_exc()

# makes sure that the necessary paths are in place as defined above
def checkPaths():
   if not os.access(UPLOAD_WEB_DIR, os.F_OK | os.W_OK):
      print UPLOAD_WEB_DIR + " directory for UPLOAD_WEB_DIR does not exist or not writable!"
      return 1
   
   #if not os.access(UPLOAD_USB_WEB_DIR, os.F_OK | os.W_OK):
   #   print UPLOAD_USB_WEB_DIR + " directory for UPLOAD_USB_WEB_DIR does not exist or not writable!"
   #   return 1
   
   if not os.access(UPLOAD_CONTINUAL_WEB_DIR, os.F_OK | os.W_OK):
      print UPLOAD_CONTINUAL_WEB_DIR + " directory for UPLOAD_CONTINUAL_WEB_DIR does not exist or not writable!"
      return 1
   
   if not os.access(UPLOAD_BOINC_DIR, os.F_OK | os.W_OK):
      print UPLOAD_BOINC_DIR + " directory for UPLOAD_BOINC_DIR does not exist or not writable!"
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
      #if (checkPaths() != 0):
      #   sys.exit(2)

      #delFilesPath(UPLOAD_BOINC_DIR)
         
      dbconn = MySQLdb.connect (host = DBHOST,
                           user = DBUSER,
                           passwd = DBPASSWD,
                           db = DBNAME)

      # basically we need to go to each 
      checkUploadZIPFiles(dbconn, FINAL_UPLOAD_DIR_SENSOR)
      #checkUploadZIPFiles(dbconn, FINAL_UPLOAD_DIR_CONTINUAL)

      dbconn.close()

   except:
      traceback.print_exc()
      sys.exit(1)

if __name__ == '__main__':
    main()



