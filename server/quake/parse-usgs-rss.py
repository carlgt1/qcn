#! /usr/bin/env python

# format of a USGS quake record from the RSS feed:
#
#    Element: item
#        Element: pubDate
#        Content: "Fri, 23 Nov 2007 17:52:45 GMT"
#        Element: title
#        Content: "M 5.0, Banda Sea"
#        Element: description
#        Content: "November 23, 2007 17:52:45 GMT"
#        Element: link
#        Content: "http://earthquake.usgs.gov/eqcenter/recenteqsww/Quakes/us2007kbaz.php"
#        Element: geo:lat
#        Content: "-4.2740"
#        Element: geo:long
#        Content: "123.5287"
#        Element: dc:subject
#        Content: "5"
#        Element: dc:subject
#        Content: "pastweek"
#        Element: dc:subject
#        Content: "10.00 km"
#        Element: guid
#                Attribute -- Name: isPermaLink  Value: false
#        Content: "us2007kbaz"
#
# format that we want in the boinc init_data.xml preferences file:
#
#   <qu005>2.6|2007/11/21 12:46:34   | 53.737       | -164.664 |    95.3     |UNIMAK ISLAND REGION, ALASKA |http://blahblahblah</qu005>
#
# guid is used & stored in the qcn_quake table so we have a "running list" of usgs events


# CMC note -- need to install 3rd party pycurl, MySQLdb & mxDateTime libraries for python
import sys, string, autotrigger, pycurl, MySQLdb, mx.DateTime
from shutil import copyfile
from xml.dom import minidom, Node
from datetime import datetime
from time import strptime, mktime

URL_USGS =  "http://earthquake.usgs.gov/earthquakes/catalogs/eqs7day-M2.5.xml"
URL_USGS_Z = "http://earthquake.usgs.gov/earthquakes/catalogs/merged_catalog.xml.gz"
URL_USGS_CSV = "http://earthquake.usgs.gov/earthquakes/catalogs/eqs7day-M1.txt"

#FILE_USGS = "c:\qcn\server\quake\usgs-quake.xml"
#FILE_QCN  = "c:\qcn\server\quake\qcn-quake.xml"
#FILE_QCN_TEMP  = "c:\qcn\server\quake\qcn-quake.tmp"
#FILE_QCN_WORLD85  = "c:\qcn\server\quake\qcn-quake-world85.xml"

FILE_USGS = "/var/www/boinc/sensor/usgs-quake.xml"
FILE_USGS_CSV = "/var/www/boinc/sensor/usgs-quake.csv"
FILE_QCN  = "/var/www/boinc/sensor/qcn-quake.xml"
FILE_QCN_TEMP  = "/var/www/boinc/sensor/qcn-quake.tmp"
FILE_QCN_WORLD85  = "/var/www/boinc/sensor/qcn-quake-world85.xml"
FILE_QCN_WORLD85_CSV  = "/var/www/boinc/sensor/qcn-quake-world85.csv"

DBNAME = "sensor"
DBHOST = "db-private"
DBUSER = "qcn"
DBPASSWD = "hahaha"

# mimic our C++ class
class SQuake:
   def __init__(self):
      self.magnitude = 0.0
      self.latitude  = 0.0
      self.longitude = 0.0
      self.depth_km  = 0.0
      self.strTime   = "1900/01/01 00:00:00"
      self.strDesc   = ""
      self.strURL    = ""
      self.strGUID   = ""

   def checkDB(self, dbconn):
       #CMC here -- lookup this guid in qcn_quake table, if not exist, insert
      try:
         cursor = dbconn.cursor()
         cursor.execute("SELECT COUNT(*) FROM qcn_quake WHERE guid='" + self.strGUID + "'")
         row = cursor.fetchone()
         #print "GUID: ", self.strGUID, "  count=", row[0]
         if row[0] == 0:
            #insert into qcn_quake table
            timepattern = '%Y/%m/%d %H:%M:%S'
            utc_strptime = strptime(self.strTime, timepattern)[0:6]
            mydt = mx.DateTime.DateTime(*utc_strptime)
            try:
               #print utc_strptime, mydt, self.strTime, self.strGUID
               strBigSQL = "INSERT INTO qcn_quake (time_utc, magnitude, depth_km, latitude, longitude, description, url, guid) " +\
                  " VALUES ( " + str(mydt.gmticks()) + "," +\
                  str(self.magnitude) + "," + str(self.depth_km) + "," + str(self.latitude) + "," +\
                  str(self.longitude) + ",'" +\
                  dbconn.escape_string(self.strDesc) + "','" +\
                  dbconn.escape_string(self.strURL) + "','" +\
                  dbconn.escape_string(self.strGUID) + "')"
               #print utc_strptime, mydt, self.strTime
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

def walk(parent, outFile, level, fileqcn, ctr, dbconn):                   # [1]
    if ctr > 300:
        return ctr

    for node in parent.childNodes:
        if node.nodeType == Node.ELEMENT_NODE:
            content = []                                        # [3]
            for child in node.childNodes:
                if child.nodeType == Node.TEXT_NODE:
                    content.append(child.nodeValue)
                if content:
                   strContent = string.join(content)
            if node.nodeName == "item":    # this is a new quake 
                ctr = ctr + 1   # note ctr starts out as -1 so first time in will be 0 to get our counters setup!
                if ctr == 0:    # first time in
                   sq.__init__()
                else:           # wrap this one up
                   sq.print_record(fileqcn, ctr, dbconn)
                   sq.__init__()
            elif node.nodeName == "description":   # UTC date in format November 23, 2007 17:52:45 GMT
                where = strContent.find(" ")   # find the first blank, from 0 to where will be the month
                where2 = strContent.find(", ")
                day = "01"
                year = "1900"
                month = "01"
                hour = "00"
                minute = "00"
                second = "00"
                if where > -1 and where2 > -1:
                   strMonth = strContent[0:where].strip()
                   month = "01"
                   if strMonth == "January":
                      month = "01"
                   elif strMonth == "February":
                      month = "02"
                   elif strMonth == "March":
                      month = "03"
                   elif strMonth == "April":
                      month = "04"
                   elif strMonth == "May":
                      month = "05"
                   elif strMonth == "June":
                      month = "06"
                   elif strMonth == "July":
                      month = "07"
                   elif strMonth == "August":
                      month = "08"
                   elif strMonth == "September":
                      month = "09"
                   elif strMonth == "October":
                      month = "10"
                   elif strMonth == "November":
                      month = "11"
                   elif strMonth == "December":
                      month = "12"

                   day = strContent[where+1:where2]
                   year = strContent[where2+2:where2+6]

                   hour = strContent[where2+7:where2+9]
                   minute = strContent[where2+10:where2+12]
                   second = strContent[where2+13:where2+15]

#        Content: "November 23, 2007 17:52:45 GMT"
#   <qu005>2.6|2007/11/21 12:46:34   | 53.737       | -164.664 |    95.3     |UNIMAK ISLAND REGION, ALASKA |http://blahblahblah</qu005>
                #print strContent + "  " + month + "/" + day + "/" + year + "  " + hour + ":" + minute + ":" + second
                sq.strTime = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second 
#                print strContent + "  " + sq.strTime

            elif node.nodeName == "title":     # magnitude & location in format M 5.0, Banda Sea
                # have to strip out magnitude, it's between the initial "M" and the first ","
                where = strContent.find(", ")
                if where > -1:  # we can now parse out the magnitude and description
                   sq.magnitude = float(strContent[1:where])
                   sq.strDesc   = strContent[where+2:]
            elif node.nodeName == "link":      # the url
                sq.strURL    = strContent
            elif node.nodeName == "geo:lat":   # latitude
                sq.latitude = float(strContent)
            elif node.nodeName == "guid":  # unique identifier for the event from USGS
                sq.strGUID = strContent
            elif node.nodeName == "geo:long":  # longitude
                sq.longitude = float(strContent)
            elif node.nodeName == "dc:subject": # if has " km" in it it's the depth in kilometers
                dtest = " km"
                where = strContent.find(dtest)
                if where > -1:  # if must be a depth, strip out the " km" and cast to float()
                   sq.depth_km = float(strContent.replace(dtest, "")) 

            # Write out the element name.
       #     printLevel(outFile, level)
       #     outFile.write('Element: %s\n' % node.nodeName)
            # Write out the attributes.
       #     attrs = node.attributes                            # [2]
       #     for attrName in attrs.keys():
       #         attrNode = attrs.get(attrName)
       #         attrValue = attrNode.nodeValue
       #         printLevel(outFile, level + 2)
       #         outFile.write('Attribute -- Name: %s  Value: %s\n' % \
       #             (attrName, attrValue))
            # Walk over any text nodes in the current node.
       #     content = []                                        # [3]
       #     for child in node.childNodes:
       #         if child.nodeType == Node.TEXT_NODE:
       #             content.append(child.nodeValue)
       #     if content:
       #         strContent = string.join(content)
       #         printLevel(outFile, level)
       #         outFile.write('Content: "')
       #         outFile.write(strContent)
       #         outFile.write('"\n')
            # Walk the child nodes.
            ctr = walk(node, outFile, level+1, fileqcn, ctr, dbconn)
    return ctr

def printLevel(outFile, level):
    for idx in range(level):
        outFile.write('    ')

def run(inFileName):                                            # [5]

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
   doc = minidom.parse(inFileName)
   rootNode = doc.documentElement
   ctr   = -1
   level = 0
   ctr = walk(rootNode, outFile, level, FILEQCN, ctr, dbconn)
   # note -- to get the last record we'll have to do one more print here!
   ctr = ctr + 1   # note ctr starts out as -1 so first time in will be 0 to get our counters setup!
   sq.print_record(FILEQCN, ctr, dbconn)
   FILEQCN.write("</quakes>\n")

   # now read in the historical quake file
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
   copyfile(FILE_QCN_TEMP, FILE_QCN) 
   
# todo: get some exit codes/ check for valid file etc
def getXMLFile(cd):
#def getCSVFile(cd):
    c  = pycurl.Curl()
    c.setopt(c.URL, URL_USGS)
    c.setopt(c.WRITEFUNCTION, cd.curl_callback)
    c.perform()
    c.close()
#   now we can write out the contents to the file quake.xml
#    print cd.xmldata
    FILEUSGS = open(FILE_USGS,"w")
    FILEUSGS.writelines(cd.xmldata)
    FILEUSGS.close()

def main():
    cd = CurlData()
    getXMLFile(cd)
    run(FILE_USGS)
    
    # run the autotrigger.py script now
    autotrigger.main()

if __name__ == '__main__':
    main()

