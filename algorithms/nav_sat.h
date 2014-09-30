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

//!  Математическая модель, описывающиая НКА
class NavSat : public Model
{
public:
    //!  Конструктор принимает провайдер измерений, провайдер эфемерид и тип обработки: 1 спутник много станций либо много станций 1 спутник
    NavSat(ObservationProvider * obs, EphemerisProvider * eph, Estimator *est, PseudorangeModel * model, ProcessingType t);

    //!  Метод запускает вычисления. В качестве аргументво принимает идентификаторы станций и спутников, для которых производить вычисления
    void Run(std::vector<int> stations, std::vector<int> satelites);

    bool get_data(boost::numeric::ublas::matrix<double> &data, unsigned t);                                     // Y(t)
    boost::numeric::ublas::matrix<double> get_F(unsigned t);                                                    // Ф(t)
    boost::numeric::ublas::matrix<double> get_H(unsigned t);                                                    // H(t) regular filter
    boost::numeric::ublas::matrix<double> get_H(boost::numeric::ublas::matrix<double> cur_state, unsigned t);   // H(x, t) extended filter
    boost::numeric::ublas::matrix<double> get_Y(boost::numeric::ublas::matrix<double> cur_state, unsigned t);   // Y(x) unlinear transition

    boost::numeric::ublas::matrix<double> get_D(unsigned t);
    boost::numeric::ublas::matrix<double> get_Q(unsigned t);    

    boost::numeric::ublas::matrix<double> get_initial_state();
    boost::numeric::ublas::matrix<double> get_initial_cov();

    boost::numeric::ublas::matrix<double> actual_state(boost::numeric::ublas::matrix<double> cur_state, unsigned t);
    boost::numeric::ublas::matrix<double> actual_cov(boost::numeric::ublas::matrix<double> cur_state, unsigned t);

    //!  Возвращает размерность вектора состояния для шага t
    int get_state_size(unsigned t);

    //!  Возвращает число станций для шага t
    int get_obs_stations_size(unsigned t);

    //!  Возвращает число обозреваемых спутников для шага t
    int get_obs_satelites_size(unsigned t);

    //!  Возвращает размер вектора измерений для шага t
    int get_observation_size(unsigned t);

    //! Получение новых данных в фильтр осуществляется этим методом
    void put_data(ObservationSample * obs);

    //!  Метод, в качестве аргумента принимабщий актуальный вектор станций
    void UpdateStations(std::vector<Station> * stations);

    //!  Вызов посетителей для вывода результатов на каждой итерации фильтра
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

    //! Модель, псевдодальности
    PseudorangeModel * prangeModel;

    Station * GetStation(int stationId);
};

#endif // NAV_SAT_H
