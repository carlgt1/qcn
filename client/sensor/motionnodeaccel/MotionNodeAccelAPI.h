/**
  @file    MotionNodeAccelAPI.h
  @author  Luke Tokheim, luke@motionnode.com
  @version 1.2

  (C) Copyright GLI Interactive LLC 2008. All rights reserved.

  UNPUBLISHED -- Rights reserved under the copyright laws of the United States.
  Use of a copyright notice is precautionary only and does not imply
  publication or disclosure.

  THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY INFORMATION OF
  GLI INTERACTIVE LLC. ANY DUPLICATION, MODIFICATION, DISTRIBUTION, OR
  DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY PROHIBITED WITHOUT
  THE PRIOR EXPRESS WRITTEN PERMISSION OF GLI INTERACTIVE LLC.
*/
#ifndef __MOTION_NODE_ACCEL_API_H_
#define __MOTION_NODE_ACCEL_API_H_


#if defined(WIN32)
#  if defined(BUILD_ACCEL_API)
#    define MOTIONNODE_IMPORT_API __declspec(dllexport)
#  else
#    define MOTIONNODE_IMPORT_API __declspec(dllimport)
#  endif // BUILD_ACCEL_API
#  define MOTIONNODE_CALL_API __stdcall
#  define MOTIONNODE_CALL_C_API __cdecl
#else
#  define MOTIONNODE_IMPORT_API
#  define MOTIONNODE_CALL_API
#  define MOTIONNODE_CALL_C_API
#endif // WIN32

/**
  ApiVersion % 100 is the sub-minor version
  ApiVersion / 100 % 1000 is the minor version
  ApiVersion / 100000 is the major version
*/
#define MOTIONNODE_ACCEL_API_VERSION 100200


/**
Implements a standalone class to integrate a single MotionNode Accel device
into a C++ application. Depends on the dynamic library supplied with your
MotionNode software installation.

Example usage:

@code
MotionNodeAccel * node = MotionNodeAccel::Factory();
if (NULL == node) {
  // Failed to create standalone instance. API
  // version mismatch.
  return false;
}

// Scan for any available MotionNode devices and
// connect to the first one found. Start reading.
if (node->connect() && node->start()) {
  int i = 0;
  while (i++ < 100) {
    // Call the blocking sample function to read
    // data from the MotionNode. Then we can copy
    // data out with the non-blocking accessor
    // functions.
    if (node->sample()) {      
      float sensor[3];
      if (node->get_sensor(sensor)) {
        std::cout
          << "a = ["
          << sensor[0] << ", "
          << sensor[1] << ", "
          << sensor[2] << "] g"
          << std::endl;
      }
    }
  }

  // We can stop reading data, close the connection
  // and reconnect with the same instance. The
  // destructor will close the connection for us,
  // but be verbose for the example.
  if (node->stop() && node->close()) {
  } else {
    // Failed to stop reading or close connection.
  }
} else {
  // Failed to connect or start reading data.
}

delete node; // Or node->destroy(); is the same.
@endcode
*/
class MOTIONNODE_IMPORT_API MotionNodeAccel {
public:
  /**
    Real valued type of this build.
  */
  typedef float real_type;
  /**
    Storage for the raw data, values from the analog to digital
    conversion.
  */
  typedef short raw_type;

  /**
    Define the version of this interface. Allow for run-time compatibility
    enforcement.
  */
  static const int ApiVersion = MOTIONNODE_ACCEL_API_VERSION;

  /**
    Public destruction of this instance. Client applications should either call
    "instance->destroy()" or "delete instance" to deallocate resources
    associated with this object.
  */
  virtual void MOTIONNODE_CALL_API destroy() = 0;

  /**
    Open a connection to a MotionNode Accel. Scans for available, unconnected
    devices unless the client calls MotionNodeAccel#set_id first.

    @pre    !is_connected()
    @post   is_connected()
    @return Returns true iff a connection was successfully made
            to a MotionNode Accel sensor.
  */
  virtual bool MOTIONNODE_CALL_API connect() = 0;
  
  /**
    Start reading data from this device.

    @pre    is_connected()
    @pre    !is_reading()
    @post   is_reading()
  */
  virtual bool MOTIONNODE_CALL_API start() = 0;
  
  /**
    Read a single sample of data from this device. This is the only blocking
    I/O call in this interface. Use the data accessors
    MotionNodeAccel#get_sensor and MotionNodeAccel#get_raw to get the data
    from this sample.

    @pre    is_reading()
    @note   Blocking I/O method.
  */
  virtual bool MOTIONNODE_CALL_API sample() = 0;
  
  /**
    Stop reading data from this device.

    @pre    is_reading()
    @post   !is_reading()
  */
  virtual bool MOTIONNODE_CALL_API stop() = 0;
  
  /**
    Close connection to this device.

    @pre    is_connected()
    @post   !is_connected()
  */
  virtual bool MOTIONNODE_CALL_API close() = 0;

  /**
    @return Returns true iff this device is currently connnected. Note that a
            connection is exclusive, only one can exist per hardware device.
  */
  virtual bool MOTIONNODE_CALL_API is_connected() const = 0;

  /**
    @pre    is_connected()
    @return Returns true iff this device is currently connnected and reading
            data.
  */
  virtual bool MOTIONNODE_CALL_API is_reading() const = 0;

  /**
    Associate this instance with a specific MotionNode device, identified by
    a data source string. For example, a Source in the MotionNode User
    Interface named "usb://000-00000" defines a USB device with serial
    number "000-00000".

    @param  id Do not scan for available devices, instead connect
            to a sensor by this id, or serial number.
            user interface.
    @pre    !is_connected()
  */
  virtual bool MOTIONNODE_CALL_API set_source(const char * source) = 0;

  /**
    Set accelerometer g select parameter.
    
    @param  value Select the accelerometer g range. Valid values
            include 2 g and 6 g for the current hardware.
    @pre    !is_connected()
  */  
  virtual bool MOTIONNODE_CALL_API set_gselect(real_type value) = 0;

  /**
    Set node sample rate by scalar delay.
    
    @param  value [0, 1] where 0 specifies the default delay. Otherwise,
            1 specifies the maximum delay and the slowest sample rate.
    @pre    !is_connected()
  */
  virtual bool MOTIONNODE_CALL_API set_delay(real_type value) = 0;

  /**
    Calibrated, but otherwise unprocessed, sensor data. Defined as
    sensor = [accelerometer] where sensor = [ax, ay, az].

    Accelerometer data is specified in g, where 1 g ~ 9.8 meter/second^2,
    or the magnitude of gravity on the surface of the Earth. Domain is
    [-2, 2] or [-6, 6] depending on selectable g range.

    @param  sensor Output buffer for the sample of sensor data.
    @pre    is_reading()
  */
  virtual bool MOTIONNODE_CALL_API get_sensor(real_type value[3]) const = 0;

  /**
    Raw sensor data, values directly out of the device ADC. Defined as
    raw = [accelerometer] where raw = [ax, ay, az].

    Valid domain of all raw data is [0, 4095].

    @param  raw Output buffer for the sample of raw data.
    @pre    is_reading()
  */
  virtual bool MOTIONNODE_CALL_API get_raw(raw_type value[3]) const = 0;

  /**
    Temperature data from the sensor. Specified in degrees Celsius.
    Not available on all MotionNode Accel sensors.

    @param  value Current temperature is stored in this value.
    @pre    is_reading()
  */
  virtual bool MOTIONNODE_CALL_API get_temperature(real_type & value) const = 0;

  /**
    Get the per sample time step for the connected device in
    seconds. The default value is 1/60 ~ 0.0167, or a 60 Hz
    sample rate.

    @pre    is_connected()
  */
  virtual bool MOTIONNODE_CALL_API get_time_step(real_type & value) const = 0;

  /**
    Return the number of available devices. If a device has an active
    connection it will not be included in this count.

    Note that this function is not connection oriented. It does, however,
    require that this object is not connected to simplify semantics.

    @pre    !is_connected()
  */
  virtual bool MOTIONNODE_CALL_API get_num_device(unsigned & count) const = 0;

  /**
    Return the last error message.

    @return Returns NULL if no error string is available.
  */
  virtual const char * MOTIONNODE_CALL_API get_error_message() const = 0;

  /**
    Return an instance of the MotionNode resource
    class. Client must call delete or destroy on
    the instance to deallocate resources.

    @param  api_version API version the client application is
            compiled with.
    @return Returns NULL if the client requests an incompatible
            API version.
  */
  static MotionNodeAccel * MOTIONNODE_CALL_API Factory(int api_version=ApiVersion);

  /**
    Allow usage of the delete operator that uses the
    memory management functions in the dynamic library.
  */
  void MOTIONNODE_CALL_API operator delete(void * p)
  {
    if (p) {
      MotionNodeAccel * instance = static_cast<MotionNodeAccel *>(p);
      instance->destroy();
    }
  }
protected:
  /**
    Hide the implementation of the constructor. Only allow
    access from child classes.
  */
  MotionNodeAccel()
  {
  }
  /**
    Replace the virtual destructor with the above delete
    operator implementation. Calls destroy in the shared
    library.
  */
  //virtual ~MotionNodeAccel() = 0;
  
  /**
    Disable the copy constructor.

    This is a resource object. Copy constructor semantics would be confusing
    at the very least. Disable it instead.
  */
  MotionNodeAccel(const MotionNodeAccel & rhs);
  /**
    Disable the assignment operator.

    @see MotionNodeAccel#MotionNodeAccel(const MotionNodeAccel &)
  */
  const MotionNodeAccel & operator=(const MotionNodeAccel & lhs);
}; // class MotionNodeAccel


extern "C" {

/**
  Return an instance of the MotionNode resource class. Client must
  call delete or destroy on the instance to deallocate resources.

  Calls MotionNodeAccel::Factory internally.

  @param  api_version API version the client application is
          compiled with.
  @return Returns NULL if the client requests an incompatible
          API version.
*/
MOTIONNODE_IMPORT_API
MotionNodeAccel * MOTIONNODE_CALL_C_API MotionNodeAccel_Factory(int api_version);
 
} // extern "C"

#endif // __MOTION_NODE_ACCEL_API_H_
