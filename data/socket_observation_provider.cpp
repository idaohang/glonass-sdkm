#include "socket_observation_provider.h"

#define HALF_DUPLEX		"/tmp/glonass_halfduplex"
#define MAX_BUF_SIZE 10048576    // Megabyte buffer should be ok

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include "jsoncpp/value.h"
#include "jsoncpp/reader.h"

#include "../socket/ServerSocket.h"
#include "../socket/SocketException.h"

/*
 Ephemeris message
 ---
 Start Ephemeris
 {
    "year":     2010,
    "month":    9,
    "day":      25,

    "observations":[
        { "stationId": 66,
          "pseudoranges": [
            { "satId": 101, "l1": 100.2, "l2": 555.6 },
            { "satId": 102, "l1": 100.2, "l2": 555.6 }
          ]
        }
    ]
 }
  Finish
 ---
 */

/*
 Stations message
 ---
 Start Stations
 {
    "stations":[
        {
            "id": 1,
            "x": 1.323,
            "y": 3.2323,
            "z": 3.332
        }
    ]
 }
  Finish
 ---
 */

using namespace std;

void SocketObservationProvider::FindData(std::vector<int> stations, std::vector<int> satelites, ProcessingType type)
{
    this->satelites = satelites;
    this->stations = stations;
    this->processing = type;

    PollSocket();
}

void SocketObservationProvider::PollSocket()
{
    try
    {
        cout << "Starting socket server." << endl;
        ServerSocket server ( 3000 );

        cout << "Waiting for connection." << endl;
        while(true)
        {
            ServerSocket new_sock;
            server.accept ( new_sock );

            try
            {
                while(true)
                {
                    bool newMessage = false;
                    string data;
                    new_sock >> data;

                    if( data.substr(0, 19) == "Start Observations ")
                    {
                        this->messageClass = Observations;

                        data = data.substr(19, data.size()-19);
                        newMessage = true;
                    }

                    else if( data.substr(0, 15) == "Start Stations ")
                    {
                        this->messageClass = Stations;

                        data = data.substr(15, data.size()-15);
                        newMessage = true;
                    }

                    if(newMessage)
                    {
                        fullMessage = "";
                    }

                    fullMessage.append(data);
                    if(fullMessage.substr(fullMessage.size()-8, 6) == "Finish")
                    {
                        fullMessage = fullMessage.substr(0, fullMessage.size()-7);
                        ProcessMessage();
                        fullMessage = "";
                    }
                }
            }
            catch ( SocketException& e )
            {
                //empty catch is bad practice but still it is here
                //cout << "Exception was caught: " << e.description() << "\nRetrying...\n";
            }
        }
    }

    catch ( SocketException& e )
    {
        cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }
}

void SocketObservationProvider::ProcessMessage()
{
    if(this->messageClass == Observations)
    {
        ObservationSample *nextSample = NULL;
        try
        {
            nextSample = ProcessObservations(fullMessage);
        }
        catch(int e){}

        if(nextSample == NULL)
        {
            cout << "Cannot parse data." << endl;
            return;
        }

        if( nextSample->GetSateliteNumber() == 0 || nextSample->GetStationNumber() == 0 )
        {
            cout << "No data found." << endl;
            return;
        }

        if( this->processing == FixedData && !this->HasFixedData(nextSample) )
        {
            cout << "No data found. FixedData mode." << endl;
            return;
        }

        model->put_data(nextSample);
    }

    else if(this->messageClass == Stations)
    {
        std::vector<Station> * stations = ProcessStations(fullMessage);

        if(stations == NULL)
        {
            cout << "Cannot parse data." << endl;
            return;
        }

        model->UpdateStations(stations);
    }

    this->fullMessage = "";
}

std::vector<Station> * SocketObservationProvider::ProcessStations(string data)
{
    cout << "Processing stations " << endl;

    vector<Station> * stations = new vector<Station>();

    Json::Value root;
    Json::Reader reader;

    bool parseSucessful = reader.parse(data, root);
    if ( !parseSucessful )
    {
        cout << "Cannot parse: " << reader.getFormatedErrorMessages() << endl;
        cout << data << endl;
        return NULL;
    }

    Json::Value sta = root.get("stations", "UTF-8");
    for(unsigned int i = 0; i < sta.size(); i++)
    {
        int stationId   = sta[i].get("id", "UTF-8").asInt();
        double x        = sta[i].get("x", "UTF-8").asDouble();
        double y        = sta[i].get("y", "UTF-8").asDouble();
        double z        = sta[i].get("z", "UTF-8").asDouble();

        Station sta;
        sta.id = stationId;
        sta.coord.x = x*1000;
        sta.coord.y = y*1000;
        sta.coord.z = z*1000;

        stations->push_back(sta);
    }

    return stations;
}

ObservationSample* SocketObservationProvider::ProcessObservations(string data)
{
    cout << "Processing observations " << endl;

    ObservationSample *currentSample = new ObservationSample();

    Json::Value root;
    Json::Reader reader;

    bool parseSucessful = reader.parse(data, root);
    if ( !parseSucessful )
    {
        cout << "Cannot parse: " << reader.getFormatedErrorMessages() << endl;
        return NULL;
    }

    Json::Value observations = root.get("observations", "UTF-8");
    int observationSize = observations.size();

    currentSample->year     = root.get("year", "UTF-8").asInt();
    currentSample->month    = root.get("month", "UTF-8").asInt();
    currentSample->day      = root.get("day", "UTF-8").asInt();
    currentSample->msec     = root.get("mseconds", "UTF-8").asUInt();

    for( int i = 0; i < observationSize; i++)
    {
        int stationId = observations[i].get("stationId", "UTF-8").asInt();

        if( this->processing != AllData && !this->ContainsStation(stationId) )
        {
            cout << "Skipping station=" << stationId << endl;
            continue;
        }

        const Json::Value pranges = observations.get(i, "UTF-8").get("pseudoranges", "UTF-8");
        for( unsigned int j = 0; j < pranges.size(); j++)
        {
            Pseudorange pr;
            pr.satId = pranges[j].get("satId", "UTF-8").asInt();
            pr.l1 = pranges[j].get("l1", "UTF-8").asDouble();
            pr.l2 = pranges[j].get("l2", "UTF-8").asDouble();

            if( this->processing != AllData && !this->ContainsSatelite(pr.satId) )
            {
                cout << "Skipping satelite=" << pr.satId << endl;
                continue;
            }

            currentSample->AddObservation(stationId, pr);
        }
    }

    return currentSample;
}

bool SocketObservationProvider::ContainsSatelite(int id)
{
    return (std::find(this->satelites.begin(), this->satelites.end(), id) != this->satelites.end());
}

bool SocketObservationProvider::ContainsStation(int id)
{
    return (std::find(this->stations.begin(), this->stations.end(), id) != this->stations.end());
}

bool SocketObservationProvider::HasFixedData(ObservationSample *nextSample)
{
    std::vector<int> stationIDs = nextSample->GetStationIDs();

    for ( int j = 0; j < this->stations.size(); j++ )
    {
        if( !(std::find(stationIDs.begin(), stationIDs.end(), this->stations[j]) != stationIDs.end()) )
            return false;

        std::vector<Pseudorange> * pranges = nextSample->pranges[this->stations[j]];
        std::vector<int> observableSatelites;
        for( int i = 0; i < pranges->size(); i++)
        {
            Pseudorange pr = pranges->at(i);
            observableSatelites.push_back(pr.satId);
        }

        for( int i = 0; i < this->satelites.size(); i++)
        {
            if( !(std::find(observableSatelites.begin(), observableSatelites.end(), this->satelites[i]) != observableSatelites.end()) )
                return false;
        }
    }

    return true;
}
