
//#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <sstream>

// ММ, ЧМ, Алгоритмы
#include "algorithms/kalman.h"
#include "algorithms/leastsqurares.h"
#include "algorithms/nav_sat.h"
#include "algorithms/abstractmatrixfacade.h"
#include "algorithms/boost_matrix_facade.h"
#include "algorithms/modelvisitor.h"
#include "algorithms/simplenavsatreportvisitor.h"
#include "algorithms/sp3navsatreportvisitor.h"
#include "algorithms/pseudorange_model.h"

// Данные
#include "data/observation.h"
#include "data/observation_provider.h"
#include "data/socket_observation_provider.h"
#include "data/sqlite_observation_provider.h"
#include "data/sp3processor.h"

// Математика
#include "algorithms/math_utilities.h"

// Время для создания файлов БД эфемерид
#include <boost/date_time.hpp>

using namespace std;

void EphemerisProcessingMode(char * directory)
{
    cout << "Running SP3 processing mode." << endl;
    SqliteEphemerisProvider *observationProvider = new SqliteEphemerisProvider("ephemeris.db", true);
    SP3Reader *sp3 = new SP3Reader(directory, observationProvider);
    sp3->Run();
}

void CalculationMode()
{
    cout << "Running Calculation mode." << endl;


    std::vector<int> satelites;
    satelites.push_back(12);
    satelites.push_back(14);

    /*for(int i = 1; i< 32; i++)
    {
        satelites.push_back(i);
        satelites.push_back(100+i);
    }*/

    std::vector<int> stations;    
   /* for(int i = 0; i < 10000; i++)
    {
        stations.push_back(i);
    }*/


    stations.push_back(150);
    stations.push_back(1590);
    stations.push_back(1562);

    ObservationProvider *observationProvider = (ObservationProvider *) new SocketObservationProvider();
    EphemerisProvider *ephemerisProvider = (EphemerisProvider *)new SqliteEphemerisProvider("ephemeris.db", false);

    AbstractMatrixFacade *matrix = (AbstractMatrixFacade *) new BoostMatrixFacade();

    KalmanFilter * kalman = new KalmanFilter(matrix);
    kalman->verbose = true;

    PseudorangeModel * prangeModel = new PseudorangeModel();

    NavSat * sat = new NavSat( observationProvider, ephemerisProvider, kalman, prangeModel, FixedData);

    ModelVisitor *simpleReport = (ModelVisitor *) new SimpleNavSatReportVisitor();
    sat->add_visitor(simpleReport);

    ModelVisitor *sp3Visitor = (ModelVisitor *) new SP3NavSatReportVisitor("ephemeris.sp3");
    sat->add_visitor(sp3Visitor);

    sat->Run(stations, satelites);
}

int main(int argc, char *argv[])
{
    //EphemerisProcessingMode("/home/cwiz/ephemeris/");
    CalculationMode();

    int optind=1;
    while ((optind < argc) && (argv[optind][0]=='-'))
    {
        string sw = argv[optind];

        if (sw=="-p") {
            optind++;
            char *dbName = argv[optind];
            EphemerisProcessingMode(dbName);
            return 0;
        }

        else if (sw=="-c") {
            optind++;
            CalculationMode();
            return 0;
        }

        else
            cout << "Unknown argument: " << argv[optind] << endl;

        optind++;
    }

    cout << "Usage: \"glo -p ephemeris.sp3\" for SP3 processing mode" << endl;
    cout << "Usage: \"glo -c\" for Calculation mode" << endl;

    return(0);
}
