#! /usr/bin/env python

import math, tempfile, smtplib, traceback, sys, os, tempfile, string, MySQLdb, shutil, zipfile
from datetime import datetime
from zipfile import ZIP_STORED
from time import strptime, mktime
from subprocess import Popen, PIPE, STDOUT

GRD_CMD = "/data/QCN/GMT/bin/grd2point /data/QCN/GMT/share/topo/topo30.grd -R"

lonTrig = -122.3485
latTrig = 37.96850

# elevation data - usage of GRD_CMD -Rlon_min/lon_max/lat_min/lat_max
#grd2point /data/QCN/GMT/share/topo/topo30.grd -R$lng/$lng2/$lat/$lat2
#> temp.xyz#output
#/data/QCN/GMT/bin/grd2point /data/QCN/GMT/share/topo/topo30.grd -R-75.01/-75.00/40.00/40.01#grd2point: GMT WARNING: (w - x_min) must equal (NX + eps) * x_inc), where NX is an integer and |eps| <= 0.0001.
#grd2point: GMT WARNING: w reset to -75.0083
#grd2point: GMT WARNING: (n - y_min) must equal (NY + eps) * y_inc), where NY is an integer and |eps| <= 0.0001.
#grd2point: GMT WARNING: n reset to 40.0083
#-75.0041666667 40.0041666667   19
grdstr = str(lonTrig - .005) + "/" + str(lonTrig + .005) + "/" + str(latTrig - .005) + "/" + str(latTrig + .005)

#cc = subprocess.check_output([GRD_CMD, grdstr])
#os.system(GRD_CMD + grdstr)

cc = Popen(GRD_CMD + grdstr, shell=True, stdout=PIPE).communicate()[0]

vals = cc.rstrip("\n").split("\t")

myElev = 0.0
if len(vals) == 3:
   myElev = float(vals[2])

print myElev
