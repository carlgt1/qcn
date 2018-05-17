#include "md5.h"

int main(int argc, char** argv)
{
   // test checksum a file

   // void MD5_file(const char* filename, char* checksum, int lCSLen);
   char strCS[33];
   if (argc!=2) {
      fprintf(stdout, "Usage: md5test filename\n");
      return 1;
   }

   md5::MD5_file(argv[1], strCS, 33);
   fprintf(stdout, "md5checksum of %s is [%s]\n", argv[1], strCS);

   return 0;
}

