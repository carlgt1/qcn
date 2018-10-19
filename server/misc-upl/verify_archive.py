#! /usr/bin/env python

#remove job files older than two months

import traceback, sys, os, time, tempfile, string, MySQLdb, shutil, zipfile
from datetime import datetime
from qcnutil import getFanoutDirFromZip

DBNAME = "sensor"
DBHOST = "db-private"
DBUSER = ""
DBPASSWD = ""

# regular sensor/sensor archive
QUERY = "SELECT id, floor(time_trigger/10000) subdir, result_name, file FROM sensor_archive.qcn_trigger WHERE received_file=100"
QUERY_UPDATE = "UPDATE sensor_archive.qcn_trigger SET received_file=99 WHERE id=%s"
FROM = "/cees2/QCN/trigger/archive/"
TO   = "/data/QCN/trigger/archive/"

#continual
#FROM = "/cees2/QCN/trigger/archive/continual/"
#TO   = "/data/QCN/trigger/archive/continual/"
#QUERY = "SELECT id, floor(time_trigger/10000) subdir, result_name, file FROM continual_archive.qcn_trigger WHERE received_file=100"
#QUERY_UPDATE = "UPDATE continual_archive.qcn_trigger SET received_file=99 WHERE id=%s"

# fields are id, subdir, result_name, file
def validateFilePath(dbconn, id, subdir, result, fname):
  # setup the paths
  oldpath = os.path.join(FROM, subdir)
  oldname = os.path.join(oldpath, fname)
  newpath = os.path.join(TO,   subdir)
  newname = os.path.join(newpath, fname)

  if not os.path.isfile(newname):
  #  print "File Already In New /data Path:  " + newname
  #else:
    # see if it's in the old path
    if os.path.isfile(oldname):
      # try to copy over
      try:
        shutil.copy(oldname, newname)
        print "Copied " + oldname + " to " + newname
      except:
        print "ERROR: copying " + oldname + " to " + newname 
    else:
      print "File Does Not Exist!  Setting received_file to 99:  " + oldname
      try:
        cUpd = dbconn.cursor()
        cUpd.execute(QUERY_UPDATE, id)
        dbconn.commit();
        cUpd.close()
      except: 
        print "ERROR: Database Error setting received_file to 99 for ID # " + str(id) + "  " + oldname
        traceback.print_exc()

######### end of function ########


# get a list from the database and in the while loop validate & copy each file (if necessary)
try:
  dbconn = MySQLdb.connect (host = DBHOST,
                           user = DBUSER,
                           passwd = DBPASSWD,
                           db = DBNAME)

  cMain = dbconn.cursor()
  cMain.execute(QUERY)
  while (1):
    rowTrig = cMain.fetchone()
    if rowTrig == None:
      break

    # fields are id, subdir, result_name, file
    validateFilePath(dbconn, rowTrig[0], str(int(rowTrig[1])), rowTrig[2], rowTrig[3])

  print "Finished - closing database"
  cMain.close()
  dbconn.close()

except:
  print "Global Error"
  traceback.print_exc()
  sys.exit(3)


