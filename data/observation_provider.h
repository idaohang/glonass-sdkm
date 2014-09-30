#ifndef OBSERVATION_PROVIDER_H
#define OBSERVATION_PROVIDER_H

#include <vector>
#include <ext/hash_map>
#include "observation.h"
#include "processing_type.h"
#include "../algorithms/nav_sat.h"

class NavSat;

//! Абстрактный класс, представляющий собой провайдер измерений. Используется в модели NavSat.
class ObservationProvider {
    public:
        virtual void FindData(std::vector<int> stations, std::vector<int> satelites, ProcessingType p) = 0;
        NavSat * model;
    };

#endif // OBSERVATION_PROVIDER_H
