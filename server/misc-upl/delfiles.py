#! /usr/bin/env python

#remove job files older than a month 

import os, time

# PATHS = ("/var/www/trigger/job", "/var/www/trigger/continual/job")
PATHS = "/var/www/boinc/qcn/upload/"

def delFilesPath(path):
  now = time.time()
  for f in os.listdir(path):
    fname = os.path.join(path, f)
    if os.stat(fname).st_mtime < now - (30 * 86400):
      if os.path.isfile(fname) and f.find(".zip") > 0:
        print fname
        os.remove(fname)

#for p in PATHS:

delFilesPath(PATHS)


