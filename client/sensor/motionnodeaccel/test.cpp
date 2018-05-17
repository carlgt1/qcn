#include <MotionNodeAccelAPI.h>

#include <iostream>

// Set to true value to enable linking at runtime
// through the dlfcn interface
#define RUNTIME_LINK 1


void run_node_test(MotionNodeAccel * node, int n)
{
  // Detect the number of available devices.
  //unsigned count = 0;
  //node->get_num_device(count);

  // Set the G range. Default is 2.
  //node->set_gselect(2.0);

  //float rate = 100;
  //float delay = 1 - (rate - 50) * 0.0125;
  //node->set_delay(0.375);

  if (node->connect()) {
    if (node->start()) {
      for (int i=0; i<n; i++) {
        float a[3] = {0, 0, 0};
        if (node->sample() && node->get_sensor(a)) {
          std::cout
            << "a = ["
            << a[0] << ", "
            << a[1] << ", "
            << a[2] << "] g"
            << std::endl;
        } else {
          std::cerr << "Failed to read sample" << std::endl;
        }
      }
      node->stop();
    } else {
      std::cerr << "Failed to start reading" << std::endl;
    }

    node->close();
  } else {
    std::cerr << "Failed to connect" << std::endl;
  }
}


#if RUNTIME_LINK

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif // WIN32

typedef MotionNodeAccel * (MOTIONNODE_CALL_C_API * MN_FACTORY_FN)(int);

int main(int argc, char * argv[])
{
#ifdef WIN32
  HMODULE library_handle = LoadLibrary("MotionNodeAccelAPI.dll");
#else
#ifdef MACOSX
  void * library_handle = dlopen("../build_osx/libMotionNodeAccelAPI.dylib", RTLD_LAZY);
#else
  void * library_handle = dlopen("../build/libMotionNodeAccelAPI.so", RTLD_LAZY);
#endif // WIN32
#endif // WIN32
  if (NULL != library_handle) {

#ifdef WIN32
    MN_FACTORY_FN factory = reinterpret_cast<MN_FACTORY_FN>(GetProcAddress(library_handle, "MotionNodeAccel_Factory"));
#else
    MN_FACTORY_FN factory = reinterpret_cast<MN_FACTORY_FN>(dlsym(library_handle, "MotionNodeAccel_Factory"));
#endif // WIN32
    if (NULL != factory) {

      MotionNodeAccel * node = factory(MOTIONNODE_ACCEL_API_VERSION);
      if (NULL != node) {
        // Do some sampling. Print results.
        run_node_test(node, 10);

        // Clean up.
        node->destroy();
      } else {
        std::cerr << "failed create node instance" << std::endl;
      }

    } else {
      std::cerr << "failed to bind C factory function" << std::endl;
    }

#ifdef WIN32
    if (FreeLibrary(library_handle)) {
#else
    if (0 == dlclose(library_handle)) {
#endif // WIN32
    } else {
      std::cerr << "failed to close library" << std::endl;
    }

  } else {
    std::cerr << "failed to open library" << std::endl;
  }

  return 0;
}

#else

int main(int argc, char * argv[])
{
  {
    MotionNodeAccel * node = MotionNodeAccel::Factory();
    if (NULL != node) {
      // Do some sampling. Print results.
      run_node_test(node, 10);

      // Clean up. Same as node->destroy().
      delete node;

    } else {
      std::cerr << "API version mismatch" << std::endl;
    }
  }
  return 0;
}

#endif // RUNTIME_LINK
