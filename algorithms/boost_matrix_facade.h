#ifndef BOOST_MATRIX_FACADE_H
#define BOOST_MATRIX_FACADE_H

#include "abstractmatrixfacade.h"

//!  Класс, реализующий операции с матрицами на CPU используя библотеку Boost
class BoostMatrixFacade : public AbstractMatrixFacade
{
public:
    BoostMatrixFacade();

    boost::numeric::ublas::matrix<double> transpose   (const boost::numeric::ublas::matrix<double> matrix);
    boost::numeric::ublas::matrix<double> invert      (const boost::numeric::ublas::matrix<double> matrix);

    boost::numeric::ublas::matrix<double> multiply    (const boost::numeric::ublas::matrix<double> a,
                                                       const boost::numeric::ublas::matrix<double> b);
};

#endif // BOOST_MATRIX_FACADE_H
