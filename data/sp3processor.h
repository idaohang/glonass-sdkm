#ifndef SP3PROCESSOR_H
#define SP3PROCESSOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fstream>

#include "ephemeris.h"
#include "observation.h"
#include "sqlite_observation_provider.h"

//! Заголовок, описывающий эпоху в SP3
struct EpochHeader
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

//! Запись в SP3, описывающая координаты спутника.
struct EpochRecord
{
    bool is_pos; // position = 1; velocity = 0
    double x;
    double y;
    double z;
    double clock;

    int sv;
    bool isGlo;
};

//! Класс, осуществляющий чтение SP3 файлов.
class SP3Reader
{
private:    
    SqliteEphemerisProvider * ephemerisProvider;
    EpochHeader epoch;
    char * directory;

    void ProcessEpochHeader(std::string);
    void ProcessSatelitePosition(std::string);
    void ProcessFile(const char *inputDirectory);

public:
    SP3Reader(char * directory, SqliteEphemerisProvider * obs);
    void Run();
};

//! Класс, осуществляющий запись SP3-файлов
class SP3Writer
{
    void WriteHeader(EpochHeader header);
    void WriteRecord(EpochRecord record);

    std::ofstream outputFile;

public:
    SP3Writer(char * fileName);
    ~SP3Writer();
    void AddEpoch(EpochHeader epochHeader, std::vector<EpochRecord> records);
};

#endif // SP3PROCESSOR_H
