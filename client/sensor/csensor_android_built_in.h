#ifndef _CSENSOR_ANDROID_BUILT_IN_H_
#define _CSENSOR_ANDROID_BUILT_IN_H_

/*
 *  csensor_android_built_in.h
 *  qcn
 *
 *  Created by Carl Christensen on 01/14/2014.
 *  Copyright 2014 Stanford University
 *
 * This file contains the definition of the Android Device Built-In Accelerometer Sensor Class
 */

#include "main.h"
using namespace std;

#include <stdio.h>
#include <stdlib.h>

#ifdef ANDROID

#include <android/input.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android/looper.h>

/* this is in client/main currently, copied over from android sdk
//#include <android_native_app_glue.h>
// struct copied from above header
struct android_poll_source {
    // The identifier of this source.  May be LOOPER_ID_MAIN or
    // LOOPER_ID_INPUT.
    int32_t id;

    // The android_app this ident is associated with.
    struct android_app* app;

    // Function to call to perform the standard processing of data from
    // this source.
    void (*process)(struct android_app* app, struct android_poll_source* source);
};
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "CSensorAndroidBuiltIn", __VA_ARGS__))
*/

#define LOOPER_ID_QCN 2112 

// this is the Linux implementation for the JoyWarrior sensor, used for QCNLive as well as the Mac service program qcnmacusb under BOINC
class CSensorAndroidBuiltIn  : public CSensor
{
   private:
      ASensorManager* m_pSensorManager;  
      ASensor* m_pSensor;
      ASensorEventQueue* m_pSensorEventQueue;
      ALooper* m_pLooper;
      
      virtual bool read_xyz(float& x1, float& y1, float& z1);  

      char m_strSensor[_MAX_PATH];
      char m_strVendor[_MAX_PATH];

      float m_fResolution;
      int m_minDelayMsec;

      float m_xyz[3];

   public:
      CSensorAndroidBuiltIn();
      virtual ~CSensorAndroidBuiltIn();

      virtual bool detect();    // this detects the Mac USB sensor
      virtual void closePort(); // closes the port if open

};

#endif   // ANDROID

#endif
