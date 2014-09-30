#ifndef MATH_UTILITIES_H
#define MATH_UTILITIES_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <iostream>
#include <string>
#include <boost/date_time.hpp>
#include "cholesky.h"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::numeric::ublas;
using namespace std;

template<class M>
void cholesky_invert (M &m)
{
  typedef typename M::size_type size_type;
  typedef typename M::value_type value_type;

  size_type size = m.size1();

  // determine the inverse of the lower traingular matrix
  for (size_type i = 0; i < size; ++ i) {
    m(i,i) = 1 / m(i,i);

    for (size_type j = i+1; j < size; ++ j) {
      value_type elem(0);

      for (size_type k = i; k < j; ++ k) {
        elem -= m(j,k)*m(k,i);
      }
      m(j,i) = elem / m(j,j);
    }
  }

  // multiply the upper and lower inverses together
  m = prod(trans(triangular_adaptor<M,lower>(m)), triangular_adaptor<M,lower>(m));
}

/* Matrix inversion routine. Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
template<class T> bool cpu_invert(const matrix<T>& input, matrix<T>& inverse)
{
    using namespace boost::numeric::ublas;
    typedef permutation_matrix<std::size_t> pmatrix;

    matrix<T> A(input);

    cholesky_decompose(A);
    cholesky_invert(A);
    inverse = A;
    return true;
}

inline double linear_interpolation(double x0, double x1, double y0, double y1, double x)
{
   return y0 + (x-x0)*(y1-y0)/(x1-x0);
}

inline string format_matrix(const matrix<double> input)
{
    stringstream stream;
    for( unsigned i = 0; i < input.size1(); i++)
    {
        for(unsigned j =0; j <input.size2(); j++)
        {
            double val = input(i, j);
            if( val != 0.0)
                stream << val << "\t";
            else
                stream << val << "\t";
        }

        stream << endl;
    }

    return stream.str();
}

inline double lagrange_interpolation (double pos[], double val[], int degree, double desiredPos)
{
   double retVal = 0;
   degree--;


   for (int i = 0; i <= degree; ++i) {
      double weight = 1.0;

      for (int j = 0; j <= degree; ++j) {
         // The i-th term has to be skipped
         if (j != i) {
            weight *= double(desiredPos - pos[j]) / double(pos[i] - pos[j]);            
        }
      }
      retVal += weight * val[i];
   }

   return retVal;
}

#endif // MATH_UTILITIES_H
