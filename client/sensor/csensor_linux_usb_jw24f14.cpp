/*
 *  csensor_linux_usb_jw24f14.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for Linux JoyWarrior 24F14 USB sensor - 
 *    note this just does a simple read of the joystick, so no setting of sample rate etc on-board the JW24F14 (which is fine)
 */

#include "main.h"
#include "csensor_linux_usb_jw24f14.h"


CSensorLinuxUSBJW24F14::CSensorLinuxUSBJW24F14()
  : CSensor(), 
       m_fdJoy(-1), m_piAxes(NULL), m_iNumAxes(0), m_iNumButtons(0), m_strButton(NULL)
{ 
   // vars lifted from the codemercs.com JW24F14 Linux example
   memset(m_strJoystick, 0x00, 80);
   memset(&m_js, 0x0, sizeof(struct js_event));
}

CSensorLinuxUSBJW24F14::~CSensorLinuxUSBJW24F14()
{
  closePort();
}

void CSensorLinuxUSBJW24F14::closePort()
{
  if (m_fdJoy > -1) {
     close(m_fdJoy);
     m_fdJoy = -1;
  }
  if (m_piAxes) {
     free(m_piAxes);
     m_piAxes = NULL;
  }
  if (m_strButton) {
     free(m_strButton);
     m_strButton = NULL;
  }

  if (getPort() > -1) { // nothing really left to close, as it's just the joystick #
    fprintf(stdout, "Joywarrior 24F14 closed on Linux joystick port!\n");
    fflush(stdout);
  }
  setType();
  setPort();
}

inline bool CSensorLinuxUSBJW24F14::read_xyz(float& x1, float& y1, float& z1)
{  
#ifndef QCN_USB
    if (qcn_main::g_iStop) return false;
#endif
    if (m_fdJoy == -1) return false;  // no open file descriptor

    // read the joystick state - range on each axis seems to be 0-1023 (-2 to 2g)
    x1 = y1 = z1 = 0.0f;

    // read 3 events to make sure we get the latest xyz?
    for (int i = 0; i < 3; i++) {
       memset(&m_js, 0x00, sizeof(struct js_event));
       read(m_fdJoy, &m_js, sizeof(struct js_event));
       if ((m_js.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS) {
           m_piAxes[ m_js.number ] = m_js.value;
       }
    }

/*
fprintf(stdout, "x=%d  y=%d  z=%d\n", 
   m_piAxes[0],
   m_piAxes[1],
   m_piAxes[2]);
*/

   // 2^14 = 16384, so 4g across 16384, range is 0 through 16383 (-2g to 2g)
    // note that x/y/z should be scaled to +/- 2g, return values as +/- 2.0f*EARTH_G (in define.h: 9.78033 m/s^2)
#ifdef QCN_RAW_DATA
                x1 = (float) m_piAxes[0];
                y1 = (float) m_piAxes[1];
                z1 = (float) m_piAxes[2];
#else           
                x1 = (((float) m_piAxes[0] - 8191.5f) / 4095.75f) * EARTH_G;
                y1 = (((float) m_piAxes[1] - 8191.5f) / 4095.75f) * EARTH_G;
                z1 = (((float) m_piAxes[2] - 8191.5f) / 4095.75f) * EARTH_G;
#endif 

    // fprintf(stderr, "x1 = %f   y1 = %f   z1 = %f\n", x1, y1, z1);

    /* 
    // see what to do with the event 
    switch (m_js.type & ~JS_EVENT_INIT) {
        case JS_EVENT_AXIS:
           m_piAxes[ m_js.number ] = m_js.value;
           break;
        case JS_EVENT_BUTTON:
           m_strButton[ m_js.number ] = m_js.value;
           break;
    }

    // print the results 
    fprintf(stderr, "X: %6d  Y: %6d  ", m_piAxes[0], m_piAxes[1] );
                
    if ( m_iNumAxes > 2 ) 
        fprintf(stderr, "Z: %6d  ", m_piAxes[2] );
                        
    if ( m_iNumAxes > 3 )
        fprintf(stderr, "R: %6d  ", m_piAxes[3] );
                        
    for( int i=0 ; i < m_iNumButtons; ++i )
        fprintf(stderr, "B%d: %d  ", i, m_strButton[i] );
      
    fprintf(stderr, "\n");
    */

    return true;
}

// tests the open joystick file descriptor to see if it's really a JW24F14, and set to read rawdata
bool CSensorLinuxUSBJW24F14::testJoystick()
{
   // if made it here, then we have opened a joystick file descriptor
   m_iNumAxes = 0;
   m_iNumButtons = 0;
   memset(m_strJoystick, 0x00, 80);

   ioctl(m_fdJoy, JSIOCGAXES, &m_iNumAxes);
   ioctl(m_fdJoy, JSIOCGBUTTONS, &m_iNumButtons);
   ioctl(m_fdJoy, JSIOCGNAME(80), m_strJoystick);

   // compare the name of device, and number of buttons & axes with valid JoyWarrior values
   if (strcmp(m_strJoystick, IDSTR_JW24F14)
     || m_iNumButtons != NUM_BUTTON_JW24F14
     || m_iNumAxes != NUM_AXES_JW24F14) {
         closePort();  // this far in, we need to close the port!
         return false;
   }

   m_piAxes = (int *) calloc( m_iNumAxes, sizeof( int ) );
   memset(m_piAxes, 0x00, sizeof(int) * m_iNumAxes);
   m_strButton = (char *) calloc( m_iNumButtons, sizeof( char ) );
   memset(m_strButton, 0x00, sizeof(char) * m_iNumButtons);
  
   fcntl( m_fdJoy, F_SETFL, O_NONBLOCK );   // use non-blocking mode

   // try a read
   float x,y,z;
   // "prime" the joystick reader
   if (! read_xyz(x,y,z)) {
      closePort();  // this far in, we need to close the port!
      return false;
   }

   // if made it here, then it's a joywarrior, set to raw data mode
   struct js_corr corr[NUM_AXES_JW24F14];

   // Zero correction coefficient structure and set all axes to Raw mode 
   for (int i=0; i<NUM_AXES_JW24F14; i++) {
     corr[i].type = JS_CORR_NONE;
     corr[i].prec = 0;
     for (int j=0; j<8; j++) {
        corr[i].coef[j] = 0;
     }
   }

   if (ioctl(m_fdJoy, JSIOCSCORR, &corr)) {
      fprintf(stderr, "CSensorLinuxUSBJW24F14:: error setting correction for raw data reads\n");
   }

   setType(SENSOR_USB_JW24F14);
   setPort(getTypeEnum());
   
   return true; // if here we can return true, i.e Joywarrior found on Linux joystick port, and hopefully set to read raw data
}

// try and open up the JoyWarrior file descriptor
bool CSensorLinuxUSBJW24F14::detect()
{
   setType();
   setPort();

   const char* strJW24F14Enum[LINUX_JOYSTICK_NUM] = LINUX_JOYSTICK_ARRAY;
   // go through and try potential Linux joystick devices from define.h
   int i;
   for (i = 0; i < LINUX_JOYSTICK_NUM; i++) {
      if (! boinc_file_exists(strJW24F14Enum[i]) ) continue; // first see if file (device) exists
      if ( ( m_fdJoy = open(strJW24F14Enum[i], O_RDONLY)) != -1 && testJoystick() ) {
         fprintf(stdout, "%s detected on joystick device %s\n", m_strJoystick, strJW24F14Enum[i]);
         break;  // found a JW24F14
      }
   }
#ifdef QCN_RAW_DATA
   setSingleSampleDT(true); // set to true in raw mode so we don't get any interpolated/avg points (i.e. just the "integer" value hopefully)
#else
   setSingleSampleDT(false);
#endif
   return (bool)(getTypeEnum() == SENSOR_USB_JW24F14);
}

