#include "sp3processor.h"
#include <string>
#include <dirent.h>

#include <string>
#include <sstream>

using namespace std;

SP3Reader::SP3Reader(char * directory, SqliteEphemerisProvider * obs)
{
    this->directory = directory;
    this->ephemerisProvider = obs;
}

void SP3Reader::Run()
{
    struct dirent *de=NULL;

    DIR  *d=opendir(directory);
      if(d == NULL)
      {
        cout << "Couldn't open directory" << endl;
        return;
      }

      // Loop while not NULL
      while(de = readdir(d))
      {
          if( strncmp(de->d_name, ".", 1) == 0 || strncmp(de->d_name, "..", 1) == 0 )
              continue;
          else
          {
            stringstream path;
            path << this->directory << de->d_name;
            cout << "File found: " << path.str() << endl;
            ProcessFile(path.str().c_str());

          }
      }
      closedir(d);
}


void SP3Reader::ProcessFile(const char *fileName)
{
    ifstream *sp3file = new ifstream(fileName);

    this->ephemerisProvider->BeginTransaction();

    int counter = 0;

    if(sp3file->is_open())
    {
        while( !sp3file->eof() )
        {
            string line;
            getline(*sp3file, line);

            if( counter >= 22 && line.substr(0, 3) != "EOF" && line != "" )
            {
                if( line[0] == '*')
                    ProcessEpochHeader(line);
                else
                    ProcessSatelitePosition(line);
            }

            counter++;
        }
    }
    sp3file->close();

    this->ephemerisProvider->EndTransaction();
}

void SP3Reader::ProcessSatelitePosition(string data)
{   
    const char * satType = data.substr(1, 1).c_str();
    bool isGlo = false;
    if( satType[0] == 'R' ) isGlo = true;    

    const char * sSatId = data.substr(2, 2).c_str();
    int satId = atoi(sSatId);

    const char * sPosX = data.substr(4, 14).c_str();
    double posX = atof(sPosX);

    const char * sPosY = data.substr(19, 14).c_str();
    double posY = atof(sPosY);

    const char * sPosZ = data.substr(33, 14).c_str();
    double posZ = atof(sPosZ);

    const char * sClock = data.substr(47, 14).c_str();
    double clock = atof(sClock);

    double msec = 3600000*this->epoch.hour + 60000*this->epoch.minute + this->epoch.second*1000;

    this->ephemerisProvider->AddEphemeris(this->epoch.year,
                                          this->epoch.month,
                                          this->epoch.day,
                                          msec,
                                          satId + (isGlo? 100 : 000),
                                          posX*1000,
                                          posY*1000,
                                          posZ*1000,
                                          clock);
}

void SP3Reader::ProcessEpochHeader(string data)
{
    const char * sYear = data.substr(3, 4).c_str();
    this->epoch.year = atoi(sYear);

    const char * sMonth = data.substr(8, 2).c_str();
    this->epoch.month = atoi(sMonth);

    const char * sDay  = data.substr(11, 2).c_str();
    this->epoch.day = atoi(sDay);

    const char * sHour  = data.substr(14, 2).c_str();
    this->epoch.hour = atoi(sHour);

    const char * sMinute  = data.substr(17, 2).c_str();
    this->epoch.minute = atoi(sMinute);

    const char * sSecond  = data.substr(20, 11).c_str();
    this->epoch.second = atof(sSecond);
}

SP3Writer::SP3Writer(char *fileName)
{
    outputFile.open(fileName, ios_base::out);
}

SP3Writer::~SP3Writer()
{
    outputFile.close();
}

void SP3Writer::WriteHeader(EpochHeader header)
{
    outputFile << string("*  ") << header.year << " ";

    if(header.month < 10)
        outputFile << " ";
    outputFile << header.month << " ";

    if(header.day < 10)
        outputFile << " ";
    outputFile << header.day << " ";

    if(header.hour < 10)
        outputFile << " ";
    outputFile << header.hour << " ";

    if(header.minute < 10)
        outputFile << " ";
    outputFile << header.minute << " ";

    if(header.second < 10)
        outputFile << " ";
    outputFile << header.second << " ";

    outputFile << endl;
}

// TODO: fix line intentions
void SP3Writer::WriteRecord(EpochRecord record)
{
    if(record.is_pos)
        outputFile << "P";
    else
        outputFile << "V";

    if(record.isGlo)
        outputFile << "G";
    else
        outputFile << "R";

    // coordinates are in km
    // clock in microseconds
    outputFile << setiosflags(ios::fixed) << setprecision(6)
            << record.sv << " "
            << record.x/1000 << " "
            << record.y/1000 << " "
            << record.z/1000 << " "
            << record.clock/1e6
            << endl;
}

void SP3Writer::AddEpoch(EpochHeader epochHeader, std::vector<EpochRecord> records)
{
    this->WriteHeader(epochHeader);
    for( unsigned int i = 0; i < records.size(); i++ )
        this->WriteRecord(records.at(i));
}
