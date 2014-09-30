#ifndef KALMAN_H
#define KALMAN_H

#include "model.h"
#include "abstractmatrixfacade.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <fstream>

//!  Фильтр Калмана
/*!
  Класс описывающий фильтр Калмана.
  Реализован таким образом, что может работать как с линейными, так и с нелинейными моделями.
*/
class KalmanFilter : public Estimator
{
    public:

        //!  Метод запускающий вычисления
        void Run();

        //!  Логировать вычисления?
        bool verbose;

        //!  Констркутор принимает класс, который производит вычисления с матрицами
        KalmanFilter(AbstractMatrixFacade * matrix);

    private:
        std::ofstream matrixReport;
        bool initialized;
        void Initialize();
        int step;

        //!  Оценка текущего состояния
        boost::numeric::ublas::matrix<double> state_now_est;    // (state_size, 1);

        //!  Оценка текущей ошибки
        boost::numeric::ublas::matrix<double> error_now_est;    //(state_size, state_size);

        //!  Оценка состояния на следующий шаг
        boost::numeric::ublas::matrix<double> state_next;       //(state_size, 1);

        //!  Оценка ошибки на следующий шаг
        boost::numeric::ublas::matrix<double> error_next;       //(state_size, state_size);

        boost::numeric::ublas::matrix<double> data;

        AbstractMatrixFacade * matrixFacade;
};

#endif
