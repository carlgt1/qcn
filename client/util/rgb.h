#ifndef __RGB_H__
#define __RGB_H__

#define NUM_RGB	120

#pragma warning( disable : 4305 )  // Disable warning messages for const double to float conversion

struct RGB {
    float r, g, b;
};

struct RGBA {
    float r, g, b, a;
};

#endif  // __RGB_H__
