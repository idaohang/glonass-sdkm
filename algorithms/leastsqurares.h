#ifndef LEASTSQURARES_H
#define LEASTSQURARES_H

#include "model.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <fstream>

class LeastSquares : public Estimator
{
public:
    void Run();
    bool verbose;
private:
    std::ofstream matrixReport; //("kalman.txt");
    bool initialized;
    void Initialize();
    int step;

    boost::numeric::ublas::matrix<double> state_now_est;
    boost::numeric::ublas::matrix<double> error_now_est;
    boost::numeric::ublas::matrix<double> state_pre_est;
    boost::numeric::ublas::matrix<double> error_pre_est;
    boost::numeric::ublas::matrix<double> data;
};

#endif // LEASTSQURARES_H
