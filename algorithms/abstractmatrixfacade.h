#ifndef ABSTRACTMATRIXFACADE_H
#define ABSTRACTMATRIXFACADE_H

#include <boost/numeric/ublas/matrix.hpp>


//!  Абстрактный класс для матричных операций
/*!
  Класс описывает операции над матрицами, необходимые фильтру Калмана
  В качестве класса описывающего матрицу принимается boost::numeric::ublas::matrix из библиотеки Boost
*/
class AbstractMatrixFacade
{
public:

    //!  Операция транспонирования
    virtual boost::numeric::ublas::matrix<double> transpose   (const boost::numeric::ublas::matrix<double> matrix ) = 0;

    //!  Операция обращения
    virtual boost::numeric::ublas::matrix<double> invert      (const boost::numeric::ublas::matrix<double> matrix) = 0;

    //!  Операция умножения
    virtual boost::numeric::ublas::matrix<double> multiply    (const boost::numeric::ublas::matrix<double> a,
                                                               const boost::numeric::ublas::matrix<double> b) = 0;

};

#endif // ABSTRACTMATRIXFACADE_H
