// basic signal handling a la boinc/lib/diagnostics.C

#include "qcn_signal.h"

#ifdef ANDROID
  #include <unistd.h>
  #include <sys/syscall.h>
  // need our own implementation of getsid for Android NDK
  pid_t getsid(pid_t pid) {
    return syscall(__NR_getsid, pid);
  }
#endif // ANDROID

namespace qcn_signal 
{
int InstallHandlers(void(*handler)(int), bool bIgnoreSIGPIPE)
{
#ifdef SIGHUP
    SetHandler(SIGHUP, handler);
#endif
//#ifdef SIGINFO
//    SetHandler(SIGINFO, handler);
//#endif
#ifdef SIGINT
    SetHandler(SIGINT, handler);
#endif
#ifdef SIGQUIT
    SetHandler(SIGQUIT, handler);
#endif
#ifdef SIGILL
    SetHandler(SIGILL, handler);
#endif
#ifdef SIGTRAP
    SetHandler(SIGTRAP, handler);
#endif
#ifdef SIGEMT
    SetHandler(SIGEMT, handler);
#endif
#ifdef SIGFPE
    SetHandler(SIGFPE, handler);
#endif
#ifdef SIGBUS
    SetHandler(SIGBUS, handler);
#endif
#ifdef SIGABRT
    SetHandler(SIGABRT, handler);
#endif
#ifdef SIGTERM
    SetHandler(SIGTERM, handler);
#endif
#ifdef SIGSEGV
    SetHandler(SIGSEGV, handler);
#endif
#ifdef SIGSYS
    SetHandler(SIGSYS, handler);
#endif
/*  CMC - not a good idea since ntpdate uses SIGALRM and BOINC can use SIGUSR1 & 2
#ifdef SIGALRM
    SetHandler(SIGALRM, handler);
#endif
#ifdef SIGUSR1
    SetHandler(SIGUSR1, handler);
#endif
#ifdef SIGUSR2
    SetHandler(SIGUSR2, handler);
#endif
*/
#ifdef SIGPIPE
    if (bIgnoreSIGPIPE) {
       signal(SIGPIPE, SIG_IGN);
    }
#endif
    return 0;
}

int InstallHandlerSIGPIPE(void(*handler)(int))
{ // handle this separately as we use named pipes in QCN and don't want to terminate on a pipe error
#ifdef SIGPIPE
    SetHandler(SIGPIPE, handler);
#endif
    return 0;
}

// Set a signal handler only if it is not currently ignored
// this is ripped off from boinc/lib/boinc_diagnostics.C - boinc_set_signal_handler
void SetHandler(int sig, void(*handler)(int)) {
#if defined(HAVE_SIGACTION) && !defined(_WIN32)
    struct sigaction temp;
    sigaction(sig, NULL, &temp);
    if (temp.sa_handler != SIG_IGN) {
        temp.sa_handler = handler;
    //        sigemptyset(&temp.sa_mask);
        sigaction(sig, &temp, NULL);
    }
#else
    void (*temp)(int);
    temp = signal(sig, handler);
    if (temp == SIG_IGN) {
        signal(sig, SIG_IGN);
    }
#endif /* HAVE_SIGACTION */
}

// check process ID's

bool PIDRunning(const long lPID)
{
    bool bRetVal = false;
    int iRetVal  = 0;

#if defined(_WIN32) || defined(_WIN64)
    static unsigned long ulExitCode = 0L;
    static HANDLE hProcess = 0x00;
    
    // open this process id
    if ((hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, lPID)))
    {
         if (::GetExitCodeProcess(hProcess, &ulExitCode)) {
             if (ulExitCode == STILL_ACTIVE) {
                bRetVal = true;
             }
         }
         ::CloseHandle(hProcess);
    }
    else {
       fprintf(stderr, "No Process Handle\n");
    }
#else
    // Linux & Mac
#ifdef MONITOR
   // CMC -- use to get rid of zombies (child procs of the monitor)
   int iStatus;
   iRetVal = waitpid(lPID, &iStatus, WNOHANG); 
#endif

/*
    // if processes are same user, we can use kill(pid, 0) - if returns 0 it's running
    iRetVal = kill(lPID, 0);
    bRetVal = (bool) (iRetVal == 0);
    if (iRetVal != lPID) {
       fprintf(stderr, "iRetVal = %d : PID %ld not running or errno %d\n", iRetVal, lPID, errno);
    }
*/
   // try getsid
   errno = 0;
   iRetVal = (int) ::getsid(lPID);
   bRetVal = (bool) (iRetVal != -1 && errno != ESRCH);

#endif

    return bRetVal;
}

}  // namespace qcn_signal

