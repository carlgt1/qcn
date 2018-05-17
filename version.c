#include <stdlib.h>
#include <stdio.h>

#include "version.h"

/* dumb program to increment & save version # */

void print_version_file(float fVersion)
{
   FILE* fp = fopen("version.h", "w");
   if (fp) {
   fprintf(fp, "#ifndef QCN_VERSION_H\n");
   fprintf(fp, "#define QCN_VERSION_H\n");
   fprintf(fp, "#define QCN_RELEASE\n");
   fprintf(fp, "#define QCN_PACKAGE_NAME \"QCN\"\n");
   fprintf(fp, "#define QCN_VERSION_STRING \"%2.02f\"\n", fVersion);
   fprintf(fp, "#endif\n");
   fclose(fp);
   }
   else {
     printf("Cannot open file!\n");
   }
}

int main(int argc, char** argv)
{
   float fVal = atof(QCN_VERSION_STRING);
   if (argc==2 && !strcmp(argv[1], "1")) {
     fVal += .01f;
     print_version_file(fVal);
     printf("QCN Version Changed To %2.02f\n", fVal);
   }
   else {
     print_version_file(fVal);
     printf("%2.02f\n", fVal);
   }
}

