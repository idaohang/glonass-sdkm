#include "sp3navsatreportvisitor.h"
#include <iostream>
#include "constants.h"

using namespace std;

SP3NavSatReportVisitor::SP3NavSatReportVisitor(char * fileName)
{    
    this->sp3Writer = new SP3Writer(fileName);
}

void SP3NavSatReportVisitor::write_est_data(boost::numeric::ublas::matrix<double> cur_state, unsigned t, void *object)
{
    //cout << "SP3NavSatReportVisitor iteration." << endl;
    NavSat * navSat = (NavSat *)object;
    ObservationSample * observation = navSat->observations.at(t);

    EpochHeader header;
    header.year = observation->year;
    header.month = observation->month;
    header.day = observation->day;
    header.hour = (int)observation->msec / 3600000;
    header.minute = (observation->msec - 3600000*header.hour)/60000;
    header.second = (observation->msec - 3600000*header.hour - 60000*header.minute)/1000;

    std::vector<int> satids = observation->GetSateliteIDs();
    std::vector<EpochRecord> epochRecords;
    for(unsigned int i = 0; i < satids.size(); i++)
    {
        double clock = 0.0;
        for( int j=0; j<3; j++)
        {
            clock += cur_state(4* navSat->get_obs_stations_size(t)+3*i + j, 0);
        }

        SatPosition * pos = navSat->wrapper.GetSatelite(satids.at(i));

        EpochRecord record;
        record.clock = clock/c;
        record.sv = satids.at(i);
        record.isGlo = (record.sv > 100);
        record.is_pos = true;

        record.x = pos->coord.x;
        record.y = pos->coord.y;
        record.z = pos->coord.z;

        // for testing purposes. remove in rls
        EpochRecord initial;
        initial.clock = pos->clock*1e6;
        initial.isGlo = (record.sv > 100);
        initial.is_pos = true;
        initial.sv = record.sv;
        initial.x = record.x;
        initial.y = record.y;
        initial.z = record.z;

        epochRecords.push_back(initial);
        epochRecords.push_back(record);
    }

    this->sp3Writer->AddEpoch(header, epochRecords);
}
