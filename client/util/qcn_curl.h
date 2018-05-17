#ifndef _QCN_CURL_H_
#define _QCN_CURL_H_

#include "define.h"
#include "str_util.h"  // from boinc, for strlcat()
#include <curl/curl.h>

namespace qcn_curl {

extern const long BYTESIZE_CURL;   // max length to write a curl reply is our qcn_shmem.h (really define.h) MAX_PROJPREFS (132KB)
extern long g_curlBytes;

extern size_t qcnwx_curl_write_data(void *ptr, size_t size, size_t nmemb, void *stream);

// decl for curl function
extern bool execute_curl(const char* strURL, char* strReply, const int iLen);

}  // namespace qcn_curl

#endif  // QCN_CURL_H
