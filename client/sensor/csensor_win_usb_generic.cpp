/*
 *  csensor_win_usb_generic.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 09/18/2008.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for generic Mac USB driver interface to the qcnusb program
 */

#include "main.h"
#include "csensor_win_usb_generic.h"

CSensorMacUSBGeneric::CSensorMacUSBGeneric()
  : CSensor()
{
    tsm = NULL; 
    tsmState = NULL; 
    dTimeActive = dTimeCheck = dTimeLast = 0.0f;
    fdPipe[PIPE_STATE] = fdPipe[PIPE_SENSOR] = 0;
}

CSensorMacUSBGeneric::~CSensorMacUSBGeneric()
{
  closePort();
}

// NB: bClosePort is true if we want to close the accelerometer port on a pipe error
bool CSensorMacUSBGeneric::writePipeState(bool bClosePort)
{
    try {
       if (write(fdPipe[PIPE_STATE], tsm, sizeof(CQCNUSBState)) == EPIPE) {
           // write these settings to the pipe, if error close pipes and return false
           if (bClosePort) closePort();
           return false;
       }
    }
    catch(...) { 
        if (bClosePort) closePort();
        return false;
    }
   return true;
}

void CSensorMacUSBGeneric::closePort()
{
    if (tsmState) {
        tsmState->alPID[PID_QCN] = tsmState->alPID[PID_USB] = 0L;
        tsmState->bStop = true; // this line and the previous line will "turn off" (pause) the qcnusb "service"
        writePipeState(false); // pass in false since we are already in closePort -- don't want a recursive call!
        delete tsmState; // free the memory
        tsmState = NULL;
    }
    if (tsm) {
        delete tsm;  // free memory
        tsm = NULL;
    }
    if (fdPipe[PIPE_STATE] > -1)  close(fdPipe[PIPE_STATE]);
    if (fdPipe[PIPE_SENSOR] > -1) close(fdPipe[PIPE_SENSOR]);
    fdPipe[PIPE_STATE] = fdPipe[PIPE_SENSOR] = 0;
}

inline bool CSensorMacUSBGeneric::mean_xyz()
{
   if (qcn_main::g_iStop || !sm) {
       throw EXCEPTION_SHUTDOWN;   // see if we're shutting down, if so throw an exception which gets caught in the sensor_thread
   }

   if (!tsm || !tsmState || !sm)  {
      fprintf(stderr, "CSensorMacUSBGeneric::mean_xyz: Invalid memory pointer!\n");
      return false;
   }

    if ( read(fdPipe[PIPE_SENSOR], tsm, sizeof(CQCNUSBSensor)) )  {
     tsm->bReading = true;

     sm->x0[sm->lOffset] = tsm->x0;
     sm->y0[sm->lOffset] = tsm->y0;
     sm->z0[sm->lOffset] = tsm->z0;
     sm->t0[sm->lOffset] = tsm->t0;
     //if (sm->dt == 0.0f || sm->dt < DT || sm->dt > DT_SLOW) sm->dt = tsm->dt; // dt should only be set if it's 0.0
     sm->t0active = tsm->t0active;
     sm->t0check  = tsm->t0check;

     sm->lSampleSize = tsm->lSampleSize;
     sm->ullSampleTotal = tsm->ullSampleTotal;
     sm->ullSampleCount = tsm->ullSampleCount;
     sm->fRealDT = tsm->fRealDT;

     tsm->bReading = false;
   }

#ifdef _DEBUG
   fprintf(stdout, "lOffset=%ld  x0=%f  y0=%f  z0=%f  t0=%f  dt=%f  t0active=%f  t0check=%f\n",
      sm->lOffset, sm->x0[sm->lOffset], sm->y0[sm->lOffset], sm->z0[sm->lOffset], sm->t0[sm->lOffset], sm->dt, sm->t0active, sm->t0check 
   );
#endif

   // check for timing error
   if (sm->lOffset > 1 && sm->lOffset < MAXI) {
        dTimeLast = sm->t0[sm->lOffset] - sm->t0[sm->lOffset-1];  // get the difference in times, should be < sm->dt & not more than TIME_ERROR_SECONDS
        if (fabs(dTimeLast) > TIME_ERROR_SECONDS) { // if our times are different by a second, that's a big lag, so let's reset t0check to t0active
            fprintf(stdout, "Timing error encountered t0check=%f  t0active=%f  diff=%f  dt=%f, resetting...\n",
               sm->t0[sm->lOffset], sm->t0[sm->lOffset-1], dTimeLast, sm->dt);
            fprintf(stderr, "Timing error encountered t0check=%f  t0active=%f  diff=%f  dt=%f, resetting...\n",
               sm->t0[sm->lOffset], sm->t0[sm->lOffset-1], dTimeLast, sm->dt);
            fflush(stdout);
            fflush(stderr);

            tsmState->alPID[PID_QCN] = getpid();  // set the process ID of this routine
            tsmState->bStop = true; // this will start monitoring on the qcnusb process
            tsm->eSensor = SENSOR_NOTFOUND; // init
            writePipeState(true);
            return false;   // if we're not debugging, this is a serious run-time problem, so reset time & counters & try again
        }
   }

   return true;

}

inline bool CSensorMacUSBGeneric::read_xyz(float& x1, float& y1, float& z1)
{  // there really is no read_xyz for the proc, is there?   just use mean_xyz
   if (!tsm || !sm || !mean_xyz()) return false;
   x1 = tsm->x0;
   y1 = tsm->y0;
   z1 = tsm->z0;
   return true;
}

bool CSensorMacUSBGeneric::detect()
{
   // first try and discover the HID interface (JoyWarrior)
   setType(SENSOR_NOTFOUND);  // initialize to no sensor until detected below

   // try and "talk" to the qcnusb "service" program - if running it has setup shared memory & we use semaphores for synchronization
    fdPipe[PIPE_STATE]  = open(QCN_USB_STATE,  O_WRONLY | O_NONBLOCK);   // state is non-blocking
    fdPipe[PIPE_SENSOR] = open(QCN_USB_SENSOR, O_RDONLY);                // sensor is blocking
    if (fdPipe[PIPE_STATE] == -1 || fdPipe[PIPE_SENSOR] == -1) {
        fprintf(stderr, "Error %d opening pipe %d %d, try later, or reboot!\n", errno, fdPipe[PIPE_STATE], fdPipe[PIPE_SENSOR]);
        fflush(stderr);
        closePort();
        return false;
    }

    tsm = new CQCNUSBSensor(); // create the tsm instance which will be used for writing to the pipe
    if (!tsm) {
        fprintf(stderr, "Failed to attach to shared memory segment for sensor data, try later, or reboot!\n");
        closePort();
        return false;
    }
    tsmState = new CQCNUSBState(); // create the tsm instance which will be used for writing to the pipe
    if (!tsmState) {
        fprintf(stderr, "Failed to attach to shared memory segment for state data, try later, or reboot!\n");
        closePort();
        return false;
    }

    // if made it here, the shared mem exists and is presumably running
    // to start monitoring, set the process ID and set bStop to false

    tsmState->alPID[PID_QCN] = getpid();  // set the process ID of this routine
    tsmState->bStop = false; // this will start monitoring on the qcnusb process
    tsm->eSensor = SENSOR_NOTFOUND; // init

    if (!writePipeState(true)) {
       closePort();
       return false;
    }

    // read from sensor pipe, tsm would have changed from SENSOR_NOTFOUND if it's active
    if ( ! read(fdPipe[PIPE_SENSOR], tsm, sizeof(CQCNUSBSensor)) ) {
    }

    if (tsm->eSensor == SENSOR_NOTFOUND) {
       tsmState->alPID[PID_QCN] = 0;
       tsmState->bStop = true; // this line and the previous line will "turn off" (pause) the qcnusb "service"
       fprintf(stderr, "No Mac USB Sensor found\n");
       closePort();
       return false;  // it really wasn't found!
    }

    if (tsm->eSensor != SENSOR_NOTFOUND) {
          fprintf(stdout, "Mac USB Sensor found\n");
    }

    // if made it here, we have a sensor of type tsm->eSensor
    setType(tsm->eSensor);  // this returns the actual sensor i.e. JoyWarrior F28
    setPort(getTypeEnum()); // set > -1 so we know we have a sensor
    setSingleSampleDT(false);  // note the usb sensor just requires 1 sample per dt, hardware does the rest

    fprintf(stdout, "Using the QCN Generic USB Driver for the Mac (qcnusb)\n");

    return (bool)(getTypeEnum() != SENSOR_NOTFOUND);
}

