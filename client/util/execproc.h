#ifndef _EXECPROC_H_
#define _EXECPROC_H_

namespace execproc {

bool execute_program(const char* strExec, const char* strArgs, const char* strDir, 
    char* strReply, int iLenReply, 
    bool bNice = true, bool bQuote = true, 
    int iTimeoutSeconds = 60
);

}

#endif
