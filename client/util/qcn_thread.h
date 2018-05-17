#ifndef _QCN_THREAD_H
#define _QCN_THREAD_H

#ifdef _WIN32
//      typedef (UINT WINAPI *QCNThreadFunctionPointer)(LPVOID);
      typedef DWORD (WINAPI *QCNThreadFunctionPointer)(LPVOID);
#else
      typedef void* (*QCNThreadFunctionPointer)(void*);
#endif

class CQCNThread
{
   private:
#ifdef _WIN32
      HANDLE    m_handleThread;
#else
      pthread_t m_handleThread;
#endif
      long m_lStackSize;
      QCNThreadFunctionPointer m_funcptr;
      bool m_bRunThread;  // thread state running status so we know internally if Start() really did create the thread etc
      bool m_bRunning;    // internal to the thread run state, which we will access via qcn_main::g_threadSensor->IsRunning() & set within the thread function etc
      bool m_bSuspended;  // flag if thread is suspended
      unsigned long m_ThreadId;

   public:
      CQCNThread(QCNThreadFunctionPointer ptrfunc = NULL, long lStackSize = 1048576L);
      ~CQCNThread();
      
      void SetFunctionPointer(QCNThreadFunctionPointer ptrfunc);
      bool Start(bool bHighPriority = false);
      bool Stop();
      bool Suspend();
      bool Resume();

      bool IsRunning() { return m_bRunning; };
      bool IsSuspended() { return m_bSuspended; };
      void SetRunning(const bool& bRun = true) { m_bRunning = bRun; };  // override run state set in thread
};

#endif
