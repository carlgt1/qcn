#!/usr/bin/python
# #! /usr/bin/env python

# format of a USGS quake record from ther JSON feed
#  refer to this web page for format of the JSON data
#     http://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php

# format that we want in the boinc init_data.xml preferences file:
#
#   <qu005>2.6|2007/11/21 12:46:34   | 53.737       | -164.664 |    95.3     |UNIMAK ISLAND REGION, ALASKA |http://blahblahblah</qu005>
#
# guid is used & stored in the qcn_quake table so we have a "running list" of usgs events


# CMC note -- need to install 3rd party pycurl, MySQLdb & mxDateTime libraries for python
import sys, traceback, string, simplejson, autotrigger, pycurl, MySQLdb, mx.DateTime
from shutil import copyfile
from xml.dom import minidom, Node
from datetime import datetime
from time import strptime, mktime

URL_USGS_JSON = "http://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/2.5_week.geojson"

FILE_USGS_JSON = "/var/www/boinc/sensor/usgs-quake.json"
FILE_QCN  = "/var/www/boinc/sensor/qcn-quake.xml"
FILE_QCN_TEMP  = "/var/www/boinc/sensor/qcn-quake.tmp"
FILE_QCN_WORLD85  = "/var/www/boinc/sensor/qcn-quake-world85.xml"
FILE_QCN_WORLD85_CSV  = "/var/www/boinc/sensor/qcn-quake-world85.csv"

DBNAME = "sensor"
DBHOST = "db-private"
DBUSER = "qcn"
DBPASSWD = ""

DBNAME_TRIGMEM = "trigmem"
DBHOST_TRIGMEM = "localhost"
DBUSER_TRIGMEM = "trigmem"
DBPASSWD_TRIGMEM = ""

TIME_PATTERN = '%Y/%m/%d %H:%M:%S'

# mimic our C++ class
class SQuake:
   def __init__(self):
      self.magnitude = 0.0
      self.latitude  = 0.0
      self.longitude = 0.0
      self.depth_km  = 0.0
      self.strTime   = "1900/01/01 00:00:00"
      self.time = 0.0
      self.strDesc   = ""
      self.strURL    = ""
      self.strGUID   = ""

   def checkDB(self, dbconn):
      #lookup this guid in qcn_quake table, if not exist, insert
      try:
         cursor = dbconn.cursor()
         cursor.execute("SELECT COUNT(*) FROM qcn_quake WHERE guid='" + self.strGUID + "'")
         row = cursor.fetchone()
         #print "GUID: ", self.strGUID, "  count=", row[0]
         if row[0] == 0:
            #insert into qcn_quake table
            try:
               strBigSQL = "INSERT INTO qcn_quake (time_utc, magnitude, depth_km, latitude, longitude, description, url, guid) " +\
                  " VALUES ( " + str(sq.time) + "," +\
                  str(self.magnitude) + "," + str(self.depth_km) + "," + str(self.latitude) + "," +\
                  str(self.longitude) + ",'" +\
                  dbconn.escape_string(self.strDesc) + "','" +\
                  dbconn.escape_string(self.strURL) + "','" +\
                  dbconn.escape_string(self.strGUID) + "')"
               #print strBigSQL, self.strGUID, self.strTime
               cursor.execute(strBigSQL)
               dbconn.commit()
               print "added usgs quake " + self.strGUID + " to database"
            except:
              print "could not insert guid ", self.strGUID
         cursor.close()
      except:
         print "checkDB cursor error for ", self.strGUID

   def print_record(self, file, ctr, dbconn):
      if ctr > 0 and ctr <= 300:   # if not greater than 0, it didn't walk anything in the XML dom, possibly empty document?
        file.write("<qu%03d>%5.1f|%s|%9.4f|%9.4f|%7.2f|%s|%s</qu%03d>\n" %\
              (ctr, self.magnitude, self.strTime, self.latitude, \
               self.longitude, self.depth_km, self.strDesc, self.strURL, ctr) \
        )
      self.checkDB(dbconn)


sq = SQuake()

class CurlData:
   def __init__(self):
      self.xmldata = ""

   def curl_callback(self, buf):
      self.xmldata = self.xmldata + buf

def parseJSON(data, outFile, fileqcn, dbconn):                   # [1]
  ctr = 0
  try:
    json_data=open(FILE_USGS_JSON)
    data = simplejson.load(json_data)
    for item in data["features"]:
      ctr = ctr + 1 
      if ctr > 300:
         break
      sq.__init__()

      #print item['id'], item['properties']['time'],item['properties']['place'],item['properties']['mag'],item['properties']['url'],item['geometry']['coordinates'][0],item['geometry']['coordinates'][1],item['geometry']['coordinates'][2]

      sq.time = float(item['properties']['time'])/1000.0
      sq.strTime = datetime.fromtimestamp(int(sq.time)).strftime(TIME_PATTERN)
      sq.magnitude = float(item['properties']['mag'])
      sq.strDesc   = item['properties']['place']
      sq.strURL    = item['properties']['url']
      sq.latitude = float(item['geometry']['coordinates'][1])
      sq.longitude = float(item['geometry']['coordinates'][0])
      sq.strGUID = item['id']
      sq.depth_km = float(item['geometry']['coordinates'][2])
      sq.print_record(fileqcn, ctr, dbconn)
      sq.__init__()

    return ctr
  except:
    print "Error in parseJSON" + FILE_USGS_JSON
    traceback.print_stack()
    traceback.print_exc(file=sys.stdout)
    return -1


def printLevel(outFile, level):
    for idx in range(level):
        outFile.write('    ')

def run(inFileName):                                            # [5]
   # flush the memory tables periodically
   try:
      dbconn = MySQLdb.connect (host = DBHOST_TRIGMEM,
                           user = DBUSER_TRIGMEM,
                           passwd = DBPASSWD_TRIGMEM,
                           db = DBNAME_TRIGMEM)
   except:
       print "Database error!"
       sys.exit(1)

   cursor = dbconn.cursor()
   cursor.execute("DELETE FROM qcn_trigger_memory ")
   cursor.close()
   dbconn.close()

   try:
      dbconn = MySQLdb.connect (host = DBHOST,
                           user = DBUSER,
                           passwd = DBPASSWD,
                           db = DBNAME)
   except:
       print "Database error!"
       sys.exit(1)

   FILEQCN = open(FILE_QCN_TEMP,"w")
   FILEQCN.write("<quakes>\n")
   outFile = sys.stdout
   data = []
   ctr = parseJSON(data, outFile, FILEQCN, dbconn)
   FILEQCN.write("</quakes>\n")

   # always read in the historical quake file
   try:
     FILEQCNWORLD85 = open(FILE_QCN_WORLD85, "r")
     FILEQCN.write(FILEQCNWORLD85.read()) 
     FILEQCNWORLD85.close()
   except:
     print "File I/O Error on World85 Quake File: " + FILE_QCN_WORLD85

   FILEQCN.close()
   dbconn.close()

   # if we made it here then it's safe to overwrite the FILE_QCN
   # this way if there's an error we didn't blow out the file
   if (ctr >= 0):
      copyfile(FILE_QCN_TEMP, FILE_QCN) 
   
# todo: get some exit codes/ check for valid file etc
def getJSONFile(cd):
#def getCSVFile(cd):
    c  = pycurl.Curl()
    c.setopt(c.URL, URL_USGS_JSON)
    c.setopt(c.WRITEFUNCTION, cd.curl_callback)
    c.perform()
    c.close()
#   now we can write out the contents to the file quake.xml
#    print cd.xmldata
    FILEUSGS = open(FILE_USGS_JSON,"w")
    FILEUSGS.writelines(cd.xmldata)
    FILEUSGS.close()

def main():
    cd = CurlData()
    getJSONFile(cd)
    run(FILE_USGS_JSON)
    
    # run the autotrigger.py script now
    autotrigger.main()

if __name__ == '__main__':
    main()

