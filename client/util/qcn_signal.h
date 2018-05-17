#ifndef _QCN_SIGNAL_H_
#define _QCN_SIGNAL_H_

// basic signal handling a la boinc/lib/diagnostics.C

#include "define.h"
#include <stdio.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <errno.h>
#endif

#include "error_numbers.h"  // boinc codes

namespace qcn_signal 
{

extern int ExitCode;

int InstallHandlers(void(*handler)(int), bool bIgnoreSIGPIPE = true);
int InstallHandlerSIGPIPE(void(*handler)(int));   // handle pipe errors separately if desired

// Set a signal handler only if it is not currently ignored
void SetHandler(int sig, void(*handler)(int));

// check process ID's

// call this with the process ID of the QCN BOINC application (which it should have set in sm)
bool PIDRunning(const long lPID);

}

#endif //_QCN_SIGNAL_H_
