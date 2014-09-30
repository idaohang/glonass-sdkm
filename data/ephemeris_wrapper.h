#ifndef EPHEMERISWRAPPER_H
#define EPHEMERISWRAPPER_H

#include <vector>
#include "observation.h"

// Обертка(wrpapper) на коллекцией эфемерид, позволяющая получать нужные спутники по их sv
class EphemerisWrapper
{
public:
    std::vector<SatPosition> ephemeris;

    inline SatPosition * GetSatelite(int sv)
    {
        for(unsigned int i = 0; i < ephemeris.size(); i++)
            if( ephemeris[i].sv == sv)
                return &ephemeris[i];

        return NULL;
    }
};

#endif // EPHEMERISWRAPPER_H
