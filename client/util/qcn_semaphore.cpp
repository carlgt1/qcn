#include "qcn_semaphore.h"

namespace qcn_semaphore {

void create(void* semStructure, int initialValue)
{
    #ifdef __APPLE_CC__
    semaphore_create(mach_task_self(), (semaphore_t *)semStructure, SYNC_POLICY_FIFO, initialValue);
    #else
    int pshared = 0;
    sem_init((sem_t *)semStructure, pshared, initialValue);
    #endif
}

void signal(void* semStructure)
{
    #ifdef __APPLE_CC__
    semaphore_signal(*((semaphore_t *)semStructure));
    #else
    sem_post((sem_t *)semStructure);
    #endif
}

void wait(void* semStructure)
{
    #ifdef __APPLE_CC__
    semaphore_wait(*((semaphore_t *)semStructure));
    #else
    sem_wait((sem_t *)semStructure);
    #endif
}

}

