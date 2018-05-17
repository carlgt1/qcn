#ifndef __COUNTRY_H__
#define __COUNTRY_H__

#include "define.h"
#include "cserialize.h"

// Geographic data labels

#ifdef _WIN32
#pragma warning( disable : 4305 )  // Disable warning messages for const double to float conversion
#endif


struct Country {
    char  *name;
    float lon, lat;
    char  *capital;
    char  *area;
};

extern Country country[NUM_COUNTRY];

/*
class GraphicDataCountry : public CSerialize
{
   public:
     GraphicDataCountry() {};
     ~GraphicDataCountry() {};
     static Country country[NUM_COUNTRY];
};
*/

#endif
