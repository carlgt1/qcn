/* 

Mac OS X test program for USB Joywarrior codemercs.com device, compile with:

g++ -g -O0 HID_Utilities.cpp IOHIDDevice_.cpp IOHIDElement_.cpp -framework IOKit -framework Carbon testusb.cpp -o testusb

rm testusb ; g++ -g -O0 HID_Utilities.cpp IOHIDDevice_.cpp IOHIDElement_.cpp -framework IOKit -framework Carbon testusb.cpp -o testusb ; ./testusb

*/

#include <cstdio>
#include <cctype>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include "HID_Utilities.h"

// setup the shared memory segment (a class that resides in shared mem) for the QCN (& graphics) app
#include <list>
using std::list;

int main(int argc, char** argv)
{
   UInt32 uiPages[1000], uiUsages[1000], uiNumDev = 1000;
   //HIDBuildMultiDeviceList( uiPages, uiUsages, uiNumDev);
   int retval = HIDBuildMultiDeviceList( nil, nil, 0);

   fprintf(stdout, "NumDevices=%d retval=%d mgrref=%x  devaryref=%x  elref=%x\n", uiNumDev, retval, gIOHIDManagerRef, gDeviceCFArrayRef, gElementCFArrayRef);

   return 0;
}

