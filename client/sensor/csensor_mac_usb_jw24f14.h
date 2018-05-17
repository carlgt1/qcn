#ifndef _CSENSOR_MAC_USB_JW24F14_H_
#define _CSENSOR_MAC_USB_JW24F14_H_

/*
 *  csensor-mac-usb-jw24f14.h
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * This file contains the definition of the Mac JoyWarrior 24F14 USB Sensor class
 */

#include "main.h"
using namespace std;

#include <stdio.h>

/* CMC Note: the USB add/remove logic doesn't seem to be working
void global_JoyWarriorAddedOrRemoved(void *refCon, io_iterator_t iterator);
void global_updateDeviceState();
*/


// this is the Mac implementation for the JoyWarrior sensor, used for QCNLive as well as the Mac service program qcnmacusb under BOINC
class CSensorMacUSBJW24F14  : public CSensor
{
   private:
      // Mac IO Utilities vars for detecting and using the USB JoyWarrior
      IOHIDDeviceInterface122** m_USBDevHandle[2];
      //pRecDevice m_prdJW24F14;
      //pRecElement m_prelJW24F14[3];
      bool m_bFoundJW;
      CFMutableArrayRef m_maDeviceRef;

      virtual bool read_xyz(float& x1, float& y1, float& z1);  
	
      struct cookie_struct m_cookies;

      bool m_bDevHandleOpen;     // boolean to denote if the DevHandle is open

      CFMutableDictionaryRef SetUpHIDMatchingDictionary (int inVendorID, int inDeviceID);
      io_iterator_t FindHIDDevices (const mach_port_t masterPort, int inVendorID, int inDeviceID);
      IOHIDDeviceInterface122** CreateHIDDeviceInterface (io_object_t hidDevice);
      CFMutableArrayRef DiscoverHIDInterfaces (int vendorID, int deviceID);
      bool getHIDCookies(IOHIDDeviceInterface122** handle, cookie_struct_t cookies);

      bool SetQCNState();
	
      bool openDevHandle();    // open the DevHandle for "joystick" access via HID
      bool closeDevHandle();   // close the handle

      void closeHandles();

	// utility functions ported from codemercs
	//int JWDisableCommandMode24F14 (IOHIDDeviceInterface122 **hidInterface);
	//int JWEnableCommandMode24F14 (IOHIDDeviceInterface122 **hidInterface);

        //void OpenImage(IOHIDDeviceInterface122 **hidInterface, bool bOpen);
	//void Mode(IOHIDDeviceInterface122 **hidInterface, bool bOpen);
	
	//bool JWReadByteFromAddress24F14 (IOHIDDeviceInterface122 **hidInterface, UInt8 inAddress, UInt8 *result);
	//bool JWWriteByteToAddress24F14 (IOHIDDeviceInterface122 **hidInterface, UInt8 cmd, UInt8 inAddress, UInt8 inData);
	
	//SInt16 JWMergeAxisBytes24F14 (UInt8 inLSB, UInt8 inMSB);
	//SInt16 JWMergeOffsetBytes24F14 (UInt8 inLSB, UInt8 inMSB);
	
	//void JWDiffMsbLsb24F14 (UInt16 value, UInt8 *inLSB, UInt8 *inMSB);


      bool QCNReadSensor(IOHIDDeviceInterface122** interface, int& iRange, int& iBandwidth);
      bool QCNWriteSensor(IOHIDDeviceInterface122** interface, const int& iRange, const int& iBandwidth);
	

   public:
      CSensorMacUSBJW24F14();
      virtual ~CSensorMacUSBJW24F14();

      virtual bool detect();    // this detects the Mac USB sensor
	
      virtual void closePort(); // closes the port if open
};

#endif

