/*
 *  csensor_linux_usb_onavi01.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 01/24/2012
 *  Copyright 2012 Stanford University.  All rights reserved.
 *
 * Implementation file for Linux ONavi 1 Serial Comunications
 */

#include "main.h"
#include "csensor_linux_usb_onavi.h"
#include <fnmatch.h>
#include <glob.h>
#include "filesys.h"    // boinc_file_or_symlink_exists
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>

const int g_ciLen = 9;  // ##XXYYZZC 

CSensorLinuxUSBONavi::CSensorLinuxUSBONavi()
  : CSensor(), m_fd(-1), m_usBitSensor(0)
{ 
}

CSensorLinuxUSBONavi::~CSensorLinuxUSBONavi()
{
  closePort();
}

void CSensorLinuxUSBONavi::closePort()
{
    if (m_fd > -1) {
	  close(m_fd);
    }
    m_fd = -1;
    m_usBitSensor = 0;
    setPort();
    setType();
}

bool CSensorLinuxUSBONavi::detect()
{
	// first see if the port actually exists (the device is a "file" at /dev/ttyACM0, given in STR_LINUX_USB_ONAVI01 

        // use glob to match names, if count is > 0, we found a match
        glob_t gt;
        memset(&gt, 0x00, sizeof(glob_t));
        if (glob(STR_LINUX_USB_ONAVI01, GLOB_NOCHECK, NULL, &gt) || !gt.gl_pathc) {  // either glob failed or no match
           // device string failed, but try the new string onavi (really Exar USB driver) may be using
           //if (glob(STR_USB_ONAVI02, GLOB_NOCHECK, NULL, &gt) || !gt.gl_matchc) {  // either glob failed or no match
             globfree(&gt);
             return false;
           //}
        }

        char* strDevice = new char[_MAX_PATH];
        memset(strDevice, 0x00, sizeof(char) * _MAX_PATH);
        strncpy(strDevice, gt.gl_pathv[0], _MAX_PATH);
        globfree(&gt); // can get rid of gt now

	if (!boinc_file_or_symlink_exists(strDevice)) {
#ifdef _DEBUG
           fprintf(stderr, "Device %s does not exist\n", strDevice);
#endif
           delete [] strDevice;
           strDevice = NULL;
           return false;
        }

	m_fd = open(strDevice, O_RDWR | O_NOCTTY);

        delete [] strDevice; // don't need strDevice after this call
        strDevice = NULL;

	if (m_fd == -1) { // failure
#ifdef _DEBUG
           fprintf(stderr, "Cannot create ONavi fd\n");
#endif
           return false;
        }

    // setup basic modem I/O
    struct termios options;
    memset(&options, 0x00, sizeof(options));
    options.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
        
    options.c_lflag = 0;
        
    // read blocking conditions - force to read the whole g_ciLen otherwise we get bad truncation errors
    //  use VTIME so it doesn't block forever (ie 1 second timeout) 
    options.c_cc[VTIME]    = 10;  // VTIME is in .1 secs, so this times out after a second (sensor should reset) 
    options.c_cc[VMIN]     = g_ciLen;  
        
    tcflush(m_fd, TCIFLUSH);

    if (tcsetattr(m_fd, TCSANOW, &options) == -1) {
           fprintf(stderr, "Cannot set ONavi attributes\n");
       closePort();
       return false;
    }

        setPort(m_fd);

	setSingleSampleDT(true); // Onavi does the 50Hz rate

        // try to read a value and get the sensor bit-type (& hence sensor type)
        float x,y,z;
        m_usBitSensor = 0;
        if (read_xyz(x,y,z) && m_usBitSensor > 0) {
	   // exists, so setPort & Type
           switch(m_usBitSensor) {
             case 12:
	         setType(SENSOR_USB_ONAVI_A_12); break;
             case 16:
	         setType(SENSOR_USB_ONAVI_B_16);
                 break;
             case 24:
	         setType(SENSOR_USB_ONAVI_C_24);
                 break;
             default: // error!
               fprintf(stderr, "Error in ONavi sensor type %d bits\n", m_usBitSensor);
               closePort();
               return false;
	   }
        }
        else {
           fprintf(stderr, "Error in ONavi read_xyz %f,%f,%f  %d bits\n", x,y,z, m_usBitSensor);
           closePort();
           return false;
        }
 
    return true;
}

inline bool CSensorLinuxUSBONavi::read_xyz(float& x1, float& y1, float& z1)
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
	
        QCN_BYTE bytesIn[g_ciLen+1], cs;  // note pad bytesIn with null \0
	int x = 0, y = 0, z = 0;
	int iCS = 0;
	int iRead = 0;
        x1 = x0; y1 = y0; z1 = z0;  // use last good values
	const char cWrite = '*';

	if ((iRead = write(m_fd, &cWrite, 1)) == 1) {   // send a * to the device to get back the data
	    memset(bytesIn, 0x00, g_ciLen+1);

            if ((iRead = read(m_fd, bytesIn, g_ciLen)) == g_ciLen) { // read the g_ciLen
	       // good data length read in, now test for appropriate characters ie start with * # $
	       if (bytesIn[g_ciLen] == 0x00 && 
                    ((bytesIn[0] == 0x2A && bytesIn[1] == 0x2A) 
                  || (bytesIn[0] == 0x23 && bytesIn[1] == 0x23)
                  || (bytesIn[0] == 0x24 && bytesIn[1] == 0x24)) ) {
	        // format is ##XXYYZZC\0
	        // we found both, the bytes in between are what we want (really bytes after lOffset[0]
                  if (m_usBitSensor == 0) { // need to find sensor bit type i.e. 12/16/24-bit ONavi
                    switch(bytesIn[0]) {
                    case 0x2A:
                       m_usBitSensor = 12; break;
                    case 0x23:
                       m_usBitSensor = 16; break;
                    case 0x24:
                       m_usBitSensor = 24; break;
                    }
                  }

                  // scale appropriately
          	x = (bytesIn[2] * 256) + bytesIn[3];
	        y = (bytesIn[4] * 256) + bytesIn[5];
	        z = (bytesIn[6] * 256) + bytesIn[7];
	        cs   = bytesIn[8];
/*
	        for (int i = 2; i <= 7; i++) iCS += bytesIn[i];
                fprintf(stdout, "%f [%c%c]  %x %x %05d   %x %x %05d   %x %x %05d   %x\n", 
                 sm->t0active, 
                 bytesIn[0],
                 bytesIn[1],
                 bytesIn[2],
                 bytesIn[3], x,
                 bytesIn[4],
                 bytesIn[5], y, 
                 bytesIn[6],
                 bytesIn[7], z,
                 bytesIn[8]);
*/

#ifdef QCN_RAW_DATA	
	        // for testing on USGS shake table - they just want the raw integer data sent out
         	x1 = (float) x;
	        y1 = (float) y;
	        z1 = (float) z;
#else
	// convert to g decimal value
	// g  = x - 32768 * (5 / 65536) 
	// Where: x is the data value 0 - 65536 (x0000 to xFFFF). 
					
        	x1 = ((float) x - 32768.0f) * FLOAT_LINUX_ONAVI_FACTOR * EARTH_G;
        	y1 = ((float) y - 32768.0f) * FLOAT_LINUX_ONAVI_FACTOR * EARTH_G;
        	z1 = ((float) z - 32768.0f) * FLOAT_LINUX_ONAVI_FACTOR * EARTH_G;
#endif
                x0 = x1; y0 = y1; z0 = z1;  // preserve values
                bRet = true;
           }
           else { //mismatched start bytes ie not ** or ## or $$
	      fprintf(stderr, "%f: ONavi Error in read_xyz() - mismatched start bytes %c %c\n", 
               sm->t0active, bytesIn[0], bytesIn[1]);
              bRet = false;
           }
        }
	else {
	   fprintf(stderr, "%f: ONavi Error in read_xyz() - read(m_fd) returned %d -- errno = %d : %s\n", sm->t0active, iRead, errno, strerror(errno));
	   bRet = false;
	}  // read bytesIn
     }
     else {
       fprintf(stderr, "%f: ONavi Error in read_xyz() - write(*) returned %d -- errno = %d : %s\n", sm->t0active, iRead, errno, strerror(errno));
       bRet = false;
     }  // write *
     return bRet;
}

