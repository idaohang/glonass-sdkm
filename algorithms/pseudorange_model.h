#ifndef PSEUDORANGE_MODEL_H
#define PSEUDORANGE_MODEL_H

#include "data/observation.h"
#include "cmath"
#include "constants.h"
#include "boost/numeric/ublas/matrix.hpp"

//! Модель псевдодальности
class PseudorangeModel
{
private:
    //!  Вращение Земли
    Point Rotate( Point point, double alpha);

    //!  Релятивистский эффект
    double CalculateRelativisticEffect();

    //!  Задержка обусловленная тропосферой
    double CalculateTropoDelay(SatPosition satelite, Station station);

    //!  Расстояние между двумя точками в трехмерном пространстве
    double CalculateRange(Point p1, Point p2);

    //!  Зенитный угол между спутником и станцией
    double CalculateZenithAngle(SatPosition satelite, Station station);

public:
    PseudorangeModel();

     //!  Считает псевдодальность между станицей и спунтиком
    double CalculatePrange(Pseudorange prange, SatPosition satelite, Station station);
};

#endif // PSEUDORANGE_MODEL_H
