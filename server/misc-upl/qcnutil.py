#! /usr/local/bin/python
#// #! /usr/bin/env python

# program to get sac metadata given trigger info
# processes an input zip file, no database or URL info required
#def getSACMetadata(zipinname, hostid, latTrig, lonTrig, lvlTrig, lvlType, idQuake, timeQuake, depthKmQuake, latQuake, lonQuake, magQuake):

import math, tempfile, smtplib, traceback, sys, os, string, shutil, zipfile
from datetime import datetime
from zipfile import ZIP_STORED
from time import strptime, mktime
from subprocess import Popen, PIPE, STDOUT

global SAC_CMD, SACSWAP_CMD, GRD_CMD

os.environ["SACAUX"] = "/usr/local/sac/aux"

SAC_CMD = "/usr/local/sac/bin/sac"
SACSWAP_CMD = "/usr/local/sac/bin/sacswap"
GRD_CMD = "/usr/local/gmt/bin/grd2point /usr/local/gmt/share/topo/topo30.grd -R"

#fanout dir name
def getFanoutDirFromZip(myzip):
    dbin = "0"
    dtime = 0
    if not myzip.endswith(".zip"):
      return (dbin, dtime)

    dzip = myzip.find(".zip")
    dund = myzip.rfind("_")
    if dzip>0 and dund>0 and dund<dzip:
       # found zip & underscore
       dtime = myzip[dund+1:dzip]
       dbin = myzip[dund+1:dund+7]
       if dbin.endswith(".zip"):
         dbin = dtime
    
    return (dbin, long(dtime))

def makeFanoutDir(rootdir, fandir):
    #make directory
    fullpath = os.path.join(rootdir, fandir)    
    if not os.path.isdir(fullpath):
      os.mkdir(fullpath)
    return fullpath

# this will put metadata into the SAC file using values from the database for this trigger
# it's very "quick & dirty" and just uses SAC as a cmd line program via a script
def getSACMetadata(zipinname, hostid, latTrig, lonTrig, lvlTrig, lvlType, idQuake, timeQuake, depthKmQuake, latQuake, lonQuake, magQuake):
  global SAC_CMD, SACSWAP_CMD, GRD_CMD


# elevation data - usage of GRD_CMD -Rlon_min/lon_max/lat_min/lat_max
#grd2point /usr/local/gmt/share/topo/topo30.grd -R$lng/$lng2/$lat/$lat2
#> temp.xyz
#output
#/usr/local/gmt/bin/grd2point /usr/local/gmt/share/topo/topo30.grd -R-75.01/-75.00/40.00/40.01
#grd2point: gmt WARNING: (w - x_min) must equal (NX + eps) * x_inc), where NX is an integer and |eps| <= 0.0001.
#grd2point: gmt WARNING: w reset to -75.0083
#grd2point: gmt WARNING: (n - y_min) must equal (NY + eps) * y_inc), where NY is an integer and |eps| <= 0.0001.
#grd2point: gmt WARNING: n reset to 40.0083
#-75.0041666667 40.0041666667   19

#outputs closest lon/lat point and elevation in meters
  myElev = 0.0
  # lvlType of 4 or 5 means they explicitly put in the elevation, so no need to look up 

  # CMC disable this for now
  #
  #if lvlType not in (4,5):
  #  grdstr = str(lonTrig - .005) + "/" + str(lonTrig + .005) + "/" + str(latTrig - .005) + "/" + str(latTrig + .005)
  #  cc = Popen(GRD_CMD + grdstr, shell=True, stdout=PIPE).communicate()[0]
  #  vals = cc.rstrip("\n").split("\t")
  #  if len(vals) == 3:
  #    myElev = float(vals[2])

  # at this point myElev is either 0 or their estimated elevation in meters based on lat/lng

#lvlType should be one of:
#|  1 | Floor (+/- above/below surface)    | 
#|  2 | Meters (above/below surface)       | 
#|  3 | Feet (above/below surface)         | 
#|  4 | Elevation - meters above sea level | 
#|  5 | Elevation - feet above sea level   |  note 4 & 5 they input actual elevation , so use that
#
  # we want level in meters, ideally above sea level, but now just convert to meters (1 floor = 3 m)
  myLevel = myElev
  if lvlType == 1:
    myLevel = myElev + (lvlTrig * 3.0)
  elif lvlType == 2:
    myLevel = myElev + lvlTrig
  elif lvlType == 3:
    myLevel = myElev + (lvlTrig * 0.3048)
  elif lvlType == 4:
    myLevel = lvlTrig
  elif lvlType == 5:
    myLevel = lvlTrig * 0.3048

#  sac values to fill in are: stlo, stla, stel (for station)
#                             evlo, evla, evdp, mag (for quake)

#  print "\n\nmyLevel = " + str(myLevel) + " meters\n\n"
  fullcmd = SAC_CMD + " << EOF\n" +\
    "r " + zipinname + "\n" +\
    "chnhdr kstnm " + str(hostid) + "\n" +\
    "chnhdr stlo " + str(lonTrig) + "\n" +\
    "chnhdr stla " + str(latTrig) + "\n" +\
    "chnhdr stel " + str(myLevel) + "\n"

  #if myLevel != 0.0:
  #  fullcmd = fullcmd + "chnhdr stel " + str(myLevel) + "\n" 

  if idQuake > 0:
    fullcmd = fullcmd +\
      "chnhdr evlo " + str(lonQuake) + "\n" +\
      "chnhdr evla " + str(latQuake) + "\n" +\
      "chnhdr evdp " + str(1000.0 * depthKmQuake) + "\n" +\
      "chnhdr mag "  + str(magQuake) + "\n"

  fullcmd = fullcmd +\
      "chnhdr leven TRUE\n" +\
      "write over \n" +\
      "quit\n" +\
      "EOF\n"

# debug info
#  print fullcmd

  cc = Popen(fullcmd, shell=True, stdout=PIPE).communicate()[0]

# now need to run sacswap for some reason
#  fullcmd = SACSWAP_CMD + " " + zipinname
#  cc = Popen(fullcmd, shell=True, stdout=PIPE).communicate()[0]

# if we have a file named zipinname.swap, then we need to move back over to zipinname 
#  if os.path.isfile(zipinname + ".swap"):
#    shutil.move(zipinname + ".swap", zipinname)

# done metadata updating of SAC files

