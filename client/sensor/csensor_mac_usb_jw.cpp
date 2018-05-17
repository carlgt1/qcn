/*
 *  csensor_mac_usb_jw.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for sensor classes
 */

#include "main.h"
#include "csensor_mac_usb_jw.h"

// making sense of IOReturn (IOKit) error codes:  
// http://developer.apple.com/qa/qa2001/qa1075.html

// IOReturn codes in:
// /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/IOKit.framework/Headers/IOReturn.h

IOHIDDeviceInterface122** CSensorMacUSBJW::CreateHIDDeviceInterface(io_object_t hidDevice)
{
    io_name_t             className;
    IOCFPlugInInterface** plugInInterface = NULL;
    HRESULT               plugInResult = S_OK;
    SInt32                score = 0;
    IOReturn              ioReturnValue = kIOReturnSuccess;
    IOHIDDeviceInterface122** pphidDeviceInterface = NULL;
	
    ioReturnValue = IOObjectGetClass(hidDevice, className);
    if (ioReturnValue != kIOReturnSuccess) {
        fprintf(stderr, "CreateHIDDeviceInterface: Failed to get class name.");
        return NULL;
    }
    ioReturnValue = IOCreatePlugInInterfaceForService (
         hidDevice, 
         kIOHIDDeviceUserClientTypeID,
         kIOCFPlugInInterfaceID, 
	 &plugInInterface, 
	 &score
    );
    if (ioReturnValue == kIOReturnSuccess) {
        // Call a method of the intermediate plug-in to create the device interface
        plugInResult = (*plugInInterface)->QueryInterface (plugInInterface,
                                CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID),
				(LPVOID *) &pphidDeviceInterface);
        if (plugInResult != S_OK) {
            fprintf(stderr, "CreateHIDDeviceInterface: Couldn't query HID class device interface from plugInInterface");
        }
        (*plugInInterface)->Release (plugInInterface);
    }
    else {
        fprintf(stderr, "CreateHIDDeviceInterface: Failed to create **plugInInterface via IOCreatePlugInInterfaceForService.");
        return NULL;
    }
    return pphidDeviceInterface;
}

CFMutableDictionaryRef CSensorMacUSBJW::SetUpHIDMatchingDictionary (int inVendorID, int inDeviceID)
{
    CFMutableDictionaryRef 	refHIDMatchDictionary = NULL;
	
    // Set up a matching dictionary to search I/O Registry by class name for all IOWarrior devices.
    refHIDMatchDictionary = IOServiceMatching (kIOHIDDeviceKey);
    if (refHIDMatchDictionary != NULL) {
		CFNumberRef numberRef;
				
		numberRef = CFNumberCreate (kCFAllocatorDefault, kCFNumberIntType, &inVendorID);
		CFDictionarySetValue (refHIDMatchDictionary, CFSTR (kIOHIDVendorIDKey), numberRef);
		CFRelease (numberRef);

		numberRef = CFNumberCreate (kCFAllocatorDefault, kCFNumberIntType, &inDeviceID);
		CFDictionarySetValue (refHIDMatchDictionary, CFSTR (kIOHIDProductIDKey), numberRef);
		CFRelease (numberRef);

    }
    else {
        fprintf(stderr, "Failed to get HID CFMutableDictionaryRef via IOServiceMatching.");
    }
    return refHIDMatchDictionary;
}

// Returns an iterator object, which can be used to iterate through all hid devices available on the machine. 
// You have to release the iterator after usage be calling IOObjectRelease (hidObjectIterator).
io_iterator_t CSensorMacUSBJW::FindHIDDevices (const mach_port_t masterPort, int inVendorID, int inDeviceID)
{
    CFMutableDictionaryRef	hidMatchDictionary = NULL;
    IOReturn				ioReturnValue = kIOReturnSuccess;
    io_iterator_t			hidObjectIterator;
	
    // Set up matching dictionary to search the I/O Registry for HID devices we are interested in. Dictionary reference is NULL if error.
    hidMatchDictionary = SetUpHIDMatchingDictionary (inVendorID, inDeviceID);
    if (NULL == hidMatchDictionary) {
        fprintf(stderr, "Couldn't create a matching dictionary.");
        return NULL;
    }
	
    // Now search I/O Registry for matching devices.
    ioReturnValue = IOServiceGetMatchingServices (masterPort, hidMatchDictionary, &hidObjectIterator);
    // If error, print message and hang (for debugging purposes).
    if ((ioReturnValue != kIOReturnSuccess) || (!hidObjectIterator)) {
        return NULL;
    }
	
    // IOServiceGetMatchingServices consumes a reference to the dictionary, so we don't need to release the dictionary ref.
    hidMatchDictionary = NULL;
	
    return hidObjectIterator;
}

CFMutableArrayRef CSensorMacUSBJW::DiscoverHIDInterfaces(int vendorID, int deviceID)
{
	mach_port_t    masterPort = NULL;
	io_iterator_t  hidObjectIterator = NULL;
	IOReturn       ioReturnValue;
	CFMutableArrayRef result = CFArrayCreateMutable(kCFAllocatorDefault,0,NULL);

	//ioReturnValue = IOMasterPort(bootstrap_port, &masterPort);
	ioReturnValue = IOMasterPort(MACH_PORT_NULL, &masterPort);
	if (!masterPort || ioReturnValue != kIOReturnSuccess) {
		fprintf(stderr, "DiscoverHIDInterfaces: Couldn't create a master I/O Kit Port.");
		return result;
	}
	hidObjectIterator = FindHIDDevices(masterPort, vendorID, deviceID); 
	if (hidObjectIterator) {
		io_object_t hidDevice = NULL;
		IOReturn    ioReturnValue = kIOReturnSuccess;
		
		while ((hidDevice = IOIteratorNext(hidObjectIterator)))
		{
			kern_return_t             err;
			CFMutableDictionaryRef    properties = 0;
			IOHIDDeviceInterface122** hidInterface;
			CFNumberRef               hidInterfaceRef;
			
			err = IORegistryEntryCreateCFProperties (hidDevice, &properties, kCFAllocatorDefault, kNilOptions);
			hidInterface = CreateHIDDeviceInterface (hidDevice);
			
			hidInterfaceRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberLongType, &hidInterface);
	
                    	CFArrayAppendValue(result,hidInterfaceRef);
			CFRelease (properties);
			ioReturnValue = IOObjectRelease(hidDevice);
            	}
	}
	IOObjectRelease (hidObjectIterator);
	mach_port_deallocate (mach_task_self(), masterPort);
	return result;
}


IOReturn CSensorMacUSBJW::DisableCommandMode (IOHIDDeviceInterface122** hidInterface)
{
        UInt8 writeBuffer[8];
        IOReturn ioReturnValue;

        memset(writeBuffer, 0x00, sizeof (writeBuffer));
        writeBuffer[0] = 0x00;

        ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 0, NULL, NULL, NULL);
        if (ioReturnValue != kIOReturnSuccess)  {
           fprintf(stderr, "DisableCommandMode: Could not write setReport on hid device interface\n");
        }
        return ioReturnValue;
}


IOReturn CSensorMacUSBJW::EnableCommandMode (IOHIDDeviceInterface122** hidInterface)
{

	UInt8	writeBuffer[8];
	IOReturn ioReturnValue;
	
	// enable Command mode
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x80;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 0, NULL, NULL, NULL);
        //if (ioReturnValue != kIOReturnSuccess) {
        //    fprintf(stderr, "EnableCommandMode: Could not write setReport on hid device interface\n");
        //} 
	return ioReturnValue;

}

IOReturn CSensorMacUSBJW::ReadByteFromAddress (IOHIDDeviceInterface122** hidInterface, const UInt8 inAddress, UInt8 *result) // , bool bJoystick)
{
	UInt8	 readBuffer[8];
	UInt8	 writeBuffer[8];
	IOReturn ioReturnValue;
	
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050
	uint32_t	 readDataSize;
#else
	UInt32   readDataSize; // may be a problem with Mac's 10.4 to 10.5+
#endif

	*result = 0x00;
	
	// open the interface
	//ioReturnValue = (*hidInterface)->open(hidInterface, kIOHIDOptionsTypeSeizeDevice);
	ioReturnValue = (*hidInterface)->open(hidInterface, kIOHIDOptionsTypeNone);
        if (ioReturnValue != kIOReturnSuccess) {
               fprintf(stderr, "ReadByteFromAddress: couldn't open interface 0x%x - err 0x%x\n",
                            (unsigned long) hidInterface, (unsigned long) ioReturnValue);
		return ioReturnValue;
	}
	
	if (kIOReturnSuccess != ( ioReturnValue = EnableCommandMode (hidInterface))) {
	   (*hidInterface)->close(hidInterface);
	    return ioReturnValue;
        }
	
	// enable command mode
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x82;
	writeBuffer[1] = 0x80 | inAddress;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 0, NULL, NULL, NULL);
        if (ioReturnValue != kIOReturnSuccess) {
           fprintf(stderr, "ReadByteFromAddress: Could not write setReport on hid device interface\n");
	   (*hidInterface)->close(hidInterface);
           return ioReturnValue;
        }  
	// read something from interface
	readDataSize = 8;
	ioReturnValue = (*hidInterface)->getReport (hidInterface, kIOHIDReportTypeInput,
		0, readBuffer, &readDataSize, 0, NULL, NULL, NULL);
        if (ioReturnValue != kIOReturnSuccess) {
           fprintf(stderr, "ReadByteFromAddress: Could not call getReport on hid device interface\n");
	   (*hidInterface)->close(hidInterface);
           return ioReturnValue;
	}
	*result = readBuffer[2] ;
	
	// disable command mode
	if (kIOReturnSuccess != ( ioReturnValue = DisableCommandMode (hidInterface))) {
	   (*hidInterface)->close(hidInterface);
           return ioReturnValue;
        }
	
	// close the interface
	ioReturnValue = (*hidInterface)->close(hidInterface);

	return ioReturnValue;

}


CSensorMacUSBJW::CSensorMacUSBJW(enum e_sensor eSensorType)
  : CSensor()
{
   m_USBDevHandle[0] = m_USBDevHandle[1] = NULL;
   m_bFoundJW = false;
   m_maDeviceRef = NULL;
   m_bDevHandleOpen = false;
   closeHandles();
}

CSensorMacUSBJW::~CSensorMacUSBJW()
{
  closePort();
}

void CSensorMacUSBJW::closePort()
{
  for (int i = 0; i < 2; i++) {
     if (m_USBDevHandle[i]) {
       try {
          // don't think we need the next line, just close & Release
          if (i==0) WriteData(m_USBDevHandle[0], 0x02, 0x00, 0x00, "closePort()::Free JW");
          (*m_USBDevHandle[i])->close(m_USBDevHandle[i]);
          (*m_USBDevHandle[i])->Release(m_USBDevHandle[i]);
          m_USBDevHandle[i] = NULL;
        }
        catch(...) {
            fprintf(stderr, "Could not close JoyWarrior USB port %d...\n", i);
        }
     }
  }

  if (m_maDeviceRef) {
     CFRelease(m_maDeviceRef);
     m_maDeviceRef = NULL;
  }

  closeHandles();

  if (getPort() > -1) { // nothing really left to close, as it's just the joystick #
    fprintf(stdout, "Joywarrior 24F8 closed!\n");
    fflush(stdout);
    setPort(-1);
  }

}

void CSensorMacUSBJW::closeHandles()
{
/*
      m_prdJW = NULL;
      m_prelJW[0] = NULL;
      m_prelJW[1] = NULL;
      m_prelJW[2] = NULL;
 */
      closeDevHandle();
      m_bFoundJW = false;
      m_USBDevHandle[0] = NULL;
      m_USBDevHandle[1] = NULL;
      m_maDeviceRef = NULL;
}

bool CSensorMacUSBJW::ReadData(IOHIDDeviceInterface122** hidInterface, const UInt8 addr, UInt8* cTemp, const char* strCallProc)
{
   *cTemp = 0x00;
   IOReturn ioReturnValue = ReadByteFromAddress(hidInterface, addr, cTemp);
   if (ioReturnValue != kIOReturnSuccess) {
       fprintf(stderr, "CSensorMacUSBJW::ReadData():: couldn't open interface 0x%x - ioRetVal=0x%x  (sys=0x%x subsys=0x%x code=0x%x) - proc %s\n", 
         (unsigned long) hidInterface, (unsigned long) ioReturnValue, 
         err_get_system(ioReturnValue), err_get_sub(ioReturnValue), err_get_code(ioReturnValue),
         strCallProc ? strCallProc : "Unknown Proc");
       if (ioReturnValue == kIOReturnNotPrivileged) {
          fprintf(stderr, "CSensorMacUSBJW::ReadData():: privilege violation 0x%x\n", (unsigned int) ioReturnValue);
       }
       return false;
   }
   return true;
}

bool CSensorMacUSBJW::WriteData(IOHIDDeviceInterface122** hidInterface, const UInt8 cmd, const UInt8 addr, const UInt8 data, const char* strCallProc)
{
    UInt8	writeBuffer[8];
    IOReturn    ioReturnValue;
		
    // open the interface
    //ioReturnValue = (*hidInterface)->open(hidInterface, kIOHIDOptionsTypeSeizeDevice);
    ioReturnValue = (*hidInterface)->open(hidInterface, kIOHIDOptionsTypeNone);

    if (ioReturnValue != kIOReturnSuccess) {
		fprintf(stderr, "CSensorMacUSBJW::WriteData():: couldn't open interface 0x%x - err 0x%x %s\n", 
                    (unsigned long) hidInterface, (unsigned long) ioReturnValue, strCallProc ? strCallProc : "Unknown Proc");
                (*hidInterface)->close(hidInterface);
		return false;
    }

    if (kIOReturnSuccess != ( ioReturnValue = EnableCommandMode (hidInterface))) {
                (*hidInterface)->close(hidInterface);
		return false; 
    }
	
    // write data
    memset(writeBuffer, 0x00, sizeof (writeBuffer));
    writeBuffer[0] = cmd;
    writeBuffer[1] = addr;
    writeBuffer[2] = data;

    ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 0, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess) {
       	   fprintf(stderr, "csensor_mac_ubs_jw.cpp:WriteData():: Could not write setReport on hid device interface retvalue=%d cmd=%d addr=%d data=%d\n", 
             ioReturnValue, cmd, addr, data);
           (*hidInterface)->close (hidInterface);
		return false;
    }  
	
    // disable command mode
    if (kIOReturnSuccess != ( ioReturnValue = DisableCommandMode(hidInterface))) {
                (*hidInterface)->close(hidInterface);
		return false;
    }
	
    // close the interface
    ioReturnValue = (*hidInterface)->close(hidInterface);
	
    return (ioReturnValue == kIOReturnSuccess);
}

/*
// for Apple HID Utilities -- walk the linked list of elements for a given USB device
void CSensorMacUSBJW::printElement(const int level, const pRecElement pelem)
{
           fprintf(stdout, "%sCookie=0x%x  Min=%ld  Max=%ld  Size=%ld  UsagePg=%ld  Usage=%ld  Name=%s\n",
               level == 1 ? "      " : level == 2 ? "           " : "                ",
               (unsigned int) pelem->cookie,
               pelem->min,
               pelem->max,
               pelem->size,
               pelem->usagePage,
               pelem->usage,
               pelem->name
            );
}

bool CSensorMacUSBJW::walkElement(const int level, const pRecElement pretmp)
{
     if (pretmp) {
        printElement(level, pretmp);

        if (m_bFoundJW)  {  // we're in the JW element we want? 
           switch ((int) pretmp->cookie)
           {
              case 0xb: 
              case 0xc: 
              case 0xd: 
                 m_prelJW[(int) pretmp->cookie - 0xb] = pretmp;
                 // fprintf(stdout, "Found Cookie 0x%x\n", (int) pretmp->cookie);
           }
        }

        // walk the children, when null go to the siblings then their children
        walkElement(level+1, pretmp->pChild);
        walkElement(level, pretmp->pSibling);

        return true;
     }
     else {
        return false;
     }
}

*/


/*
// note this isn't part of CSensorMacUSBJW::
void global_JoyWarriorAddedOrRemoved(void *refCon, io_iterator_t iterator)
{
    io_service_t            usbDevice;
    while ((usbDevice = IOIteratorNext(iterator)))
    {
        fprintf(stdout, "USB device added or removed\n");
        IOObjectRelease(usbDevice);
    }
    global_updateDeviceState();
}

void global_updateDeviceState()
{
    // delay so there is a timing error to force redetection
    fprintf(stdout, "JoyWarrior USB accelerometer removal detected!\n");
    fflush(stdout);
    boinc_sleep(2.5f);

}
*/

bool CSensorMacUSBJW::openDevHandle()
{
    if (!m_USBDevHandle[0]) return false;  // handle isn't open yet
    if (m_bDevHandleOpen) closeDevHandle();

    IOReturn result = (*m_USBDevHandle[0])->open(m_USBDevHandle[0], kIOHIDOptionsTypeNone);
    if (result != kIOReturnSuccess) {
       m_bDevHandleOpen = false;
       fprintf(stderr, "CSensorMacUSBJW::openDevHandle: couldn't open interface 0x%x - err 0x%x\n",
         (unsigned long) m_USBDevHandle[0], (unsigned long) result);
       return false;
    }
    m_bDevHandleOpen = true; // open was successful, set to true
    return true;
}

bool CSensorMacUSBJW::closeDevHandle()
{
    m_bDevHandleOpen = false; // set our boolean to false
    if (!m_USBDevHandle[0]) return false;  // handle isn't open yet so can't use

    IOReturn result = (*m_USBDevHandle[0])->close(m_USBDevHandle[0]);
    if (result != kIOReturnSuccess) {
       fprintf(stderr, "CSensorMacUSBJW::closeDevHandle: couldn't close interface 0x%x - err 0x%x\n",
         (unsigned long) m_USBDevHandle[0], (unsigned long) result);
       return false;
    }
    return true;
}

/*
 // not using the joystick interface
inline bool CSensorMacUSBJW::read_xyz(float& x1, float& y1, float& z1)
{
	if (!m_USBDevHandle[1]) return false;
	x1=y1=z1=0.0f;
	UInt8						rawData[6];
	int							i;
	SInt16						x = 0, y = 0, z = 0;
	
	for (i = 0; i < 6; i++)
	{
		JWReadByteFromAddress (m_USBDevHandle[1], 0x02 + i, &rawData[i]);
	}
	
	x = JWMergeAxisBytes(rawData[0], rawData[1]);
	y = JWMergeAxisBytes(rawData[2], rawData[3]);
	z = JWMergeAxisBytes(rawData[4], rawData[5]);
	
	x1 = (((float) x)) / 256.0f * EARTH_G;
	y1 = (((float) y)) / 256.0f * EARTH_G;
	z1 = (((float) z)) / 256.0f * EARTH_G;
	
	return true;
}
*/

// using the joystick interface
inline bool CSensorMacUSBJW::read_xyz(float& x1, float& y1, float& z1)
{  
	
	// past here is for the JW24F8 sensor
	
    //static int iTestCtr = 0;  // static so we can detect every few seconds if USB stick is still plugged in
    IOReturn result = kIOReturnSuccess;
    IOHIDEventStruct hidEvent;
    float fVal[3];

#ifndef QCN_USB
    if (qcn_main::g_iStop) return false;
#endif

    // major error if dev handle isn't open or can't be opened & read_xyz being called!
    if (!m_bDevHandleOpen && !openDevHandle()) { // this opens once at the start of reading to save CPU time (8%!)
       // but doesn't seem to detect if handle is bad i.e. when USB device is yanked out
       fprintf(stderr, "CSensorMacUSBJW::read_xyz: could not open Mac HID device handle!\n");
       return false; 
    }

    // cookies are 0xb for x-axis, 0xc for y-axis, 0xd for z-axis
    for (int i = 0; i < 3; i++) {
      fVal[i] = 0.0f;

      hidEvent.value = 0;
      hidEvent.longValueSize = 0;
      hidEvent.longValue = 0;

      result = (*m_USBDevHandle[0])->getElementValue(m_USBDevHandle[0], m_cookies.gAxisCookie[i], &hidEvent);
      // note that x/y/z should be scaled to +/- 2g, return values as +/- 2.0f*EARTH_G (in define.h: 9.78033 m/s^2)
      if (result == kIOReturnSuccess) {
#ifdef QCN_RAW_DATA	
	 // for testing on USGS shake table - they just want the raw integer data sent out
	 fVal[i]  = (float) hidEvent.value;
#else
         fVal[i]  = (((float) hidEvent.value - 512.0f) / 256.0f) * EARTH_G;
#endif
       }
    }

    x1 = fVal[0]; y1 = fVal[1]; z1 = fVal[2];

    return true;
}

bool CSensorMacUSBJW::detect()
{
   // first try and discover the HID interface (JoyWarrior)
   setType(SENSOR_NOTFOUND);  // initialize to no sensor until detected below
   closeHandles();  // reset the handles for JW detection
   
#ifndef QCN_USB
    if (qcn_main::g_iStop) return false;
#endif
	
   m_maDeviceRef = DiscoverHIDInterfaces(USB_VENDORID_JW, USB_DEVICEID_JW24F8); // from codemercs - inits the JW device in sys registry
   if (!m_maDeviceRef || CFArrayGetCount(m_maDeviceRef) < 2) { // not found, we'd have at least 2 interfaces for the JW USB
       closePort();
#ifdef _DEBUG
       fprintf(stdout, "No JoyWarrior USB device detected.\n");
#endif
       return false;
   }

   CFNumberRef interfaceRef = (CFNumberRef) CFArrayGetValueAtIndex(m_maDeviceRef, 0);
   CFNumberGetValue(interfaceRef, kCFNumberLongType, &m_USBDevHandle[0]);
   interfaceRef = (CFNumberRef) CFArrayGetValueAtIndex(m_maDeviceRef, 1);
   CFNumberGetValue(interfaceRef, kCFNumberLongType, &m_USBDevHandle[1]);
   CFRelease(interfaceRef);

   if (!m_USBDevHandle[0] || !m_USBDevHandle[1] || ! SetQCNState()) {
       // exit if this fails esp SetQCNState, can't communicate with JW properly...
       closePort();
       fprintf(stdout, "Could not setup JoyWarrior USB device.\n");
       return false;
   } 

   getHIDCookies(m_USBDevHandle[0], &m_cookies);

    // open port for read_xyz sequential reads...
    //(*m_USBDevHandle[0])->open(m_USBDevHandle[0], kIOHIDOptionsTypeSeizeDevice);
    //(*m_USBDevHandle[0])->open(m_USBDevHandle[0], kIOHIDOptionsTypeNone);

/*
   // start of using Mac HID Utilities
   // now use HID Utilities to open the JW USB sensor
   ::HIDBuildDeviceList(0, 0);

   pRecDevice newDevice = ::HIDGetFirstDevice();
   while (newDevice) { // search for the JoyWarrior "joystick" interface -- axis = 3 or inputs 11 (3 axis + 8 "buttons")
       // this joystick interface is much faster and fast enough for QCN (the other interface device is slow and can't keep up 50Hz)
       if (newDevice->vendorID == USB_VENDOR && newDevice->productID == USB_JOYWARRIOR) {
          if (newDevice->inputs == 11 || newDevice->axis == 3) { // this is the joystick interface (index 1)
             m_USBDevHandle[1] = (IOHIDDeviceInterface122**) newDevice->interface;
             m_prdJW = newDevice;
             m_bFoundJW = true;
             walkElement(1, m_prdJW->pListElements);
             fprintf(stdout, "Found JW Joystick Interface at 0x%x\n", (unsigned int) m_USBDevHandle[1]);
          }
          else { // must be the accelerometer interface (index 0)
             m_USBDevHandle[0] = (IOHIDDeviceInterface122**) newDevice->interface;
             fprintf(stdout, "Found JW Accelerometer Interface at 0x%x\n", (unsigned int) m_USBDevHandle[0]);
          }
       }
       if (m_USBDevHandle[0] && m_USBDevHandle[1]) break; // found a JW, break out of loop
       newDevice = ::HIDGetNextDevice(newDevice);
   }

   if (!m_USBDevHandle[0] || !m_USBDevHandle[1] || ! SetQCNState()) {
       // exit if this fails esp SetQCNState, can't communicate with JW properly...
       ::HIDReleaseDeviceList();  // cleanup HID devices
       return false;  // didn't find it
   } 

   fprintf(stdout, "JoyWarrior USB HID Detected - prDev 0x%x  XYZ = (0x%x, 0x%x, 0x%x)\n", 
         (unsigned int) m_prdJW,
         (unsigned int) m_prelJW[0],
         (unsigned int) m_prelJW[1],
         (unsigned int) m_prelJW[2]
   );
*/

/*
   CFMutableArrayRef interfaces, deviceProperties;
   CFNumberRef hidInterfaceRef[2];
   int iCount, iCountJW;

   // JoyWarrior24 Force 8
   interfaces = (CFMutableArrayRef) ::DiscoverHIDInterfaces(USB_VENDOR, USB_JOYWARRIOR);
   iCountJW = ::CFArrayGetCount(interfaces);
   if (iCountJW < 2) { // should have 2 for JW USB
      return false;
   }

   // get the hidInterface refs for the member variables to the 0 & 1 interface (accelerometer & joystick interfaces)
   hidInterfaceRef[0] = (CFNumberRef) CFArrayGetValueAtIndex(interfaces, 0);
   CFNumberGetValue(hidInterfaceRef[0], kCFNumberLongType, &m_USBDevHandle[0]);
   hidInterfaceRef[1] = (CFNumberRef) CFArrayGetValueAtIndex(interfaces, 1);
   CFNumberGetValue(hidInterfaceRef[1], kCFNumberLongType, &m_USBDevHandle[1]);

   // get properties for the JW device (just to get the product name & serial number for now)
   deviceProperties = ::DiscoverHIDDeviceProperties(USB_VENDOR, USB_JOYWARRIOR);
   iCount = ::CFArrayGetCount(deviceProperties);
   CFStringRef cfstrProduct = NULL, cfstrSerial = NULL;
   if (iCount) {
        CFDictionaryRef properties = (CFDictionaryRef) ::CFArrayGetValueAtIndex(deviceProperties, 0);
        iCount = ::CFDictionaryGetCount(properties);
        if (iCount >= 2) {
          cfstrProduct = (CFStringRef) ::CFDictionaryGetValue(properties, CFSTR("Product"));
          cfstrSerial = (CFStringRef) ::CFDictionaryGetValue(properties, CFSTR("SerialNumber"));
          if (cfstrProduct && cfstrSerial) {
            fprintf(stdout, "Product %s  SerialNumber %s\n", 
                CFStringGetCStringPtr(cfstrProduct, NULL),
                CFStringGetCStringPtr(cfstrSerial, NULL)
            );
          }
        }
   }
   ::CFRelease(cfstrProduct);
   ::CFRelease(cfstrSerial);


   fprintf(stdout, "%d JoyWarrior Handles found: 0=0x%x 1=0x%x\n",  
                iCountJW,
                (unsigned int) m_USBDevHandle[0],
                (unsigned int) m_USBDevHandle[1]
   );
*/

/*
   if (interfaces)
      ::CFRelease(interfaces);

   if (deviceProperties)
      ::CFRelease(deviceProperties);

    // close the current m_USBDevHandle's as we are going to use the Mac HID Utilties below for better speed    

    for (int i = 0; i < 2; i++) {
          if (m_USBDevHandle[i]) {
                WriteData(m_USBDevHandle[i], 0x02, 0x00, 0x00, "detect()::Free JW");
                (*m_USBDevHandle[i])->close(m_USBDevHandle[i]);
                m_USBDevHandle[i] = NULL;
          }
    }

*/
    
       if (! openDevHandle()) return false;

   // OK, we have a JoyWarrior USB sensor, and I/O is setup using Apple HID Utilities at 50Hz, +/- 2g
   setType(SENSOR_USB_JW24F8);
   setPort(getTypeEnum()); // set > -1 so we know we have a sensor
#ifdef QCN_RAW_DATA
   setSingleSampleDT(true); // set to true in raw mode so we don't get any interpolated/avg points (i.e. just the "integer" value hopefully)
#else
   setSingleSampleDT(false);
#endif

/* CMC Note:  the USB add/remove device logic doesn't seem to work -- may need to revisit if this becomes important

   // CMC Note: the next two callbacks can be used to detect if the JW USB accelerometer was removed or added
   int result[2];
   if ((result[0] = ::AddUSBDeviceAddedCallback(USB_VENDOR, USB_JOYWARRIOR, global_JoyWarriorAddedOrRemoved)) == -1) {
     fprintf(stderr, "Could not add USB AddedCallback()\n");
   }

   if ((result[1] = ::AddUSBDeviceRemovedCallback(USB_VENDOR, USB_JOYWARRIOR, global_JoyWarriorAddedOrRemoved)) == -1) {
     fprintf(stderr, "Could not add USB RemovedCallback()\n");
   }
  
   fprintf(stdout, "AddUSBDeviceAddedCallback=%d  AddUSBDeviceRemovedCallback=%d\n", result[0], result[1]);
   fflush(stdout);
*/
    fprintf(stdout, "Joywarrior 24F8 opened!\n");

    return (bool)(getTypeEnum() == SENSOR_USB_JW24F8);
}

bool CSensorMacUSBJW::SetQCNState()
{ // puts the Joystick Warrior USB sensor into the proper state for QCN (50Hz, +/- 2g)
  // and also writes these settings to EEPROM (so each device needs to just get set once hopefully)
	
   UInt8 mReg14 = 0x00;
   if (! ReadData(m_USBDevHandle[1], 0x14, &mReg14, "SetQCNState:R1")) {  // get current settings of device
       fprintf(stdout, "  * Could not read from JoyWarrior USB (SetQCNState:R1), exiting...\n");
       return false;
   }

   // if not set already, set it to +/-2g accel (0x00) and 50Hz internal bandwidth 0x01
   // NB: 0x08 & 0x10 means accel is set to 4 or 8g, if not bit-and with 0x01 bandwidth is other than 50Hz

   if ((mReg14 & 0x08) || (mReg14 & 0x10) || ((mReg14 & 0x01) != 0x01)) {
        fprintf(stdout, "Setting JoyWarrior 24F8 USB to QCN standard 50Hz sample rate, +/- 2g\n");

        UInt8 uiTmp = 0x00;
        if (! ReadData(m_USBDevHandle[1], 0x14, &uiTmp, "SetQCNState:R2") ) {
           fprintf(stdout, "  * Could not read from JoyWarrior 24F8 USB (SetQCNState:R2), exiting...\n");
           return false;
        }

        mReg14 = 0x01 | (uiTmp & 0xE0);

        // write settings to register
        if (! WriteData(m_USBDevHandle[1], 0x82, 0x14, mReg14, "SetQCNState:W1")) {
           fprintf(stdout, "  * Could not write to JoyWarrior 24F8 USB (SetQCNState:W1), exiting...\n");
           return false;
		}

	   /* CMC don't write to eeprom, just the reg image above
        // write settings to EEPROM for persistent state
        if (! WriteData(m_USBDevHandle[1], 0x82, 0x0A, 0x10, "SetQCNState:W2")) {  // start EEPROM write
           fprintf(stdout, "  * Could not write to JoyWarrior 24F8 USB (SetQCNState:W2), exiting...\n");
           return false;
        }
        boinc_sleep(.050f);
        if (! WriteData(m_USBDevHandle[1], 0x82, 0x34, mReg14, "SetQCNState:W3")) {
           fprintf(stdout, "  * Could not write to JoyWarrior 24F8 USB (SetQCNState:W3), exiting...\n");
           return false;
        }
        boinc_sleep(.050f);
        if (! WriteData(m_USBDevHandle[1], 0x82, 0x0A, 0x02, "SetQCNState:W4")) {  // end EEPROM write
           fprintf(stdout, "  * Could not write to JoyWarrior 24F8 USB (SetQCNState:W4), exiting...\n");
           return false;
        }
		*/
        boinc_sleep(.100f);
   } 
/*
   else {
      fprintf(stdout, "JoyWarrior 24F8 USB already set to QCN standard 50Hz sample rate, +/- 2g\n");
   }
*/
   return true;
}

/*
// Calculate a 10 bit value with MSB and LSB
short CSensorMacUSBJW::CalcMsbLsb(unsigned char lsb, unsigned char msb)
{
	short erg;
	short LSB, MSB, EXEC;

	EXEC = (msb & 0x80) << 8;
	EXEC = EXEC & 0x8000;

	// Calculate negative value
	if(EXEC & 0x8000)
		EXEC = EXEC | 0x7C00;

	MSB = msb << 2;
	MSB = MSB & 0x03FC;
	LSB = (lsb & 0xC0) >> 6;
	LSB = LSB & 0x0003;

	erg = MSB | LSB | EXEC;

	return erg;
}

*/

bool CSensorMacUSBJW::getHIDCookies(IOHIDDeviceInterface122** handle, cookie_struct_t cookies)
{
	
	CFTypeRef                               object;
	long                                    number;
	IOHIDElementCookie                      cookie;
	long                                    usage;
	long                                    usagePage;
	CFArrayRef                              elements; //
	CFDictionaryRef                         element;
	IOReturn                                success;
	
	memset(cookies, 0x00, sizeof(struct cookie_struct));
	
	if (!handle || !(*handle)) return false;
	
	// Copy all elements, since we're grabbing most of the elements
	// for this device anyway, and thus, it's faster to iterate them
	// ourselves. When grabbing only one or two elements, a matching
	// dictionary should be passed in here instead of NULL.
	success = (*handle)->copyMatchingElements(handle, NULL, &elements);
	
	if (success == kIOReturnSuccess) {
		CFIndex i;
		//printf("ITERATING...\n");
		for (i=0; i<CFArrayGetCount(elements); i++)
		{
			element = (CFDictionaryRef) CFArrayGetValueAtIndex(elements, i);
			// printf("GOT ELEMENT.\n");
			
			//Get cookie
			object = (CFDictionaryGetValue(element,
										   CFSTR(kIOHIDElementCookieKey)));
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
				continue;
			if(!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType,
								 &number))
				continue;
			cookie = (IOHIDElementCookie) number;
			
			//Get usage
			object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementUsageKey));
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
				continue;
			if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType,
								  &number))
				continue;
			usage = number;
			
			//Get usage page
			object = CFDictionaryGetValue(element,
										  CFSTR(kIOHIDElementUsagePageKey));
			if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID())
				continue;
			if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType,
								  &number))
				continue;
			usagePage = number;
			
			//Check for x axis
			if (usage == USB_COOKIE_X_JW24F8 && usagePage == 1)
				cookies->gAxisCookie[0] = cookie;
			//Check for y axis
			else if (usage == USB_COOKIE_Y_JW24F8 && usagePage == 1)
				cookies->gAxisCookie[1] = cookie;
			//Check for z axis
			else if (usage == USB_COOKIE_Z_JW24F8 && usagePage == 1)
				cookies->gAxisCookie[2] = cookie;
			//Check for buttons
			else if (usage == 1 && usagePage == 9)
				cookies->gButtonCookie[0] = cookie;
			else if (usage == 2 && usagePage == 9)
				cookies->gButtonCookie[1] = cookie;
			else if (usage == 3 && usagePage == 9)
				cookies->gButtonCookie[2] = cookie;
		}
		/*
		 fprintf(stdout, "JoyWarrior HID Cookies for X/Y/Z axes = (0x%x, 0x%x, 0x%x)\n", 
		 (unsigned int) cookies->gAxisCookie[0],
		 (unsigned int) cookies->gAxisCookie[1],
		 (unsigned int) cookies->gAxisCookie[2]
		 );
		 */
	}
	else {
		fprintf(stderr, "copyMatchingElements failed with error %d\n", success);
	}
	
	return true;
}

int CSensorMacUSBJW::JWDisableCommandMode (IOHIDDeviceInterface122 **hidInterface)
{
	UInt8	writeBuffer[8];
	int     ioReturnValue;
	
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x00;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
       	CFShow (CFSTR ("Could not write setReport on hid device interface"));
    }  
	return ioReturnValue;
}


int CSensorMacUSBJW::JWEnableCommandMode (IOHIDDeviceInterface122 **hidInterface)
{
	UInt8	writeBuffer[8];
	int     ioReturnValue;
	
	// enable Command mode
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x80;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
       	CFShow (CFSTR ("Could not write setReport on hid device interface"));
    } 
	return ioReturnValue;
	
}

int CSensorMacUSBJW::JWReadByteFromAddress (IOHIDDeviceInterface122 **hidInterface, UInt8 inAddress, UInt8 *result)
{
	UInt8	readBuffer[8];
	UInt8	writeBuffer[8];
	int     ioReturnValue;
    
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1050
	uint32_t readDataSize;
#else
	UInt32 readDataSize; // may be a problem with Mac's 10.4 to 10.5+
#endif
	
	*result = 0;
	
	// open the interface
	ioReturnValue = (*hidInterface)->open (hidInterface, 0);
    if (ioReturnValue != kIOReturnSuccess)
	{
		CFShow (CFSTR ("couldn't open interface"));
		return ioReturnValue;
	}
	
	if (kIOReturnSuccess != ( ioReturnValue = JWEnableCommandMode (hidInterface)))
		return ioReturnValue;
	
	// enable command mode
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x82;
	writeBuffer[1] = 0x80 | inAddress;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
       	CFShow (CFSTR ("Could not write setReport on hid device interface"));
        return ioReturnValue;
    }  
	// read something from interface
	readDataSize = 8;
	ioReturnValue = (*hidInterface)->getReport (hidInterface, kIOHIDReportTypeInput,
												0, readBuffer, &readDataSize, 100, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
        CFShow (CFSTR ("Could not call getReport on hid device interface"));
        return ioReturnValue;
	}
	*result = readBuffer[2] ;
	
	// disable command mode
	if (kIOReturnSuccess != ( ioReturnValue = JWDisableCommandMode (hidInterface)))
		return ioReturnValue;
	
	// close the interface
	ioReturnValue = (*hidInterface)->close (hidInterface);
	
	return ioReturnValue;
}

int CSensorMacUSBJW::JWWriteByteToAddress (IOHIDDeviceInterface122 **hidInterface, UInt8 inAddress, UInt8 inData)
{
	UInt8	writeBuffer[8];
	int     ioReturnValue;
	
	// open the interface
	ioReturnValue = (*hidInterface)->open (hidInterface, 0);
    if (ioReturnValue != kIOReturnSuccess)
	{
		CFShow (CFSTR ("couldn't open interface"));
		return ioReturnValue;
	}
	if (kIOReturnSuccess != ( ioReturnValue = JWEnableCommandMode (hidInterface)))
		return ioReturnValue;
	
	// write data
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x82;
	writeBuffer[1] = inAddress;
	writeBuffer[2] = inData;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
       	CFShow (CFSTR ("Could not write setReport on hid device interface"));
        return ioReturnValue;
    }  
	
	// disable command mode
	if (kIOReturnSuccess != ( ioReturnValue = JWDisableCommandMode (hidInterface)))
		return ioReturnValue;
	
	// close the interface
	ioReturnValue = (*hidInterface)->close (hidInterface);
	
	return ioReturnValue;
}


SInt16 CSensorMacUSBJW::JWMergeAxisBytes (UInt8 inLSB, UInt8 inMSB)
{
	SInt16 result;
	SInt16 msb;
	SInt16 lsb;
	
	result = (inMSB & 0x80) << 8;  // move first bit of msb 8 bits to left
	
	if (result & 0x8000) // if first byte of result is set after the shift (e.g. its negative)
		result = result | 0x7C00; // enable bits 14 to 10 
	
	msb = inMSB << 2; // shift MSB two bytes to left ot make room for LSB data
	
	lsb = (inLSB & 0xC0) >> 6; // shift upper two bits of LSB to lower to bits
	
	return msb | lsb | result; // merge everything together
}

SInt16 CSensorMacUSBJW::JWMergeOffsetBytes (UInt8 inLSB, UInt8 inMSB)
{
	SInt16 msb;
	SInt16 lsb;
	
	msb = inMSB << 2; // shift MSB two bytes to left ot make room for LSB data
	
	lsb = (inLSB & 0xC0) >> 6; // shift upper two bits of LSB to lower to bits
	
	return msb | lsb; // merge everything together
}


void CSensorMacUSBJW::JWDiffMsbLsb (UInt16 value, UInt8 *inLSB, UInt8 *inMSB)
{	
	*inLSB = ((value & 0x003) << 6) & 0xC0;
	*inMSB = ((value & 0x3FC) >> 2) & 0xFF;
	
}

