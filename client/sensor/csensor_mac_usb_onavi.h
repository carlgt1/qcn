#ifndef _CSENSOR_MAC_USB_ONAVI01_H_
#define _CSENSOR_MAC_USB_ONAVI01_H_

/*
 *  csensor-mac-usb-onavi01.h
 *  qcn
 *
 *  Created by Carl Christensen on 10/10/2009.
 *  Copyright 2009 Stanford University
 *
 * This file contains the declarations for the CSensor-derived class for Windows USB JoyWarrior accelerometer
 */

#include <stdio.h>

#include "main.h"
using namespace std;


// this is the Mac tty device for the ONavi-1 Mac kernel extension driver
#define STR_USB_ONAVI01     "/dev/cu.xrusbmodem*"
#define STR_USB_ONAVI02     "/dev/cu.usbmodem*"
#define FLOAT_ONAVI_FACTOR  7.629394531250e-05f

// this is the Windows implementation of the sensor - IBM/Lenovo Thinkpad, HP, USB Stick
class CSensorMacUSBONavi  : public CSensor
{
   private:
    int m_fd;
     unsigned short m_usBitSensor;
	
      virtual bool read_xyz(float& x1, float& y1, float& z1);  

   public:
      CSensorMacUSBONavi();
      virtual ~CSensorMacUSBONavi();

     virtual void closePort(); // closes the port if open
     virtual bool detect();   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found
};

#endif

