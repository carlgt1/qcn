#ifndef _CSENSOR_H_
#define _CSENSOR_H_

/*
 *  csensor.h
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2007 Stanford University
 *
 * This file contains the base class for QCN-approved accelerometers, mostly pure virtual functions requiring definition in csensor-*
 */

#include "define.h"
#include <string>
#include <map>

using std::pair;
using std::make_pair;
using std::map;



// simple class to store/retrieve sensor info
class CSensorType
{
private:
	int m_id;
	char m_strLong[64];
	char m_strShort[3];
	
public:
	CSensorType() { clear(); }
	CSensorType(const int id, const char* strLong, const char* strShort) 
	{ 
		init(id, strLong, strShort);
	}
	~CSensorType() { clear(); }
	void clear() { memset(this, 0x00, sizeof(CSensorType)); }
	void init(const int id, const char* strLong, const char* strShort)
	{
		clear(); 
		m_id = id; 
		strncpy(m_strLong, strLong, sizeof(m_strLong)); 
		strncpy(m_strShort, strShort, sizeof(m_strShort)); 
	}	
	int getID() { return m_id; }
	void setStr(const char* strIn) { memset(m_strLong, 0x00, sizeof(char) * 64); strncpy(m_strLong, strIn, 63); }
	const char* getStr() { return m_strLong; }
	const char* getStrSh() { return m_strShort; }
	
	// public static member functions to operate on all instances of CSensorType (i.e. enumerate strings)
	//static const int getIDByStr(const char* strTest);
	//static const char* getStrByID(const int iTest);
	//static const char* getStrShByID(const int iTest);
	
};

/*
struct StSensor
{
   int iSerialNum;
   int eSensor;
   int iPort;
   char strName[32];

   StSensor::StSensor() { memset(this, 0x00, sizeof(StSensor); }
}
 */

// this is the base class for all QCN sensors
class CSensor
{
protected:
	e_sensor m_iType; // what type of sensor, i.e. Thinkpad, HP, USB?
	
private:
      // private member vars
      int m_port;  // port number, -1 if no sensor opened, if >-1 then we have a port number (i.e. joystick port, Apple I/O port, subclass-specific)
      bool m_bSingleSampleDT; // set to true if just want a single sample per dt interval
      std::string m_strSensor;  // identifying string (optional, can also use getTypeStr() for a generic sensor name)  
 
      // private function
      // note that x/y/z should be scaled to +/- 2g, return values as +/- 2.0f*EARTH_G (in define.h: 9.78033 m/s^2)
      virtual bool read_xyz(float& x1, float& y1, float& z1) = 0;   // read raw sensor data, pure virtual function subclass implemented  

      static map<int, CSensorType> m_map;  // map enum ID's to sensor name
      //static vector<struct StSensor> m_sensors; // store all sensors detected here

   public:
     CSensor();
     virtual ~CSensor();  // virtual destructor that will basically just call closePort

     void setPort(const int iPort = -1);
     int getPort();

     void setType(e_sensor esType = SENSOR_NOTFOUND);

     const char* getSensorStr();
     void setSensorStr(const char* strIn = NULL);

     bool getSingleSampleDT();
     void setSingleSampleDT(const bool bSingle);

     // pure virtual functions that subclasses of CSensor (for specific sensor types) need to implement
     virtual bool detect() = 0;   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found
     //virtual bool detect_multiple() { m_sensors.clear(); };   // this detects & initializes a sensor on a Mac G4/PPC or Intel laptop, sets m_iType to 0 if not found

	 // get sensor id or string based on the map of sensors (m_map private member var initialized in constructor)
	 //void setTypeStr(const char* strTypeStr);  // set the sensor name for this iType
	 virtual const char* getTypeStr(int iType = -1);  // return the sensor name for this iType
	 virtual const char* getTypeStrShort(int iType = -1);  // return the sensor name (short) for this iType or m_Type if not entered
	 // const int getID();
	
     // public virtual functions implemented in CSensor but can be overridden
     virtual void closePort(); // closes the port if open
     virtual e_sensor getTypeEnum(); // return the iType member variable

     virtual bool mean_xyz();   // mean sensor data, implemented here but can be overridden
	
};

#endif

