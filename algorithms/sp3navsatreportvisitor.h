#ifndef SP3NAVSATREPORTVISITOR_H
#define SP3NAVSATREPORTVISITOR_H

#include "data/observation.h"
#include "nav_sat.h"
#include "modelvisitor.h"
#include "data/sp3processor.h"

//!  Посетитель модели NavSat, генерирующий SP3-файлы
class SP3NavSatReportVisitor: public ModelVisitor
{
private:
    SP3Writer * sp3Writer;

public:
    void write_est_data(boost::numeric::ublas::matrix<double> cur_state, unsigned t, void * object);
    SP3NavSatReportVisitor(char * fileName);
};

#endif // SP3NAVSATREPORTVISITOR_H
