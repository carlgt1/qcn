#ifndef _CSENSOR_WIN_LAPTOP_HP_H_
#define _CSENSOR_WIN_LAPTOP_HP_H_

/*
 *  csensor-win-laptop-hp.h
 *  qcn
 *
 *  Created by Carl Christensen on 10/05/2008.
 *  Copyright 2007 Stanford University
 *
 * This file contains the declarations for the CSensor-derived Windows HP laptop accelerometer
 *
 * NB: much of the implementatoin done by Rafal Ostenak  rostanek@op.pl
 *     since HP seems to have changed their security model from my old version
 */

#include <stdio.h>
#include <windows.h>
#include <setupapi.h>
#include "csensor.h"

using namespace std;

// HP specific stuff 
// function prototypes and ordinals of interest
// Note -- the HP DLL accesses a service running on the laptop --
// if this service isn't running the DLL function access will work but the sensor data values never change

typedef __declspec( dllimport ) unsigned long (__stdcall *IsSoftwareEnabled)(void *, unsigned char *);
typedef __declspec( dllimport ) unsigned long (__stdcall *GetRealTimeXYZ)(void *, short *, _OVERLAPPED *);

typedef __declspec( dllimport ) unsigned char (__stdcall *stdcall_FindAccelerometerDevice)(void * *);
typedef __declspec( dllimport ) unsigned char (__cdecl *cdecl_FindAccelerometerDevice)(void * *);

typedef stdcall_FindAccelerometerDevice FindAccelerometerDev; // depends on operating system and architecture


// this is the Windows implementation of the sensor - HP
class CSensorWinHP  : public CSensor
{
   private: 
        short m_xyz[3];
        HMODULE m_hLibrary;
        HANDLE m_hDevice;
        FindAccelerometerDev m_findAccelerometerDev;
        GetRealTimeXYZ m_getRealTimeXYZ;
		OVERLAPPED m_overlapped;

        static const char* m_cstrDLL;

        bool LoadLibrary();
        void Init();

   public:
      CSensorWinHP();
      virtual ~CSensorWinHP();

      virtual void closePort(); // closes the port if open
      virtual bool detect();   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found

      // note that x/y/z should be scaled to +/- 2g, return values as +/- 2.0f*EARTH_G (in define.h: 9.78033 m/s^2)
      virtual bool read_xyz(float& x1, float& y1, float& z1);  
};

#endif  // _CSENSOR_WIN_LAPTOP_HP_H_
