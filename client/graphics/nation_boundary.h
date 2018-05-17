#ifndef __NATION_BOUNDARY_H__
#define __NATION_BOUNDARY_H__

#include "define.h"
#include "cserialize.h"

#ifdef _WIN32
#pragma warning( disable : 4305 )  // Disable warning messages for const double to float conversion
#endif


struct NationBoundary {
    float lat, lon;
};

extern NationBoundary nation[NUM_NATION_BOUNDARY];

/*
class GraphicDataNation : public CSerialize
{  public:
     static NationBoundary nation[NUM_NATION_BOUNDARY];
};
*/

/*
extern NationBoundary bdy_africa[NUM_BDY_AFRICA]; 
extern NationBoundary bdy_asia[NUM_BDY_ASIA]; 
extern NationBoundary bdy_europe[NUM_BDY_EUROPE]; 
extern NationBoundary bdy_north_america[NUM_BDY_NORTH_AMERICA]; 
extern NationBoundary bdy_south_america[NUM_BDY_SOUTH_AMERICA]; 
*/

#endif  
