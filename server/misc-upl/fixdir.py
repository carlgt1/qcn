#! /usr/bin/env python
#remove job files older than two months

import os, time, shutil

#FROM = "/data/QCN/trigger/continual/"
#TO   = "/data/QCN/trigger/continual/"

FROM = "/data/QCN/trigger/continual/"
TO   = "/data/QCN/trigger/continual/"

def moveFilesPath(frompath, topath):
  now = time.time()
  for subdir in os.listdir(frompath):
    dirpath = os.path.join(frompath, subdir)
    if os.path.isdir(dirpath):
       for f in os.listdir(dirpath):
          fullpath = os.path.join(dirpath, f)
          if os.path.isfile(fullpath):
             try:
               shutil.move(fullpath, topath)
               print "Moved " + fullpath + " to " + topath
             except:
               print "ERROR moving " + fullpath+ " to " + topath
          else:
             print "ERROR - " + fullpath + " is not a file"
       # now remove the subdir
       shutil.rmtree(dirpath)

moveFilesPath(FROM, TO)

