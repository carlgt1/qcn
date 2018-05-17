/*
 *  csensor.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University.  All rights reserved.
 *
 * Implementation file for CSensor base classes
 * note it requires a reference to the sm shared memory datastructure (CQCNShMem)
 */

#include "main.h"
#include "csensor.h"


// begin sensor strings, the ID's are the e_sensor enum in define.h
// strings for sensor types
/* should match the qcn_sensor table

create table qcn_sensor (id smallint not null primary key, is_usb boolean not null default 0, 
    is_gps boolean not null default 0, description varchar(64));
insert into qcn_sensor values (0   , 0, 0, 'Not Found');
insert into qcn_sensor values (1   , 0, 0, 'Mac PPC 1');
insert into qcn_sensor values (2   , 0, 0, 'Mac PPC 2');
insert into qcn_sensor values (3   , 0, 0, 'Mac PPC 3');
insert into qcn_sensor values (4   , 0, 0, 'Mac Intel');
insert into qcn_sensor values (5   , 0, 0, 'Lenovo Thinkpad');
insert into qcn_sensor values (6   , 0, 0, 'HP Laptop');
insert into qcn_sensor values (7   , 0, 0, 'Android Device');
insert into qcn_sensor values (100 , 1, 0, 'JoyWarrior 24F8 USB');
insert into qcn_sensor values (101 , 1, 0, 'MotionNode Accel USB');
insert into qcn_sensor values (102 , 1, 0, 'ONavi 1 USB');
insert into qcn_sensor values (103 , 1, 0, 'JoyWarrior 24F14 USB');
insert into qcn_sensor values (104 , 1, 0, 'ONavi A 12-bit USB');
insert into qcn_sensor values (105 , 1, 0, 'ONavi B 16-bit USB');
insert into qcn_sensor values (106 , 1, 0, 'ONavi C 24-bit USB');
insert into qcn_sensor values (107 , 1, 0, 'Phidgets 1056 228ug USB');
insert into qcn_sensor values (108 , 1, 0, 'Phidgets 1042 976ug USB');
insert into qcn_sensor values (109 , 1, 0, 'Phidgets 1044 76ug USB');
insert into qcn_sensor values (110 , 1, 0, 'Phidgets 1041 976ug USB');
insert into qcn_sensor values (111 , 1, 0, 'Phidgets 1043 76ug USB');
insert into qcn_sensor values (1000, 1, 1, 'Phidgets 1040 GPS USB');

*/

#define SENSOR_STRLG_NOTFOUND "Not Found"
#define SENSOR_STRSH_NOTFOUND ""


#define SENSOR_STRLG_MAC_PPC1  "PPC Mac Laptop Type 1"
#define SENSOR_STRSH_MAC_PPC1  "MP"
#define SENSOR_STRLG_MAC_PPC2  "PPC Mac Laptop Type 2"
#define SENSOR_STRSH_MAC_PPC2  "MP"
#define SENSOR_STRLG_MAC_PPC3  "PPC Mac Laptop Type 3"
#define SENSOR_STRSH_MAC_PPC3  "MP"
#define SENSOR_STRLG_MAC_INTEL "Intel Mac Laptop"
#define SENSOR_STRSH_MAC_INTEL "MI"

#define SENSOR_STRLG_WIN_THINKPAD "Lenovo Thinkpad Laptop"
#define SENSOR_STRSH_WIN_THINKPAD "TP"

#define SENSOR_STRLG_WIN_HP "HP Laptop"
#define SENSOR_STRSH_WIN_HP "HP"

#define SENSOR_STRLG_ANDROID "Android Device"
#define SENSOR_STRSH_ANDROID "AN"

#define SENSOR_STRLG_USB_MAC_DRIVER "Mac USB Driver"
#define SENSOR_STRSH_USB_MAC_DRIVER "MD"

#define SENSOR_STRLG_USB_JW24F8 "JoyWarrior 24F8 USB"
#define SENSOR_STRSH_USB_JW24F8 "JW"

#define SENSOR_STRLG_USB_JW24F14 "JoyWarrior 24F14 USB"
#define SENSOR_STRSH_USB_JW24F14 "JT"

#define SENSOR_STRLG_USB_MN "MotionNode Accel USB"
#define SENSOR_STRSH_USB_MN "MN"

#define SENSOR_STRLG_USB_ONAVI_1 "OBSOLETE ONavi G1 USB"
#define SENSOR_STRSH_USB_ONAVI_1 "O1"

#define SENSOR_STRLG_USB_ONAVI_A_12 "ONavi A 12-bit USB"
#define SENSOR_STRSH_USB_ONAVI_A_12 "OA"

#define SENSOR_STRLG_USB_ONAVI_B_16 "ONavi B 16-bit USB"
#define SENSOR_STRSH_USB_ONAVI_B_16 "OB"

#define SENSOR_STRLG_USB_ONAVI_C_24 "ONavi C 24-bit USB"
#define SENSOR_STRSH_USB_ONAVI_C_24 "OC"

#define SENSOR_STRLG_USB_PHIDGETS_1056 "Phidgets 1056 228ug USB"
#define SENSOR_STRSH_USB_PHIDGETS_1056 "P1"

#define SENSOR_STRLG_USB_PHIDGETS_1042 "Phidgets 1042 976ug USB"
#define SENSOR_STRSH_USB_PHIDGETS_1042 "P2"

#define SENSOR_STRLG_USB_PHIDGETS_1044 "Phidgets 1044 76ug USB"
#define SENSOR_STRSH_USB_PHIDGETS_1044 "P3"

#define SENSOR_STRLG_USB_PHIDGETS_1041 "Phidgets 1041 976ug USB"
#define SENSOR_STRSH_USB_PHIDGETS_1041 "P4"

#define SENSOR_STRLG_USB_PHIDGETS_1043 "Phidgets 1043 76ug USB"
#define SENSOR_STRSH_USB_PHIDGETS_1043 "P5"

// end sensor strings

map<int, CSensorType> CSensor::m_map;

CSensor::CSensor()
  : 
    m_iType(SENSOR_NOTFOUND), 
    m_port(-1),
    m_bSingleSampleDT(false),
    m_strSensor("")
{
	if (m_map.size() == 0) { // build map of sensor types
		
		// setup the map for sensors
		m_map.clear();
		CSensorType cst;
		
		cst.init(SENSOR_NOTFOUND, SENSOR_STRLG_NOTFOUND, SENSOR_STRSH_NOTFOUND);
		m_map.insert(make_pair(SENSOR_NOTFOUND, cst));
		
		cst.init(SENSOR_MAC_PPC_TYPE1, SENSOR_STRLG_MAC_PPC1, SENSOR_STRSH_MAC_PPC1);
		m_map.insert(make_pair(SENSOR_MAC_PPC_TYPE1, cst));
		
		cst.init(SENSOR_MAC_PPC_TYPE2, SENSOR_STRLG_MAC_PPC2, SENSOR_STRSH_MAC_PPC2);
		m_map.insert(make_pair(SENSOR_MAC_PPC_TYPE2, cst));
		
		cst.init(SENSOR_MAC_PPC_TYPE3, SENSOR_STRLG_MAC_PPC3, SENSOR_STRSH_MAC_PPC3);
		m_map.insert(make_pair(SENSOR_MAC_PPC_TYPE3, cst));
		
		cst.init(SENSOR_MAC_INTEL, SENSOR_STRLG_MAC_INTEL, SENSOR_STRSH_MAC_INTEL);
		m_map.insert(make_pair(SENSOR_MAC_INTEL, cst));
		
		cst.init(SENSOR_WIN_THINKPAD, SENSOR_STRLG_WIN_THINKPAD, SENSOR_STRSH_WIN_THINKPAD);
		m_map.insert(make_pair(SENSOR_WIN_THINKPAD, cst));
		
		cst.init(SENSOR_WIN_HP, SENSOR_STRLG_WIN_HP, SENSOR_STRSH_WIN_HP);
		m_map.insert(make_pair(SENSOR_WIN_HP, cst));
		
		cst.init(SENSOR_ANDROID, SENSOR_STRLG_ANDROID, SENSOR_STRSH_ANDROID);
		m_map.insert(make_pair(SENSOR_ANDROID, cst));
		
		cst.init(SENSOR_USB_JW24F8, SENSOR_STRLG_USB_JW24F8, SENSOR_STRSH_USB_JW24F8);
		m_map.insert(make_pair(SENSOR_USB_JW24F8, cst));
		
		cst.init(SENSOR_USB_JW24F14, SENSOR_STRLG_USB_JW24F14, SENSOR_STRSH_USB_JW24F14);
		m_map.insert(make_pair(SENSOR_USB_JW24F14, cst));
		
		cst.init(SENSOR_USB_MOTIONNODEACCEL, SENSOR_STRLG_USB_MN, SENSOR_STRSH_USB_MN);
		m_map.insert(make_pair(SENSOR_USB_MOTIONNODEACCEL, cst));
		
		cst.init(SENSOR_USB_ONAVI_1, SENSOR_STRLG_USB_ONAVI_1, SENSOR_STRSH_USB_ONAVI_1);
		m_map.insert(make_pair(SENSOR_USB_ONAVI_1, cst));

		cst.init(SENSOR_USB_ONAVI_A_12, SENSOR_STRLG_USB_ONAVI_A_12, SENSOR_STRSH_USB_ONAVI_A_12);
		m_map.insert(make_pair(SENSOR_USB_ONAVI_A_12, cst));

		cst.init(SENSOR_USB_ONAVI_B_16, SENSOR_STRLG_USB_ONAVI_B_16, SENSOR_STRSH_USB_ONAVI_B_16);
		m_map.insert(make_pair(SENSOR_USB_ONAVI_B_16, cst));

		cst.init(SENSOR_USB_ONAVI_C_24, SENSOR_STRLG_USB_ONAVI_C_24, SENSOR_STRSH_USB_ONAVI_C_24);
		m_map.insert(make_pair(SENSOR_USB_ONAVI_C_24, cst));

		cst.init(SENSOR_USB_PHIDGETS_1056, SENSOR_STRLG_USB_PHIDGETS_1056, SENSOR_STRSH_USB_PHIDGETS_1056);
		m_map.insert(make_pair(SENSOR_USB_PHIDGETS_1056, cst));

		cst.init(SENSOR_USB_PHIDGETS_1044, SENSOR_STRLG_USB_PHIDGETS_1044, SENSOR_STRSH_USB_PHIDGETS_1044);
		m_map.insert(make_pair(SENSOR_USB_PHIDGETS_1044, cst));
    
		cst.init(SENSOR_USB_PHIDGETS_1042, SENSOR_STRLG_USB_PHIDGETS_1042, SENSOR_STRSH_USB_PHIDGETS_1042);
		m_map.insert(make_pair(SENSOR_USB_PHIDGETS_1042, cst));

		cst.init(SENSOR_USB_PHIDGETS_1041, SENSOR_STRLG_USB_PHIDGETS_1041, SENSOR_STRSH_USB_PHIDGETS_1041);
		m_map.insert(make_pair(SENSOR_USB_PHIDGETS_1041, cst));

		cst.init(SENSOR_USB_PHIDGETS_1043, SENSOR_STRLG_USB_PHIDGETS_1043, SENSOR_STRSH_USB_PHIDGETS_1043);
		m_map.insert(make_pair(SENSOR_USB_PHIDGETS_1043, cst));

        
	}
}

CSensor::~CSensor()
{
    if (m_port>-1) closePort();
}

bool CSensor::getSingleSampleDT()
{
   return m_bSingleSampleDT;
}

void CSensor::setSingleSampleDT(const bool bSingle)
{
   m_bSingleSampleDT = bSingle;
}

const char* CSensor::getSensorStr() 
{
   return m_strSensor.c_str();
}

void CSensor::setSensorStr(const char* strIn)
{
    if (strIn)
       m_strSensor.assign(strIn);
    else
#if defined(_WIN32) || defined(__APPLE_CC__)
       m_strSensor.clear();
#else
       m_strSensor.assign("");
#endif
}

void CSensor::setType(e_sensor esType)
{
   m_iType = esType;
}

void CSensor::setPort(const int iPort)
{
   m_port = iPort;
}

int CSensor::getPort() 
{
   return m_port;
}

void CSensor::closePort()
{
    fprintf(stdout, "Closing port...\n");
}

e_sensor CSensor::getTypeEnum()
{
   return m_iType;
}

const char* CSensor::getTypeStr(int iType)
{
	if (iType == -1) iType = m_iType;  // default is to use the type for the given CSensor
	//if (m_strSensor.empty()) {
		map<int, CSensorType>::iterator iter = m_map.find(iType);
		if (iter != m_map.end() ) 
			return iter->second.getStr();
		else 
			return "";
	//}
	//else {
	//	return m_strSensor.c_str();
	//}
}


const char* CSensor::getTypeStrShort(int iType)
{
	if (iType == -1) iType = m_iType;  // default is to use the type for the given CSensor
	map<int, CSensorType>::iterator iter = m_map.find(iType);
	if (iter != m_map.end() ) 
		return iter->second.getStrSh();
	else 
		return "";
}



// this is the heart of qcn -- it gets called 50-500 times a second!
inline bool CSensor::mean_xyz()
{
/* This subroutine finds the mean amplitude for x,y, & z of the sudden motion 
 * sensor in a window dt from time t0.
 */
   static long lLastSample = 10L;  // store last sample size, start at 10 so doesn't give less sleep below, but will if lastSample<3
   static double dLast[4] = {0.0, 0.0, 0.0, 0.0};
   static long lError = 0;
   static long lErrorCumulative = 0;
   float x1,y1,z1;
   double dTimeDiff=0.0f;

   // set up pointers to array offset for ease in functions below
   float *px2, *py2, *pz2;
   double *pt2;

#ifdef QCN_USB
   if (!sm || smState->bStop) throw EXCEPTION_SHUTDOWN;   // see if we're shutting down, if so throw an exception which gets caught in the sensor_thread
   px2 = (float*) &(sm->x0);
   py2 = (float*) &(sm->y0);
   pz2 = (float*) &(sm->z0);
   pt2 = (double*) &(sm->t0);
#else
   if (qcn_main::g_iStop || !sm) throw EXCEPTION_SHUTDOWN;   // see if we're shutting down, if so throw an exception which gets caught in the sensor_thread

   px2 = (float*) &(sm->x0[sm->lOffset]);
   py2 = (float*) &(sm->y0[sm->lOffset]);
   pz2 = (float*) &(sm->z0[sm->lOffset]);
   pt2 = (double*) &(sm->t0[sm->lOffset]);
#endif

#ifdef _DEBUG   // lots of output below!
	static FILE* fileDebug = NULL;
	if (!fileDebug) {
		fileDebug = (FILE*) fopen("sensoroutput.txt", "wt");
	}
#endif
	
   sm->bWriting = true;
   sm->lSampleSize = 0L; 
   *px2 = *py2 = *pz2 = 0.0f;  // zero sample averages
   *pt2 = 0.0f;
		 
   // first check if we're behind time, i.e. the last time is greater than our dt, if so carry over the last value and get out fast so it can catch up
   // note if we want the raw data, don't do this check as we want the "misses"
#ifndef QCN_RAW_DATA
	if (dLast[3] > sm->t0check) {
	   // weird timing issue, i.e. current time is greater than the requested time		
		dLast[3] = dtime();  // save this current time
		*px2 = dLast[0]; 
		*py2 = dLast[1]; 
		*pz2 = dLast[2];
		*pt2 = sm->t0check;  // use this point for the requested time and hope it catches up, if it exceeds our error time (.5 sec) it will reset  
#ifdef _DEBUG
		if (fileDebug) { 
			fprintf(fileDebug, "Falling back time:  Cur=%f  Req=%f  Err=%ld\n", dLast[3], sm->t0check, lError);
			fprintf(fileDebug, "sensorout,%f,%f,%f,%d,%ld,%f, %f, %f, %f\n",
					sm->t0check, sm->t0active, dTimeDiff, sm->iNumReset, sm->lSampleSize, sm->dt, dLast[0], dLast[1], dLast[2]);
		}
#endif		
          // reset if this correction exceeds our time limit or this has been done too much in one session
		if (++lError > (TIME_ERROR_SECONDS / sm->dt) || ++lErrorCumulative > ERROR_CUMULATIVE_CUTOFF) {
			dTimeDiff = (double) (lError-1) * sm->dt;
			sm->bWriting = false;
			goto error_Timing;
		}
		sm->t0check += sm->dt;  // make a new "target" t0check
                sm->bWriting = false;
		//usleep(DT_MICROSECOND_SAMPLE); // sleep a little so it's not an instantaneous return
		return true;
	}
#endif // QCN_RAW_DATA
	
   // this will get executed at least once, then the time is checked to see if we have enough time left for more samples
   do {
#ifndef QCN_USB
	   if (qcn_main::g_iStop || !sm) throw EXCEPTION_SHUTDOWN;   // see if we're shutting down, if so throw an exception which gets caught in the sensor_thread
#endif
       if ( (!m_bSingleSampleDT && sm->lSampleSize < SAMPLE_SIZE)
		 || (m_bSingleSampleDT && sm->lSampleSize == 0) )  { // only go in if less than our sample # and we're not a single-sample sensor, or a single-sample sensor & haven't been in yet
           x1 = y1 = z1 = 0.0f; 
    	   // note that x/y/z should be scaled to +/- 2g, return values as +/- 2.0f*EARTH_G (in define.h: 9.78033 m/s^2)
           if (read_xyz(x1, y1, z1)) {  // not a fatal error if fails, just don't count this point
			   *px2 += (x1 * sm->fCorrectionFactor); 
			   *py2 += (y1 * sm->fCorrectionFactor); 
			   *pz2 += (z1 * sm->fCorrectionFactor); 
            }
            else {
		*px2 += dLast[0]; 
		*py2 += dLast[1]; 
		*pz2 += dLast[2];
#ifndef QCN_RAW_DATA
                if (++lError > (TIME_ERROR_SECONDS / sm->dt) || ++lErrorCumulative > ERROR_CUMULATIVE_CUTOFF) {
                        dTimeDiff = (double) (lError-1) * sm->dt;
                        sm->bWriting = false;
                        goto error_Timing;
                }
#endif // skip the reset for raw data

            }
            sm->lSampleSize++; // only increment if not a single sample sensor
        }  // done sample size stuff

       // dt is in seconds, want to slice it into 10 (SAMPLING_FREQUENCY), put into microseconds, so multiply by 100000
       // using usleep saves all the FP math every millisecond

       // sleep for dt seconds, this is where the CPU time gets used up, for dt/10 6% CPU, for dt/1000 30% CPU!
       // note the use of the "lLastSample" -- if we are getting low sample rates i.e. due to an overworked computer,
       // let's drop the sleep time dramatically and hope it can "catch up"
       //usleep((long) lLastSample < 3 ? DT_MICROSECOND_SAMPLE/100 : DT_MICROSECOND_SAMPLE);   

       usleep(DT_MICROSECOND_SAMPLE); // usually 2000, which is 2 ms or .002 seconds, 10 times finer than the .02 sec / 50 Hz sample rate
       sm->t0active = dtime(); // use the function in the util library (was used to set t0)
       dTimeDiff = sm->t0check - sm->t0active;  // t0check should be bigger than t0active by dt, when t0check = t0active we're done
   }
   while (dTimeDiff > 0.0f && dTimeDiff < TIME_ERROR_SECONDS && sm->t0active > dLast[3]);
	
   // somehow it seems the clock can occasionally have less time than the last value, 
   // so set the tactive/pt2 to be the last time + dt, which is more realistic
	if (sm->t0active < dLast[3]) {
		sm->t0active = dLast[3] + sm->dt;
	}

   lLastSample = sm->lSampleSize;

   // store values i.e. divide by sample size
   if (sm->lSampleSize > 1) {  // only divide to get the mean if greater than 1 sample taken
		*px2 /= (float) sm->lSampleSize; 
		*py2 /= (float) sm->lSampleSize; 
		*pz2 /= (float) sm->lSampleSize; 
   }
   *pt2 = sm->t0active; // save the time into the array, this is the real clock time
#ifdef _DEBUG   // lots of output below!
   fprintf(fileDebug, "sensorout,%f,%f,%f,%d,%ld,%f,%f,%f,%f\n",
      sm->t0check, sm->t0active, dTimeDiff, sm->iNumReset, sm->lSampleSize, sm->dt, *px2, *py2, *pz2);
   //fflush(stdout);
#endif


   if (fabs(dTimeDiff) > TIME_ERROR_SECONDS) { 
      // if our times are different by a second, that's a big lag, so let's reset t0check to t0active
           sm->bWriting = false;
	   goto error_Timing;
   }

   sm->ullSampleTotal += sm->lSampleSize;
   sm->ullSampleCount++;

   sm->fRealDT += (float) fabs(sm->t0active - sm->t0check);

   dLast[0] = *px2; 
   dLast[1] = *py2; 
   dLast[2] = *pz2;    // save current values as they may carry forward if the computer "skips"
   dLast[3] = *pt2;
   lError = 0;  // reset timing error values as must be OK now

   // if active time is falling behind the checked (wall clock) time -- set equal, may have gone to sleep & woken up etc
   sm->t0check += sm->dt;   // t0check is the "ideal" time i.e. start time + the dt interval

   sm->bWriting = false;
   //sm->writepos = 10;
   
   return true;

error_Timing:    // too many timing errors encountered, should probably drop back dt?
	fprintf(stdout, "Timing error encountered t0check=%f  t0active=%f  diff=%f  timeadj=%d  sample_size=%ld, dt=%f, lErr=%ld, lErrCum=%ld resetting...\n", 
			sm->t0check, sm->t0active, dTimeDiff, sm->iNumReset, sm->lSampleSize, sm->dt, lError, lErrorCumulative);
	fprintf(stdout, "  Last values were %f %f %f %f\n", dLast[0], dLast[1], dLast[2], dLast[3]);
	fprintf(stderr, "Timing error encountered t0check=%f  t0active=%f  diff=%f  timeadj=%d  sample_size=%ld, dt=%f, lErr=%ld, lErrCum=%ld resetting...\n", 
			sm->t0check, sm->t0active, dTimeDiff, sm->iNumReset, sm->lSampleSize, sm->dt, lError, lErrorCumulative);
	fprintf(stderr, "  Last values were %f %f %f %f\n", dLast[0], dLast[1], dLast[2], dLast[3]);
	lError = 0;  
	lErrorCumulative = 0;  // reset error counts  

//#ifdef _DEBUG_QCNLIVE
//	return true;
//#else
	return false;   // if we're not debugging, this is a serious run-time problem, so reset time & counters & try again
//#endif
	
}

