#ifndef NAV_SAT_H
#define NAV_SAT_H

#include "model.h"
#include <vector>

#include "data/processing_type.h"
#include "data/observation_provider.h"
#include "data/sqlite_observation_provider.h"
#include "data/observation.h"
#include "data/ephemeris_wrapper.h"
#include "pseudorange_model.h"

class ObservationProvider;
class EphemerisProvider;

//!  �������������� ������, ������������ ���
class NavSat : public Model
{
public:
    //!  ����������� ��������� ��������� ���������, ��������� �������� � ��� ���������: 1 ������� ����� ������� ���� ����� ������� 1 �������
    NavSat(ObservationProvider * obs, EphemerisProvider * eph, Estimator *est, PseudorangeModel * model, ProcessingType t);

    //!  ����� ��������� ����������. � �������� ���������� ��������� �������������� ������� � ���������, ��� ������� ����������� ����������
    void Run(std::vector<int> stations, std::vector<int> satelites);

    bool get_data(boost::numeric::ublas::matrix<double> &data, unsigned t);                                     // Y(t)
    boost::numeric::ublas::matrix<double> get_F(unsigned t);                                                    // �(t)
    boost::numeric::ublas::matrix<double> get_H(unsigned t);                                                    // H(t) regular filter
    boost::numeric::ublas::matrix<double> get_H(boost::numeric::ublas::matrix<double> cur_state, unsigned t);   // H(x, t) extended filter
    boost::numeric::ublas::matrix<double> get_Y(boost::numeric::ublas::matrix<double> cur_state, unsigned t);   // Y(x) unlinear transition

    boost::numeric::ublas::matrix<double> get_D(unsigned t);
    boost::numeric::ublas::matrix<double> get_Q(unsigned t);    

    boost::numeric::ublas::matrix<double> get_initial_state();
    boost::numeric::ublas::matrix<double> get_initial_cov();

    boost::numeric::ublas::matrix<double> actual_state(boost::numeric::ublas::matrix<double> cur_state, unsigned t);
    boost::numeric::ublas::matrix<double> actual_cov(boost::numeric::ublas::matrix<double> cur_state, unsigned t);

    //!  ���������� ����������� ������� ��������� ��� ���� t
    int get_state_size(unsigned t);

    //!  ���������� ����� ������� ��� ���� t
    int get_obs_stations_size(unsigned t);

    //!  ���������� ����� ������������ ��������� ��� ���� t
    int get_obs_satelites_size(unsigned t);

    //!  ���������� ������ ������� ��������� ��� ���� t
    int get_observation_size(unsigned t);

    //! ��������� ����� ������ � ������ �������������� ���� �������
    void put_data(ObservationSample * obs);

    //!  �����, � �������� ��������� ����������� ���������� ������ �������
    void UpdateStations(std::vector<Station> * stations);

    //!  ����� ����������� ��� ������ ����������� �� ������ �������� �������
    void write_est_data(boost::numeric::ublas::matrix<double> cur_state, unsigned t);

    // Encapsulation breakdown for sake of visitors. must be fixed
    std::vector<ObservationSample *> observations;

    // Same here
    EphemerisWrapper wrapper;

 private:    
    double get_delta(unsigned t);
    double S2, S3;

    std::vector<Station> stations;
    std::vector<int> selectedStations;
    std::vector<int> selectedSatelites;

    ProcessingType processing;

    // providers
    ObservationProvider * observationProvider;
    EphemerisProvider * ephemerisProvider;

    //! ������, ���������������
    PseudorangeModel * prangeModel;

    Station * GetStation(int stationId);
};

#endif // NAV_SAT_H
