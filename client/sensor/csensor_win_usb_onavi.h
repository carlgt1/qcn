#ifndef _CSENSOR_WIN_USB_ONAVI01_H_
#define _CSENSOR_WIN_USB_ONAVI01_H_

/*
 *  csensor-win-usb-onavi01.h
 *  qcn
 *
 *  Created by Carl Christensen on 10/10/2009.
 *  Copyright 2009 Stanford University
 *
 * This file contains the declarations for the CSensor-derived class for Windows USB JoyWarrior accelerometer
 */

#include <stdio.h>
#include <windows.h>

#include "main.h"
using namespace std;


// this is the Mac tty device for the ONavi-1 Mac kernel extension driver
#define STR_USB_ONAVI01 "XR21V1410"
#define FLOAT_ONAVI_FACTOR  7.629394531250e-05f

// Struct used when enumerating the available serial ports
// Holds information about an individual serial port.
struct SSerInfo {
    SSerInfo() : bUsbDevice(FALSE) {}
	std::string strDevPath;          // Device path for use with CreateFile()
	std::string strPortName;         // Simple name (i.e. COM1)
	std::string strFriendlyName;     // Full name to be displayed to a user
    bool bUsbDevice;             // Provided through a USB connection?
	std::string strPortDesc;         // friendly name without the COMx
	int iCOM; // Windows COM Port used
};

// this is the Windows implementation of the sensor - IBM/Lenovo Thinkpad, HP, USB Stick
class CSensorWinUSBONavi  : public CSensor
{
   private:
	  // usb stick stuff
	  HANDLE	 m_hcom;
	  SSerInfo   m_si;
     unsigned short m_usBitSensor;

      // two usb-specific methods, one for init, one for reading data
      void GetCapabilities(HANDLE handle);
      virtual bool read_xyz(float& x1, float& y1, float& z1);  

// Routine for enumerating the available serial ports. Throws a CString on
// failure, describing the error that occurred. If bIgnoreBusyPorts is TRUE,
// ports that can't be opened for read/write access are not included.
	  bool SearchONaviSerialPort(SSerInfo& si, const bool bIgnoreBusyPorts=true);

   public:
      CSensorWinUSBONavi();
      virtual ~CSensorWinUSBONavi();

     virtual void closePort(); // closes the port if open
     virtual bool detect();   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found
};

#endif

