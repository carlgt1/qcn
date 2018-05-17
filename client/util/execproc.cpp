#include "main.h"

extern size_t strlcat(char *dst, const char *src, size_t size);

// note since we include main.h above we can check for changes to *g_piStop and exit gracefully

// Returns true if the small executable finished with exit status 0, or false
// if it finished with a non-zero exit status.
namespace execproc {

   int g_iTimeoutSeconds = 60;  // number of seconds for Timeout
   int volatile *g_piStop = NULL;  // user a pointer to our stop flag, set when execproc invoked

#ifdef QCNDEMO // need a dummy g_iStop for demo program
   int volatile g_iStop = FALSE;
#endif

#ifdef _WIN32
    HANDLE g_hChildStd_IN_Rd = INVALID_HANDLE_VALUE;
    HANDLE g_hChildStd_IN_Wr = INVALID_HANDLE_VALUE;
    HANDLE g_hChildStd_OUT_Rd = INVALID_HANDLE_VALUE;
    HANDLE g_hChildStd_OUT_Wr = INVALID_HANDLE_VALUE;

    HANDLE g_hInputFile = INVALID_HANDLE_VALUE;

// this create the process and monitors (waits) for exit code, or exits early if *g_piStop request received
bool CreateChildProcess(LPSTR strCmd)
{
   PROCESS_INFORMATION piProcInfo; 
   STARTUPINFO siStartInfo;
   BOOL bSuccess = FALSE; 
 
// Set up members of the PROCESS_INFORMATION structure. 
 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
// Set up members of the STARTUPINFO structure. 
// This structure specifies the STDIN and STDOUT handles for redirection.
 
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
// Create the child process - last chance for an easy quit request, otherwise we have to kill the process and cleanup handles
   if (*g_piStop) {
       return false;
   }
 
   bSuccess = CreateProcess(NULL, 
      strCmd,     // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,  // creation flags  - note we're making it IDLE_PRIORITY_CLASS since the default is to make a process at the same priority class
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
   
   // If an error occurs, exit the application. 
   if ( ! bSuccess )  {
       fprintf(stderr, "execproc: Error in creating Windows process for %s\n", strCmd);
       return false;
   }

   // wait for exit
   // CMC - this is a good place to put the g_iStop check and cleanup & exit from here
   BOOL bExit;
   DWORD dwExitCode = STILL_ACTIVE;
   int iMinute = 0;
   while ((bExit = GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode)) 
      && dwExitCode == STILL_ACTIVE 
      && iMinute++ < (g_iTimeoutSeconds * 1000) ) {
         if (*g_piStop) break;
         usleep(1000);
   }

   if (*g_piStop || dwExitCode == STILL_ACTIVE) {  // either stop request or otherwise proc still running
       // have to kill the process
      fprintf(stderr, "execproc: Stop request received, exiting process...\n");
      TerminateProcess(piProcInfo.hProcess, 1); 
   }

   // Close handles to the child process and its primary thread.
   // Some applications might keep these handles to monitor the status
   // of the child process, for example. 

   CloseHandle(piProcInfo.hProcess);
   CloseHandle(piProcInfo.hThread);

   return (bool) !*g_piStop;
}

void CleanupHandles()
{
   if (g_hChildStd_IN_Rd != INVALID_HANDLE_VALUE) CloseHandle(g_hChildStd_IN_Rd);
   if (g_hChildStd_IN_Wr != INVALID_HANDLE_VALUE) CloseHandle(g_hChildStd_IN_Wr);
   if (g_hChildStd_OUT_Rd != INVALID_HANDLE_VALUE) CloseHandle(g_hChildStd_OUT_Rd);
   if (g_hChildStd_OUT_Wr != INVALID_HANDLE_VALUE) CloseHandle(g_hChildStd_OUT_Wr);
   if (g_hInputFile != INVALID_HANDLE_VALUE) CloseHandle(g_hInputFile);
}

bool ReadFromPipe(char* chBuf, int BUFSIZE)
{
   DWORD dwRead, dwWritten;
   //CHAR chBuf[BUFSIZE];
   BOOL bSuccess = FALSE;
   HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   if (hParentStdOut == INVALID_HANDLE_VALUE) {
       fprintf(stderr, "execproc: Can't get stdout handle to read from process\n");
       return false;  // can't get stdout handle!
   }
// Close the write end of the pipe before reading from the 
// read end of the pipe, to control child process execution.
// The pipe is assumed to have enough buffer space to hold the
// data the child process has already written to it.
 
   if (g_hChildStd_OUT_Wr == INVALID_HANDLE_VALUE || !CloseHandle(g_hChildStd_OUT_Wr)) {
       fprintf(stderr, "execproc: Error in closing stdout pipe for Windows proc\n");
       return false;
   }
   g_hChildStd_OUT_Wr = INVALID_HANDLE_VALUE;

   for (;;)
   {
      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
      if( ! bSuccess || dwRead == 0 ) break; 
      bSuccess = WriteFile(hParentStdOut, chBuf, 
                           dwRead, &dwWritten, NULL);
      if (! bSuccess || *g_piStop) break;
   }
   if (hParentStdOut != INVALID_HANDLE_VALUE) CloseHandle(hParentStdOut);
   return (bool) !*g_piStop;
}

#else // the my_popen() function for Mac & Linux -- allows for killing the proc & timeout

// need something with more control than popen() so can quit early etc
// found on the Internet and modded by CMC
static FILE* my_popen
(
    const char*  strCmd,
    const char*  strType,
    pid_t*       ppid,
    char**       args
)
{
  int   p[2];
  FILE* fp;

  // it has to be r or w, we could probably support r+ (bidirectional read/write) too
  if (strType[0] != 'r' && strType[0] != 'w') //  && !strcmp(strType, "r+")) 
    return NULL;

  if (pipe(p) < 0)
    return NULL;

  if ((*ppid = fork()) > 0) { // then we are the parent 
    if (strType[0] == 'r') {
      close(p[1]);
      fp = fdopen(p[0], strType);
    } else {
      close(p[0]);
      fp = fdopen(p[1], strType);
    }
    return fp;
  }
  else if (*ppid == 0) {  // we're the child 

    // make our thread id the process group leader 
    setpgid(0, 0);

    if (strType[0] == 'r') {
      fflush(stdout);
      fflush(stderr);
      close(1);
      if (dup(p[1]) < 0)
		  fprintf(stderr, "my_pfopen: dup of write side of pipe failed\n");
      close(2);
      if (dup(p[1]) < 0)
        fprintf(stderr, "my_pfopen: dup of read side of pipe failed\n");
    }  
    else { 
      close(0);
      if (dup(p[0]) < 0)
        perror("dup of read side of pipe failed");
    }

    close(p[0]); // close since we dup()'ed what we needed 
    close(p[1]);

    //execve(strCmd, args, environ);
    execve(args[0], args, environ);

    // should never get here
    fprintf(stderr, "execproc: my_popen(): execve(%s) failed!\n", strCmd);
  }
  else {         // we're having major problems...
    close(p[0]);
    close(p[1]);
    fprintf(stderr, "execproc: my_popen(): fork() failure!\n");
  }

  return NULL;
}

#endif

bool execute_program(const char* strExec, const char* strArgs, const char* strDir, 
    char* strReply, int iLenReply, 
    bool bNice, bool bQuote, 
    int iTimeoutSeconds
)
{
    g_iTimeoutSeconds = iTimeoutSeconds;  // set the timeout value, defaults to 60 seconds
    // set our g_iStop flag (really a pointer to either one declared here or the global qcn_main::g_iStop
#ifdef QCNDEMO // need a dummy g_iStop for demo program
   g_piStop = &g_iStop;
#else
   g_piStop = &qcn_main::g_iStop;
#endif

    memset(strReply, 0x00, sizeof(char) * iLenReply);
   // try popen, opens a proc, returns a FILE* to retrieve stdio
   std::string sstrCmd("");

// use nice for Mac & Linux unless not wanted
// CMC note - can't rely on nice being present?
/*
#ifndef _WIN32
  if (bNice && boinc_file_exists("/usr/bin/nice")) 
     sstrCmd = "/usr/bin/nice -n19 ";
  else 
    if (bNice && boinc_file_exists("/bin/nice")) 
       sstrCmd = "/bin/nice -n19 ";
#endif
*/

   // first check if we got a stop request
   if (*g_piStop) return false;

   // make a copy of our args since parse_command_line modified the string
   char*  strARG = new char[2 * _MAX_PATH];
   char** argv = new char*[100];
   memset(strARG, 0x00, 2 * _MAX_PATH * sizeof(char));
   memset(argv, 0x00, 100 * sizeof(char*));
   int argc = 0;

   if (bQuote)
      sstrCmd += '\"';  // surround windows command with quotes for the full path

   if (strDir)  {  // prepend the directory & path separator
      sstrCmd += strDir;
      sstrCmd += qcn_util::cPathSeparator();
   }

   sstrCmd += strExec;
   if (bQuote)
      sstrCmd += '\"';  // surround windows command with quotes for the full path

#ifdef _WIN32  // just concat args to the strCmd
   strlcpy(strARG, strArgs, 2 * _MAX_PATH); // that should be plenty
   sstrCmd += " ";
   sstrCmd += strARG;
#else
   // we need to parse the args into an "argv" style string array!
   sprintf(strARG, "%s %s", sstrCmd.c_str(), strArgs);
   argc = parse_command_line(strARG, argv);

/*
    fprintf(stdout, "\n\nfull cmd-line:\n%s\n  ptr strARG %x  argv %x\n\n", strARG, &strARG, argv);
for (int i = 0 ; i < argc ; i++) {
    if (argv[i]) fprintf(stdout, "    argv[%d] = %s\t%x\n", i, argv[i], &argv[i]);
 }
*/

#endif

#ifdef _WIN32
   // note in Windows it's best to do it via pipes   

   SECURITY_ATTRIBUTES saAttr; 

   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
   saAttr.bInheritHandle = TRUE; 
   saAttr.lpSecurityDescriptor = NULL; 

// Create a pipe for the child process's STDOUT. 
   if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) {
       fprintf(stderr, "execproc: Error in creating pipe 1 for Windows proc execution\n");
       CleanupHandles();
       if (strARG) delete [] strARG;
       if (argv) delete [] argv;
       return false;
   }

   // check if we got a stop request
   if (*g_piStop) {
      CleanupHandles();
       if (strARG) delete [] strARG;
       if (argv) delete [] argv;
      return false;
   }

// Ensure the read handle to the pipe for STDOUT is not inherited.
   if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )  {
       fprintf(stderr, "execproc: Error in SetHandleInformation\n");
       CleanupHandles();
       if (strARG) delete [] strARG;
       if (argv) delete [] argv;
       return false;
   }

   // check if we got a stop request
   if (*g_piStop) {
      CleanupHandles();
       if (strARG) delete [] strARG;
       if (argv) delete [] argv;
      return false;
   }

/*
// Create a pipe for the child process's STDIN.  
   if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) 
      ErrorExit(TEXT("Stdin CreatePipe")); 
*/
    if (!CreateChildProcess((LPSTR) sstrCmd.c_str())) {
       // see if it was a g_iStop
       if (!*g_piStop) {
          fprintf(stderr, "execproc: Could not create child process\n");
       }
       CleanupHandles();
       if (strARG) delete [] strARG;
       if (argv) delete [] argv;
       return false;
    }

    bool bRet = ReadFromPipe(strReply, iLenReply);
       if (strARG) delete [] strARG;
       if (argv) delete [] argv;
    CleanupHandles();
    return bRet;

#else // Mac & Linux
   // begin of the execproc for Mac & Linux
   pid_t pid = 0;
   FILE* fPipe;
   int iRetVal = 0, iNumRead = 0, iNumTotal = 0, iPIDStatus = 0;
   const int ciBufLen = iLenReply;
   char* strBuf = new char[ciBufLen];

   try {
      // try passing the entire cmd line as strExec arg
      if( (fPipe = my_popen(sstrCmd.c_str(), "r", &pid, argv )) == NULL )  {
         waitpid(pid, &iPIDStatus, WNOHANG); // zombie prevention
         return false;
      }

      double dEndTime = dtime() + (float) g_iTimeoutSeconds;  // set the timeout value

      // read pipe output into our buffer
      while (!feof(fPipe) && !ferror(fPipe)) {
           if (*g_piStop) {
               //kill(-pid, SIGINT); // note the - before pid - kills all in the process group created, also see killpg
               kill(-pid, SIGKILL); // note the - before pid - kills all in the process group created, also see killpg
               iRetVal = -pid;
               break;
           }
           if (dtime() > dEndTime) { // timeout
               //kill(-pid, SIGINT); // note the - before pid - kills all in the process group created, also see killpg
               kill(-pid, SIGKILL); // note the - before pid - kills all in the process group created, also see killpg
               iRetVal = 2;
               break;
           }

           memset(strBuf, 0x00, sizeof(char) * ciBufLen);
           if ( (iNumRead = (int)(sizeof(char) * fread(strBuf, sizeof(char), ciBufLen, fPipe))) ) {
                   iNumTotal += iNumRead;
                   if (iNumTotal < (iLenReply-1)) { // we have enough room to tack it on
                           strlcat(strReply, strBuf, iNumRead);
                   }
           }
      }

      // close pipe
      iRetVal = fclose(fPipe);

      waitpid(pid, &iPIDStatus, WNOHANG); // zombie prevention
  }
  catch(...) {
    waitpid(pid, &iPIDStatus, WNOHANG); // zombie prevention
    iRetVal = -999;
  }

  if (strBuf) delete [] strBuf;
  if (strARG) delete [] strARG;
  if (argv) delete [] argv;

  if (iRetVal) {
      fprintf(stderr, "Failed to execute %s with error # %d\n", sstrCmd.c_str(), iRetVal);
      fflush(stderr);
  }

  return (bool) (iRetVal == 0);

#endif

}

}  // namespace 

