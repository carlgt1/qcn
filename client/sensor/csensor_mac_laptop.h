#ifndef _CSENSOR_MAC_LAPTOP_H_
#define _CSENSOR_MAC_LAPTOP_H_

/*
 *  csensor-mac-laptop.h
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * Definition of the QCN Mac Laptop sensor class derived from CSensor
 */

#include "main.h"
using namespace std;

#include <stdio.h>

#include <mach/mach_port.h>
#include <CoreFoundation/CoreFoundation.h>

// data structures for sensor information

   // struct for PowerPPC Mac data
   struct stDataMacPPC {
     int8_t x;
     int8_t y;
     int8_t z;
     int8_t pad[57];
   };

   // struct for Intel Mac data
   struct stDataMacIntel {
    int16_t x;
    int16_t y;
    int16_t z;
    int8_t pad[34];
   };

class CSensorMacLaptop : public CSensor
{
   private:
   
      int m_iKernel; 
      int m_iStruct;

      // initialize the appropriate sensor
      void init_ppc(const int iType);  // G4 powerbooks & ibooks
      void init_intel(const int iType);  // Intel macbook pro
 
      // read sensor data -- this is private as only used by the (public) mean_xyz function
      virtual bool read_xyz(float& x1, float& y1, float& z1);  

   public:
      CSensorMacLaptop();
      virtual ~CSensorMacLaptop();

     virtual void closePort(); // closes the port if open
     virtual bool detect();   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found
};

#endif
