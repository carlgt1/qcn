/*
 *  csensor_win_laptop_thinkpad.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for Windows Lenovo Thinkpad sensor classes
 */

#include "main.h"
#include "csensor_win_laptop_thinkpad.h"

// Thinkpad DLL to access, they also must be running the Thinkpad service
#ifdef _WIN64  // seems to be a different name on Win64
const char CSensorWinThinkpad::m_cstrDLL[] = {"sensor64.dll"};   // this would have to be in the PATH
#else
const char CSensorWinThinkpad::m_cstrDLL[] = {"sensor.dll"};   // this would have to be in the PATH
#endif

CSensorWinThinkpad::CSensorWinThinkpad()
  : CSensor(), m_WinDLLHandle(NULL), m_getDataThinkpad(NULL)
{ 
}

CSensorWinThinkpad::~CSensorWinThinkpad()
{
  closePort();
}

void CSensorWinThinkpad::closePort()
{
    // close thinkpad dll
    if (m_WinDLLHandle) {
	    ::FreeLibrary(m_WinDLLHandle);
	    //::CloseHandle(m_WinDLLHandle);
	    m_WinDLLHandle = NULL;
    }
    if (getPort() > -1) {
        fprintf(stdout, "Port closed!\n");
        fflush(stdout);
        setPort();
    }
}

bool CSensorWinThinkpad::detect()
{
	 // basically, just try to open the sensor.dll and read a value, if it fails, they don't have a thinkpad
 
    // Load DLL file
	m_WinDLLHandle = ::LoadLibrary(m_cstrDLL);

    if (m_WinDLLHandle == NULL) {
       return false;
    }
 
    // Get function pointer
	m_getDataThinkpad = (ThinkpadImportFunction) ::GetProcAddress(m_WinDLLHandle, "ShockproofGetAccelerometerData");
    if (m_getDataThinkpad == NULL) {
		::FreeLibrary(m_WinDLLHandle);
        return false;
    }

	try {
		memset(&m_ThinkpadData, 0x00, sizeof(m_ThinkpadData));
		m_getDataThinkpad(&m_ThinkpadData);
	}
	catch(...) {
            FreeLibrary(m_WinDLLHandle);
            return false;
	}

    //FreeLibrary(libraryHandle);
    setType(SENSOR_WIN_THINKPAD);
	setPort((int) getTypeEnum());
        setSingleSampleDT(false);
	
	fprintf(stdout, "Thinkpad sensor detected.\n");
	return true;
}

bool CSensorWinThinkpad::read_xyz(float& x1, float& y1, float& z1)
{
   // note x/y/z values should be +/-2g where g = 9.78 (see define.h:: EARTH_G)
	bool bRetVal = true;
	try {
		memset(&m_ThinkpadData, 0x00, sizeof(m_ThinkpadData));
		m_getDataThinkpad(&m_ThinkpadData);
#ifdef QCN_RAW_DATA
		x1 = (float) m_ThinkpadData.x;
		y1 = (float) m_ThinkpadData.y;
		z1 = 0.0f;
#else
		x1 = (((float) m_ThinkpadData.x - 512.0f) / 256.0f) * EARTH_G;
		y1 = (((float) m_ThinkpadData.y - 512.0f) / 256.0f) * EARTH_G;
		z1 = 0.0f;
#endif
	}
	catch(...) {
		bRetVal = false;
	}
	return bRetVal;
}

