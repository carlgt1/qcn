#include "qcn_curl.h"

extern size_t strlcat(char *dst, const char *src, size_t size);
extern size_t strlcpy(char*, const char*, size_t);

namespace qcn_curl {

// we need a few globals such as curl write functions and our big global shared mem struct sm
const long BYTESIZE_CURL =  102400L;   // max length to write a curl reply
long g_curlBytes = 0L;    // cumulative bytes written by curl write function

// decl for curl write function
size_t qcnwx_curl_write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
   long lLeft = BYTESIZE_CURL - g_curlBytes - (long)(size * nmemb);
   if (size > 0 && g_curlBytes < BYTESIZE_CURL) { // we have some room left to write
      strlcat((char*) stream, (char*) ptr, BYTESIZE_CURL);
	  if (lLeft > 0) { //we have enough room left for simple strlcat
         g_curlBytes += (long)(size * nmemb);
	  }
	  else { // we ran out of room just set g_curlBytes to BYTESIZE_CURL
		  g_curlBytes = BYTESIZE_CURL;
	  }
   }
   return size * nmemb; // no matter what, return that we handled everything
}

// decl for curl function
bool execute_curl(const char* strURL, char* strReply, const int iLen)
{
   // easycurl should be fine, just send a request to maxmind (strURL has the key & ip etc),
   // and output to strReply up to iLen size
   CURLcode cc;
   CURL* curlHandle = curl_easy_init();
   g_curlBytes = 0L;  // reset long num of curl bytes read
   if (!curlHandle) return false;  // problem with init
   cc = curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 0L);
   cc = curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1L);
   cc = curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 30L);  // 30 seconds should be plenty of time to get the quake list
   cc = curl_easy_setopt(curlHandle, CURLOPT_URL, strURL);
   cc = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, strReply);
   cc = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, qcnwx_curl_write_data);
   cc = curl_easy_perform(curlHandle);
   curl_easy_cleanup(curlHandle);
   return (bool) (cc == 0 && sizeof(strReply)>0);  // 0 is good CURLcode
}

}  // namespace

