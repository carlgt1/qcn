#!/usr/bin/python

import sys, traceback, string, simplejson, autotrigger, pycurl, MySQLdb, mx.DateTime

DBNAME_TRIGMEM = "trigmem"
DBHOST_TRIGMEM = "localhost"
DBUSER_TRIGMEM = "trigmem"
DBPASSWD_TRIGMEM = ""

def run(): 
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

if __name__ == '__main__':
    run()

