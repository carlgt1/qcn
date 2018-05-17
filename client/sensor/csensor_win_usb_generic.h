#ifndef _CSENSOR_WIN_USB_GENERIC_H_
#define _CSENSOR_WIN_USB_GENERIC_H_

/*
 *  csensor_win_usb_generic.h
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * This file contains the definition of the Mac USB class for accessing the qcnusb service program
 */

#include "main.h"
#include "shmem.h"  // boinc shmem stuff

#include "qcn_shmem_usb.h"

using namespace std;

#include <stdio.h>

// this is the Mac implemention of a "USB driver" program that will run at login, so that BOINC can access it via a low-privilege account
class CSensorMacUSBGeneric  : public CSensor
{
   private:
      CQCNUSBSensor* tsm;
      CQCNUSBState* tsmState;
      double dTimeActive;
      double dTimeCheck;
      double dTimeLast;

      int fdPipe[2]; // first is PIPE_STATE to write state data, second is PIPE_SENSOR to read sensor data

      virtual bool read_xyz(float& x1, float& y1, float& z1);  
      bool writePipeState(bool bClosePort); // bClosePort is true if we want to close the accelerometer port on a pipe error

   public:
      CSensorMacUSBGeneric();
      virtual ~CSensorMacUSBGeneric();

      virtual bool detect();    // this detects the Mac USB sensor
      virtual void closePort(); // closes the port if open

      virtual bool mean_xyz();   // mean sensor data, implemented here but can be overridden
};

#endif

