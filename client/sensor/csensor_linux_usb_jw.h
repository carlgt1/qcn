#ifndef _CSENSOR_LINUX_USB_JW_H_
#define _CSENSOR_LINUX_USB_JW_H_

/*
 *  csensor_linux_usb_jw.h
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * This file contains the definition of the Linux JoyWarrior USB Sensor class
 */

#include "main.h"
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

// this is the Linux implementation for the JoyWarrior sensor, used for QCNLive as well as the Mac service program qcnmacusb under BOINC
class CSensorLinuxUSBJW  : public CSensor
{
   private:
      // vars lifted from the codemercs.com JW24F8 Linux example
      int m_fdJoy, *m_piAxes, m_iNumAxes, m_iNumButtons;
      char *m_strButton, m_strJoystick[80];
      struct js_event m_js;

      virtual bool read_xyz(float& x1, float& y1, float& z1);  

      bool testJoystick();  // tests that it really is the JoyWarrior & sets to "raw data" mode

   public:
      CSensorLinuxUSBJW();
      virtual ~CSensorLinuxUSBJW();

      virtual bool detect();    // this detects the Mac USB sensor
      virtual void closePort(); // closes the port if open

};

#endif
