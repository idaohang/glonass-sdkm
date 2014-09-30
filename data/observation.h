#ifndef OBSERVATION_H
#define OBSERVATION_H

#include <vector>
#include <ext/hash_map>
#include <cmath>

//! Струкутура, описывающаяя координаты в трехмерном пространстве
struct Point
{
    double x;
    double y;
    double z;
};

//!  Координаты спутника
struct SatPosition
{
    int sv;
    Point coord;
    double clock;
};

//!  Координаты станции
struct Station
{
    int id;
    Point coord;
};

//!  Псевдодальность
struct Pseudorange
{
    int satId;
    double l1;
    double l2;
};

//!  Измерения для эпохи
struct ObservationSample
{
    // datetime
    int year;
    int month;
    int day;
    long msec;

    // data

    //!  id станций
    std::vector<int> stations;

    //!  id спутников
    std::vector<int> satelites;

    //!  id псевдодальности
    __gnu_cxx::hash_map<int, std::vector<Pseudorange> * > pranges;

    //!  Метод, добавляющий измерения
    inline void AddObservation(int stationId, Pseudorange pr)
    {
        this->AddStationId(stationId);
        this->AddSateliteId(pr.satId);
        this->pranges[stationId]->push_back(pr);
    }

    //! Метод, возвращающий число станций задействованых в измерении
    inline int GetStationNumber()
    {
        return this->stations.size();
    }

    //! Метод, возвращающий численные идентификаторы станций
    inline std::vector<int> GetStationIDs()
    {
        return this->stations;
    }

    //! Метод, возвращающий численные идентификаторы спутников
    inline std::vector<int> GetSateliteIDs()
    {
        return this->satelites;
    }

    //! Метод, возвращающий число спутников задействованых в измерении
    inline int GetSateliteNumber()
    {
        return this->satelites.size();
    }

    inline void AddStationId(int stationId)
    {
        bool hasElement = false;
        for(unsigned int i = 0; i < this->stations.size(); i++)
        {
            if(this->stations[i] == stationId)
            {
                hasElement = true;
                break;
            }
        }

        if(!hasElement)
        {
            this->stations.push_back(stationId);
            pranges[stationId] = new std::vector<Pseudorange>();
        }
    }

    inline void AddSateliteId(int sateliteId)
     {
         bool hasElement = false;
         for(unsigned int i = 0; i < this->satelites.size(); i++)
         {
             if(this->satelites[i] == sateliteId)
             {
                 hasElement = true;
                 break;
             }
         }

         if(!hasElement)
         {
             this->satelites.push_back(sateliteId);
         }
     }
};

#endif // OBSERVATION_H
