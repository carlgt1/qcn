#ifndef _CSENSOR_WIN_LAPTOP_THINKPAD_H_
#define _CSENSOR_WIN_LAPTOP_THINKPAD_H_

/*
 *  csensor-win-laptop-thinkpad.h
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * This file contains the declarations for the CSensor-derived Windows Lenovo Thinkpad accelerometer
 */

#include <stdio.h>
#include <windows.h>

#include "main.h"
using namespace std;

// Thinkpad specific stuff 
// CMC note:  Thinkpad tips per Ben Suter -- how to access Thinkpad DLL
// x = 499, y = 514 seems to be disabled setting?

typedef struct
{
	int status;
	unsigned short x;
	unsigned short y;
	long junk[4];  // pad the struct, as get buffer overruns without it (i.e. if just status/x/y)
} ThinkpadSensorData;

// DLL function signature for Thinkpad DLL -- note the use of __stdcall so Windows does the cleanup (as opposed to __cdecl)
// Note -- the Thinkpad DLL accesses a service running on the laptop --
// if this service isn't running the DLL function access will work but the sensor data values never change
typedef int (__stdcall *ThinkpadImportFunction)(ThinkpadSensorData* psd); 

// this is the Windows implementation of the sensor - IBM/Lenovo Thinkpad, HP, USB Stick
class CSensorWinThinkpad  : public CSensor
{
   private:   
      static const char m_cstrDLL[];

       // private member vars
      HMODULE m_WinDLLHandle;

	  ThinkpadImportFunction m_getDataThinkpad;
	  ThinkpadSensorData m_ThinkpadData;

      virtual bool read_xyz(float& x1, float& y1, float& z1);  

   public:
      CSensorWinThinkpad();
      virtual ~CSensorWinThinkpad();

     virtual void closePort(); // closes the port if open
     virtual bool detect();   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found
};

#endif

