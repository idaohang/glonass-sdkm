#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <boost/format.hpp>

#include "../algorithms/math_utilities.h"
#include "observation.h"
#include "sqlite_observation_provider.h"

using namespace std;

SqliteEphemerisProvider::SqliteEphemerisProvider(const char * fileName, bool create)
{
    if( create )
        this->PrepareDataBase(fileName);
    else
        sqlite3_open(fileName, &this->db);
}

void SqliteEphemerisProvider::BeginTransaction()
{
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL  );
}

void SqliteEphemerisProvider::EndTransaction()
{
    sqlite3_exec(db, "COMMIT", NULL, NULL, NULL  );
}

void SqliteEphemerisProvider::PrepareDataBase(const char * fileName)
{
    char * errMsg = 0;
    sqlite3_open_v2( fileName, &this->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    char * create_ephemeris     = "CREATE TABLE ephemeris   (year INTEGER, month INTEGER, day INTEGER, msec INTEGER, sateliteId Integer, x REAL, y REAL, z REAL, clock REAL, PRIMARY KEY(year, month, day, msec, sateliteId));";
    sqlite3_exec(db, create_ephemeris, NULL, NULL, &errMsg  );
}

void SqliteEphemerisProvider::AddEphemeris(int year, int month, int day, long msec, int satId, double x, double y, double z, double clock)
{
    char insert_ephemeris[500];
    char * errMsg = 0;

    // Slows the processing significantly
    /*
    char delete_prior_insert[500];
    sprintf(delete_prior_insert, "DELETE FROM ephemeris WHERE year=%i AND month=%i AND day=%i AND msec=%i AND sateliteId=%i;",
            year, month, day, msec, satId);
    sqlite3_exec(db, delete_prior_insert, NULL, NULL, &errMsg);
    */

    sprintf(insert_ephemeris,
            "INSERT INTO ephemeris(year, month, day, msec, sateliteId, x, y, z, clock) VALUES(%i, %i, %i, %i, %i, %f, %f, %f, %f);",
            year, month, day, msec, satId, x, y, z, clock);
    sqlite3_exec(db, insert_ephemeris, NULL, NULL, &errMsg);
}

int SqliteEphemerisProvider::SelectEphemerisCallback(void *observationInstance, int argc, char **argv, char **azColName)
{
    SqliteEphemerisProvider * obs = (SqliteEphemerisProvider *)observationInstance;

    obs->mseconds.push_back(atoi(argv[3]));
    obs->x.push_back(atof(argv[5]));
    obs->y.push_back(atof(argv[6]));
    obs->z.push_back(atof(argv[7]));
    obs->clock.push_back(atof(argv[8]));

    return 0;
}

EphemerisWrapper SqliteEphemerisProvider::FindEphemeris(int year, int month, int day, long msec, std::vector<int> satelites)
{
    EphemerisWrapper ephemeris;

    for( unsigned j = 0; j < satelites.size(); j++)
    {
        stringstream select_ephemeris;
        select_ephemeris << " SELECT DISTINCT * FROM ephemeris"
                << " WHERE year=" << year << " AND month=" << month << " AND day=" << day
                << " AND msec >= " << msec - 3600000
                << " AND msec <= " << msec + 3600000
                << " AND sateliteId=" << satelites[j]
                << " ORDER BY year, month, day, msec;";

        sqlite3_exec(db, select_ephemeris.str().c_str(), SelectEphemerisCallback, this, NULL);

        if(mseconds.size() == 0)
        {
            continue;
        }

        double * time = (double*)(malloc(sizeof(double)*mseconds.size()));
        for( unsigned i = 0; i < mseconds.size(); i++ )
        {
            time[i] = mseconds[i];
        }

        double * a_x = (double*)(malloc(sizeof(double)*mseconds.size()));
        for( unsigned i = 0; i < mseconds.size(); i++ )
        {
            a_x[i] = x[i];
        }

        double * a_y = (double*)(malloc(sizeof(double)*mseconds.size()));
        for( unsigned i = 0; i < mseconds.size(); i++ )
        {
            a_y[i] = y[i];
        }

        double * a_z = (double*)(malloc(sizeof(double)*mseconds.size()));
        for( unsigned i = 0; i < mseconds.size(); i++ )
        {
            a_z[i] = z[i];
        }

        double * a_c = (double*)(malloc(sizeof(double)*mseconds.size()));
        for( unsigned i = 0; i < mseconds.size(); i++ )
        {
            a_c[i] = clock[i];
        }

        double ix = lagrange_interpolation(time, a_x, mseconds.size(), msec);
        double iy = lagrange_interpolation(time, a_y, mseconds.size(), msec);
        double iz = lagrange_interpolation(time, a_z, mseconds.size(), msec);
        double ic = lagrange_interpolation(time, a_c, mseconds.size(), msec);

        free(a_x);
        free(a_y);
        free(a_z);
        free(a_c);

        this->mseconds.clear();
        this->x.clear();
        this->y.clear();
        this->z.clear();
        this->clock.clear();

        SatPosition satPos;
        satPos.clock = ic;
        satPos.sv = satelites[j];
        satPos.coord.x = ix;
        satPos.coord.y = iy;
        satPos.coord.z = iz;

        ephemeris.ephemeris.push_back(satPos);
    }

    return ephemeris;
}
