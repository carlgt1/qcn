#ifndef _CSENSOR_MAC_USB_JW_H_
#define _CSENSOR_MAC_USB_JW_H_

/*
 *  csensor-mac-usb-jw.h
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * This file contains the definition of the Mac JoyWarrior USB Sensor class
 */

#include "main.h"
using namespace std;

#include <stdio.h>


//#include "DiscoverHIDInterface.h" 
//#include "JoyWarrior24F8.h"
//#include "../mac_hid_lib/HID_Utilities_External.h"   // this links to the Apple HID library in qcn/client/mac_hid_lib


/* CMC Note: the USB add/remove logic doesn't seem to be working
void global_JoyWarriorAddedOrRemoved(void *refCon, io_iterator_t iterator);
void global_updateDeviceState();
*/

// this is the Mac implementation for the JoyWarrior sensor, used for QCNLive as well as the Mac service program qcnmacusb under BOINC
class CSensorMacUSBJW  : public CSensor
{
   private:
      // Mac IO Utilities vars for detecting and using the USB JoyWarrior
      IOHIDDeviceInterface122** m_USBDevHandle[2];
      //pRecDevice m_prdJW;
      //pRecElement m_prelJW[3];
      bool m_bFoundJW;
      CFMutableArrayRef m_maDeviceRef;

      virtual bool read_xyz(float& x1, float& y1, float& z1);  
	
      struct cookie_struct m_cookies;

      bool m_bDevHandleOpen;     // boolean to denote if the DevHandle is open

/*
      void printElement(const int level, const pRecElement pelem);
      bool walkElement(const int level, const pRecElement pretmp);
*/
	  bool getHIDCookies(IOHIDDeviceInterface122** handle, cookie_struct_t cookies);

      CFMutableDictionaryRef SetUpHIDMatchingDictionary (int inVendorID, int inDeviceID);
      io_iterator_t FindHIDDevices (const mach_port_t masterPort, int inVendorID, int inDeviceID);
      IOHIDDeviceInterface122** CreateHIDDeviceInterface (io_object_t hidDevice);
      CFMutableArrayRef DiscoverHIDInterfaces (int vendorID, int deviceID);

      IOReturn DisableCommandMode(IOHIDDeviceInterface122** hidInterface);
      IOReturn EnableCommandMode (IOHIDDeviceInterface122** hidInterface);
      IOReturn ReadByteFromAddress(IOHIDDeviceInterface122** hidInterface, const UInt8 inAddress, UInt8 *result); //, bool bJoystick = false);

      bool SetQCNState();
	
      bool ReadData(IOHIDDeviceInterface122** hidInterface, const UInt8 addr, UInt8* cTemp, const char* strCallProc = NULL);
      bool WriteData(IOHIDDeviceInterface122** hidInterface, const UInt8 cmd, const UInt8 addr, const UInt8 data, const char* strCallProc = NULL);

      bool openDevHandle();    // open the DevHandle for "joystick" access via HID
      bool closeDevHandle();   // close the handle

      void closeHandles();


	void JWDiffMsbLsb(UInt16 value, UInt8 *inLSB, UInt8 *inMSB);
	SInt16 JWMergeOffsetBytes(UInt8 inLSB, UInt8 inMSB);
	SInt16 JWMergeAxisBytes(UInt8 inLSB, UInt8 inMSB);
	int JWWriteByteToAddress(IOHIDDeviceInterface122 **hidInterface, UInt8 inAddress, UInt8 inData);
	int JWReadByteFromAddress(IOHIDDeviceInterface122 **hidInterface, UInt8 inAddress, UInt8 *result);
	int JWEnableCommandMode (IOHIDDeviceInterface122 **hidInterface);
	int JWDisableCommandMode (IOHIDDeviceInterface122 **hidInterface);

   public:
      CSensorMacUSBJW(enum e_sensor eSensorType = SENSOR_USB_JW24F8);
      virtual ~CSensorMacUSBJW();

      virtual bool detect();    // this detects the Mac USB sensor
      virtual void closePort(); // closes the port if open
};

#endif

