#include "main.h"
#include "qcn_thread.h"

#ifdef _WIN32
      HANDLE    m_handleThread;
#else
   #include <pthread.h>
      pthread_t m_handleThread;
#endif
      long m_lStackSize;
      QCNThreadFunctionPointer m_funcptr;
      bool m_bRunThread;  // thread state running status so we know internally if Start() really did create the thread etc
      bool m_bRunning;    // internal to the thread run state, which we will access via qcn_main::g_threadSensor->IsRunning() & set within the thread function etc
      bool m_bSuspended;  // flag if thread is suspended
      unsigned long m_ThreadId;


CQCNThread::CQCNThread(QCNThreadFunctionPointer ptrfunc, long lStackSize)
   : m_handleThread(0), 
     m_lStackSize(lStackSize), 
     m_funcptr(ptrfunc), 
     m_bRunThread(false),
     m_bRunning(false),
     m_bSuspended(false),
     m_ThreadId(0)
{
}

CQCNThread::~CQCNThread()
{
   if (m_bRunThread && m_handleThread) {
       Stop();
   }
}

void CQCNThread::SetFunctionPointer(QCNThreadFunctionPointer ptrfunc)
{
   m_funcptr = ptrfunc; 
}

bool CQCNThread::Start(bool bHighPriority)
{
    if (!m_funcptr) {
       fprintf(stderr, "CQCNThread: No function pointer set\n");
       return false; 
    }
    if (m_bRunThread && IsRunning()) {
       //fprintf(stderr, "CQCNThread: Thread is already running!\n");
       return false; 
    }
    if (m_bSuspended) {
       //fprintf(stderr, "CQCNThread: Thread is suspended!\n");
       return false; 
    }

    Stop();  // this will clear the thread state, if we made it here we have a valid thread handle from before, but IsRunning() is false so thread is done

#ifdef _WIN32
    // get the worker thread handle
    //
    m_handleThread = ::CreateThread(NULL, m_lStackSize, m_funcptr, NULL, 0, &m_ThreadId);
    if (!m_handleThread) {
        // thread wasn't created
        fprintf(stderr, "CQCNThread: Windows thread not created, error = %ld\n", ::GetLastError());
        return false;
    }

// use higher priority for QCNLive
#ifdef QCNLIVE
    ::SetThreadPriority(m_handleThread, bHighPriority ? THREAD_PRIORITY_HIGHEST : THREAD_PRIORITY_NORMAL);
#else
    ::SetThreadPriority(m_handleThread, bHighPriority ? THREAD_PRIORITY_HIGHEST : THREAD_PRIORITY_IDLE);
#endif

/*
    ::DuplicateHandle(
        GetCurrentProcess(),
        GetCurrentThread(),
        GetCurrentProcess(),
        &m_handleThread,
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS
    );

    // Create the thread
    m_thread = _beginthreadex(
        NULL,
        m_lStackSize,       // stack size
        m_funcptr,
        0,
        0,
        &m_uiThreadId
    );

    if (!m_thread) {
        fprintf(stderr, "CQCNThread::Start(): _beginthreadex() failed, errno %d\n", errno);
        return false;
    }

    // lower our priority
    //
#ifdef QCNLIVE
    ::SetThreadPriority(m_handleThread, THREAD_PRIORITY_NORMAL); // no need for low priority in the GUI
#else
    ::SetThreadPriority(m_handleThread, THREAD_PRIORITY_IDLE);
#endif
*/


#else   // POSIX threads (pthreads) for Mac & Linux
    // the thread will have the priority class of the process i.e. idle if under BOINC, normal if under QCNLIVE
    pthread_attr_t thread_attrs;
    sched_param sparam;
    int iRet = 0;

    // get initial threat attributes
    iRet = pthread_attr_init(&thread_attrs);

    // set stack size for thread
    if (iRet || (iRet = pthread_attr_setstacksize(&thread_attrs, m_lStackSize))) {
       fprintf(stderr, "CQCNThread::Start(): Error in setting thread stack size of %ld - iRet = %d\n", m_lStackSize, iRet);
    } /*
    else {
       fprintf(stdout, "Successfully set thread stack size of %ld\n", m_lStackSize);
    } */

    // set priority level if higher priority required
    if (bHighPriority) {
      iRet = pthread_attr_getschedparam (&thread_attrs, &sparam);  // first get the default scheduling params
      sparam.sched_priority = SCHED_FIFO;  // set the priority level to highest
      // set the new high priority in the sched_param
      if (iRet || (iRet = pthread_attr_setschedparam (&thread_attrs, &sparam))) {
         fprintf(stderr, "CQCNThread::Start(): Error in setting thread priority %d\n", iRet);
      }
#ifdef _DEBUG
      else {
         fprintf(stdout, "CQCNThread::Start(): Thread priority set to high\n");
      }
#endif
    }

    // now create the POSIX thread using our attributes
    iRet = pthread_create(&m_handleThread, &thread_attrs, m_funcptr, NULL);
    if (iRet) {
        fprintf(stderr, "CQCNThread::Start(): pthread_create(): %d", iRet);
        return false;
    }
#endif  // POSIX threads
    m_bRunThread = true;
    m_bSuspended = false;
    return true;
}

bool CQCNThread::Stop()  
{
   // all this does is set the m_bRunThread flag to false
   // eventually perhaps have it kill the m_threadHandle etc
   m_bRunThread = false;
   return false;
}

// similar to Stop() - these just set flags and don't do any "physical" thread suspension/resumation
bool CQCNThread::Suspend()
{
   if (!m_bRunThread || !IsRunning()) {
      //fprintf(stderr, "CQCNThread::Suspend(): thread not running\n");
      return false;
   }
   if (m_bSuspended) {
      //fprintf(stderr, "CQCNThread::Suspend(): thread already suspended\n");
      return false;
   }
   m_bSuspended = true;
   return true;
}

bool CQCNThread::Resume()
{
   if (!m_bRunThread || !IsRunning()) {
      //fprintf(stderr, "CQCNThread::Resume(): thread not running\n");
      return false;
   }
   if (!m_bSuspended) {
      //fprintf(stderr, "CQCNThread::Resume(): thread not suspended\n");
      return false;
   }
   m_bSuspended = false;
   return true;
}


