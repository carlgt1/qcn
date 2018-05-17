#include <stdio.h>
#include "filesys.h"
#include "qcn_gzip.h"

int do_gzip(const char* strGZ, const char* strInput)
{
        // take an input file (strInput) and turn it into a compressed file (strGZ)
        // get rid of the input file after
        FILE* fIn = boinc_fopen(strInput, "rb");
        if (!fIn)  return 1; //error
        gzFile fOut = gzopen(strGZ, "wb");
        if (!fOut) return 1; //error
        fseek(fIn, 0, SEEK_SET);  // go to the top of the files
        gzseek(fOut, 0, SEEK_SET);
        unsigned char buf[1024];
        long lRead = 0, lWrite = 0;
        while (!feof(fIn)) { // read 1KB at a time until end of file
                memset(buf, 0x00, 1024);
                lRead = 0;
                lRead = (long) fread(buf, 1, 1024, fIn);
                lWrite = (long) gzwrite(fOut, buf, lRead);
                if (lRead != lWrite) break;
        }
        gzclose(fOut);
        fclose(fIn);
        if (lRead != lWrite) return 1;  //error -- read bytes != written bytes
        // if we made it here, it compressed OK, can erase strInput and leave
        boinc_delete_file(strInput);
        return 0;
}

// CMC - commented out status calls, are they too paranoid?
//       if needed use sm->statusBOINC instead (for quit_request etc)

int do_gunzip(const char* strGZ, const char* strInput, bool bKeep)
{
        // take an input file (strInput) and turn it into a compressed file (strGZ)
        // get rid of the input file after
        //s.quit_request = 0;
        //checkBOINCStatus();
        FILE* fIn = boinc_fopen(strInput, "wb");
        if (!fIn)  return 1; //error
        gzFile fOut = gzopen(strGZ, "rb");
        if (!fOut) return 1; //error
        fseek(fIn, 0, SEEK_SET);  // go to the top of the files
        gzseek(fOut, 0, SEEK_SET);
        unsigned char buf[1024];
        long lRead = 0, lWrite = 0;
        while (!gzeof(fOut)) { // read 1KB at a time until end of file
                memset(buf, 0x00, 1024);
                lRead = 0;
                lRead = (long) gzread(fOut,buf,1024);
                lWrite = (long) fwrite(buf, 1, 1024, fIn);
                if (lRead != lWrite) break;
                //boinc_get_status(&s);
                //if (s.quit_request || s.abort_request || s.no_heartbeat) break;
        }
        gzclose(fOut);
        fclose(fIn);
        //checkBOINCStatus();
        if (lRead != lWrite) return 1;  //error -- read bytes != written bytes
        // if we made it here, it compressed OK, can erase strInput and leave
        if (!bKeep) boinc_delete_file(strGZ);
        return 0;
}

