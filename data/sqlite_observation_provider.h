#ifndef SQLITE_OBSERVATION_PROVIDER_H
#define SQLITE_OBSERVATION_PROVIDER_H

#include "observation.h"
#include "observation_provider.h"
#include "../sqlite/sqlite3.h"
#include "ephemeris_wrapper.h"

//! Абстрактный класс, представляющий собой провайдер эфемерид.
class EphemerisProvider
{
public:
    virtual EphemerisWrapper FindEphemeris(int year, int month, int day, long msec, std::vector<int> satelites) = 0;
};

//! Провайдер эфемерид, использующий встроенную БД SQLite
class SqliteEphemerisProvider: public EphemerisProvider
{
private:
    sqlite3 * db;
    ProcessingType processing;
    void PrepareDataBase( const char* fileName );

public:
    // Have to break encapsulation for sake of SqlLiteCallbacks
    std::vector<long> mseconds;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<double> clock;

    int year, month, day;
    long msec;
    // --- Everithing else is fine oop

    void BeginTransaction();
    void EndTransaction();

    SqliteEphemerisProvider(const char * fileName, bool create);
    void AddEphemeris(int year, int month, int day, long msec, int satId, double x, double y, double z, double clock);
    EphemerisWrapper FindEphemeris(int year, int month, int day, long msec, std::vector<int> satelites);

    // encapsulation breakdown
    static int SelectEphemerisCallback(void *observationInstance, int argc, char **argv, char **azColName);
};

#endif // SQLITE_OBSERVATION_PROVIDER_H
