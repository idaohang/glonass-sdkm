#include "nav_sat.h"

#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#include <sstream>
#include <string>
#include "math_utilities.h"
#include "constants.h"

using namespace std;
using namespace boost::numeric::ublas;

void NavSat::UpdateStations(std::vector<Station> *stations)
{
    if (this->stations.size() != 0)
        this->stations.clear();
    this->stations = *stations;
}

Station *NavSat::GetStation(int stationId)
{
    for(unsigned int i = 0; i < this->stations.size(); i++)
    {
        Station sta = stations.at(i);

        if(sta.id == stationId)
            return &sta;
    }

    return NULL;
}

void NavSat::put_data(ObservationSample * obs)
{
    observations.push_back(obs);
    this->estimator->Run();
}

int NavSat::get_obs_satelites_size(unsigned t)
{
    return this->observations[t]->GetSateliteNumber();
}

int NavSat::get_obs_stations_size(unsigned t)
{
    return this->observations[t]->GetStationNumber();
}

int NavSat::get_state_size(unsigned t)
{
    return 4*get_obs_stations_size(t) + 3*get_obs_satelites_size(t);
}

int NavSat::get_observation_size(unsigned t)
{
    return get_obs_satelites_size(t)*get_obs_stations_size(t);
}

NavSat::NavSat(ObservationProvider *observationProvider,
               EphemerisProvider *ephemerisProvider,
               Estimator *est,
               PseudorangeModel * prangeModel,
               ProcessingType p) : Model(est)
{
    this->observationProvider = observationProvider;
    this->ephemerisProvider = ephemerisProvider;
    this->observationProvider->model = this;
    this->prangeModel = prangeModel;
    this->processing = p;

    double h0 = 2e-19;
    double h2 = 2e-20;
    S2 = 2*h0;
    S3 = 8*PI*PI*h2;
}

matrix<double> NavSat::get_F(unsigned t)
{
    matrix<double>F;
    matrix<double> I = identity_matrix<double>(get_state_size(t));
    matrix<double> A(get_state_size(t), get_state_size(t));

    A.clear();
    for( int i = 0; i < get_obs_stations_size(t); i++)
        A( 4*i + 3, 4*i + 2) = 1;

    for( int i = 0; i < get_obs_satelites_size(t); i++)
    {
        A(4*get_obs_stations_size(t) + 3*i + 1, 4*get_obs_stations_size(t) + 3*i + 0) = 1;
        A(4*get_obs_stations_size(t) + 3*i + 2, 4*get_obs_stations_size(t) + 3*i + 1) = 1;
    }

    F = I + A*get_delta(t);

    return F;
}

matrix<double> NavSat::get_H(unsigned t)
{
/**
    matrix<double> H(get_observation_size(t), get_state_size(t));
     H.clear();

     double Tau1 = 0;
     double Tau2 = (this->observations[t]->msec - this->observations[0]->msec)*1e-3;

     int curSat = 0;

     for( int k = 0; k <  get_observation_size(t); k++)
     {
         for( int i = 0; i < this->get_obs_stations_size(t); i++)
         {
             H(k, 4*i + 0 ) = 0;
             H(k, 4*i + 1 ) = 0;
             H(k, 4*i + 2 ) = Tau1;
             H(k, 4*i + 3 ) = 1;
         }

         if( this->processing == SingleStation )
         {
             for( int j = 0; j < get_obs_stations_size(t)-1; j++)
             {
                 if( (j*get_obs_satelites_size(t)) <= k && (k < (j+1)*get_obs_satelites_size(t) ))
                 {
                     curSat = j;
                     break;
                 }
             }
         }
         else if( this->processing == SingleSatelite )
         {
             curSat = 0;
         }

         H(k, 4*get_obs_stations_size(t) + 3*curSat + 0) = -Tau2;
         H(k, 4*get_obs_stations_size(t) + 3*curSat + 1) = -Tau2*Tau2;
         H(k, 4*get_obs_stations_size(t) + 3*curSat + 2) = -1;

         curSat++;
         if( curSat == this->get_obs_satelites_size(t) )
             curSat = 0;
     }

     return H;
*/


    matrix<double> H(get_observation_size(t), get_state_size(t));
    H.clear();

    double Tau1 = 0;
    double Tau2 = (this->observations[t]->msec - this->observations[0]->msec)*1e-3;

    for( int k = 0; k < get_observation_size(t); k++)
    {
        for( int i = 0; i < this->get_obs_stations_size(t); i++)
        {
            H(k, 4*i + 0 ) = 0;
            H(k, 4*i + 1 ) = 0;
            H(k, 4*i + 2 ) = Tau1;
            H(k, 4*i + 3 ) = 1;
        }

        std::vector<int> stations = this->observations[t]->GetStationIDs();
        std::vector<int> sats = this->observations[t]->GetSateliteIDs();

        for( int i = 0; i < stations.size(); i++)
        {
            std::vector<Pseudorange> * pranges=  this->observations[t]->pranges[stations[i]];

            for( int j = 0; j < sats.size(); j++)
            {
                bool found = false;

                for ( int k  = 0; k < pranges->size(); k++)
                {
                    Pseudorange pr = pranges->at(k);
                    if( pr.satId == sats[j] )
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                    H(k, 4*get_obs_stations_size(t) + 3*j + 0) = -Tau2;
                    H(k, 4*get_obs_stations_size(t) + 3*j + 1) = -Tau2*Tau2;
                    H(k, 4*get_obs_stations_size(t) + 3*j + 2) = -1;
                }
            }
        }
    }

    return H;
}

bool NavSat::get_data(matrix<double> &data, unsigned t)
{
    if( t >= this->observations.size())
        return false;

    std::vector<int> stations = this->observations[t]->GetStationIDs();
    std::vector<int> sats = this->observations[t]->GetSateliteIDs();
    wrapper = this->ephemerisProvider->FindEphemeris(this->observations[t]->year, this->observations[t]->month,
                                                     this->observations[t]->day, this->observations[t]->msec,
                                                     this->selectedSatelites);

    data.resize(get_observation_size(t), 1);
    data.clear();

    int counter = 0;

    for( unsigned int i = 0; i < stations.size(); i++)
    {
        std::vector<Pseudorange> * pranges =  this->observations[t]->pranges[stations[i]];

        for( unsigned int j = 0; j < sats.size(); j++)
        {
            bool found = false;

            Pseudorange prange;
            for ( unsigned int k  = 0; k < pranges->size(); k++)
            {
                Pseudorange pr = pranges->at(k);
                if( pr.satId == sats[j] )
                {
                    prange = pr;
                    found = true;
                    break;
                }
            }

            SatPosition * sat = wrapper.GetSatelite(sats[j]);
            Station     * sta = this->GetStation(stations[i]);

            double value = 0.0;
            if(sat != NULL && sta != NULL && found)
            {
                value = this->prangeModel->CalculatePrange(prange, *sat, *sta);
            }          

            data(counter++, 0) = value;
        }
    }

    return true;
}

matrix<double> NavSat::actual_state(matrix<double> cur_state, unsigned t)
{
    if( t == 0 || this->processing == FixedData )
        return cur_state;

    matrix<double> data(this->get_state_size(t), 1);
    data.clear();

    std::vector<int> currentStationIDs = this->observations[t]->GetStationIDs();
    std::vector<int> prevStationIDs = this->observations[t-1]->GetStationIDs();


    for( unsigned int i = 0; i < currentStationIDs.size(); i++)
    {
        bool found = false;
        int previousId = 0;

        for( unsigned int j = 0; j < prevStationIDs.size(); j++)
        {
            if(currentStationIDs[i] == prevStationIDs[j])
            {
                found = true;
                previousId = j;
                break;
            }
        }

        for( unsigned int k = 0; k < 4; k++ )
        {
            double val = 0.0;

            if(found)
                val = cur_state(4*previousId + k, 0);

            else
                val = 0;

            data(4*i + k, 0) = val;
        }
    }

    std::vector<int> currentSateliteIDs = this->observations[t]->GetSateliteIDs();
    std::vector<int> previousSateliteIDs = this->observations[t-1]->GetSateliteIDs();

    for( unsigned int i = 0; i < currentSateliteIDs.size(); i++)
    {
        bool found = false;
        int previousId = 0;

        for( unsigned int j = 0; j < previousSateliteIDs.size(); j++)
        {
            if(currentSateliteIDs[i] == previousSateliteIDs[j])
            {
                found = true;
                previousId = j;
                break;
            }
        }

        for( unsigned int k = 0; k < 3; k++ )
        {
            double val = 0.0;

            if(found)
                val = cur_state(4*this->get_obs_stations_size(t-1) + 3*previousId + k, 0);

            else
                val = 0;

            data(4*this->get_obs_stations_size(t) + 3*i + k, 0) = val;
        }
    }

    return data;
}

matrix<double> NavSat::actual_cov(matrix<double> err, unsigned t)
{
    if( t == 0 || this->processing == FixedData )
        return err;

    matrix<double> data(this->get_state_size(t), this->get_state_size(t));
    data.clear();

    std::vector<int> currentStationIDs = this->observations[t]->GetStationIDs();
    std::vector<int> prevStationIDs = this->observations[t-1]->GetStationIDs();

    for( unsigned int i = 0; i < currentStationIDs.size(); i++)
    {
        bool found = false;
        int previousId = 0;

        for( unsigned int j = 0; j < prevStationIDs.size(); j++)
        {
            if(currentStationIDs[i] == prevStationIDs[j])
            {
                found = true;
                previousId = j;
                break;
            }
        }

        for( unsigned int k = 0; k < 4; k++ )
        {
            double val = 0.0;

            if(found)
                val = err(4*previousId + k, 4*previousId + k);

            else
                val = 0;

            data(4*i + k, 4*i + k) = val;
        }
    }

    std::vector<int> currentSateliteIDs = this->observations[t]->GetSateliteIDs();
    std::vector<int> previousSateliteIDs = this->observations[t-1]->GetSateliteIDs();

    for( unsigned int i = 0; i < currentSateliteIDs.size(); i++)
    {
        bool found = false;
        int previousId = 0;

        for( unsigned int j = 0; j < previousSateliteIDs.size(); j++)
        {
            if(currentSateliteIDs[i] == previousSateliteIDs[j])
            {
                found = true;
                previousId = j;
                break;
            }
        }

        for( unsigned int k = 0; k < 3; k++ )
        {
            double val = 0.0;

            if(found)
                val = err(4*this->get_obs_stations_size(t-1) + 3*previousId + k, 4*this->get_obs_stations_size(t-1) + 3*previousId + k);

            else
                val = 0;

            data(4*this->get_obs_stations_size(t) + 3*i + k, 4*this->get_obs_stations_size(t) + 3*i + k) = val;
        }
    }

    return data;
}

double NavSat::get_delta(unsigned t)
{
    if( t == 0 )
        return 0;

    double delta = (this->observations[t]->msec - this->observations[t-1]->msec)*1e-3;
    return delta;
}

matrix<double> NavSat::get_H(matrix<double> cur_state, unsigned t)
{
    return get_H(t);
}

matrix<double> NavSat::get_initial_cov()
{
    matrix<double> I = identity_matrix<double>(get_state_size(0))*1e6;
    return I;
}

matrix<double> NavSat::get_initial_state()
{
    matrix<double> state(get_state_size(0), 1);
    state.clear();
    return state;
}

matrix<double> NavSat::get_Q(unsigned t)
{
    int state = get_state_size(t);
    matrix<double> Q(state, state);
    Q.clear();

    for( int i = 0; i < get_obs_stations_size(t); i++)
    {
        Q(4*i + 2, 4*i + 2) = S2*pow(get_delta(t), 2)/2;
        Q(4*i + 2, 4*i + 3) = S2*pow(get_delta(t), 2)/2;
        Q(4*i + 3, 4*i + 2) = S2*pow(get_delta(t), 2)/2;
        Q(4*i + 3, 4*i + 3) = S3*get_delta(t) + S2*pow(get_delta(t), 3)/3;
    }

    for( int j = 0; j < get_obs_satelites_size(t); j++)
    {
        Q(4*get_obs_stations_size(t) + 3*j + 1, 4*get_obs_stations_size(t) + 3*j + 1 ) = S2*pow(get_delta(t), 2)/2;
        Q(4*get_obs_stations_size(t) + 3*j + 1, 4*get_obs_stations_size(t) + 3*j + 2 ) = S2*pow(get_delta(t), 2)/2;
        Q(4*get_obs_stations_size(t) + 3*j + 2, 4*get_obs_stations_size(t) + 3*j + 1 ) = S2*pow(get_delta(t), 2)/2;
        Q(4*get_obs_stations_size(t) + 3*j + 2, 4*get_obs_stations_size(t) + 3*j + 2 ) = S3*get_delta(t) + S2*pow(get_delta(t), 3)/3;
    }

    return Q*c*c;
}

matrix<double> NavSat::get_D(unsigned t)
{
    matrix<double> D = identity_matrix<double>(get_observation_size(t))*100;
    return D;
}

matrix<double> NavSat::get_Y(matrix<double> cur_state, unsigned t)
{
    return prod(get_H(t), cur_state);
}


void NavSat::Run(std::vector<int> stations, std::vector<int> satelites)
{
    this->selectedSatelites = satelites;
    this->selectedStations = stations;

    observationProvider->FindData( this->selectedStations, this->selectedSatelites, this->processing);
}

void NavSat::write_est_data(matrix<double> cur_state, unsigned t)
{
    for(unsigned int i = 0; i < visitors.size(); i++)
    {
        ModelVisitor * visitor = visitors[i];
        visitor->write_est_data(cur_state, t, this);
    }
}
