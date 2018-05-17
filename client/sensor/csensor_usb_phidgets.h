#ifndef _CSENSOR_USB_PHIDGETS_H_
#define _CSENSOR_USB_PHIDGETS_H_

/*
 *  csensor_usb_phidgets_1056.h
 *  qcn
 *
 *  Created by Carl Christensen on 10/06/2008
 *  Copyright 2008 Stanford University
 *
 * This file contains the declarations for the CSensor-derived class for the Phidgets 1056 USB accelerometer
 *     http://www.phidgets.com/products.php?product_id=1056

 */

#include <stdio.h>
#ifdef _WIN32
#include <phidgets/phidget21win.h>
#else
#include <phidgets/phidget21.h>
#endif

#include "main.h"

using namespace std;

#if defined(WIN32)
#  if defined(BUILD_ACCEL_API)
#    define PHIDGETS_IMPORT_API __declspec(dllexport)
#  else
#    define PHIDGETS_IMPORT_API __declspec(dllimport)
#  endif // BUILD_ACCEL_API
#  define PHIDGETS_CALL_API __stdcall
#  define PHIDGETS_CALL_C_API __cdecl
#else
#  define PHIDGETS_IMPORT_API
#  define PHIDGETS_CALL_API
#  define PHIDGETS_CALL_C_API
#endif // WIN32

// detach handler is kept in main/qcn_thread_sensor_util.cpp to handle closing of the port etc
extern int CCONV PhidgetsAttachHandler(CPhidgetHandle spatial, void *userPtr);
extern int CCONV PhidgetsDetachHandler(CPhidgetHandle spatial, void *userPtr);

// function pointers in the Phidgets shared object/dylib/DLL
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_open)  (CPhidgetHandle phid, int serialNumber);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_close) (CPhidgetHandle phid);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_delete) (CPhidgetHandle phid);

typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_waitForAttachment)  (CPhidgetHandle phid, int milliseconds);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getDeviceName)  (CPhidgetHandle phid, const char **deviceName);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getSerialNumber)  (CPhidgetHandle phid, int *serialNumber);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getDeviceVersion)  (CPhidgetHandle phid, int *deviceVersion);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getDeviceStatus)  (CPhidgetHandle phid, int *deviceStatus);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getLibraryVersion)  (const char **libraryVersion);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getDeviceType)  (CPhidgetHandle phid, const char **deviceType);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getDeviceLabel) (CPhidgetHandle phid, const char **deviceLabel);
typedef int     (PHIDGETS_CALL_API * PtrCPhidget_getDeviceID)    (CPhidgetHandle phid, CPhidget_DeviceID *deviceID);

/*
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetManager_getAttachedDevices)  (CPhidgetManagerHandle phidm, CPhidgetHandle *phidArray[], int *count);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetManager_freeAttachedDevicesArray) (CPhidgetHandle phidArray[]);
typedef int     (PHIDGETS_CALL_API * PtrCPhidgetManager_close) (CPhidgetManagerHandle phidm);
typedef int     (PHIDGETS_CALL_API * PtrCPhidgetManager_create) (CPhidgetManagerHandle *phidm);
typedef int     (PHIDGETS_CALL_API * PtrCPhidgetManager_open) (CPhidgetManagerHandle phidm);
typedef int     (PHIDGETS_CALL_API * PtrCPhidgetManager_delete) (CPhidgetManagerHandle phidm);
typedef void 	(PHIDGETS_CALL_API * PtrCPhidgetManager_free) (void *arg);
*/

// callback function handlers
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_set_OnDetach_Handler) (CPhidgetHandle phid, int(CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_set_OnAttach_Handler) (CPhidgetHandle phid, int(CCONV *fptr)(CPhidgetHandle phid, void *userPtr), void *userPtr);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_set_OnError_Handler) (CPhidgetHandle phid, int(CCONV *fptr)(CPhidgetHandle phid, void *userPtr, int errorCode, const char *errorString), void *userPtr);

// CPhidgetSpacial specific functions
typedef int		(PHIDGETS_CALL_API * PtrCPhidgetSpatial_create) (CPhidgetSpatialHandle *phid);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidget_getErrorDescription) (int errorCode, const char **errorString);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getAccelerationAxisCount) (CPhidgetSpatialHandle phid, int *count);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getGyroAxisCount) (CPhidgetSpatialHandle phid, int *count);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getCompassAxisCount) (CPhidgetSpatialHandle phid, int *count);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getAcceleration) (CPhidgetSpatialHandle phid, int index, double *acceleration);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getAccelerationMax) (CPhidgetSpatialHandle phid, int index, double *max);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getAccelerationMin) (CPhidgetSpatialHandle phid, int index, double *min);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getDataRate) (CPhidgetSpatialHandle phid, int *milliseconds);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_setDataRate) (CPhidgetSpatialHandle phid, int milliseconds);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getDataRateMax) (CPhidgetSpatialHandle phid, int *max);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_getDataRateMin) (CPhidgetSpatialHandle phid, int *min);
typedef int 	(PHIDGETS_CALL_API * PtrCPhidgetSpatial_set_OnSpatialData_Handler) (CPhidgetSpatialHandle phid, CPhidget_DeviceID *deviceID);

// some useful logging stuff
typedef int     (PHIDGETS_CALL_API * PtrCPhidget_enableLogging) (CPhidgetLog_level level, const char *outputFile);
typedef int     (PHIDGETS_CALL_API * PtrCPhidget_disableLogging) ();
typedef int     (PHIDGETS_CALL_API * PtrCPhidget_log) (CPhidgetLog_level level, const char *id, const char *message, ...);


// this is the Windows implementation of the sensor - IBM/Lenovo Thinkpad, HP, USB Stick
class CSensorUSBPhidgets  : public CSensor
{
  private:
	int m_iSerialNum;
	int m_iVersion;
	int m_iNumAccelAxes;
	int m_iNumGyroAxes;
	int m_iNumCompassAxes;
	int m_iDataRateMax;
	int m_iDataRateMin;
        bool m_bLogging;
	int m_iModelSearch;
	const char* m_cstrDeviceType;
	const char* m_cstrDeviceLabel;
	const char* m_cstrDeviceName;
	CPhidget_DeviceID m_enumPhidgetDeviceID;

	CPhidgetSpatialHandle m_handlePhidgetSpatial;
	
	static const char m_cstrDLL[];
	
	
#ifdef _WIN32
	HMODULE m_handleLibrary;
#else
	void* m_handleLibrary;
#endif
	
	
	PtrCPhidget_open m_PtrCPhidget_open;
	PtrCPhidget_close m_PtrCPhidget_close;
	PtrCPhidget_delete m_PtrCPhidget_delete;
	PtrCPhidget_waitForAttachment m_PtrCPhidget_waitForAttachment;
	PtrCPhidget_getDeviceName m_PtrCPhidget_getDeviceName;
	PtrCPhidget_getSerialNumber m_PtrCPhidget_getSerialNumber;
	PtrCPhidget_getDeviceVersion m_PtrCPhidget_getDeviceVersion;
	PtrCPhidget_getDeviceStatus m_PtrCPhidget_getDeviceStatus;
	PtrCPhidget_getLibraryVersion m_PtrCPhidget_getLibraryVersion;
	PtrCPhidget_getDeviceType m_PtrCPhidget_getDeviceType;
	PtrCPhidget_getDeviceLabel m_PtrCPhidget_getDeviceLabel;
	PtrCPhidget_getDeviceID m_PtrCPhidget_getDeviceID;

/*
        PtrCPhidgetManager_getAttachedDevices m_PtrCPhidgetManager_getAttachedDevices;
        PtrCPhidgetManager_freeAttachedDevicesArray m_PtrCPhidgetManager_freeAttachedDevicesArray;
        PtrCPhidgetManager_close m_PtrCPhidgetManager_close;
        PtrCPhidgetManager_create m_PtrCPhidgetManager_create;
        PtrCPhidgetManager_open m_PtrCPhidgetManager_open;
        PtrCPhidgetManager_delete m_PtrCPhidgetManager_delete;
        PtrCPhidgetManager_free m_PtrCPhidgetManager_free;
*/
	
	// callback function handlers	
	PtrCPhidget_set_OnAttach_Handler m_PtrCPhidget_set_OnAttach_Handler;
	PtrCPhidget_set_OnDetach_Handler m_PtrCPhidget_set_OnDetach_Handler;
	PtrCPhidget_set_OnError_Handler m_PtrCPhidget_set_OnError_Handler;
	PtrCPhidgetSpatial_set_OnSpatialData_Handler m_PtrCPhidgetSpatial_set_OnSpatialData_Handler;
	
	PtrCPhidgetSpatial_create m_PtrCPhidgetSpatial_create;
	PtrCPhidget_getErrorDescription m_PtrCPhidget_getErrorDescription;
	PtrCPhidgetSpatial_getAccelerationAxisCount m_PtrCPhidgetSpatial_getAccelerationAxisCount;
	PtrCPhidgetSpatial_getGyroAxisCount m_PtrCPhidgetSpatial_getGyroAxisCount;
	PtrCPhidgetSpatial_getCompassAxisCount m_PtrCPhidgetSpatial_getCompassAxisCount;
	PtrCPhidgetSpatial_getAcceleration m_PtrCPhidgetSpatial_getAcceleration;
	PtrCPhidgetSpatial_getAccelerationMax m_PtrCPhidgetSpatial_getAccelerationMax;
	PtrCPhidgetSpatial_getAccelerationMin m_PtrCPhidgetSpatial_getAccelerationMin;
	PtrCPhidgetSpatial_getDataRate m_PtrCPhidgetSpatial_getDataRate;
	PtrCPhidgetSpatial_setDataRate m_PtrCPhidgetSpatial_setDataRate;
	PtrCPhidgetSpatial_getDataRateMax m_PtrCPhidgetSpatial_getDataRateMax;
	PtrCPhidgetSpatial_getDataRateMin m_PtrCPhidgetSpatial_getDataRateMin;
	
	PtrCPhidget_enableLogging m_PtrCPhidget_enableLogging;
	PtrCPhidget_disableLogging m_PtrCPhidget_disableLogging;
	PtrCPhidget_log m_PtrCPhidget_log;
	
	bool setupFunctionPointers();
	virtual bool read_xyz(float& x1, float& y1, float& z1);  

	
   public:
      CSensorUSBPhidgets(int iModel = 1056);
      virtual ~CSensorUSBPhidgets();
      virtual void closePort(); // closes the port if open
      virtual bool detect();   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found
      //bool getList();   // get a list of attached phidgets devices
	
};

#endif

