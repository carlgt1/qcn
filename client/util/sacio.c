#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "qcn_util.h"

#define   MAXI 10001
#define   FALSE 0

int main(void)
{


   float x0[MAXI],y0[MAXI],tb,te,dt;
   int   i,npts, nerr, max=MAXI;
   char fname[64];
   long lfalse = FALSE;
   tb = 0.;
   dt = 0.1;
   for (i=1; i <= max/2; i++ ) {
      x0[i] = tb+i*dt;
      y0[i] = 0.;
      if (x0[i] > 100 && x0[i] < 200) y0[i] = 2.;
   };
   y0[1000]=2.;
   te = x0[max];
   strcpy(fname,"file1.sac");
   newhdr_();
   npts = max/2;
   setnhv_("npts"   , &   max, &nerr, 4 );
   setlhv_("leven"  , & lfalse, &nerr, 5 );
   setfhv_("b"      , &    tb, &nerr, 1 );
   setfhv_("e"      , &    te, &nerr, 1 );
   setihv_("iftype" , "ixy"  , &nerr, 6, 3 );
   wsac0_(fname, x0, y0, &nerr, strlen(fname));
   return 0;
}
