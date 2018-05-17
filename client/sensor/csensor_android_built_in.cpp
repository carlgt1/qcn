/*
 *  csensor_android_built_in.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 01/14/2014.
 *  Copyright 2014 Stanford University.  All rights reserved.
 *
 * Implementation file for Linux JoyWarrior 24F8 USB sensor - 
 *    note this just does a simple read of the joystick, so no setting of sample rate etc on-board the JW24F8 (which is fine)
 */

#ifdef ANDROID

#include "main.h"
#include "csensor_android_built_in.h"

/*
static int QCN_ALooper_callback(int fd, int events, void* data)
{
  if (!m_pSensorEventQueue) return 1;

//  int hasEvents = ASensorEventQueue_hasEvents(m_pSensorEventQueue);
//  fprintf(stdout, "callback has events = %d\n", hasEvents);

  ASensorEvent event;
  event.acceleration.x = event.acceleration.y = event.acceleration.z = 0.0f;
  while (m_pSensorEventQueue && ASensorEventQueue_getEvents(m_pSensorEventQueue, &event, 1) > 0) {
     if(event.type == ASENSOR_TYPE_ACCELEROMETER) {
        //l_SensorVector.z = event.acceleration.z;

//        fprintf(stdout, "callback: accl(x,y,z,t): %f %f %f %lld\n",
//           l_SensorVector.x, 
//           l_SensorVector.y, 
//           l_SensorVector.z, 
//           event.timestamp);

     }
  }
  return 1;  // return 1 to continue receiving callbacks
}
*/

CSensorAndroidBuiltIn::CSensorAndroidBuiltIn()
  : CSensor(), m_pSensorManager(NULL), m_pSensor(NULL), m_pLooper(NULL), 
       m_fResolution(0.0f), m_minDelayMsec(0)
{ 

  memset(m_xyz, 0x00, sizeof(float) * 3);
  memset(m_strSensor, 0x00, _MAX_PATH);
  memset(m_strVendor, 0x00, _MAX_PATH);
}

CSensorAndroidBuiltIn::~CSensorAndroidBuiltIn()
{
  closePort();
}

void CSensorAndroidBuiltIn::closePort()
{
  if (m_pSensor && m_pSensorEventQueue) {
     ASensorEventQueue_disableSensor(m_pSensorEventQueue, m_pSensor);
  }
  if (m_pSensorManager && m_pSensorEventQueue) {
     ASensorManager_destroyEventQueue(m_pSensorManager, m_pSensorEventQueue);
  }

  strcpy(m_strVendor, "");
  strcpy(m_strSensor, "");

  m_pSensorManager = NULL;
  m_pSensor = NULL;
  m_pSensorEventQueue = NULL;
  m_pLooper = NULL;

  m_fResolution = 0.0f;
  m_minDelayMsec = 0;

  memset(m_xyz, 0x00, sizeof(float) * 3);
  memset(m_strSensor, 0x00, _MAX_PATH);
  memset(m_strVendor, 0x00, _MAX_PATH);

  setType();
  setPort();

}


/*
ASENSOR_TYPE_ACCELEROMETER

int ASensor_getType(ASensor const* sensor);

float ASensor_getResolution(ASensor const* sensor) __NDK_FPABI__;

int ASensor_getMinDelay(ASensor const* sensor);

int ASensorEventQueue_enableSensor(ASensorEventQueue* queue, ASensor const* sensor);

int ASensorEventQueue_disableSensor(ASensorEventQueue* queue, ASensor const* sensor);

 * Returns this sensor's name (non localized)
const char* ASensor_getName(ASensor const* sensor);

 * Returns this sensor's vendor's name (non localized)
const char* ASensor_getVendor(ASensor const* sensor);

 * Return this sensor's type
int ASensor_getType(ASensor const* sensor);

 * Returns this sensors's resolution
float ASensor_getResolution(ASensor const* sensor) __NDK_FPABI__;

 * Returns the minimum delay allowed between events in microseconds.
 * A value of zero means that this sensor doesn't report events at a
 * constant rate, but rather only when a new data is available.
int ASensor_getMinDelay(ASensor const* sensor);

*/


inline bool CSensorAndroidBuiltIn::read_xyz(float& x1, float& y1, float& z1)
{  
    if (qcn_main::g_iStop) return false;

    if (! m_pSensor || ! m_pLooper | ! m_pSensorEventQueue) {
      fprintf(stderr, "read_xyz error: no sensor %x or looper %x or event queue %x setup!\n", m_pSensor, m_pLooper, m_pSensorEventQueue);
      return false;  // no open file descriptor
    }

   int ident, events;
   float fCtr = 0.0;

   // should probably be while loop but taking too long on some devices
   // set the member vars m_xyz so can be reused if it has a timeout (so x1 etc aren't 0, ie carries over)
   if ((ident = ALooper_pollAll(m_minDelayMsec/2, NULL, &events, NULL)) >= 0) {
         if (ident == LOOPER_ID_QCN) {
            ASensorEvent event;
            if (ASensorEventQueue_getEvents(m_pSensorEventQueue, &event, 1) > 0) {
              m_xyz[0] = event.acceleration.x;
              m_xyz[1] = event.acceleration.y;
              m_xyz[2] = event.acceleration.z;
            }
         }
   }

   // m_xyz (ie android accelerometer return values) are in G's already, so just multiply by earth g 9.8
   x1 = m_xyz[0] * EARTH_G;
   y1 = m_xyz[1] * EARTH_G;
   z1 = m_xyz[2] * EARTH_G;

   //fprintf(stdout, "read_xyz:  %f %f %f\n", x1, y1, z1);

   return true;
}

// try and open up the JoyWarrior file descriptor
bool CSensorAndroidBuiltIn::detect()
{
   setType();
   setPort();

   // get the singleton instance of the m_pSensorManager
   if (!m_pSensorManager) m_pSensorManager = (ASensorManager*) ASensorManager_getInstance();

/*
   // sensor listing
   ASensorList pSensorList = NULL;
   int iNum = ASensorManager_getSensorList(m_pSensorManager, &pSensorList);
   if (iNum && pSensorList) {
      fprintf(stdout, "\n\n%d Detected Sensors:\n", iNum);
      //int i = 0;
      for (int i=0; i < iNum; i++) {
      //while (i<10 && (pSensorList+i)) { //for (int i=0; i < iNum; i++) {
          fprintf(stdout, "  %s\n", ASensor_getName(*(pSensorList+i)));
          i++;
      }
   }
   else {
      fprintf(stdout, "\n\nNo Sensor List?  %d\n\n", iNum);
   }
*/

   // create looper
   m_pLooper = ALooper_forThread(); // get existing looper
   if (!m_pLooper) {  // make new looper
     //m_pLooper = ALooper_prepare(0);
     m_pLooper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
   }
   if (!m_pLooper) { // no existing or new looper -- error
     fprintf(stderr, "can't create Looper\n");
     return false; // can't create looper
   }

   // setup event queue
   //m_pSensorEventQueue = ASensorManager_createEventQueue(m_pSensorManager, m_pLooper, 
   //     LOOPER_ID_QCN, QCN_ALooper_callback, &l_SensorVector);
   m_pSensorEventQueue = ASensorManager_createEventQueue(m_pSensorManager, m_pLooper, 
        LOOPER_ID_QCN, NULL, NULL);
   if (!m_pSensorEventQueue) {
     fprintf(stderr, "can't create SensorEventQueue\n");
     return false;  // can't setup queue
   }

   // get the default accelerometer
   m_pSensor = (ASensor*) ASensorManager_getDefaultSensor(m_pSensorManager, ASENSOR_TYPE_ACCELEROMETER);
   if (!m_pSensor) {
      //fprintf(stdout, "No Android accelerometer detected.\n");
      return false; // no sensor
   }

   int iRetVal = 0;
   if ((iRetVal = ASensorEventQueue_enableSensor(m_pSensorEventQueue, m_pSensor)) < 0) {
     fprintf(stderr, "Error in enableSensor %d\n", iRetVal);
     return false;
   };
   m_fResolution = ASensor_getResolution(m_pSensor);
   m_minDelayMsec = ASensor_getMinDelay(m_pSensor);
   int rateMsec = (int)((sm->dt > 0. ? sm->dt : g_DT) * 1000.);
   //fprintf(stdout, "Rates: m_minDelayMSec = %d   raceMsec = %d\n", m_minDelayMsec, rateMsec);
   //if (rateMsec > m_minDelayMsec) m_minDelayMsec = rateMsec;
   if (rateMsec < m_minDelayMsec) m_minDelayMsec = rateMsec;

   fprintf(stdout, "Setting data rate to %d Hz\n", 1000L/m_minDelayMsec);
   strlcpy(m_strSensor, ASensor_getName(m_pSensor), _MAX_PATH);
   strlcpy(m_strVendor, ASensor_getVendor(m_pSensor), _MAX_PATH);

   // NB: the rate is in microseconds!
   if ((iRetVal = ASensorEventQueue_setEventRate(m_pSensorEventQueue, m_pSensor, m_minDelayMsec * 1000L)) < 0) { 
     fprintf(stderr, "Error in setEventRate %d\n", iRetVal);
     // return false; // not really a fatal error
   }

   fprintf(stdout, "Android Default Sensor Detected: \n\n   %s - %s\n"
                   "  Res = %f --- Min Delay msec = %d\n"
                   "  m_pSensor=%x    m_pSensorEventQueue=%x\n",
           m_strVendor, m_strSensor, m_fResolution, m_minDelayMsec, m_pSensor, m_pSensorEventQueue);

   setType(SENSOR_ANDROID);

   setSingleSampleDT(true); // set to true in raw mode so we don't get any interpolated/avg points (i.e. just the "integer" value hopefully)
   return (bool)(getTypeEnum() == SENSOR_ANDROID);
}

#endif // ANDROID
