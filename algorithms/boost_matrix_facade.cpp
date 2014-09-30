#include "boost_matrix_facade.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "algorithms/math_utilities.h"

BoostMatrixFacade::BoostMatrixFacade()
{
}

matrix<double> BoostMatrixFacade::transpose(boost::numeric::ublas::matrix<double> matrix)
{
    return boost::numeric::ublas::trans(matrix);
}

matrix<double> BoostMatrixFacade::invert(const boost::numeric::ublas::matrix<double> matrix)
{
    boost::numeric::ublas::matrix<double> inverted(matrix.size1(), matrix.size2());
    cpu_invert(matrix, inverted);

    for( int i = 0; i < inverted.size1(); i++ )
        for( int j = 0; j < inverted.size2(); j++ )
            if( isnan(inverted(i, j)) )
                inverted(i, j) = 0;

    return inverted;
}

matrix<double> BoostMatrixFacade::multiply(const boost::numeric::ublas::matrix<double> a, const boost::numeric::ublas::matrix<double> b)
{
    return boost::numeric::ublas::prod(a, b);
}
