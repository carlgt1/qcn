/*
 *  csensor_mac_usb_jw24f14.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for sensor classes
 */

#include "main.h"
#include "csensor_mac_usb_jw24f14.h"

// making sense of IOReturn (IOKit) error codes:  
// http://developer.apple.com/qa/qa2001/qa1075.html

// IOReturn codes in:
// /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/IOKit.framework/Headers/IOReturn.h

IOHIDDeviceInterface122** CSensorMacUSBJW24F14::CreateHIDDeviceInterface(io_object_t hidDevice)
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

CFMutableDictionaryRef CSensorMacUSBJW24F14::SetUpHIDMatchingDictionary (int inVendorID, int inDeviceID)
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
io_iterator_t CSensorMacUSBJW24F14::FindHIDDevices (const mach_port_t masterPort, int inVendorID, int inDeviceID)
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

CFMutableArrayRef CSensorMacUSBJW24F14::DiscoverHIDInterfaces(int vendorID, int deviceID)
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

CSensorMacUSBJW24F14::CSensorMacUSBJW24F14()
  : CSensor()
{
   m_USBDevHandle[0] = m_USBDevHandle[1] = NULL;
   m_bFoundJW = false;
   m_maDeviceRef = NULL;
   m_bDevHandleOpen = false;
   closeHandles();
}

CSensorMacUSBJW24F14::~CSensorMacUSBJW24F14()
{
  closePort();
}

void CSensorMacUSBJW24F14::closePort()
{
  for (int i = 0; i < 2; i++) {
     if (m_USBDevHandle[i]) {
       try {
          // don't think we need the next line, just close & Release
          //if (i==0) WriteData(m_USBDevHandle[0], 0x02, 0x00, 0x00, "closePort()::Free JW24F14");
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
    fprintf(stdout, "Joywarrior 24F14 closed!\n");
    fflush(stdout);
    setPort(-1);
  }

}

void CSensorMacUSBJW24F14::closeHandles()
{
/*
      m_prdJW24F14 = NULL;
      m_prelJW24F14[0] = NULL;
      m_prelJW24F14[1] = NULL;
      m_prelJW24F14[2] = NULL;
 */
      closeDevHandle();
      m_bFoundJW = false;
      m_USBDevHandle[0] = NULL;
      m_USBDevHandle[1] = NULL;
      m_maDeviceRef = NULL;
}

bool CSensorMacUSBJW24F14::openDevHandle()
{
    if (!m_USBDevHandle[0]) return false;  // handle isn't open yet
    if (m_bDevHandleOpen) closeDevHandle();

    IOReturn result = (*m_USBDevHandle[0])->open(m_USBDevHandle[0], kIOHIDOptionsTypeNone);
    if (result != kIOReturnSuccess) {
       m_bDevHandleOpen = false;
       fprintf(stderr, "CSensorMacUSBJW24F14::openDevHandle: couldn't open interface 0x%x - err 0x%x\n",
         (unsigned long) m_USBDevHandle[0], (unsigned long) result);
       return false;
    }
    m_bDevHandleOpen = true; // open was successful, set to true
    return true;
}

bool CSensorMacUSBJW24F14::closeDevHandle()
{
    m_bDevHandleOpen = false; // set our boolean to false
    if (!m_USBDevHandle[0]) return false;  // handle isn't open yet so can't use

    IOReturn result = (*m_USBDevHandle[0])->close(m_USBDevHandle[0]);
    if (result != kIOReturnSuccess) {
       fprintf(stderr, "CSensorMacUSBJW24F14::closeDevHandle: couldn't close interface 0x%x - err 0x%x\n",
         (unsigned long) m_USBDevHandle[0], (unsigned long) result);
       return false;
    }
    return true;
}

/*
// not using joystick HID interface
inline bool CSensorMacUSBJW24F14::read_xyz(float& x1, float& y1, float& z1)
{
#ifdef _DEBUG
	static int x_max = -10000, x_min = 10000;
#endif
	
	if (!m_USBDevHandle[1]) return false;
	x1=y1=z1=0.0f;
			UInt8						rawData[6];
			int							i;
			SInt16						x = 0, y = 0, z = 0;
			
				JWEnableCommandMode24F14(m_USBDevHandle[1]);
				
				for (i = 0; i < 6; i++)
				{
					JWReadByteFromAddress24F14(m_USBDevHandle[1], 0x02 + i, &rawData[i]);
				}
				
				JWDisableCommandMode24F14(m_USBDevHandle[1]);
				
				x = ((rawData[1] << 8) | (rawData[0] ));
				x >>= 2;        
				y = ((rawData[3] << 8) | (rawData[2] ));
				y >>= 2;
				z = ((rawData[5] << 8) | (rawData[4] ));
				z >>= 2;

	// this gives 0 to -39.123
	//x1 = (((float) x - 16384.f)) / 8192.f * EARTH_G;
	//y1 = (((float) y - 16384.f)) / 8192.f * EARTH_G;

#ifdef _DEBUG
	if (x > x_max) x_max = x;
	if (x < x_min) x_min = x;
#endif
	
	x1 = ((((float) x)) / 4096.0f) * EARTH_G;
	y1 = ((((float) y)) / 4096.0f) * EARTH_G;
	z1 = ((((float) z)) / 4096.0f) * EARTH_G;
	
	return true;
}
*/

// using joystick HID interface
inline bool CSensorMacUSBJW24F14::read_xyz(float& x1, float& y1, float& z1)
{  	
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
		// note that x/y/z should be scaled to +/- 2g, 14 bits = 2^14 = 16384, values from 0 (-2g) to 16383 (+2g)
		// return values as +/- 2.0f*EARTH_G (in define.h: 9.78033 m/s^2)
		if (result == kIOReturnSuccess)  {
#ifdef QCN_RAW_DATA	
			// for testing on USGS shake table - they just want the raw integer data sent out
			fVal[i]  = (float) hidEvent.value;
#else
			fVal[i]  = (((float) hidEvent.value - 8191.5f) / 4095.75f) * EARTH_G;
#endif
		}
    }
	
    x1 = fVal[0]; y1 = fVal[1]; z1 = fVal[2];
	
	return true;
}

bool CSensorMacUSBJW24F14::detect()
{
   // first try and discover the HID interface (JoyWarrior)
   setType(SENSOR_NOTFOUND);  // initialize to no sensor until detected below
   closeHandles();  // reset the handles for JW24F14 detection
   
#ifndef QCN_USB
    if (qcn_main::g_iStop) return false;
#endif
	
   m_maDeviceRef = DiscoverHIDInterfaces(USB_VENDORID_JW, USB_DEVICEID_JW24F14); // from codemercs - inits the JW24F14 device in sys registry
   if (!m_maDeviceRef || CFArrayGetCount(m_maDeviceRef) < 2) { // not found, we'd have at least 2 interfaces for the JW24F14 USB
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
       // exit if this fails esp SetQCNState, can't communicate with JW24F14 properly...
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
   // now use HID Utilities to open the JW24F14 USB sensor
   ::HIDBuildDeviceList(0, 0);

   pRecDevice newDevice = ::HIDGetFirstDevice();
   while (newDevice) { // search for the JoyWarrior "joystick" interface -- axis = 3 or inputs 11 (3 axis + 8 "buttons")
       // this joystick interface is much faster and fast enough for QCN (the other interface device is slow and can't keep up 50Hz)
       if (newDevice->vendorID == USB_VENDOR && newDevice->productID == USB_JOYWARRIOR) {
          if (newDevice->inputs == 11 || newDevice->axis == 3) { // this is the joystick interface (index 1)
             m_USBDevHandle[1] = (IOHIDDeviceInterface122**) newDevice->interface;
             m_prdJW24F14 = newDevice;
             m_bFoundJW = true;
             walkElement(1, m_prdJW24F14->pListElements);
             fprintf(stdout, "Found JW24F14 Joystick Interface at 0x%x\n", (unsigned int) m_USBDevHandle[1]);
          }
          else { // must be the accelerometer interface (index 0)
             m_USBDevHandle[0] = (IOHIDDeviceInterface122**) newDevice->interface;
             fprintf(stdout, "Found JW24F14 Accelerometer Interface at 0x%x\n", (unsigned int) m_USBDevHandle[0]);
          }
       }
       if (m_USBDevHandle[0] && m_USBDevHandle[1]) break; // found a JW24F14, break out of loop
       newDevice = ::HIDGetNextDevice(newDevice);
   }

   if (!m_USBDevHandle[0] || !m_USBDevHandle[1] || ! SetQCNState()) {
       // exit if this fails esp SetQCNState, can't communicate with JW24F14 properly...
       ::HIDReleaseDeviceList();  // cleanup HID devices
       return false;  // didn't find it
   } 

   fprintf(stdout, "JoyWarrior USB HID Detected - prDev 0x%x  XYZ = (0x%x, 0x%x, 0x%x)\n", 
         (unsigned int) m_prdJW24F14,
         (unsigned int) m_prelJW24F14[0],
         (unsigned int) m_prelJW24F14[1],
         (unsigned int) m_prelJW24F14[2]
   );
*/

/*
   CFMutableArrayRef interfaces, deviceProperties;
   CFNumberRef hidInterfaceRef[2];
   int iCount, iCountJW24F14;

   // JoyWarrior24 Force 8
   interfaces = (CFMutableArrayRef) ::DiscoverHIDInterfaces(USB_VENDOR, USB_JOYWARRIOR);
   iCountJW24F14 = ::CFArrayGetCount(interfaces);
   if (iCountJW24F14 < 2) { // should have 2 for JW24F14 USB
      return false;
   }

   // get the hidInterface refs for the member variables to the 0 & 1 interface (accelerometer & joystick interfaces)
   hidInterfaceRef[0] = (CFNumberRef) CFArrayGetValueAtIndex(interfaces, 0);
   CFNumberGetValue(hidInterfaceRef[0], kCFNumberLongType, &m_USBDevHandle[0]);
   hidInterfaceRef[1] = (CFNumberRef) CFArrayGetValueAtIndex(interfaces, 1);
   CFNumberGetValue(hidInterfaceRef[1], kCFNumberLongType, &m_USBDevHandle[1]);

   // get properties for the JW24F14 device (just to get the product name & serial number for now)
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
                iCountJW24F14,
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
                WriteData(m_USBDevHandle[i], 0x02, 0x00, 0x00, "detect()::Free JW24F14");
                (*m_USBDevHandle[i])->close(m_USBDevHandle[i]);
                m_USBDevHandle[i] = NULL;
          }
    }

*/
	
//	bool CSensorMacUSBJW24F14::ReadData(IOHIDDeviceInterface122** hidInterface, const UInt8 addr, UInt8* cTemp, const char* strCallProc)

    if (! openDevHandle()) return false;
    
   // OK, we have a JoyWarrior USB sensor, and I/O is setup using Apple HID Utilities at 50Hz, +/- 2g
   setType(SENSOR_USB_JW24F14);
   setPort(getTypeEnum()); // set > -1 so we know we have a sensor
#ifdef QCN_RAW_DATA
   setSingleSampleDT(true); // set to true in raw mode so we don't get any interpolated/avg points (i.e. just the "integer" value hopefully)
#else
   setSingleSampleDT(false);
#endif

/* CMC Note:  the USB add/remove device logic doesn't seem to work -- may need to revisit if this becomes important

   // CMC Note: the next two callbacks can be used to detect if the JW24F14 USB accelerometer was removed or added
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
    fprintf(stdout, "Joywarrior 24F14 opened!\n");

    return (bool)(getTypeEnum() == SENSOR_USB_JW24F14);
}

bool CSensorMacUSBJW24F14::SetQCNState()
{ // puts the Joystick Warrior USB sensor into the proper state for QCN (50Hz, +/- 2g)
  // and also writes these settings to EEPROM (so each device needs to just get set once hopefully)
	
	return true;
/*	
	const int ciRange = 4;       // 2g range (+/-)
	//const int ciBandwidth = 120; // 1200Hz bw
	const int ciBandwidth = 56;  // 75Hz bandwidth & 0% compensation

	int iRange = 0, iBandwidth = 0;
	// note the command-mode takes the 2nd handle
	//if (! QCNReadSensor(m_USBDevHandle[1], iRange, iBandwidth)) return false;

	//return true;

	//if (iRange == ciRange && iBandwidth == ciBandwidth) return true; // already set
	
	// if here need to set
	if (! QCNWriteSensor(m_USBDevHandle[1], ciRange, ciBandwidth)) return false;
	
    return true;
 */
}

bool CSensorMacUSBJW24F14::getHIDCookies(IOHIDDeviceInterface122** handle, cookie_struct_t cookies)
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
			if (usage == USB_COOKIE_X_JW24F14 && usagePage == 1)
				cookies->gAxisCookie[0] = cookie;
			//Check for y axis
			else if (usage == USB_COOKIE_Y_JW24F14 && usagePage == 1)
				cookies->gAxisCookie[1] = cookie;
			//Check for z axis
			else if (usage == USB_COOKIE_Z_JW24F14 && usagePage == 1)
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

/* bandwidth & comp
 
 // Read Bandwidth & Compensation
 JWReadByteFromAddress24F14 (interface, 0x20, &temp);
 usleep(10000);
 [deviceBandwidthField selectItemAtIndex:(temp & 0xF0) >> 4];
 [deviceCompensationField selectItemAtIndex:(temp & 0x0F)];
 
 // Read Range
 JWReadByteFromAddress24F14 (interface, 0x35, &temp);
 usleep(10000);
 temp &= 0x0E;
 [deviceRangeField selectItemAtIndex:(temp >> 1)];
 
*/

bool CSensorMacUSBJW24F14::QCNReadSensor(IOHIDDeviceInterface122** interface, int& iRange, int& iBandwidth)
{	
	/*
	// Read	
	// Get values from sensor
	UInt8 temp; //, iComp;
	
	OpenImage(interface, true);

	// Read Bandwidth & Compensation
	if (!JWReadByteFromAddress24F14 (interface, 0x20, &temp)) return false;
	//iBandwidth = (temp & 0xF0) >> 4;
	//iComp = temp & 0x0F;
	iBandwidth = temp;
	
	// Read Range
	if (!JWReadByteFromAddress24F14 (interface, 0x35, &temp)) return false;
	iRange = temp;
	
	
	// Read customer specific byte 1
	//JWReadByteFromAddress24F14 (interface, 0x2c, &temp)
	//usleep(10000);
	//[customerSpecificByte1Field setStringValue:[NSString stringWithFormat:@"%x", temp] ];
	
	// Read customer specific byte 2
	//JWReadByteFromAddress24F14 (interface, 0x2d, &temp);
	//usleep(10000);
	//[customerSpecificByte2Field setStringValue:[NSString stringWithFormat:@"%x", temp] ];
	
	//JWReadByteFromAddress24F14(interface, 0x35, &temp);
	//temp &= 0xF1;
	//usleep(10000);
	
	//jw24f14_write(hid.HidHandle[1], 0x82, 0x35, 0x04 | oldData);
	//Sleep(10);
	//jw24f14_open_image(hid.HidHandle[1], false);
	//Sleep(10);
	OpenImage(interface, false);
	Mode(interface, false);
	 
	*/
	
	return true;	
}



bool CSensorMacUSBJW24F14::QCNWriteSensor(IOHIDDeviceInterface122** interface, const int& iRange, const int& iBandwidth)
{
	
/*
	// Write

	UInt8 temp = 0x00;
	//int range			= 2;   // 2g range, 0=1, 1=1.5, 2=2, 3=3, 4=4, 5=8, 6=16
	//int bandwidth		= 3;   // 75Hz,  0=10, 1=20, 2=40, 3=75, 4=150, 5=300, 6=600, 7=1200
	//int compensation	= 8;   // 0% comp,  7=-.5%, 8=0, 9=+.5% etc
	
	OpenImage(interface, true);

    //if (JWEnableCommandMode24F14 (interface) != kIOReturnSuccess) return false;
    
	// Open 
	if (!JWReadByteFromAddress24F14 (interface, 0x0D, &temp)) return false;
	usleep(10000);
	temp &= 0xEF;
	temp |= 0x10;
	if (!JWWriteByteToAddress24F14 (interface, 0x82, 0x0D, temp)) return false;
	usleep(10000);
	
	// write high_dur * dis_i2c  
	temp = 50;
	if (!JWWriteByteToAddress24F14 (interface, 0x82, 0x27, temp)) return false;
	usleep(10000);

	// Write Bandwidth & Compensation
	//JWReadByteFromAddress24F14 (interface, 0x20, &temp);
	//usleep(10000);
	//temp &= 0x00;
	//temp |= (bandwidth<<4);
	//temp |= compensation;
	temp = iBandwidth;
	if (!JWWriteByteToAddress24F14 (interface, 0x82, 0x20, temp)) return false;
	usleep(10000);
	
	// Write Range
	//if (!JWReadByteFromAddress24F14 (interface, 0x35, &temp)) return false;
	//usleep(10000);
	//temp &= 0xF1;
	//temp |= (range<<1);
	temp = iRange;
	if (!JWWriteByteToAddress24F14 (interface, 0x82, 0x35, temp)) return false;
	usleep(10000);
	
	// Write customer specific byte 1
    theScanner = [NSScanner scannerWithString:[customerSpecificByte1Field stringValue]]; 
    int value;
    [theScanner scanHexInt:(unsigned int*)&value];
    temp = value;
	JWWriteByteToAddress24F14 (interface, 0x82, 0x2c, value);
	usleep(10000);
	
	// Write customer specific byte 2
    theScanner = [NSScanner scannerWithString:[customerSpecificByte2Field stringValue]]; 
    [theScanner scanHexInt:(unsigned int*)&value];
    temp = value;
	JWWriteByteToAddress24F14 (interface, 0x82, 0x2d, temp);
	usleep(10000);
	
	
	OpenImage(interface, false);
	Mode(interface, false);

	//no eeprom saves
		// Save changes to EEPROM by touching the registers we want to change
		JWWriteByteToAddress24F14 (interface, 0x82, 0x40 & 0xFE, 0);
		usleep(10000);
		JWWriteByteToAddress24F14 (interface, 0x82, 0x55 & 0xFE, 0);
		usleep(10000);
		
		// Soft-reset (save EEPROM-state)
		JWWriteByteToAddress24F14 (interface, 0x82, 0x10, 0xB6);
		usleep(10000);
	
	//}
	*/
	
	return true;
}

/*
//Enable or disable direct connection
void CSensorMacUSBJW24F14::Mode(IOHIDDeviceInterface122 **hidInterface, bool bOpen)
{
	JWWriteByteToAddress24F14(hidInterface, (bOpen ? 0x82 : 0x00), 0x00, 0x00);
	usleep(10000);
}

void CSensorMacUSBJW24F14::OpenImage(IOHIDDeviceInterface122 **hidInterface, bool bOpen)
{	
	UInt8 state = 0x00;
	JWReadByteFromAddress24F14(hidInterface, 0x0D, &state);
	state &= 0xEF;
	JWWriteByteToAddress24F14(hidInterface, 0x82, 0x0D, (bOpen ? 0x10 : 0x00) | state);
	usleep(10000);
}

int CSensorMacUSBJW24F14::JWDisableCommandMode24F14 (IOHIDDeviceInterface122 **hidInterface)
{
	UInt8	writeBuffer[8];
	int     ioReturnValue;
	
    // open the interface
	ioReturnValue = (*hidInterface)->open (hidInterface, 0);
    if (ioReturnValue != kIOReturnSuccess)
	{
       	fprintf(stderr, "JW24F14:DisableComandMode: couldn't open interface\n");
		return ioReturnValue;
	}
    
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x00;
    
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
       	fprintf(stderr, "JW24F14:DisableComandMode: couldn't write setReport\n");
    }
    
    ioReturnValue = (*hidInterface)->close (hidInterface);
    
	return ioReturnValue;
}
int CSensorMacUSBJW24F14::JWEnableCommandMode24F14(IOHIDDeviceInterface122 **hidInterface)
{
	UInt8	writeBuffer[8];
	int     ioReturnValue;
	
    // open the interface
	ioReturnValue = (*hidInterface)->open (hidInterface, 0);
    if (ioReturnValue != kIOReturnSuccess)
	{
       	fprintf(stderr, "JW24F14:EnableComandMode: couldn't open interface\n");
		return ioReturnValue;
	}
    
	// enable Command mode
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x80;
    
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
       	fprintf(stderr, "JW24F14:EnableComandMode: couldn't write setReport\n");
    } 
    
    ioReturnValue = (*hidInterface)->close (hidInterface);
    
	return ioReturnValue;
}

bool CSensorMacUSBJW24F14::JWReadByteFromAddress24F14(IOHIDDeviceInterface122 **hidInterface, UInt8 inAddress, UInt8 *result)
{
	UInt8	readBuffer[8];
	UInt8	writeBuffer[8];
	int     ioReturnValue;
	//uint32_t		readDataSize;
	UInt32	readDataSize;
	
	*result = 0;
	
	// open the interface
	ioReturnValue = (*hidInterface)->open (hidInterface, 0);
    if (ioReturnValue != kIOReturnSuccess)
	{
		fprintf(stderr, "jw24f14:ReadByte couldn't open interface");
		return false;
	}
	
	// enable command mode
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = 0x82;
	writeBuffer[1] = inAddress | 0x80;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
		fprintf(stderr, "jw24f14:ReadByte couldn't write setReport in interface");
        return false;
    }  
	// read something from interface
	readDataSize = 8;
	ioReturnValue = (*hidInterface)->getReport (hidInterface, kIOHIDReportTypeInput,
												0, readBuffer, &readDataSize, 100, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
		fprintf(stderr, "jw24f14:ReadByte couldn't call setReport in interface");
        return false;
	}
	*result = readBuffer[2] ;
	
	// close the interface
	ioReturnValue = (*hidInterface)->close (hidInterface);
	return true;
}

bool CSensorMacUSBJW24F14::JWWriteByteToAddress24F14 (IOHIDDeviceInterface122 **hidInterface, UInt8 cmd, UInt8 inAddress, UInt8 inData)
{
	UInt8	writeBuffer[8];
	int     ioReturnValue;
	
	// open the interface
	ioReturnValue = (*hidInterface)->open (hidInterface, 0);
    if (ioReturnValue != kIOReturnSuccess)
	{
		fprintf(stderr, "jw24f14:WriteByte couldn't open interface");
		return false;
	}
	//if (kIOReturnSuccess != ( ioReturnValue = JWEnableCommandMode24F14 (hidInterface)))
	//	return ioReturnValue;
	
	// write data
	bzero (writeBuffer, sizeof (writeBuffer));
	writeBuffer[0] = cmd;
	writeBuffer[1] = inAddress;
	writeBuffer[2] = inData;
	
	ioReturnValue = (*hidInterface)->setReport (hidInterface, kIOHIDReportTypeOutput, 0, writeBuffer, sizeof(writeBuffer), 50, NULL, NULL, NULL);
    if (ioReturnValue != kIOReturnSuccess)
    {
		fprintf(stderr, "jw24f14:WriteByte couldn't write setReport interface");
        return false;
    }  
	
	// disable command mode
	//if (kIOReturnSuccess != ( ioReturnValue = JWDisableCommandMode24F14 (hidInterface)))
	//	return ioReturnValue;
	
	// close the interface
	ioReturnValue = (*hidInterface)->close (hidInterface);
	return true;
}


SInt16 CSensorMacUSBJW24F14::JWMergeAxisBytes24F14 (UInt8 inLSB, UInt8 inMSB)
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

SInt16 CSensorMacUSBJW24F14::JWMergeOffsetBytes24F14 (UInt8 inLSB, UInt8 inMSB)
{
	SInt16 msb;
	SInt16 lsb;
	
	msb = inMSB << 2; // shift MSB two bytes to left ot make room for LSB data
	
	lsb = (inLSB & 0xC0) >> 6; // shift upper two bits of LSB to lower to bits
	
	return msb | lsb ; // merge everything together
}


void CSensorMacUSBJW24F14::JWDiffMsbLsb24F14 (UInt16 value, UInt8 *outLSB, UInt8 *outMSB)
{
	
	*outLSB = ((value & 0x003) << 6) & 0xC0;
	*outMSB = ((value & 0x3FC) >> 2) & 0xFF;
	
}

 */
