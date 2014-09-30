#ifndef SIMPLENAVSATREPORTVISITOR_H
#define SIMPLENAVSATREPORTVISITOR_H

#include <fstream>
#include <ext/hash_map>
#include <boost/numeric/ublas/matrix.hpp>

#include "data/observation.h"
#include "nav_sat.h"
#include "modelvisitor.h"

//!  Посетитель модели NavSat, генерирующий простые отчеты в текстовые файлы для каждого из спутников и станций
class SimpleNavSatReportVisitor : public ModelVisitor
{
public:
    void write_est_data(boost::numeric::ublas::matrix<double> cur_state, unsigned t, void * object);

private:
    std::ofstream stationReport;
    __gnu_cxx::hash_map<int, std::ofstream *> sateliteReport;
};

#endif // SIMPLENAVSATREPORTVISITOR_H
