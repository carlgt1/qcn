#include "define.h"
#include "cserialize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include "filesys.h"

using std::ios;

#ifdef _ZLIB
  #include "gzstream.h"
#else
  #include <fstream>  
  using std::ofstream;
  using std::ifstream;
#endif

CSerialize::CSerialize()
{
}

CSerialize::~CSerialize()
{
}

bool CSerialize::serialize(void* ptr, long len, const char* lpcszFile)
{
        bool retval = false;
        if (!lpcszFile) return false;  // no file name entered, or file doesn't exist!

        static bool bInHere = false;  // mutex
        if (bInHere) return false;
        bInHere = true;
        try {
#ifdef _ZLIB
                ogzstream arc(lpcszFile, ios::binary|ios::trunc|ios::out);
#else
                ofstream arc(lpcszFile, ios::binary|ios::trunc|ios::out);
#endif
                arc.write(reinterpret_cast<const char *>(ptr), len);
                arc.close();
                retval = true;
        }
        catch(...)
        {
                fprintf(stdout, "CSerialize::serialize: Cannot serialize file %s\n", lpcszFile);
                retval = false;
        }
        bInHere = false;
        return retval;
}


bool CSerialize::deserialize(void* ptr, long len, const char* lpcszFile)
{
        bool retval = false; 
        if (!lpcszFile) return false;
        if (!boinc_file_exists(lpcszFile)) { // no file name entered, or file doesn't exist!
             fprintf(stdout, "CSerialize::deserialize: Cannot find file %s\n", lpcszFile);
             return false;
        }

        static bool bInHere = false;  // mutex
        if (bInHere) return false;
        bInHere = true;
        try {
#ifdef _ZLIB
            igzstream arc;
            arc.open(lpcszFile, std::ios::binary|std::ios::in);
#else
            ifstream arc(lpcszFile, ios::binary|ios::in);
#endif
            arc.read(reinterpret_cast<char *>(ptr), len);
            arc.close();
            retval = true;
        }
        catch(...)
        {
           fprintf(stdout, "CSerialize::deserialize: Cannot deserialize file %s\n", lpcszFile);
            retval = false;
        }
        bInHere = false;
        return retval;
}

