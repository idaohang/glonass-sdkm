#include "pseudorange_model.h"

using namespace boost::numeric::ublas;

PseudorangeModel::PseudorangeModel()
{
}

double PseudorangeModel::CalculateRange(Point p1, Point p2)
{
    return sqrt( pow(p2.x-p1.x, 2) + pow(p2.y-p1.y, 2) + pow(p2.z - p1.z, 2) );
}

double PseudorangeModel::CalculateZenithAngle(SatPosition satelite, Station station)
{
    Point center;
    center.x = 0.0;
    center.y = 0.0;
    center.z = 0.0;

    double b = CalculateRange( satelite.coord, station.coord);
    double a = CalculateRange( center, station.coord);
    double c = CalculateRange( satelite.coord, center);

    double angle = acos( (b*b + a*a - c*c)/(2*a*b) );
    return angle;
}

Point PseudorangeModel::Rotate(Point point, double alpha)
{
    matrix<double> A(3, 3);
    A(0, 0) = cos(alpha);   A(0, 1) = sin(alpha);   A(0, 2) = 0;
    A(1, 0) = -sin(alpha);  A(1, 1) = cos(alpha);   A(1, 2) = 0;
    A(2, 0) = 0;            A(2, 1) = 0;            A(2, 2) = 1;

    matrix<double> R(3, 1);
    R(0, 0) = point.x;
    R(1 ,0) = point.y;
    R(2, 0) = point.z;

    R = prod(A, R);
    Point rotated;
    rotated.x = R(0, 0);
    rotated.y = R(1, 0);
    rotated.z = R(2, 0);

    return rotated;
}

double PseudorangeModel::CalculateTropoDelay(SatPosition satelite, Station station)
{
    /*
    double height[] =   {0,     0.5e3,  1e3,    1.5e3,  2.0e3,  2.5e3,  3.0e3,  4.0e3,  5.0e3 };
    double pressure[] = {1.156, 1.079,  1.006,  0.938,  0.874,  0.813,  0.757,  0.654,  0.563 };

    double H0 = 0;
    double P0 = 1013.25;
    double T0 = 18 + 273.16;
    double RH0 = 0.5;

    double a = 6378136;
    double al= 1/298.25784;
    double b = a*(1-al);

    double r = CalculateRange(x2, y2, z2, 0, 0, 0);
    double alpha = atan2(y2, x2);
    double beta = asin(z2/r);

    double e_x = a*cos(alpha)*cos(beta);
    double e_y = a*cos(alpha)*sin(beta);
    double e_z = b*sin(alpha);

    double h = 0; //abs(6378.1e3) - sqrt( pow(e_x, 2) + pow(e_y, 2) + pow(e_z, 2) );

    double pres = 0;
    if( h < height[8] )
        pres = lagrange_interpolation(height, pressure, 9, h);

    double P = P0*pow( 1 - 0.000226*(h-H0), 5.225);
    double T = T0 - 0.0065*(h-H0);
    double RH = RH0*exp( -0.0006396*(h-H0));

    double e = RH*exp(-37.2465 + 0.213166*T - 0.000256908*T*T);
    double zen = CalculateZenithAngle(x1, y1, z1, x2, y2, z2);

    double B = 0.003; // TODO: надо приделать таблицу

    return (0.002277/cos(zen))*(P + (1225/T)*e + B*pow(tan(zen), 2));
    */

    return 0.0;
}

double PseudorangeModel::CalculatePrange(Pseudorange prange, SatPosition satelite, Station station)
{
    // поворот
    satelite.coord = Rotate(satelite.coord, prange.l1*EARTH_ROTATION);

    // геометрическое расстояние до спутника
    double distance_to_satelite = CalculateRange(satelite.coord, station.coord);

    // релятивистский эффект

    // компенсация групповой задержки
    double tgd1 =       (prange.l1 - prange.l2)/(1-GAMMA);
    double tgd2 = GAMMA*(prange.l1 - prange.l2)/(1-GAMMA);

    // компенсция ионосферной задержки
    double iono_delay = (prange.l2 - GAMMA*prange.l1)/(1 - GAMMA);

    // компенскаия тропосферной задержки
    double tropo_delay = CalculateTropoDelay(satelite, station);

    double corrRange = prange.l1*c - distance_to_satelite - iono_delay - tropo_delay - tgd1;
    return corrRange;
}
