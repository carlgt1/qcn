#ifndef _QCN_GZIP_H_
#define _QCN_GZIP_H_

#include <zlib.h>

int do_gzip(const char* strGZ, const char* strInput);
int do_gunzip(const char* strGZ, const char* strInput, bool bKeep = false);

#endif

