/*
 *  csensor_mac_usb_generic.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 09/18/2008.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for generic Mac USB driver interface to the qcnusb program
 */

#include "main.h"
#include "csensor_mac_usb_generic.h"
#ifndef _WIN32
#include <fcntl.h>
#endif

CSensorMacUSBGeneric::CSensorMacUSBGeneric()
  : CSensor()
{
    tsm = NULL; 
    tsmState = NULL; 
    dTimeActive = dTimeCheck = dTimeLast = 0.0f;
    fdPipe[PIPE_STATE] = fdPipe[PIPE_SENSOR] = -1;
}

CSensorMacUSBGeneric::~CSensorMacUSBGeneric()
{
  closePort();
}

bool CSensorMacUSBGeneric::writePipeState(const int iSeconds, const int iMicroSeconds)
{
   if (fdPipe[PIPE_STATE] == -1) return false;

   struct timeval tvTimeout; // timeout value for the select
   tvTimeout.tv_sec = iSeconds;
   tvTimeout.tv_usec = 0;

   FDSET_GROUP fdsCopy; // FDSET_GROUP is a struct taken from boinc/lib/network.h
   FD_COPY(&(fdsWatch.read_fds), &(fdsCopy.read_fds));
   FD_COPY(&(fdsWatch.write_fds), &(fdsCopy.write_fds));
   FD_COPY(&(fdsWatch.exc_fds), &(fdsCopy.exc_fds));
   fdsCopy.max_fd = fdsWatch.max_fd;

   // write the results to the sensor pipe, i.e. if sensor was found or not
   if (FD_ISSET(fdPipe[PIPE_STATE], &fdsWatch.write_fds)) {
     if (select(fdsCopy.max_fd+1, NULL, &(fdsCopy.write_fds), NULL,
       iSeconds > 0 || iMicroSeconds > 0 ? &tvTimeout : NULL) < 0) { // check the write fs (PIPE_STATE)
          //fprintf(stderr, "writePipeState Level 0\n");
          return false;
     }
     // which file descriptors are ready to be read from?
     if (FD_ISSET(fdPipe[PIPE_STATE], &fdsCopy.write_fds)) {
         // file is ready to be written to
         // now write this sensor information
         int retval = write(fdPipe[PIPE_STATE], tsm, sizeof(CQCNUSBState));
         if (retval < 0) { // pipe error
            //fprintf(stderr, "writePipeState Level 1\n");
            return false;
         }
         else if (!retval) { //pipe has been closed
            //fprintf(stderr, "writePipeState Level 2\n");
            return false;
         }
         //else { // bytes were written to the pipe OK
         //}
     }
     else { // something wrong, just return
         //fprintf(stderr, "writePipeState Level 3\n");
         return false;
     }
   }
   return true;  // must have written OK
}

bool CSensorMacUSBGeneric::readPipeSensor(const int iSeconds, const int iMicroSeconds)
{
   if (fdPipe[PIPE_SENSOR] == -1) return false;

   struct timeval tvTimeout; // timeout value for the select
   tvTimeout.tv_sec = iSeconds;
   tvTimeout.tv_usec = iMicroSeconds;

   FDSET_GROUP fdsCopy; // FDSET_GROUP is a struct taken from boinc/lib/network.h
   FD_COPY(&(fdsWatch.read_fds), &(fdsCopy.read_fds));
   FD_COPY(&(fdsWatch.write_fds), &(fdsCopy.write_fds));
   FD_COPY(&(fdsWatch.exc_fds), &(fdsCopy.exc_fds));
   fdsCopy.max_fd = fdsWatch.max_fd;

   // write the results to the sensor pipe, i.e. if sensor was found or not
   if (FD_ISSET(fdPipe[PIPE_SENSOR], &fdsWatch.read_fds)) {
     if (select(fdsCopy.max_fd+1, &(fdsCopy.read_fds), NULL, NULL,
       iSeconds > 0 || iMicroSeconds > 0 ? &tvTimeout : NULL) < 0) { // check the write fs (PIPE_STATE)
          //fprintf(stderr, "readPipeSensor Level 0\n");
          return false;
     }

     // which file descriptors are ready to be read from?
     if (FD_ISSET(fdPipe[PIPE_SENSOR], &fdsCopy.read_fds)) {
        //file is ready to be read from
        int retval = read(fdPipe[PIPE_SENSOR], tsm, sizeof(CQCNUSBSensor));
        if (retval < 0) { // pipe error
           //fprintf(stderr, "readPipeSensor Level 1\n");
           return false;
        }
        else if (!retval) { //pipe has been closed
           //remove the closed pipe from the set
           //fprintf(stderr, "readPipeSensor Level 2\n");
           return false;
        }
     }
     else { // something wrong, just return
       //fprintf(stderr, "readPipeSensor Level 3\n");
       return false;
     }
   }
   return true;  // must have read OK
}

void CSensorMacUSBGeneric::closePort()
{
    fflush(stderr);
    fflush(stdout);

    if (tsmState) {
        tsmState->bStop = true;
        tsmState->alPID[PID_QCN] = 0L;
        try {
           writePipeState(1);
        }
        catch(...) {
        }
        delete tsmState;
        tsmState = NULL;
    }
    if (tsm) {
        delete tsm;
        tsm = NULL;
    }
    if (fdPipe[PIPE_STATE] != -1)  close(fdPipe[PIPE_STATE]);
    if (fdPipe[PIPE_SENSOR] != -1) close(fdPipe[PIPE_SENSOR]);
    fdPipe[PIPE_STATE] = fdPipe[PIPE_SENSOR] = -1;
}

inline bool CSensorMacUSBGeneric::mean_xyz()
{
   if (qcn_main::g_iStop || !sm) {
       throw EXCEPTION_SHUTDOWN;   // see if we're shutting down, if so throw an exception which gets caught in the sensor_thread
   }

   if (!tsm || !tsmState || !sm || fdPipe[PIPE_STATE] == -1 || fdPipe[PIPE_SENSOR] == -1)  {
      fprintf(stderr, "CSensorMacUSBGeneric::mean_xyz: Invalid memory pointer or pipe!\n");
      return false;
   }

   if (!readPipeSensor(1)) {
      fprintf(stderr, "CSensorMacUSBGeneric::Read error on sensor pipe from QCN client\n");
      closePort();
      return false;
   }

   dTimeCheck = tsm->t0check;
   tsm->bReading = true;

   /* CMC - this is causing problems, errs where resets device hundreds of times -- maybe try a 'throw EXCEPTION_SHUTDOWN' for a better reset?
   // check the tsm->eSensor to make sure USB device wasn't yanked out
   if (tsm->eSensor == SENSOR_NOTFOUND || !tsm->bSensorFound)  {
      fprintf(stderr, "CSensorMacUSBGeneric::mean_xyz - USB device not found, possibly unplugged?  %d %d\n", tsm->eSensor, tsm->bSensorFound);
      closePort();
      return false;
   }
   */

   sm->x0[sm->lOffset] = tsm->x0 * sm->fCorrectionFactor;
   sm->y0[sm->lOffset] = tsm->y0 * sm->fCorrectionFactor;
   sm->z0[sm->lOffset] = tsm->z0 * sm->fCorrectionFactor;
   sm->t0[sm->lOffset] = tsm->t0;
   //if (sm->dt == 0.0f || sm->dt < DT || sm->dt > DT_SLOW) sm->dt = tsm->dt; // dt should only be set if it's 0.0
   sm->t0active = tsm->t0active;
   sm->t0check  = tsm->t0check;

   sm->lSampleSize = tsm->lSampleSize;
   sm->ullSampleTotal = tsm->ullSampleTotal;
   sm->ullSampleCount = tsm->ullSampleCount;
   sm->fRealDT = tsm->fRealDT;
   tsm->bReading = false;

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
            tsm->bSensorFound = false; // init values which will be ready from sensor pipe below
            writePipeState(1);
            return false;   // if we're not debugging, this is a serious run-time problem, so reset time & counters & try again
        }
   }

   return true;

}

inline bool CSensorMacUSBGeneric::read_xyz(float& x1, float& y1, float& z1)
{  // there really is no read_xyz for the proc, is there?   just use mean_xyz
   if (!tsm || !sm || !mean_xyz()) return false;
   x1 = tsm->x0;// * sm->fCorrectionFactor;
   y1 = tsm->y0;// * sm->fCorrectionFactor;
   z1 = tsm->z0;// * sm->fCorrectionFactor;
   return true;
}

bool CSensorMacUSBGeneric::detect()
{
   // first try and discover the HID interface (JoyWarrior)
   int iWait = 0; 
   bool bRetVal = false;

   setType(SENSOR_NOTFOUND);  // initialize to no sensor until detected below

   // try and "talk" to the qcnusb "service" program - if running it has setup shared memory & we use semaphores for synchronization
    fdPipe[PIPE_SENSOR] = open(QCN_USB_SENSOR, O_RDONLY);                // sensor is blocking
    if (fdPipe[PIPE_SENSOR] == -1) {
        fprintf(stderr, "No Mac USB driver found\n"); // make a less scary msg than below!
        //fprintf(stderr, "Error %d opening pipe %d, try later, or reboot!\n", errno, fdPipe[PIPE_SENSOR]);
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
    iWait = 1;
    fdPipe[PIPE_STATE]  = open(QCN_USB_STATE,  O_WRONLY | O_NONBLOCK);
    while (fdPipe[PIPE_STATE] == -1 && iWait++ < 10) {
      usleep(100000); // .1 seconds, so wait a second total, that's 10 retries, should be able to open by then!
      fdPipe[PIPE_STATE]  = open(QCN_USB_STATE,  O_WRONLY | O_NONBLOCK);
      if (qcn_main::g_iStop) {
        fprintf(stderr, "qcn generic client - stop request 1 received iWait=%f\n", .10f * (float) iWait);
        closePort();
        return false;
      }
    }
    if (fdPipe[PIPE_STATE] == -1) {
        fprintf(stderr, "Error %d opening pipe %d, try later, or reboot!\n", errno, fdPipe[PIPE_STATE]);
        closePort();
        return false;
    }

    // now setup fds'
    fdsWatch.zero();
    FD_SET(fdPipe[PIPE_SENSOR], &(fdsWatch.read_fds));
    FD_SET(fdPipe[PIPE_STATE],  &(fdsWatch.write_fds));
    fdsWatch.max_fd = fdPipe[PIPE_STATE] > fdPipe[PIPE_SENSOR] ? fdPipe[PIPE_STATE] : fdPipe[PIPE_SENSOR];

    // to start monitoring, set the process ID and set bStop to false
    tsmState->alPID[PID_QCN] = getpid();  // set the process ID of this routine
    tsmState->bStop = false;

    tsm->bSensorFound = false; // init values which will be ready from sensor pipe below
    tsm->eSensor      = SENSOR_NOTFOUND; 

    if (!writePipeState(1)) {
        fprintf(stderr, "The qcnusb service does not appear to be running, try a reboot!\n");
        closePort();
        return false;
    }

    // read from sensor pipe, tsm would have changed from SENSOR_NOTFOUND if it's active
    iWait = 1;
    bRetVal = readPipeSensor(0, 100000);
    while (!tsm->bSensorFound && ++iWait<20) { // retry for up to 2 seconds to make sure, although if found in first readPipe this loop skips....
       if (qcn_main::g_iStop) {
         fprintf(stderr, "qcn generic client - stop request 2 received iWait=%f\n", .10f * (float) iWait);
         closePort();
         return false;
       }
       bRetVal = readPipeSensor(0, 100000);
    }
    if (!tsm->bSensorFound || tsm->eSensor == SENSOR_NOTFOUND)  { // didn't get anything 
       fprintf(stderr, "No sensor found from qcnusb service within %f seconds: bRetVal=%s bSensorFound=%s  eSensor=%d\n",
          .10f * (float) iWait, 
          bRetVal ? "true" : "false", 
          tsm->bSensorFound ? "true" : "false", 
          (int) tsm->eSensor);
       closePort();
       return false;
    }

    fprintf(stderr, "Sensor type %d found from qcnusb service in %f seconds\n",
          (int) tsm->eSensor, .10f * (float) iWait
    );
   
    // if made it here, we have a sensor of type tsm->eSensor
    setType(tsm->eSensor);  // this returns the actual sensor i.e. JoyWarrior F28
    setPort(getTypeEnum()); // set > -1 so we know we have a sensor
    setSingleSampleDT(true);  // note the usb sensor the sampling is done in the qcnusb driver, with pipes synchronizing the communication

    fprintf(stdout, "Using the QCN Generic USB Driver for the Mac (qcnusb)\nSensor Type # %d - %s detected in %f seconds\n", 
			getTypeEnum(), getTypeStr(), (float) iWait * .1f
    );

    // initialize time checks
    dTimeActive = dTimeCheck = dTimeLast = 0.0f;

    return (bool)(getTypeEnum() != SENSOR_NOTFOUND);
}

