// CMC -- OpenGL Graphics for QCN project
// (c) 2007 Stanford University

#ifdef _WIN32
#include "boinc_win.h"
#else
#include <math.h>
#endif

#include "parse.h"
#include "gutil.h"
#include "boinc_gl.h"
#include "boinc_api.h"

#include "country.h"
//#include "coastline.h"
//#include "nation_boundary.h" 
//#include "plate_boundary.h" 

using std::string;
using std::vector;

// this will read in all the classes and serialize them out

int main(int argc, char** argv)
{
#ifndef TEST
   // OK, instantiate
   //GraphicDataCoastline gdc;
   GraphicDataCountry gdc2;
   //GraphicDataNation gdn;
   //GraphicDataPlate gdp;
   bool bRet[4];
   long lSize[4];

   fprintf(stdout, "%s\n", gdc2.country[123].name);

   //lSize[0] = sizeof(GraphicDataCoastline);
   lSize[1] = sizeof(gdc2.country);
   //lSize[2] = sizeof(GraphicDataNation);
   //lSize[3] = sizeof(GraphicDataPlate);

   // bool serialize(void* ptr, long len, const char* lpcszFile);
   // sm->deserialize(sm, sizeof(CQCNShMem), (const char*) sm->strCurFile);
   //bRet[0] = gdc.serialize(&gdc, lSize[0], "coast.gdz");
   bRet[1] = gdc2.serialize(&gdc2, lSize[1], "cntry.gdz");
   //bRet[2] = gdn.serialize(&gdn, lSize[2], "nation.gdz");
   //bRet[3] = gdp.serialize(&gdp, lSize[3], "plate.gdz");

   fprintf(stdout, "size          = %ld %ld %ld %ld\n", lSize[0], lSize[1], lSize[2], lSize[3]);
   fprintf(stdout, "return values = %d %d %d %d\n", bRet[0], bRet[1], bRet[2], bRet[3]);
#else
    // try to deserialize country?
   strcpy(GraphicDataCountry::country[0].name,"");
   GraphicDataCountry* gg = new GraphicDataCountry;
   if (gg->deserialize(gg, sizeof(GraphicDataCountry), "cntry.gdz")) {
      fprintf(stdout, "%s\n", gg->country[123].name);
      fprintf(stdout, "%s\n", gg->country[124].name);
   }
   delete gg;
#endif
   return 0;
}


