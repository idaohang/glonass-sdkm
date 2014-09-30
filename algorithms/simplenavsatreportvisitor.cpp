#include "simplenavsatreportvisitor.h"
#include <iostream>
#include <boost/numeric/ublas/io.hpp>
#include "constants.h"

using namespace std;

void SimpleNavSatReportVisitor::write_est_data(boost::numeric::ublas::matrix<double> cur_state, unsigned t, void *object)
{
    cout << "Simple NavSat Report Visitor iteration." << endl;
    cout << cur_state << endl;

    NavSat * navSat = (NavSat *)object;

    ObservationSample * observation = navSat->observations.at(t);
    stationReport << observation->msec << ' ';

    for( unsigned i=0; i < cur_state.size1(); i++)
    {
        stationReport << cur_state(i, 0)/c << ' ';
    }

    stationReport << endl;

    std::vector<int> satids = observation->GetSateliteIDs();
    for(unsigned int i = 0; i < satids.size(); i++)
    {
        if( sateliteReport[satids[i]] == NULL)
        {
            string fileName;
            stringstream strStream;
            strStream << "satelite" << satids[i];
            fileName = strStream.str();
            fileName.append(".txt");
            sateliteReport[ satids[i]] = new std::ofstream();
            sateliteReport[ satids[i]]->open(fileName.c_str());
        }

        *sateliteReport[satids[i]] << observation->msec << ' ';

        for( int j=0; j<3; j++)
        {
            *sateliteReport[satids[i]] << cur_state(4* navSat->get_obs_stations_size(t)+3*i + j, 0) << ' ';
        }

        *sateliteReport[satids[i]] << endl;
    }


    for(int i = 0; i < navSat->get_obs_stations_size(t); i++)
    {
        int id = 2000 + observation->stations.at(i);
        if( sateliteReport[id] == NULL)
        {
            string fileName;
            stringstream strStream;
            strStream << "station" << observation->stations.at(i);
            fileName = strStream.str();
            fileName.append(".txt");
            sateliteReport[2000 + observation->stations.at(i)] = new std::ofstream();
            sateliteReport[2000 + observation->stations.at(i)]->open(fileName.c_str());
        }

        *sateliteReport[2000 + observation->stations.at(i)] << observation->msec << ' ';

        for( int j=0; j<4; j++)
        {
            *sateliteReport[2000 + observation->stations.at(i)] << cur_state(4*i + j, 0)/c << ' ';
        }

        *sateliteReport[2000 + observation->stations.at(i)] << endl;
    }

}
