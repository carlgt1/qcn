/*
 *  csensor_mac_usb_onavi01.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 10/25/2009.
 *  Copyright 2009 Stanford University.  All rights reserved.
 *
 * Implementation file for Mac ONavi 1 Serial Comunications
 */

#include "main.h"
#include "csensor_mac_usb_onavi.h"
#include <fnmatch.h>
#include <glob.h>
#include "filesys.h"    // boinc_file_or_symlink_exists
#include <termios.h>

CSensorMacUSBONavi::CSensorMacUSBONavi()
  : CSensor(), m_fd(-1), m_usBitSensor(0)
{ 
}

CSensorMacUSBONavi::~CSensorMacUSBONavi()
{
  closePort();
}

void CSensorMacUSBONavi::closePort()
{
	if (m_fd > -1) {
	  close(m_fd);
    }
    m_fd = -1;
    m_usBitSensor = 0;
    setPort();
    setType();
}

bool CSensorMacUSBONavi::detect()
{
	// first see if the port actually exists (the device is a "file" at /dev/tty.xrusbmodemNNN, given in STR_USB_ONAVI01 and now STR_USB_ONAVI02 since they seem to have changed the device name

        // use glob to match names, if count is > 0, we found a match
        glob_t gt;
        memset(&gt, 0x00, sizeof(glob_t));
        if (glob(STR_USB_ONAVI01, GLOB_NOCHECK, NULL, &gt) || !gt.gl_matchc) {  // either glob failed or no match
           // device string failed, but try the new string onavi (really Exar USB driver) may be using
           if (glob(STR_USB_ONAVI02, GLOB_NOCHECK, NULL, &gt) || !gt.gl_matchc) {  // either glob failed or no match
             globfree(&gt);
             return false;
           }
        }
        char* strDevice = new char[_MAX_PATH];
        memset(strDevice, 0x00, sizeof(char) * _MAX_PATH);
        strncpy(strDevice, gt.gl_pathv[0], _MAX_PATH);
        globfree(&gt); // can get rid of gt now

	if (!boinc_file_or_symlink_exists(strDevice)) {
           delete [] strDevice;
           strDevice = NULL;
           return false;
        }
	
	m_fd = open(strDevice, O_RDWR); // | O_NOCTTY | O_NONBLOCK); 
        delete [] strDevice; // don't need strDevice after this call
        strDevice = NULL;

	if (m_fd == -1) { //failure
           return false;
        }
        

	// if here we opened the port, now set comm params
	struct termios tty;
	if (tcgetattr(m_fd, &tty) == -1) {  // get current terminal state
		closePort();
		return false;
	}

	cfmakeraw(&tty);  // get raw tty settings
	
	// set terminal speed 115.2K
	if (cfsetspeed(&tty, B115200) == -1) {
		closePort();
		return false;
	}
	
	// flow contol
	tty.c_iflag = 0;
	tty.c_oflag = 0;
	tty.c_cflag = CS8 | CREAD | CLOCAL;

	if (tcsetattr(m_fd, TCSANOW, &tty) == -1 || tcsendbreak(m_fd, 10) == -1 ) { // tcflow(m_fd, TCION) == -1) { // || tcflush(m_fd, TCIOFLUSH) == -1) {
		closePort();
		return false;
	}

        setPort(m_fd);

	setSingleSampleDT(true); // onavi samples itself

        // try to read a value and get the sensor bit-type (& hence sensor type)
        float x,y,z;
        m_usBitSensor = 0;
        if (read_xyz(x,y,z) && m_usBitSensor > 0) {
	   // exists, so setPort & Type
           switch(m_usBitSensor) {
             case 12:
	         setType(SENSOR_USB_ONAVI_A_12);
                 break;
             case 16:
	         setType(SENSOR_USB_ONAVI_B_16);
                 break;
             case 24:
	         setType(SENSOR_USB_ONAVI_C_24);
                 break;
             default: // error!
               closePort();
               return false;
	   }
        }
        else {
           closePort();
           return false;
        }
 
    return true;
}

inline bool CSensorMacUSBONavi::read_xyz(float& x1, float& y1, float& z1)
{
	/*
We tried to keep the data as simple as possible. The data looks like: 

##xxyyzzs 

Two ASCII '#' (x23) followed by the X value upper byte, X value lower byte, Y value upper byte, Y value lower byte, Z value upper byte, Z value lower byte and an eight bit checksum.  

The bytes are tightly packed and there is nothing between the data values except for the sentence start ##.  

The X and Y axis will have a 0g offset at 32768d (x8000) and the Z axis offset at +1g 45874d (xB332) when oriented in the X/Y horizontal and Z up position.  The  s  value is a one byte checksum.  

It is a sum of all of the bytes, truncated to the lower byte.  This firmware does not transmit the temperature value. 

Finding g as a value:

g  = x - 32768 * (5 / 65536) 

Where: x is the data value 0 - 65536 (x0000 to xFFFF). 

Values >32768 are positive g and <32768 are negative g. The sampling rate is set to 200Hz, and the analog low-pass filters are set to 50Hz.  The data is oversampled 2X over Nyquist. We are going to make a new version of the module, with 25Hz LP analog filters and dual sensitivity 2g / 6g shortly.  Same drivers, same interface.  I ll get you one as soon as I we get feedback on this and make a set of those.

	*/
	
	static float x0 = 0.0f, y0 = 0.0f, z0 = 0.0f; // keep last values

	// first check for valid port
	if (getPort() < 0) {
		   return false;
    }
	
	bool bRet = true;
	
	const int ciLen = 9;  // use a 24 byte buffer
    QCN_BYTE bytesIn[ciLen+1], cs;  // note pad bytesIn with null \0
	int x = 0, y = 0, z = 0;
	int iCS = 0;
	int iRead = 0;
	//x1 = y1 = z1 = 0.0f; // don't init to 0 as ONavi 24-bit is having errors we need to debug
        x1 = x0; y1 = y0; z1 = z0;  // use last good values
	const char cWrite[2] = {"*"};

	/*
	int iCtr = 0;
	while ((iRead = write(m_fd, &cWrite, 1)) != 1 && iCtr++<10) {
	    // try again, may need a little pause on the first time
		boinc_sleep(0.1f);
	}
	if (iRead == 1) {   // send a * to the device to get back the data
	*/
	if ((iRead = write(m_fd, &cWrite, 2)) == 2) {   // send a * to the device to get back the data
		memset(bytesIn, 0x00, ciLen+1);
		iRead = read(m_fd, bytesIn, ciLen);
		switch (iRead) {
			case -1:  
				bRet = false; // error
				fprintf(stderr, "%f: ONavi Error in read_xyz() - read(m_fd) returned %d\n", sm->t0active, iRead);
				fflush(stderr);
				break;
			case ciLen:  
				// good data length read in, now test for appropriate characters
				if (bytesIn[ciLen] == 0x00) { // && bytesIn[0] == 0x23 && bytesIn[1] == 0x23) {
					// format is ##XXYYZZC\0
					// we found both, the bytes in between are what we want (really bytes after lOffset[0]
                                        if (m_usBitSensor == 0) { // need to find sensor bit type i.e. 12/16/24-bit ONavi
					   if (bytesIn[0] == 0x2A && bytesIn[1] == 0x2A) {  // **
                                              m_usBitSensor = 12;
                                           }
					   else if (bytesIn[0] == 0x23 && bytesIn[1] == 0x23) { // ##
                                              m_usBitSensor = 16;
                                           }
					   else if (bytesIn[0] == 0x24 && bytesIn[1] == 0x24) {  // $$
                                              m_usBitSensor = 24;
                                           }
                                        }

					x = (bytesIn[2] * 256) + bytesIn[3];
					y = (bytesIn[4] * 256) + bytesIn[5];
					z = (bytesIn[6] * 256) + bytesIn[7];
					cs   = bytesIn[8];
					for (int i = 2; i <= 7; i++) iCS += bytesIn[i];

#ifdef QCN_RAW_DATA	
					// for testing on USGS shake table - they just want the raw integer data sent out
					x1 = (float) x;
					y1 = (float) y;
					z1 = (float) z;
#else
					// convert to g decimal value
					// g  = x - 32768 * (5 / 65536) 
					// Where: x is the data value 0 - 65536 (x0000 to xFFFF). 
					
					x1 = ((float) x - 32768.0f) * FLOAT_ONAVI_FACTOR * EARTH_G;
					y1 = ((float) y - 32768.0f) * FLOAT_ONAVI_FACTOR * EARTH_G;
					z1 = ((float) z - 32768.0f) * FLOAT_ONAVI_FACTOR * EARTH_G;
#endif
					
					x0 = x1; y0 = y1; z0 = z1;  // preserve values
					
					bRet = true;
				}
				break;
			default:
				fprintf(stderr, "%f: ONavi Error in read_xyz() - read(m_fd) returned %d\n", sm->t0active, iRead);
				fflush(stderr);
				x1 = x0; y1 = y0; z1 = z0;  // use last good values
		}
	}
	else {
		fprintf(stderr, "%f: ONavi Error in read_xyz() - write(m_fd) returned %d\n", sm->t0active, iRead);
		fflush(stderr);
		bRet = false;
	}
		
/*	
	memset(bytesIn, 0x00, ciLen);
	if ((iRead = read(m_fd, bytesIn, ciLen)) > 8) {
		for (int i = ciLen-1; i >= 0; i--) { // look for hash-mark i.e. ## boundaries (two sets of ##)
			if (bytesIn[i] == 0x23 && bytesIn[i-1] == 0x23) { // found a hash-mark set
				if (!lOffset[1]) {
					lOffset[1] = i;
					i-=8;
				}
				else {
					lOffset[0] = i+1; // must be the start
					break;  // found both hash marks - can leave loop
				}
			}
 		}
		if (lOffset[0] && lOffset[1] && lOffset[1] == (lOffset[0] + 8)) { 
			// we found both, the bytes in between are what we want (really bytes after lOffset[0]
			x = (bytesIn[lOffset[0]] * 256) + bytesIn[lOffset[0]+1];
			y = (bytesIn[lOffset[0]+2] * 256) + bytesIn[lOffset[0]+3];
			z = (bytesIn[lOffset[0]+4] * 256) + bytesIn[lOffset[0]+5];
			cs   = bytesIn[lOffset[0]+6];
			for (int i = 0; i <= 5; i++) iCS += bytesIn[lOffset[0] + i];

			// convert to g decimal value
			// g  = x - 32768 * (5 / 65536) 
			// Where: x is the data value 0 - 65536 (x0000 to xFFFF). 

			x1 = ((float) x - 32768.0f) * FLOAT_ONAVI_FACTOR * EARTH_G;
			y1 = ((float) y - 32768.0f) * FLOAT_ONAVI_FACTOR * EARTH_G;
			z1 = ((float) z - 32768.0f) * FLOAT_ONAVI_FACTOR * EARTH_G;
			
			x0 = x1; y0 = y1; z0 = z1;  // preserve values

			bRet = true;
		}
		else {
			x1 = x0; y1 = y0; z1 = z0;  // use last good values
			bRet = false;  // could be just empty, return
		}
	}
	else {
		bRet = false;
	}
    tcflush(m_fd, TCIOFLUSH);
*/
	
	return bRet;
}


