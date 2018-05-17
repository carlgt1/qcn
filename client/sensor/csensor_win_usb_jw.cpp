/*
 *  csensor_win_usb_jw.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for Windows USB JoyWarrior sensor class
 */

#include "main.h"
#include "csensor_win_usb_jw.h"

CSensorWinUSBJW::CSensorWinUSBJW()
  : CSensor(), m_USBHandle(NULL)
{ 
   m_USBDevHandle[0] = NULL;
   m_USBDevHandle[1] = NULL;
}

CSensorWinUSBJW::~CSensorWinUSBJW()
{
  closePort();
}

void CSensorWinUSBJW::closePort()
{
  for (int i = 0; i < 2; i++) {
     if (m_USBDevHandle[i]) {
       try {
          // don't think we need the next line, just close & Release
      WriteData(m_USBDevHandle[i], 0x02, 0x00, 0x00);  // Free JW
	  ::CancelIo(m_USBDevHandle[i]);
	  ::CloseHandle(m_USBDevHandle[i]);
	  m_USBDevHandle[i] = NULL;
          // mac version:
          //WriteData(m_USBDevHandle[i], 0x02, 0x00, 0x00, "closePort()::Free JW");
          //(*m_USBDevHandle[i])->close(m_USBDevHandle[i]);
          //(*m_USBDevHandle[i])->Release(m_USBDevHandle[i]);
          //m_USBDevHandle[i] = NULL;
        }
        catch(...) {
            fprintf(stderr, "Could not close JoyWarrior USB port %d...\n", i);
        }
     }
  }

  if (m_USBHandle) {
		::CloseHandle(m_USBHandle);
		m_USBHandle = NULL;
  }

  if (getPort() > -1) {
    setPort();
    fprintf(stdout, "Port closed!\n");
    fflush(stdout);
  }
}

bool CSensorWinUSBJW::detect()
{
    // tries to detect & initialize the USB JW Sensor
	setType();
    e_sensor esTmp = SENSOR_NOTFOUND; 
	int iPort = -1;  
	if (m_USBHandle) {
		::CloseHandle(m_USBHandle);
		m_USBHandle = NULL;
	}

   // enumerate usb ports looking for the joywarrior or mousewarrior device
   // taken from the codemercs CNeigungswinkelDlg class

	HIDD_ATTRIBUTES	Attributes;
	SP_DEVICE_INTERFACE_DATA devInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	HDEVINFO hDevInfo;

	bool bStart = false;
	int	MemberIndex = 0;
	int	DeviceIndex = 0;
	LONG DevInfo;
	ULONG Length = 0;
	ULONG Required;
	GUID HidGuid;

    ZeroMemory(&devInfoData, sizeof(devInfoData));
    devInfoData.cbSize = sizeof(devInfoData);

	HidD_GetHidGuid(&HidGuid);	

	hDevInfo = SetupDiGetClassDevsW(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	do
	{
		DevInfo = SetupDiEnumDeviceInterfaces (hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData);

		if (DevInfo != 0)
		{
			SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);

			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(Length);
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			//SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInfoData, detailData, Length, &Required, NULL);
			SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length, &Required, NULL);

			m_USBHandle = ::CreateFile (detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);

			Attributes.Size = sizeof(Attributes);

			HidD_GetAttributes(m_USBHandle, &Attributes);

			if (m_USBHandle && Attributes.VendorID == USB_VENDORID_JW) 
			{
				if(Attributes.ProductID == USB_DEVICEID_JW24F8)
				{
					//if(Attributes.ProductID == USB_JOYWARRIOR) GetDlgItem(IDC_STATIC_DEVICE)->SetWindowTextW(_T("JoyWarrior"));
					//if(Attributes.ProductID == USB_MOUSEWARRIOR) GetDlgItem(IDC_STATIC_DEVICE)->SetWindowTextW(_T("MouseWarrior"));

					GetCapabilities(m_USBHandle);

					//m_USBDevHandle[DeviceIndex] = CreateFileW(detailData->DevicePath, 
					m_USBDevHandle[DeviceIndex] = CreateFile(detailData->DevicePath, 
														GENERIC_WRITE | GENERIC_READ, 
														FILE_SHARE_READ|FILE_SHARE_WRITE, 
														(LPSECURITY_ATTRIBUTES)NULL, 
														OPEN_EXISTING, 
														0, 
														NULL);

					DeviceIndex++;
					//GetDlgItem(IDC_STATIC_SERIAL)->SetWindowTextW(GetSerialNumber(m_USBDevHandle[1]));
					bStart = true;
				}
				else
					NULL;
			}
			else {
				if (m_USBHandle) {
					::CloseHandle(m_USBHandle);
					m_USBHandle = NULL;
				}
			}

			free(detailData);
		}

		MemberIndex++;

	} while (DevInfo != NULL);

	SetupDiDestroyDeviceInfoList(hDevInfo);

	if (bStart && SetQCNState() && SetupJoystick() >= 0) {
		esTmp = SENSOR_USB_JW24F8;
		iPort = getPort();
        // no single sample, JW actually needs to sample within the 50hz,
        // since we're reading from joystick port, not the downsampling "chip"
#ifdef QCN_RAW_DATA
   setSingleSampleDT(true); // set to true in raw mode so we don't get any interpolated/avg points (i.e. just the "integer" value hopefully)
#else
   setSingleSampleDT(false);
#endif
	fprintf(stdout, "JoyWarrior 24F8 USB sensor detected on Windows joystick port %d\n"
			"Set to 50Hz internal bandwidth, +/- 2g acceleration.\n", getPort());
	}

    closePort();  // close the HID USB stuff and just use joystick calls from here on out

	// NB: closePort resets the type & port, so have to set again 
    setType(esTmp);
	setPort(iPort);

	return (bool)(getTypeEnum() == SENSOR_USB_JW24F8);
}

// USB stick accelerometer specific stuff (codemercs.com JoyWarrior 24F8)
// http://codemercs.com/JW24F8_E.html

void CSensorWinUSBJW::GetCapabilities(HANDLE handle)
{
	PHIDP_PREPARSED_DATA PreparsedData;
	::HidD_GetPreparsedData(handle, &PreparsedData);
	::HidP_GetCaps(PreparsedData, &m_USBCapabilities);
	::HidD_FreePreparsedData(PreparsedData);
}

int CSensorWinUSBJW::SetupJoystick()
{
	const int cnumJoy = ::joyGetNumDevs();
	LPJOYCAPS pjc = new JOYCAPS;
	const int isizeJC = sizeof(JOYCAPS);
	int i;
	setPort();
	// enumerate joysticks and find a match for the JoyWarrior
	for (i = JOYSTICKID1; i < JOYSTICKID1 + cnumJoy; i++)  {
		memset(pjc, 0x00, isizeJC);
		if (::joyGetDevCaps(i, pjc, isizeJC) == JOYERR_NOERROR) {
			// see if it matches up to the Product & Vendor ID for codemercs.com JoyWarrior
			if (pjc->wMid == USB_VENDORID_JW && pjc->wPid == USB_DEVICEID_JW24F8) {
				// this is the joystick
				setPort(i);
				break;
			}
		}
	}
	delete pjc;
	if (i == cnumJoy) setPort();  // error, didn't break
	return getPort();
}

/* 
// not using joystick interface
inline bool CSensorWinUSBJW::read_xyz(float& x1, float& y1, float& z1)
{
#ifdef _DEBUG
	static int x_min = 10000, x_max = -10000;
#endif

	// joystick fn usage
	x1=y1=z1=0.0f;

	if (!m_USBDevHandle[1]) return false;
        unsigned char rawData[6];
        int i, x, y ,z;
		i=x=y=z=0;
        for (i = 0; i < 6; i++)
        {
             rawData[i] = ReadData(m_USBDevHandle[1], 0x02 + i);
        }

		x = CalcMsbLsb(rawData[0], rawData[1]);
		y = CalcMsbLsb(rawData[2], rawData[3]);
		z = CalcMsbLsb(rawData[4], rawData[5]);

#ifdef _DEBUG
		// range seems to be -512 to 511 inclusive
	if (x > x_max) x_max = x;
	if (x < x_min) x_min = x;
#endif

#ifdef QCN_RAW_DATA
	x1 = (float) x;
	y1 = (float) y;
	z1 = (float) z;
#else           
	x1 = ((((float) x)) / (x>0 ? 255.5f : 256.0f)) * EARTH_G;
	y1 = ((((float) y)) / (y>0 ? 255.5f : 256.0f)) * EARTH_G;
	z1 = ((((float) z)) / (z>0 ? 255.5f : 256.0f)) * EARTH_G;	
#endif 


	return true;
}
*/

// using joystick interface
inline bool CSensorWinUSBJW::read_xyz(float& x1, float& y1, float& z1)
{
	// joystick fn usage
	x1=y1=z1=0.0f;
	if (getPort() < 0) return false;

	static JOYINFOEX jix;
	static int iSize = sizeof(JOYINFOEX);

	memset(&jix, 0x00, iSize);
	jix.dwSize = iSize;
	//jix.dwFlags = JOY_RETURNALL; // JOY_RETURNRAWDATA; // JOY_RETURNALL; // JOY_CAL_READ5; //JOY_RETURNRAWDATA | JOY_RETURNALL; // JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ;
    jix.dwFlags = JOY_CAL_READ5; // read 5 axes calibration info
	MMRESULT mres = ::joyGetPosEx(getPort(), &jix);
        // note x/y/z values should be +/-2g where g = 9.78 (see define.h:: EARTH_G)
	if (mres == JOYERR_NOERROR) { // successfully read the joystick, -2g = 0, 0g = 32767, 2g = 65535
#ifdef QCN_RAW_DATA
		x1 = (float) jix.dwXpos;
		y1 = (float) jix.dwYpos;
		z1 = (float) jix.dwZpos;
#else
		x1 = (((float) jix.dwXpos - 32767.5f) / 16383.75f) * EARTH_G;
		y1 = (((float) jix.dwYpos - 32767.5f) / 16383.75f) * EARTH_G;
		z1 = (((float) jix.dwZpos - 32767.5f) / 16383.75f) * EARTH_G;
#endif
	}
#ifdef _DEBUG
	else {
	DWORD dw = GetLastError();
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + 48) * sizeof(TCHAR)); 
    sprintf((LPTSTR)lpDisplayBuf, 
		"%s failed with error %d: %s", 
        "JW24F14", dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
	}

#endif

	return true;
}


// USB read function
unsigned char CSensorWinUSBJW::ReadData(HANDLE handle, unsigned char addr)
{
	unsigned char			WriteBuffer[10];
	unsigned char			ReadBuffer[10];
	unsigned char			newAddr;
	long			BytesWritten = 0;
	long			NumberOfBytesRead = 0;
	int			Result;

	newAddr = 0x80 | addr;

	memset(WriteBuffer, 0, m_USBCapabilities.OutputReportByteLength+1);

	/*Enable command-mode from Jw*/
	WriteBuffer[0] = 0x00; //ReportID
	WriteBuffer[1] = 0x82; //CMD-Mode
	WriteBuffer[2] = newAddr; //CMD + Addr

	HidD_FlushQueue(handle);
	Result = WriteFile(handle, WriteBuffer, m_USBCapabilities.OutputReportByteLength, (LPDWORD) &BytesWritten, NULL);

	if(Result != NULL)
	{
		memset(ReadBuffer, 0, m_USBCapabilities.InputReportByteLength+1);
		ReadBuffer[0] = 0x00;
	
		ReadFile(handle, ReadBuffer, m_USBCapabilities.InputReportByteLength, (LPDWORD) &NumberOfBytesRead, NULL);
		return ReadBuffer[3];
	}
	else
		return 0;
}


// USB write function
bool CSensorWinUSBJW::WriteData(HANDLE handle, unsigned char cmd, unsigned char addr, unsigned char data)
{
	unsigned char			WriteBuffer[10];
	unsigned char			ReadBuffer[10];
	int			Result;
	long			BytesWritten = 0;
	long			NumberOfBytesRead = 0;

	memset(WriteBuffer, 0, m_USBCapabilities.OutputReportByteLength+1);

	WriteBuffer[0] = 0x00;
	WriteBuffer[1] = cmd;
	WriteBuffer[2] = addr;
	WriteBuffer[3] = data;

	HidD_FlushQueue(handle);
	Result = WriteFile(handle, WriteBuffer, m_USBCapabilities.OutputReportByteLength, (LPDWORD) &BytesWritten, NULL);
	if(Result != NULL)
	{
		return true;
		memset(ReadBuffer, 0, m_USBCapabilities.InputReportByteLength+1);
		ReadBuffer[0] = 0x00;
	
		ReadFile(handle, ReadBuffer, m_USBCapabilities.InputReportByteLength, (LPDWORD) &NumberOfBytesRead, NULL);
	}
	else {
/*
#ifdef _DEBUG
	DWORD dw = GetLastError();
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + 48) * sizeof(TCHAR)); 
    sprintf((LPTSTR)lpDisplayBuf, 
		"%s failed with error %d: %s", 
        "JW24F14", dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

#endif
*/
		return false;
	}
}

bool CSensorWinUSBJW::SetQCNState()
{ // puts the Joystick Warrior USB sensor into the proper state for QCN (50Hz, +/- 2g)
  // and also writes these settings to EEPROM (so each device needs to just get set once hopefully)

   unsigned char mReg14 = ReadData(m_USBDevHandle[1], 0x14);  // get current settings of device
   // if not set already, set it to +/-2g accel (0x00) and 50Hz internal bandwidth 0x01
   // NB: 0x08 & 0x10 means accel is set to 4 or 8g, if not bit-and with 0x01 bandwidth is other than 50Hz
   if ((mReg14 & 0x08) || (mReg14 & 0x10) || ((mReg14 & 0x01) != 0x01)) {
        mReg14 = 0x01 | (ReadData(m_USBDevHandle[1], 0x14) & 0xE0);

        // write settings to register
        WriteData(m_USBDevHandle[1], 0x82, 0x14, mReg14);
 
       /* no EEPROM write
        // write settings to EEPROM for persistent state
        WriteData(m_USBDevHandle[1], 0x82, 0x0A, 0x10);  // start EEPROM write
        boinc_sleep(.050f);
        WriteData(m_USBDevHandle[1], 0x82, 0x34, mReg14);
        boinc_sleep(.050f);
        WriteData(m_USBDevHandle[1], 0x82, 0x0A, 0x02);  // end EEPROM write
        */
       boinc_sleep(.100f);
   }
   return true;
}

// Calculate a 10 bit value with MSB and LSB
int CSensorWinUSBJW::CalcMsbLsb(unsigned char lsb, unsigned char msb)
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

