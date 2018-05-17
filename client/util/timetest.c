#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// CMC -- tests usages of dtime within various sized loops & usleep's
// gcc timetest.c ; ./a.out ; rm a.out

/* typical output -- so we can see a high-frequency loop of mean_xyz (500Hz) has a "base" CPU load of .656%, not bad
                     and the typical main monitoring loop at 5Hz is not even measurable

With dtime & usleep  : Time elapsed for 000005000000 iterations at 0.000002 per loop is 42.210741 - CPU time = 14.690000 - Usage = 34.801568 pct
With no sleep in loop: Time elapsed for 000500000000 iterations at 1000000.000000 per loop is 2.278452 - CPU time = 2.160001 - Usage = 94.801236 pct  (Total CPU = 16.850000)
usleep=2   iLoop=5000000  clockbase=0.000000  usage = 34.801568 pct

With dtime & usleep  : Time elapsed for 000000500000 iterations at 0.000020 per loop is 17.688996 - CPU time = 2.959999 - Usage = 16.733562 pct
With no sleep in loop: Time elapsed for 000500000000 iterations at 1000000.000000 per loop is 2.295706 - CPU time = 2.180000 - Usage = 94.959907 pct  (Total CPU = 5.139999)
usleep=20   iLoop=500000  clockbase=16.850000  usage = 16.733562 pct

With dtime & usleep  : Time elapsed for 000000050000 iterations at 0.000200 per loop is 14.018257 - CPU time = 0.530001 - Usage = 3.780789 pct
With no sleep in loop: Time elapsed for 000500000000 iterations at 1000000.000000 per loop is 2.256178 - CPU time = 2.170000 - Usage = 96.180352 pct  (Total CPU = 2.700001)
usleep=200   iLoop=50000  clockbase=21.990000  usage = 3.780789 pct

With dtime & usleep  : Time elapsed for 000000005000 iterations at 0.002000 per loop is 10.664820 - CPU time = 0.070000 - Usage = 0.656361 pct
With no sleep in loop: Time elapsed for 000500000000 iterations at 1000000.000000 per loop is 2.258903 - CPU time = 2.160000 - Usage = 95.621628 pct  (Total CPU = 2.230000)
usleep=2000   iLoop=5000  clockbase=24.690001  usage = 0.656361 pct

With dtime & usleep  : Time elapsed for 000000000500 iterations at 0.020000 per loop is 10.046671 - CPU time = 0.020000 - Usage = 0.199075 pct
With no sleep in loop: Time elapsed for 000500000000 iterations at 1000000.000000 per loop is 2.293849 - CPU time = 2.189999 - Usage = 95.472659 pct  (Total CPU = 2.209999)
usleep=20000   iLoop=500  clockbase=26.920000  usage = 0.199075 pct

With dtime & usleep  : Time elapsed for 000000000050 iterations at 0.200000 per loop is 9.811425 - CPU time = 0.000000 - Usage = 0.000000 pct
With no sleep in loop: Time elapsed for 000500000000 iterations at 1000000.000000 per loop is 2.273279 - CPU time = 2.170000 - Usage = 95.456822 pct  (Total CPU = 2.170000)
usleep=200000   iLoop=50  clockbase=29.129999  usage = 0.000000 pct

*/

double dtime();

int main(int argc, char** argv)
{ 
   int i,j,k,iLoop ;
   double dStart, dEnd;
   double dClockBase = 0.0f, dClock[2] = {0.0,0.0};
   long cSleep;
   double dUsageWork;
   for (k = 0; k < 6; k++) {
     cSleep = 2 * (long) pow(10, k);
     dClockBase = (float)clock()/(float)CLOCKS_PER_SEC;
     iLoop = 1.0e7 / cSleep;
     for (j = 0; j < 2; j++) {
       dStart = dtime();
       i = 0;
       for (i = 1; i < (j==0 ? iLoop : 5e8); i++) {
          if (j==0)  {
            dEnd = dtime();
            usleep(cSleep);
          }
       }
       dEnd = dtime();
       dClock[j] = ((float)clock()/(float)CLOCKS_PER_SEC) - dClockBase;
       //dClock[j] = clock() - dClockBase;
       if (j==0) { 
          dUsageWork = 100.0f * dClock[0]/(dEnd-dStart);
          fprintf(stdout, "With dtime & usleep  : Time elapsed for %012d iterations at %f per loop is %f - CPU time = %f - Usage = %f pct\n", 
             i, cSleep/1.0e6, dEnd-dStart, dClock[0], dUsageWork);
       }
       else {
          fprintf(stdout, "With no sleep in loop: Time elapsed for %012d iterations at %f per loop is %f - CPU time = %f - Usage = %f pct  (Total CPU = %f)\n", 
             i, 1.0e6, dEnd-dStart, dClock[1]-dClock[0], 100.0f * (dClock[1] - dClock[0])/(dEnd-dStart), dClock[j]);
       }
     }
     fprintf(stdout, "usleep=%d   iLoop=%d  clockbase=%f  usage = %f pct\n\n", 
      cSleep, iLoop, dClockBase, dUsageWork);
     fflush(stdout);
   }
   return 0;
}

// return time of day (seconds since 1970) as a double
//
double dtime() {
#ifdef GCL_SIMULATOR
    return simtime;
#else
#ifdef _WIN32
    LARGE_INTEGER time;
    FILETIME sysTime;
    double t;
    GetSystemTimeAsFileTime(&sysTime);
    time.LowPart = sysTime.dwLowDateTime;
    time.HighPart = sysTime.dwHighDateTime;  // Time is in 100 ns units
    t = (double)time.QuadPart;    // Convert to 1 s units
    t /= TEN_MILLION;                /* In seconds */
    t -= EPOCHFILETIME_SEC;     /* Offset to the Epoch time */
    return t;
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + (tv.tv_usec/1.e6);
#endif
#endif
}

