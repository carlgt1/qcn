#ifndef _COASTLINE_H_
#define _COASTLINE_H_
#include "define.h"
#include "cserialize.h"

struct Coastline 
{
    float x, y, z;
};

extern Coastline coastline[NUM_COASTLINE];

/*
class GraphicDataCoastline  :public CSerialize
{  public:
     static Coastline coastline[NUM_COASTLINE];
};
*/

#endif
