#ifndef _QCN_SEMAPHORE_H_
#define _QCN_SEMAPHORE_H_

// semaphore code for external USB accelerometers on the Mac
// ref: http://kevincathey.com/code/using-counting-semaphores-on-mac-os-x/

#ifdef __APPLE_CC__
  #include <mach/semaphore.h>
  #include <mach/mach_init.h>
  #include <mach/task.h>
#else // Linux
  #include <semaphore.h>
#endif

namespace qcn_semaphore {

void create(void* semStructure, int initialValue);
void signal(void* semStructure);
void wait(void* semStructure);

}

#endif

