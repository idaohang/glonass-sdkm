#ifndef EPHEMERIS_H
#define EPHEMERIS_H

#include <vector>
#include <ctime>
#include <boost/date_time.hpp>
#include <ext/hash_map>

struct SatelitePosition
{
    double x;
    double y;
    double z;
    double clock;
    int satId;
    bool isGlo;
};


class EphemerisCollection
{
private:
    std::vector< std::vector<SatelitePosition> > satelitePositions;
    std::vector<boost::posix_time::ptime> epochs;

    __gnu_cxx::hash_map<int, std::ofstream *> ephReport;


public:
    EphemerisCollection();

    void AddSatelite(int year, int month, int day,
                     int hour, int minute, double second,
                     int satId, bool isGlo,
                     double x, double y, double z, double clock);
    bool GetSatelitePosition(boost::posix_time::ptime time,
                             int satId, bool isGlo,
                             double &x, double &y, double &z, double &clock);
};

#endif // EPHEMERIS_H
