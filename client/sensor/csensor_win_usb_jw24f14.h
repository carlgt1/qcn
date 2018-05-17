#ifndef _CSENSOR_WIN_USB_JW24F14_H_
#define _CSENSOR_WIN_USB_JW24F14_H_

/*
 *  csensor-win-usb-jw.h
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * This file contains the declarations for the CSensor-derived class for Windows USB JoyWarrior accelerometer
 */

#include <stdio.h>
#include <windows.h>

#include "main.h"
using namespace std;

// usb stick includes (JoyWarrior codemercs) -- all must be extern "C"
extern "C" { 
#include "hidsdi.h" 
#include "setupapi.h"
#include "hidpi.h"
}

// this is the Windows implementation of the sensor - IBM/Lenovo Thinkpad, HP, USB Stick
class CSensorWinUSBJW24F14  : public CSensor
{
   private:
	  // usb stick stuff
	  HIDP_CAPS	 m_USBCapabilities;
	  HANDLE     m_USBHandle;
	  HANDLE	 m_USBDevHandle[2];

      // two usb-specific methods, one for init, one for reading data
      void GetCapabilities(HANDLE handle);

      int SetupJoystick();
      bool SetQCNState(); // persistently set accelerometer to 50Hz and +/- 2g

      virtual bool read_xyz(float& x1, float& y1, float& z1);  

	  bool JWEnableCommandMode24F14(HANDLE handle);
	  bool JWDisableCommandMode24F14(HANDLE handle);
	  bool QCNReadSensor(int& iRange, int& iBandwidth);
	  bool QCNWriteSensor(const int& iRange, const int& iBandwidth);

      float ReadFloatData(unsigned char addr_LSB, unsigned char addr_MSB, char axe);
      unsigned char ReadData(HANDLE handle, unsigned char addr, bool bFlush = true);
      bool WriteData(HANDLE handle, unsigned char addr, unsigned char cmd, bool bCommandMode = false, bool bFlush = true);
	  int CalcMsbLsb(unsigned char lsb, unsigned char msb);

   public:
      CSensorWinUSBJW24F14();
      virtual ~CSensorWinUSBJW24F14();

     virtual void closePort(); // closes the port if open
     virtual bool detect();   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found
};

#endif

