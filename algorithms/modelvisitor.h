#ifndef MODELVISITOR_H
#define MODELVISITOR_H

#include <boost/numeric/ublas/matrix.hpp>

//! Абстрактный посетитель, используемый для организации вывода результатов обсчета модели в различных форматах
class ModelVisitor
{
public:    
    virtual void write_est_data(boost::numeric::ublas::matrix<double> cur_state, unsigned t, void * object) = 0;
};

#endif // MODELVISITOR_H
