#ifndef _PLATE_BOUNDARY_H
#define _PLATE_BOUNDARY_H
#include "define.h"
#include "cserialize.h"


struct PlateBoundary
{
    float lon, lat;
};

extern PlateBoundary plate[NUM_PLATE_BOUNDARY];

/*
class GraphicDataPlate: public CSerialize
{  public:
     static PlateBoundary plate[NUM_PLATE_BOUNDARY];
};
*/

#endif

