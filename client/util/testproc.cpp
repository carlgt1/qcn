#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"  // boinc/lib
/*
g++ testproc.cpp -o test -L../../boinc/lib -I../../boinc/lib -lboinc
*/

int main(int argc, char** argv)
{
/*
   FILE* fProc = popen("/Users/carlc/qcn/client/bin/ntpdate_4.2.4_i686-apple-darwin -t 5.0 -u -q qcn-upl.stanford.edu", "r");
   if (!fProc) return false;  // error

   char strReply[1024];
   int iLenReply = 1024;
   memset(strReply, 0x00, sizeof(char) * iLenReply);

   fread(strReply, sizeof(char), iLenReply - 1, fProc);

   fprintf(stdout, "just before pclose\n");
   fflush(stdout);
   pclose(fProc);
   fprintf(stdout, "just after pclose\n");
   fflush(stdout);

   fprintf(stdout, "Reply is:\n\n%s\n", strReply);
*/
   double now = dtime(), day = dday();

   fprintf(stdout, "dtime = %f    dday = %f   diff = %f  num10 = %ld  num60 = %ld  next10 = %f\n", 
      now, day, now-day, (long) (now-day) / 600L, (long) (now-day) / 3600L, (600L * (1 + ((long)(now-day)/600L))) + day
   );
   return 0;
}

