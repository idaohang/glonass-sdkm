#include "leastsqurares.h"
#include "math_utilities.h"

using namespace std;
using namespace boost::numeric::ublas;

void LeastSquares::Initialize()
{
    matrixReport.open("leastsquares.txt");
    this->initialized = true;
    this->state_pre_est = model->get_initial_state();
    this->error_pre_est = model->get_initial_cov();

    step = 0; // номер шага
}

void LeastSquares::Run()
{
    if( !this->initialized )
        Initialize();

    while( this->model->get_data(data, step) )
    {
        matrix<double> H = model->get_H(step);
        matrix<double> F = model->get_F(step);
        H = prod(H, F);

        matrix<double> _HT = trans(H);
        matrix<double> R = model->get_weight(step);
        matrix<double> _RI;
        invert(R, _RI);        

        this->state_pre_est = model->actual_state(state_pre_est, step);
        this->error_now_est = model->actual_cov(error_now_est, step);

        // error
        matrix<double> _p_ht;
        _p_ht = prod(this->error_pre_est, _HT);

        matrix<double> _r_hph;
        _r_hph = prod(H, this->error_pre_est);
        _r_hph = R + prod(_r_hph, _HT);
        matrix<double> _r_hph_i;
        invert(_r_hph, _r_hph_i);

        matrix<double> _hp = prod(H, this->error_pre_est);

        this->error_now_est = prod(_p_ht, _r_hph_i);
        this->error_now_est = prod(this->error_now_est, _hp);
        this->error_now_est = this->error_pre_est - this->error_now_est;

        // state
        matrix<double> p_h_ri = prod(this->error_now_est, _HT);
        p_h_ri = prod(p_h_ri, _RI);

        matrix<double> _hx = prod(H, this->state_pre_est);
        this->state_now_est = prod( p_h_ri, data -  _hx);
        this->state_now_est = this->state_pre_est + this->state_now_est;

        if( verbose )
        {            
            matrixReport
                    << "H=" << endl << format_matrix(H) << endl
                    << "state_now_est=" << endl << format_matrix(state_now_est) << endl
                    << "error_now_est=" << endl << format_matrix(error_now_est) << endl
                    << "step=" << step << endl;
        }

        model->write_est_data(state_now_est, step);
        step++;

        this->error_pre_est = this->error_now_est;
        this->state_pre_est = this->state_now_est;
    }
}
