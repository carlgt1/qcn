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
#include "csensor_win_usb_jw24f14.h"

CSensorWinUSBJW24F14::CSensorWinUSBJW24F14()
  : CSensor(), m_USBHandle(NULL)
{ 
   m_USBDevHandle[0] = NULL;
   m_USBDevHandle[1] = NULL;
}

CSensorWinUSBJW24F14::~CSensorWinUSBJW24F14()
{
  closePort();
}

void CSensorWinUSBJW24F14::closePort()
{
	//return;
  for (int i = 0; i < 2; i++) {
     if (m_USBDevHandle[i]) {
       try {
          // don't think we need the next line, just close & Release
      //WriteData(m_USBDevHandle[i], 0x02, 0x00, 0x00);  // Free JW
	  ::CancelIo(m_USBDevHandle[i]);
	  ::CloseHandle(m_USBDevHandle[i]);
	  m_USBDevHandle[i] = NULL;
          // mac version:
          //WriteData(m_USBDevHandle[i], 0x02, 0x00);
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

bool CSensorWinUSBJW24F14::detect()
{
    // tries to detect & initialize the USB JW24F14 Sensor
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
				if(Attributes.ProductID == USB_DEVICEID_JW24F14)
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
				//else
				//	NULL;
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
		esTmp = SENSOR_USB_JW24F14;
		iPort = getPort();
        // no single sample, JW24F14 actually needs to sample within the 50hz,
        // since we're reading from joystick port, not the downsampling "chip"
#ifdef QCN_RAW_DATA
   setSingleSampleDT(true); // set to true in raw mode so we don't get any interpolated/avg points (i.e. just the "integer" value hopefully)
#else
   setSingleSampleDT(false);
#endif
		fprintf(stdout, "JoyWarrior 24F14 USB sensor detected on Windows joystick port %d\n"
			"Set to 50Hz internal bandwidth, +/- 2g acceleration.\n", getPort());

	}

    closePort();  // close the HID USB stuff and just use joystick calls from here on out

	// NB: closePort resets the type & port, so have to set again 
    setType(esTmp);
	setPort(iPort);

	return (bool)(getTypeEnum() == SENSOR_USB_JW24F14);
}

// USB stick accelerometer specific stuff (codemercs.com JoyWarrior 24F8)
// http://codemercs.com/JW24F1424F8_E.html

void CSensorWinUSBJW24F14::GetCapabilities(HANDLE handle)
{
	PHIDP_PREPARSED_DATA PreparsedData;
	::HidD_GetPreparsedData(handle, &PreparsedData);
	::HidP_GetCaps(PreparsedData, &m_USBCapabilities);
	::HidD_FreePreparsedData(PreparsedData);
}

int CSensorWinUSBJW24F14::SetupJoystick()
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
			if (pjc->wMid == USB_VENDORID_JW && pjc->wPid == USB_DEVICEID_JW24F14) {
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

// using joystick interface
inline bool CSensorWinUSBJW24F14::read_xyz(float& x1, float& y1, float& z1)
{
	// joystick fn usage
	if (getPort() < 0) return false;

	x1=y1=z1=0.0f;
	
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
	return true;
}

/*
// not using joystick interface
inline bool CSensorWinUSBJW24F14::read_xyz(float& x1, float& y1, float& z1)
{
#ifdef _DEBUG
	static int x_max = -10000, x_min = 10000;
#endif
			unsigned char						rawData[6];
			int							i;
			int						x = 0, y = 0, z = 0;
			
				//if (!JWEnableCommandMode24F14(m_USBDevHandle[1])) return false;
				
				for (i = 0; i < 6; i++)
				{
					rawData[i] = ReadData(m_USBDevHandle[1], 0x02 + i);
				}
				
				//JWDisableCommandMode24F14(m_USBDevHandle[1]);

				x = CalcMsbLsb(rawData[0], rawData[1]);
				y = CalcMsbLsb(rawData[2], rawData[3]);
				z = CalcMsbLsb(rawData[4], rawData[5]);

#ifdef _DEBUG
	// range seems to be -512 to 511 inclusive
	if (x > x_max) x_max = x;
	if (x < x_min) x_min = x;
#endif

	x1 = ((((float) x)) / (x>0 ? 255.5f : 256.0f)) * EARTH_G;
	y1 = ((((float) y)) / (y>0 ? 255.5f : 256.0f)) * EARTH_G;
	z1 = ((((float) z)) / (z>0 ? 255.5f : 256.0f)) * EARTH_G;	

	return true;
}
*/

unsigned char CSensorWinUSBJW24F14::ReadData(HANDLE handle, unsigned char addr, bool bFlush)
{
	unsigned char			WriteBuffer[10];
	unsigned char			ReadBuffer[10];
	long			BytesWritten = 0;
	long			NumberOfBytesRead = 0;
	int			Result;

	memset(WriteBuffer, 0x00, 10);

	/*Enable command-mode from Jw*/
	WriteBuffer[0] = 0x00; //ReportID
	WriteBuffer[1] = 0x82; 
	WriteBuffer[2] = 0x80 | addr;

	if (bFlush) HidD_FlushQueue(handle);
	Result = WriteFile(handle, WriteBuffer, m_USBCapabilities.OutputReportByteLength, (LPDWORD) &BytesWritten, NULL);

	if(Result != NULL)
	{
		memset(ReadBuffer, 0, m_USBCapabilities.InputReportByteLength+1);
		ReadBuffer[0] = 0x00;
	
		ReadFile(handle, ReadBuffer, m_USBCapabilities.InputReportByteLength, (LPDWORD) &NumberOfBytesRead, NULL);
		return ReadBuffer[3];
	}
	else
		return 0x00;
}

float CSensorWinUSBJW24F14::ReadFloatData(unsigned char addr_LSB, unsigned char addr_MSB, char axe)
{
	unsigned char MSB, LSB;

	// use the 0 interface for better speed
	LSB = ReadData(m_USBDevHandle[1], addr_LSB);
	MSB = ReadData(m_USBDevHandle[1], addr_MSB);

	return (float) CalcMsbLsb(LSB, MSB);
}

int CSensorWinUSBJW24F14::CalcMsbLsb(unsigned char lsb, unsigned char msb)
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

// USB write function
bool CSensorWinUSBJW24F14::WriteData(HANDLE handle, unsigned char addr, unsigned char cmd, bool bCommandMode, bool bFlush)
{
	unsigned char			WriteBuffer[10];
	unsigned char			ReadBuffer[10];
	int			Result;
	long			BytesWritten = 0;
	long			NumberOfBytesRead = 0;

	memset(WriteBuffer, 0x00, 10);
	WriteBuffer[0] = 0x00;
	if (bCommandMode) {
		WriteBuffer[1] = cmd;
	}
	else {
		WriteBuffer[1] = 0x82;
		WriteBuffer[2] = addr;
		WriteBuffer[3] = cmd;
	}

	if (bFlush) HidD_FlushQueue(handle);
	Result = WriteFile(handle, WriteBuffer, m_USBCapabilities.OutputReportByteLength, (LPDWORD) &BytesWritten, NULL);

	if(Result)
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

bool CSensorWinUSBJW24F14::SetQCNState()
{ // puts the Joystick Warrior USB sensor into the proper state for QCN (50Hz, +/- 2g)
  // and also writes these settings to EEPROM (so each device needs to just get set once hopefully)
   return true; // can't setup without physically removing!
/*
	const int ciRange = 4;       // 2g range (+/-)
	//const int ciBandwidth = 120;  // 1200Hz bandwidth & 0% compensation
	const int ciBandwidth = 56;  // 75Hz bandwidth & 0% compensation

	int iRange = 0, iBandwidth = 0;
	// note the command-mode takes the 2nd handle
	if (! QCNReadSensor(iRange, iBandwidth)) return false;
	
	if (iRange == ciRange && iBandwidth == ciBandwidth) return true; // already set

	// if here need to set
	if (! QCNWriteSensor(ciRange, ciBandwidth)) return false;
	
    return true;
*/
}

bool CSensorWinUSBJW24F14::JWEnableCommandMode24F14(HANDLE handle)
{ 
	return true; //WriteData(handle, 0x80, 0x80, true);
}

bool CSensorWinUSBJW24F14::JWDisableCommandMode24F14(HANDLE handle)
{ 
	return true; //WriteData(handle, 0x00, 0x00, true);
}

bool CSensorWinUSBJW24F14::QCNReadSensor(int& iRange, int& iBandwidth)
{
/*
	// Read	
	// Get values from sensor
	unsigned char temp = 0x00; //, iComp;
	if (!JWEnableCommandMode24F14(m_USBDevHandle[1])) return false;
	
	// Open 
	temp = ReadData(m_USBDevHandle[1], 0x0D);
	boinc_sleep(.05f);
	temp &= 0xEF;
	temp |= 0x10;
	//JWWriteByteToAddress24F14 (interface, 0x0D, temp);
	WriteData(m_USBDevHandle[1], 0x0D, temp);
	boinc_sleep(.05f);
	
	// Read Bandwidth & Compensation
	//if (JWReadByteFromAddress24F14 (interface, 0x20, &temp)  != kIOReturnSuccess) return false;
	temp = ReadData(m_USBDevHandle[1], 0x20);
	boinc_sleep(.05f);
	//iBandwidth = (temp & 0xF0) >> 4;
	//iComp = temp & 0x0F;
	iBandwidth = temp;
	
	// Read Range
	//if (JWReadByteFromAddress24F14 (interface, 0x35, &temp) != kIOReturnSuccess) return false;
	temp = ReadData(m_USBDevHandle[1], 0x35);
	boinc_sleep(.05f);
	//usleep(50000);
	//temp &= 0x0E;
	//iRange = temp >> 1;
	iRange = temp;
	
	
	// Close Image
	//JWReadByteFromAddress24F14 (interface, 0x0D, &temp);
	temp = ReadData(m_USBDevHandle[1], 0x0D);
	boinc_sleep(.05f);
	//usleep(50000);
	temp &= 0xEF;
	//JWWriteByteToAddress24F14 (interface, 0x0D, temp);
	WriteData(m_USBDevHandle[1], 0x0D, temp);
	boinc_sleep(.05f);
	
	JWDisableCommandMode24F14(m_USBDevHandle[1]);
	boinc_sleep(.05f);
*/
	return true;
}



bool CSensorWinUSBJW24F14::QCNWriteSensor(const int& iRange, const int& iBandwidth)
{
/*
	// Write
	
	unsigned char temp = 0x00;
	//int range			= 2;   // 2g range, 0=1, 1=1.5, 2=2, 3=3, 4=4, 5=8, 6=16
	//int bandwidth		= 3;   // 75Hz,  0=10, 1=20, 2=40, 3=75, 4=150, 5=300, 6=600, 7=1200
	//int compensation	= 8;   // 0% comp,  7=-.5%, 8=0, 9=+.5% etc
	
    if (!JWEnableCommandMode24F14(m_USBDevHandle[1])) return false;
    
	// Open 
	temp = ReadData(m_USBDevHandle[1], 0x0D);
	//if (JWReadByteFromAddress24F14 (interface, 0x0D, &temp) != kIOReturnSuccess) return false;
	boinc_sleep(0.05f);
	temp &= 0xEF;
	temp |= 0x10;
	//if (JWWriteByteToAddress24F14 (interface, 0x0D, temp) != kIOReturnSuccess) return false;
	WriteData(m_USBDevHandle[1], 0x0D, temp);
	boinc_sleep(0.05f);
	
	// Write Bandwidth & Compensation
	//JWReadByteFromAddress24F14 (interface, 0x20, &temp);
	//usleep(50000);
	//temp &= 0x00;
	//temp |= (bandwidth<<4);
	//temp |= compensation;
	temp = iBandwidth;
	//if (JWWriteByteToAddress24F14 (interface, 0x20, temp) != kIOReturnSuccess) return false;
	WriteData(m_USBDevHandle[1], 0x20, temp);
	boinc_sleep(0.05f);
	
	// Write Range
	//if (JWReadByteFromAddress24F14 (interface, 0x35, &temp) != kIOReturnSuccess) return false;
	//usleep(50000);
	//temp &= 0xF1;
	//temp |= (range<<1);
	temp = iRange;
	//if (JWWriteByteToAddress24F14 (interface, 0x35, temp) != kIOReturnSuccess) return false;
	WriteData(m_USBDevHandle[1], 0x35, temp);
	boinc_sleep(0.05f);

	
	// Are we going to save to EEPROM or Image only?
	//if ( [saveImageOrEEPROMField indexOfSelectedItem] == 0 )
	//{
	// Close Image
	//JWReadByteFromAddress24F14 (interface, 0x0D, &temp);
	temp = ReadData(m_USBDevHandle[1], 0x0D);
	boinc_sleep(.05f);
	//usleep(50000);
	temp &= 0xEF;
	//JWWriteByteToAddress24F14 (interface, 0x0D, temp);
	WriteData(m_USBDevHandle[1], 0x0D, temp);
	boinc_sleep(.05f);

	//}
	//else {
		// Save changes to EEPROM by touching the registers we want to change
	//	JWWriteByteToAddress24F14 (interface, 0x40 & 0xFE, 0);
	WriteData(m_USBDevHandle[1], 0x40 & 0xFE, 0x00);
	boinc_sleep(0.05f);
	//	JWWriteByteToAddress24F14 (interface, 0x55 & 0xFE, 0);
	WriteData(m_USBDevHandle[1], 0x55 & 0xFE, 0x00);
	boinc_sleep(0.05f);
		
		// Soft-reset (save EEPROM-state)
	//	JWWriteByteToAddress24F14 (interface, 0x10, 0xB6);
	WriteData(m_USBDevHandle[1], 0x10, 0xB6);
	boinc_sleep(0.05f);
	//}
    
    JWDisableCommandMode24F14(m_USBDevHandle[1]);
	boinc_sleep(0.05f);
*/	
	return true;
}
