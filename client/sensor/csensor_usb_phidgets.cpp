/*
 *  csensor_usb_phidgets.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2012 Stanford University.  All rights reserved.
 *
 * Implementation file for cross-platform (Mac, Windows, Linux) Phidgets 1056 accelerometer USB sensor class
 *     http://www.phidgets.com/products.php?product_id=1056
 *   NB: some "Windows" terminology used, i.e. m_handleLibrary but it's really a shared object Mac dylib or Linux so of course

 * For Linux:

Make the phidget devices accessible:

*) Open/create the udev rule file
$ sudo vi /etc/udev/rules.d/80-phidget.rules

*) Add the following content:
SUBSYSTEM=="usb", ATTRS{idVendor}=="06c2", MODE="0777"

*) set USB_DEVFS_PATH to /dev/bus/usb by adding to ~/.bashrc:
export USB_DEVFS_PATH=/dev/bus/usb

*) restart udev:
$ services udev restart

or

 sudo /etc/init.d/udev restart


 */

#include "main.h"
#include "csensor_usb_phidgets.h"

// we're using dynamic libraries for everything except i686 Linux
#define __USE_DYLIB__ 1

// for Mac & Linux we use dlopen into the Phidget .dylib (Mac) or .so (Linux)
#ifndef _WIN32
#define __USE_DLOPEN__
#include <dlfcn.h>   // dlopen and dlclose
#endif


//set the dll/so/dylib name
#ifdef _WIN32  // Windows
  #define GET_PROC_ADDR ::GetProcAddress
#ifdef _WIN64
const char CSensorUSBPhidgets::m_cstrDLL[] = {"phidget218-64.dll"};
#else
const char CSensorUSBPhidgets::m_cstrDLL[] = {"phidget218-32.dll"};
#endif // Win 32 v 64
#else // Mac & Linux
  #ifdef __APPLE_CC__
     #define GET_PROC_ADDR dlsym
  #else  // Linux - using static lib
     #undef __USE_DYLIB__
     #undef __USE_DLOPEN___
  #endif
#ifdef __APPLE_CC__
const char CSensorUSBPhidgets::m_cstrDLL[] = {"phidget218.dylib"};   
#else
#ifdef __LINUX_ARMV6__
const char CSensorUSBPhidgets::m_cstrDLL[] = {"phidget21armv6.so"};   
#else
#ifdef __LINUX_ARMV5__
const char CSensorUSBPhidgets::m_cstrDLL[] = {"phidget21armv5.so"};   
#else // i686 Linux
const char CSensorUSBPhidgets::m_cstrDLL[] = {""};   
#endif
#endif  // armv6
#endif // apple or linux
#endif // windows˚˚

CSensorUSBPhidgets::CSensorUSBPhidgets(int iModel)
  : CSensor(),
	m_handlePhidgetSpatial(NULL), m_handleLibrary(NULL), m_iModelSearch(iModel)
{ 
    m_bLogging = false;
	m_iSerialNum = 0;
	m_iVersion = 0;
	m_iNumAccelAxes = 0;
	m_iNumGyroAxes = 0;
	m_iNumCompassAxes = 0;
	m_iDataRateMax = 0;
	m_iDataRateMin = 0;	
}

CSensorUSBPhidgets::~CSensorUSBPhidgets()
{
   closePort();
}

bool CSensorUSBPhidgets::setupFunctionPointers()
{



#ifdef __USE_DYLIB__
   std::string strPath;
   // setup DLL path, if returns false then DLL doesn't exist at the path where it should
        if (!qcn_util::setDLLPath(strPath, m_cstrDLL)) {
              fprintf(stderr, "Phidgets1056: Cannot set DLL Path %s\n", m_cstrDLL);
                closePort();
                return false;
        }

  #ifdef __USE_DLOPEN__
    m_handleLibrary = dlopen(strPath.c_str(), RTLD_LAZY | RTLD_GLOBAL); // default
    if (!m_handleLibrary) {
       fprintf(stderr, "CSensorUSBPhidgets: dynamic library %s dlopen error %s\n", m_cstrDLL, dlerror());
       return false;
    }
  #else // for Windows or not using dlopen just use the direct phidget lib
    m_handleLibrary = ::LoadLibrary(strPath.c_str());
  #endif
#endif


#ifdef __USE_DYLIB__
	if (!m_handleLibrary) return false;
	m_PtrCPhidget_open = (PtrCPhidget_open) GET_PROC_ADDR(m_handleLibrary, "CPhidget_open");
	m_PtrCPhidget_close = (PtrCPhidget_close) GET_PROC_ADDR(m_handleLibrary, "CPhidget_close");
	m_PtrCPhidget_delete = (PtrCPhidget_delete) GET_PROC_ADDR(m_handleLibrary, "CPhidget_delete");
	m_PtrCPhidget_getDeviceName = (PtrCPhidget_getDeviceName) GET_PROC_ADDR(m_handleLibrary, "CPhidget_getDeviceName");
	m_PtrCPhidget_getSerialNumber = (PtrCPhidget_getSerialNumber) GET_PROC_ADDR(m_handleLibrary, "CPhidget_getSerialNumber");
	m_PtrCPhidget_getDeviceVersion = (PtrCPhidget_getDeviceVersion) GET_PROC_ADDR(m_handleLibrary, "CPhidget_getDeviceVersion");
	m_PtrCPhidget_getDeviceStatus = (PtrCPhidget_getDeviceStatus) GET_PROC_ADDR(m_handleLibrary, "CPhidget_getDeviceStatus");
	m_PtrCPhidget_getLibraryVersion = (PtrCPhidget_getLibraryVersion) GET_PROC_ADDR(m_handleLibrary, "CPhidget_getLibraryVersion");
	m_PtrCPhidget_getDeviceType = (PtrCPhidget_getDeviceType) GET_PROC_ADDR(m_handleLibrary, "CPhidget_getDeviceType");
	m_PtrCPhidget_getDeviceLabel = (PtrCPhidget_getDeviceLabel) GET_PROC_ADDR(m_handleLibrary, "CPhidget_getDeviceLabel");

	m_PtrCPhidget_set_OnAttach_Handler = (PtrCPhidget_set_OnAttach_Handler) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidget_set_OnAttach_Handler");
	m_PtrCPhidget_set_OnDetach_Handler = (PtrCPhidget_set_OnDetach_Handler) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidget_set_OnDetach_Handler");
	m_PtrCPhidget_set_OnError_Handler = (PtrCPhidget_set_OnError_Handler) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidget_set_OnError_Handler");
	
	m_PtrCPhidget_getErrorDescription = (PtrCPhidget_getErrorDescription) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidget_getErrorDescription");
	m_PtrCPhidget_waitForAttachment = (PtrCPhidget_waitForAttachment) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidget_waitForAttachment");

	m_PtrCPhidgetSpatial_create = (PtrCPhidgetSpatial_create) GET_PROC_ADDR(m_handleLibrary, 
                "CPhidgetSpatial_create");
	m_PtrCPhidgetSpatial_getAccelerationAxisCount = (PtrCPhidgetSpatial_getAccelerationAxisCount) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getAccelerationAxisCount");
	m_PtrCPhidgetSpatial_getGyroAxisCount = (PtrCPhidgetSpatial_getGyroAxisCount) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getGyroAxisCount");
	m_PtrCPhidgetSpatial_getCompassAxisCount = (PtrCPhidgetSpatial_getCompassAxisCount) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getCompassAxisCount");
	m_PtrCPhidgetSpatial_getAcceleration = (PtrCPhidgetSpatial_getAcceleration) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getAcceleration");
	m_PtrCPhidgetSpatial_getAccelerationMax = (PtrCPhidgetSpatial_getAccelerationMax) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getAccelerationMax");
	m_PtrCPhidgetSpatial_getAccelerationMin = (PtrCPhidgetSpatial_getAccelerationMin) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getAccelerationMin");
	m_PtrCPhidgetSpatial_getDataRate = (PtrCPhidgetSpatial_getDataRate) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getDataRate");
	m_PtrCPhidgetSpatial_setDataRate = (PtrCPhidgetSpatial_setDataRate) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_setDataRate");
	m_PtrCPhidgetSpatial_getDataRateMax = (PtrCPhidgetSpatial_getDataRateMax) GET_PROC_ADDR(m_handleLibrary, 
               "CPhidgetSpatial_getDataRateMax");
	m_PtrCPhidgetSpatial_getDataRateMin = (PtrCPhidgetSpatial_getDataRateMin) GET_PROC_ADDR(m_handleLibrary, 
                "CPhidgetSpatial_getDataRateMin");
	m_PtrCPhidgetSpatial_set_OnSpatialData_Handler = (PtrCPhidgetSpatial_set_OnSpatialData_Handler) 
              GET_PROC_ADDR(m_handleLibrary, "CPhidgetSpatial_set_OnSpatialData_Handler");
	m_PtrCPhidget_getDeviceID = (PtrCPhidget_getDeviceID) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidget_getDeviceID");

/*
	m_PtrCPhidgetManager_getAttachedDevices = (PtrCPhidgetManager_getAttachedDevices) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidgetManager_getAttachedDevices");
	m_PtrCPhidgetManager_freeAttachedDevicesArray = (PtrCPhidgetManager_freeAttachedDevicesArray) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidgetManager_freeAttachedDevicesArray");
	m_PtrCPhidgetManager_close = (PtrCPhidgetManager_close) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidgetManager_close");
	m_PtrCPhidgetManager_create = (PtrCPhidgetManager_create) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidgetManager_create");
	m_PtrCPhidgetManager_open = (PtrCPhidgetManager_open) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidgetManager_open");
	m_PtrCPhidgetManager_delete = (PtrCPhidgetManager_delete) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidgetManager_delete");
	m_PtrCPhidgetManager_free = (PtrCPhidgetManager_free) GET_PROC_ADDR(m_handleLibrary,
		   "CPhidgetManager_free");
*/
        m_PtrCPhidget_enableLogging = (PtrCPhidget_enableLogging) GET_PROC_ADDR(m_handleLibrary, "CPhidget_enableLogging");
        m_PtrCPhidget_disableLogging = (PtrCPhidget_disableLogging) GET_PROC_ADDR(m_handleLibrary, "CPhidget_disableLogging");
        m_PtrCPhidget_log = (PtrCPhidget_log) GET_PROC_ADDR(m_handleLibrary, "CPhidget_log");

#else   // static lib - just get pointers to the function entry points
        m_PtrCPhidget_open = (PtrCPhidget_open) CPhidget_open;
        m_PtrCPhidget_close = (PtrCPhidget_close) CPhidget_close;
        m_PtrCPhidget_delete = (PtrCPhidget_delete) CPhidget_delete;
        m_PtrCPhidget_getDeviceName = (PtrCPhidget_getDeviceName) CPhidget_getDeviceName;
        m_PtrCPhidget_getSerialNumber = (PtrCPhidget_getSerialNumber) CPhidget_getSerialNumber;
        m_PtrCPhidget_getDeviceVersion = (PtrCPhidget_getDeviceVersion) CPhidget_getDeviceVersion;
        m_PtrCPhidget_getDeviceStatus = (PtrCPhidget_getDeviceStatus) CPhidget_getDeviceStatus;
        m_PtrCPhidget_getLibraryVersion = (PtrCPhidget_getLibraryVersion) CPhidget_getLibraryVersion;
        m_PtrCPhidget_getDeviceType = (PtrCPhidget_getDeviceType) CPhidget_getDeviceType;
        m_PtrCPhidget_getDeviceLabel = (PtrCPhidget_getDeviceLabel) CPhidget_getDeviceLabel;

        m_PtrCPhidget_set_OnAttach_Handler = (PtrCPhidget_set_OnAttach_Handler) CPhidget_set_OnAttach_Handler;
        m_PtrCPhidget_set_OnDetach_Handler = (PtrCPhidget_set_OnDetach_Handler) CPhidget_set_OnDetach_Handler;
        m_PtrCPhidget_set_OnError_Handler = (PtrCPhidget_set_OnError_Handler) CPhidget_set_OnError_Handler;

        m_PtrCPhidget_getErrorDescription = (PtrCPhidget_getErrorDescription) CPhidget_getErrorDescription;
        m_PtrCPhidget_waitForAttachment = (PtrCPhidget_waitForAttachment) CPhidget_waitForAttachment;

        m_PtrCPhidgetSpatial_create = (PtrCPhidgetSpatial_create) CPhidgetSpatial_create;
        m_PtrCPhidgetSpatial_getAccelerationAxisCount = (PtrCPhidgetSpatial_getAccelerationAxisCount) CPhidgetSpatial_getAccelerationAxisCount;
        m_PtrCPhidgetSpatial_getGyroAxisCount = (PtrCPhidgetSpatial_getGyroAxisCount) CPhidgetSpatial_getGyroAxisCount;
        m_PtrCPhidgetSpatial_getCompassAxisCount = (PtrCPhidgetSpatial_getCompassAxisCount) CPhidgetSpatial_getCompassAxisCount;
        m_PtrCPhidgetSpatial_getAcceleration = (PtrCPhidgetSpatial_getAcceleration) CPhidgetSpatial_getAcceleration;
        m_PtrCPhidgetSpatial_getAccelerationMax = (PtrCPhidgetSpatial_getAccelerationMax) CPhidgetSpatial_getAccelerationMax;
        m_PtrCPhidgetSpatial_getAccelerationMin = (PtrCPhidgetSpatial_getAccelerationMin) CPhidgetSpatial_getAccelerationMin;
        m_PtrCPhidgetSpatial_getDataRate = (PtrCPhidgetSpatial_getDataRate) CPhidgetSpatial_getDataRate;
        m_PtrCPhidgetSpatial_setDataRate = (PtrCPhidgetSpatial_setDataRate) CPhidgetSpatial_setDataRate;
        m_PtrCPhidgetSpatial_getDataRateMax = (PtrCPhidgetSpatial_getDataRateMax) CPhidgetSpatial_getDataRateMax;
        m_PtrCPhidgetSpatial_getDataRateMin = (PtrCPhidgetSpatial_getDataRateMin) CPhidgetSpatial_getDataRateMin;
        m_PtrCPhidgetSpatial_set_OnSpatialData_Handler = (PtrCPhidgetSpatial_set_OnSpatialData_Handler)
              CPhidgetSpatial_set_OnSpatialData_Handler;
        m_PtrCPhidget_getDeviceID = (PtrCPhidget_getDeviceID) 
                   CPhidget_getDeviceID;

/*
        m_PtrCPhidgetManager_getAttachedDevices = (PtrCPhidgetManager_getAttachedDevices) 
                   CPhidgetManager_getAttachedDevices;
        m_PtrCPhidgetManager_freeAttachedDevicesArray = (PtrCPhidgetManager_freeAttachedDevicesArray) 
                   CPhidgetManager_freeAttachedDevicesArray;
        m_PtrCPhidgetManager_close = (PtrCPhidgetManager_close) 
                   CPhidgetManager_close;
        m_PtrCPhidgetManager_create = (PtrCPhidgetManager_create) 
                   CPhidgetManager_create;
        m_PtrCPhidgetManager_open = (PtrCPhidgetManager_open) 
                   CPhidgetManager_open;
        m_PtrCPhidgetManager_delete = (PtrCPhidgetManager_delete) 
                   CPhidgetManager_delete;
        m_PtrCPhidgetManager_free = (PtrCPhidgetManager_free) 
                   CPhidgetManager_free;
*/
        m_PtrCPhidget_enableLogging = (PtrCPhidget_enableLogging) CPhidget_enableLogging;
        m_PtrCPhidget_disableLogging = (PtrCPhidget_disableLogging) CPhidget_disableLogging;
        m_PtrCPhidget_log = (PtrCPhidget_log) CPhidget_log;

#endif

	// test that some choice functions aren't null
	return (bool) (m_PtrCPhidget_open && m_PtrCPhidget_close && m_PtrCPhidget_waitForAttachment && m_PtrCPhidget_set_OnAttach_Handler 
				   && m_PtrCPhidgetSpatial_getAccelerationAxisCount && m_PtrCPhidgetSpatial_getAcceleration && m_PtrCPhidgetSpatial_getDataRate);
}



void CSensorUSBPhidgets::closePort()
{
	if (m_handlePhidgetSpatial) {
		m_PtrCPhidget_close((CPhidgetHandle) m_handlePhidgetSpatial);
		m_PtrCPhidget_delete((CPhidgetHandle) m_handlePhidgetSpatial);
		m_handlePhidgetSpatial = NULL;
	}

	if (m_handleLibrary) {
#ifdef __USE_DLOPEN__
        if (dlclose(m_handleLibrary)) {
           fprintf(stderr, "%s: dlclose error %s\n", getTypeStr(), dlerror());
        }
#else // probably Windows - free library
   #ifdef _WIN32
        ::FreeLibrary(m_handleLibrary);
   #endif
#endif
		m_handleLibrary = NULL;
    }
	
	m_iSerialNum = 0;
	m_iVersion = 0;
	m_iNumAccelAxes = 0;
	m_iNumGyroAxes = 0;
	m_iNumCompassAxes = 0;
	m_iDataRateMax = 0;
	m_iDataRateMin = 0;
	
	setPort();
	setType();

}

/*
bool CSensorUSBPhidgets::getList()
{
  CPhidgetManagerHandle manager = NULL;
  CPhidgetManager_create(&manager);
  CPhidgetManager_open((CPhidgetManagerHandle) manager);
 
  // Allow the Phidgets time to attach
  sleep(1);
 
  // Retrieve the list of attached Phidgets from the manager
  CPhidgetHandle* phidgetList;
  int count;
 
  CPhidgetManager_getAttachedDevices((CPhidgetManagerHandle) manager, &phidgetList, &count);
 
  int serialNumber;
  const char *name;
 
  // Iterate over the returned Phidget data
  int i;
  for (i = 0; i < count; i++) {
      CPhidget_getDeviceName(phidgetList[i], &name);
      CPhidget_getSerialNumber(phidgetList[i], &serialNumber);
      printf("%s, %d\n", name, serialNumber);
      // Store name and serial number into a persistent variable
      ....
  }
 
  // Use the Phidget API to free the memory in the phidgetList Array    
  CPhidgetManager_freeAttachedDevicesArray(phidgetList);
 
  // Close the manager    
  CPhidgetManager_close((CPhidgetManagerHandle) manager);
  CPhidgetManager_delete((CPhidgetManagerHandle) manager);
}
*/

bool CSensorUSBPhidgets::detect()
{
	int ret;
	float x,y,z; //test read_xyz


   setType();
   setPort();

   if (qcn_main::g_iStop) return false;

	// check for stop signal and function pointers
	if (qcn_main::g_iStop || ! setupFunctionPointers()) return false;

// log Linux
/*
#if !defined(__APPLE_CC__) && !defined(_WIN32)	
       if (!m_bLogging) {
          m_PtrCPhidget_enableLogging(PHIDGET_LOG_VERBOSE, "phidget.txt");
          m_bLogging = true;
       }
#endif
*/
	//Declare a spatial handle
	m_handlePhidgetSpatial = NULL;
	
	//create the spatial object
	m_PtrCPhidgetSpatial_create(&m_handlePhidgetSpatial);
	if (!m_handlePhidgetSpatial) return false; // can't create spatial handle
	
	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	//CPhidget_set_OnAttach_Handler((CPhidgetHandle) m_handlePhidgetSpatial, AttachHandler, NULL);
	//CPhidget_set_OnDetach_Handler((CPhidgetHandle) m_handlePhidgetSpatial, DetachHandler, NULL);
	//CPhidget_set_OnError_Handler((CPhidgetHandle) m_handlePhidgetSpatial, ErrorHandler, NULL);
	
	//Registers a callback that will run according to the set data rate that will return the spatial data changes
	//Requires the handle for the Spatial, the callback handler function that will be called, 
	//and an arbitrary pointer that will be supplied to the callback function (may be NULL)
	//CPhidgetSpatial_set_OnSpatialData_Handler(m_handlePhidgetSpatial, SpatialDataHandler, NULL);
	
	//open the spatial object for device connections
	if ((ret = m_PtrCPhidget_open((CPhidgetHandle) m_handlePhidgetSpatial, -1))) {
	        const char *err;
		m_PtrCPhidget_getErrorDescription(ret, &err);
		fprintf(stderr, "Phidgets error open handle %d = %s\n", ret, err);
		closePort();
		return false;
	}
	
	// try a second to open
	double dTime = dtime();

    if((ret = m_PtrCPhidget_waitForAttachment((CPhidgetHandle)m_handlePhidgetSpatial, 2000))) {
	        const char *err;
		m_PtrCPhidget_getErrorDescription(ret, &err);
//#if !defined(_WIN32) && !defined(__APPLE_CC__)
#ifdef _DEBUG
		fprintf(stderr, "Phidgets error waitForAttachment %d = %s\n", ret, err);
#endif
//#endif
		closePort();
		return false;
	}
	
	//Display the properties of the attached spatial device
	//display_properties((CPhidgetHandle)spatial);
	
	//Set the data rate for the spatial events
	// CPhidgetSpatial_setDataRate(spatial, 16);
	//Display the properties of the attached phidget to the screen.  
	//We will be displaying the name, serial number, version of the attached device, the number of accelerometer, gyro, and compass Axes, and the current data rate
	// of the attached Spatial.

	m_PtrCPhidget_getSerialNumber((CPhidgetHandle) m_handlePhidgetSpatial, &m_iSerialNum);
	m_PtrCPhidget_getDeviceVersion((CPhidgetHandle) m_handlePhidgetSpatial, &m_iVersion);
	m_PtrCPhidgetSpatial_getAccelerationAxisCount(m_handlePhidgetSpatial, &m_iNumAccelAxes);
	m_PtrCPhidgetSpatial_getGyroAxisCount(m_handlePhidgetSpatial, &m_iNumGyroAxes);
	m_PtrCPhidgetSpatial_getCompassAxisCount(m_handlePhidgetSpatial, &m_iNumCompassAxes);
	m_PtrCPhidgetSpatial_getDataRateMax(m_handlePhidgetSpatial, &m_iDataRateMax);
	m_PtrCPhidgetSpatial_getDataRateMin(m_handlePhidgetSpatial, &m_iDataRateMin);
	m_PtrCPhidget_getDeviceName((CPhidgetHandle) m_handlePhidgetSpatial, &m_cstrDeviceName);
	m_PtrCPhidget_getDeviceType((CPhidgetHandle) m_handlePhidgetSpatial, &m_cstrDeviceType); 
	m_PtrCPhidget_getDeviceLabel((CPhidgetHandle) m_handlePhidgetSpatial, &m_cstrDeviceLabel); 
	m_PtrCPhidget_getDeviceID((CPhidgetHandle) m_handlePhidgetSpatial, &m_enumPhidgetDeviceID);

	if (m_iNumAccelAxes < 1) { // error as we should have 1 - 3 axes
		fprintf(stderr, "Error - Phidgets Accel with %d axes\n", m_iNumAccelAxes);
		closePort();
		return false;
	}

/*  per Phidgets forum re version # :
- with gyro & compass
1056 is <300
1042 is 300 - <400
1044 is 400 - <500

- no gyro or compass
1041 is <300
1043 is <400

*/

   if (m_iNumGyroAxes == 0 && m_iNumCompassAxes == 0) {
            if (m_iVersion < 300)
              setType(SENSOR_USB_PHIDGETS_1041);
            else if (m_iVersion >= 300 && m_iVersion < 400)
              setType(SENSOR_USB_PHIDGETS_1043);
            else {
              fprintf(stderr, "Error - Unknown Phidgets Accel with %d axes - Version %d\n", m_iNumAccelAxes, m_iVersion);
              closePort();
               return false;
            }
    }
    else {
            if (m_iVersion < 300)
              setType(SENSOR_USB_PHIDGETS_1056);
            else if (m_iVersion >= 300 && m_iVersion < 400)
              setType(SENSOR_USB_PHIDGETS_1042);
            else
              setType(SENSOR_USB_PHIDGETS_1044);
    }
    setPort(getTypeEnum());

	char *strSensor = new char[256];

	sprintf(strSensor, "%s (Serial # %d)", getTypeStr(), m_iSerialNum);
	setSensorStr(strSensor);
	delete [] strSensor;
	fprintf(stdout, "%s detected in %f milliseconds\n", getSensorStr(), (dtime() - dTime) * 1000.0);

   // OK, at this point we should be connected, so from here on out can just read_xyz until closePort()
   // set as a single sample per point
   setSingleSampleDT(false);  // mn samples itself

   // NB: closePort resets the type & port, so have to set again 
	
   // one last sanity check, test the xyz reading
   if (!read_xyz(x,y,z)) {
	   closePort();
	   return false;
   }

   // last setup a detach callback function if device is removed
   m_PtrCPhidget_set_OnAttach_Handler((CPhidgetHandle) m_handlePhidgetSpatial, PhidgetsAttachHandler, NULL);
   m_PtrCPhidget_set_OnDetach_Handler((CPhidgetHandle) m_handlePhidgetSpatial, PhidgetsDetachHandler, NULL);
	
#if !defined(__APPLE_CC__) && !defined(_WIN32)	
       if (m_bLogging) {
           m_PtrCPhidget_disableLogging();
           m_bLogging = false;
       }
#endif

   return true;
}

inline bool CSensorUSBPhidgets::read_xyz(float& x1, float& y1, float& z1)
{
	if (qcn_main::g_iStop || !m_handlePhidgetSpatial) return false; // invalid handle
	// NB: acceleration is in G's already so just multiply by Earth g 9.8
	double x = 0., y = 0., z = 0.;
	m_PtrCPhidgetSpatial_getAcceleration(m_handlePhidgetSpatial, 0, &x);
	m_PtrCPhidgetSpatial_getAcceleration(m_handlePhidgetSpatial, 1, &y);
	m_PtrCPhidgetSpatial_getAcceleration(m_handlePhidgetSpatial, 2, &z);
	x1 = (float) (x * EARTH_G);
	y1 = (float) (y * EARTH_G);
	z1 = (float) (z * -EARTH_G);  // note the minus sign as the phidgets by default is flipped on the vertical from usual QCN sensors
    return true;
}
