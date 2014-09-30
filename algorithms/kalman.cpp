#include "kalman.h"
#include "math_utilities.h"
#include <iostream>

using namespace std;
using namespace boost::numeric::ublas;

KalmanFilter::KalmanFilter(AbstractMatrixFacade * matrix)
{
    this->initialized = false;
    this->matrixFacade = matrix;
}

void KalmanFilter::Initialize()
{
    matrixReport.open("kalman.txt");
    this->initialized = true;

    step = 0; // номер шага
    int state_size = model->get_state_size(step);
    state_now_est.resize(state_size, 1);
    error_now_est.resize(state_size, state_size);
    state_next.resize(state_size, 1);
    error_next.resize(state_size, state_size);
    error_now_est = model->get_initial_cov();
    state_now_est = model->get_initial_state();
    data.resize(model->get_observation_size(step), 1);
}

// kalman filter main function
void KalmanFilter::Run()
{
    if( !this->initialized )
        Initialize();

    while(model->get_data(data, step))
    {
        state_now_est = model->actual_state(state_now_est, step);        
        error_now_est = model->actual_cov(error_now_est, step);

        matrix<double> Q = model->get_Q(step);
        matrix<double> F = model->get_F(step);
        matrix<double> H = model->get_H(state_now_est, step);
        matrix<double> D = model->get_D(step);

        // transpose
        matrix<double> _FT = matrixFacade->transpose(F); // фT
        matrix<double> _HT = matrixFacade->transpose(H); // HT

        // inversions
        matrix<double> _DI = matrixFacade->invert(D);

        //X[i+1]-
        cout << F.size1() << " " << F.size2() << endl;
        cout << state_now_est.size1() << " " << state_now_est.size2() << endl;
        state_next = matrixFacade->multiply(F, state_now_est);


        //P[i+1]-
        matrix<double> _f_p_ft = matrixFacade->multiply(F, error_now_est);
        _f_p_ft = matrixFacade->multiply(_f_p_ft, _FT);
        error_next = _f_p_ft + Q;

        matrix<double> _error_next_inv = matrixFacade->invert(error_next);

        //P[i+1]*
        matrix<double> _ht_d_h = matrixFacade->multiply(_HT, _DI);
        _ht_d_h = matrixFacade->multiply(_ht_d_h, H);
        matrix<double>_error_now_est = _error_next_inv + _ht_d_h;
        error_now_est = matrixFacade->invert(_error_now_est);

        //X[i+1]*
        matrix<double> _p_th_di = matrixFacade->multiply(error_now_est, _HT);
        _p_th_di = matrixFacade->multiply(_p_th_di, _DI);
        state_now_est = state_next + matrixFacade->multiply(_p_th_di, data - model->get_Y(state_next, step));

        if( verbose )
        {
            matrixReport    << "Q=" << endl << format_matrix(Q) << endl
                            << "F=" << endl << format_matrix(F) << endl
                            << "H=" << endl << format_matrix(H) << endl
                            << "D=" << endl << format_matrix(D) << endl
                            << "Y=" << endl << format_matrix(data) << endl

                            << "state_next=" << endl << format_matrix(state_next) << endl
                            << "error_next=" << endl << format_matrix(error_next) << endl
                            << "state_now_est=" << endl << format_matrix(state_now_est) << endl
                            << "error_now_est=" << endl << format_matrix(error_now_est) << endl
                            << "step=" << step << endl;
        }

        model->write_est_data(state_now_est, step);

        step++;
    }
}
