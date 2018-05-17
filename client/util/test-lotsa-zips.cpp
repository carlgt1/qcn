// test-lotsa-zips.cpp -- just a dumb boinc_zip test program
//
// compile line:  don't forget to use the modded boinc_zip!
// g++ -I$HOME/qcn/boinc/zip test-lotsa-zips.cpp -o test -lz -L$HOME/qcn/boinc/zip -lboinc_zip -L$HOME/qcn/boinc/lib -lboinc
// g++ -I$HOME/qcn/boinc/zip -I$HOME/qcn/ziparchive/ZipArchive test-lotsa-zips.cpp -o test -lz -L$HOME/qcn/ziparchive/ZipArchive -lziparch -L$HOME/qcn/boinc/lib -lboinc

#include <stdio.h>
//#include "boinc_zip.h"
#include "ZipArchive.h"
using std::string;
using std::vector;

#include <zlib.h>  // CMC -- test that we "co-exist" with the "stock" zlib library
int do_gzip(const char* strGZ, const char* strInput)
{
        // take an input file (strInput) and turn it into a compressed file (strGZ)
        // get rid of the input file after 

        FILE* fIn = fopen(strInput, "rb");
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
                lRead = fread(buf, 1, 1024, fIn);
                lWrite = gzwrite(fOut, buf, lRead);
                if (lRead != lWrite) break;
        }
        gzclose(fOut);
        fclose(fIn);

        if (lRead != lWrite) return 1;  //error -- read bytes != written bytes

        // if we made it here, it compressed OK, can erase strInput and leave
        //unlink(strInput);
        return 0;
}

int main(int argc, char* argv[])
{
        //ZipFileList zfl;
        vector<string> zfl;
        char strZip[255];

        zfl.clear();
        zfl.push_back("../../configure.ac");
        zfl.push_back("../../build");
        zfl.push_back("../../depcomp");
        zfl.push_back("../../getboinc");

        //do_gzip("test.cpp.bak.gz", "test.cpp.bak");

        int jj = 0, iRetVal; 
        while (jj++<100) {
          sprintf(strZip, "file%06ld.zip", jj);

          do_gzip("sac.cpp.gz", "sac.cpp");

          //iRetVal = boinc_zip(ZIP_IT, strZip, "../../configure.ac");
          //FILE* fi = fopen(strZip, "r");
          //iRetVal = boinc_zip(ZIP_IT, std::string(strZip), &zfl);
          //int iClose = fi ? fclose(fi) : 1;
          //fprintf(stdout, "Zipping %s -- retval=%d  fi=%ld  iClose=%d\n", strZip, iRetVal, fi, iClose);
          //int iRetVal2 = boinc_zip(UNZIP_IT, strZip, "test2/");

       CZipArchive ziparch;
       bool bRetVal;
       try {
       bRetVal = ziparch.Open(strZip, CZipArchive::zipCreate);
       if (!bRetVal) {
          fprintf(stdout, "Could not open zip archive %s!\n", strZip);
          return 1;
       }
       for (int i = 0; bRetVal && i < zfl.size(); i++) {
          bRetVal = ziparch.AddNewFile(zfl[i].c_str(), -1, false);
       }
       ziparch.Close(CZipArchive::afWriteDir);
       }
       catch(...) {
       }

          fprintf(stdout, "Zip Res %s -- retval=%d  unzip-retval=%d\n", strZip, bRetVal ? 1 : 0, 0);
          fflush(stdout);
        }
    return 0;
}

