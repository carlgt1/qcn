#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//test:
//date -u -r 1341234234.32344
//Mon Jul  2 13:03:54 GMT 2012

//this program output:
//dTimeZero=1341234234.323440   tv_sec=1341234234  tv_usec=323440  
//gmtime=2012/07/02 13:03:54.323440

int main (int argc, char** argv)
{
   struct timeval tv;

   double dTimeZero = 1341234234.32344;
   tv.tv_sec =  (long) dTimeZero;
   tv.tv_usec = (long) ((dTimeZero - (double) tv.tv_sec) * 1.0e6f);
   time_t ttime = (time_t) dTimeZero;
   struct tm* ptm = gmtime(&ttime);

   fprintf(stdout, "dTimeZero=%f   tv_sec=%ld  tv_usec=%ld  \n", dTimeZero, tv.tv_sec, tv.tv_usec);

/*
           int tm_sec;     // seconds (0 - 60) 
           int tm_min;     // minutes (0 - 59) 
           int tm_hour;    // hours (0 - 23) 
           int tm_mday;    // day of month (1 - 31) 
           int tm_mon;     // month of year (0 - 11) 
           int tm_year;    // year - 1900 
           int tm_wday;    // day of week (Sunday = 0) 
           int tm_yday;    // day of year (0 - 365) 
           int tm_isdst;   // is summer time in effect? 
           char *tm_zone;  // abbreviation of timezone name 
           long tm_gmtoff; // offset from UTC in seconds 
*/
   fprintf(stdout, "gmtime=%4d/%02d/%02d %02d:%02d:%02d.%d day_of_year=%03d\n", 
     ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, tv.tv_usec, ptm->tm_yday);

   return 0;
}

