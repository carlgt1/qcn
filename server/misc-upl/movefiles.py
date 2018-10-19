#! /usr/bin/env python
#remove job files older than two months

import os, time, shutil

FROM = "/data/QCN/trigger/continual/"
TO =   "/home/boinc/continual/"

def moveFilesPath(frompath, topath):
  now = time.time()
  for f in os.listdir(frompath):
    fname = os.path.join(frompath, f)
    if os.stat(fname).st_mtime < now - (60 * 86400):
      if os.path.isfile(fname) and f.find(".zip") > 0:
        try:
           shutil.move(fname, topath)
           print "Moved " + fname + " to " + topath
        except:
           print "ERROR moving " + fname + " to " + topath

#for p in PATHS:

moveFilesPath(FROM, TO)


