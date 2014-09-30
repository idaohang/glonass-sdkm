#ifndef NAMEDPIPE_OBSERVATION_PROVIDER_H
#define NAMEDPIPE_OBSERVATION_PROVIDER_H

#include "observation.h"
#include "observation_provider.h"
#include <string>

//! Перечисление сценариев типов принимемых прогрммой сообщений.
enum JsonMessageClass { Observations, Stations };

//! Провайдер измерений, представляющий собой веб-сервис, ожидающий JSON-данные.
class SocketObservationProvider: public ObservationProvider {
    public:
        void FindData(std::vector<int> stations, std::vector<int> satelites, ProcessingType p);

    private:
        std::vector<int> stations;
        std::vector<int> satelites;
        JsonMessageClass messageClass;

        void PollSocket();
        ObservationSample* ProcessObservations(std::string data);
        std::vector<Station> * ProcessStations(std::string data);

        ProcessingType processing;

        void ProcessMessage();

        std::string fullMessage;

        bool ContainsSatelite(int id);
        bool ContainsStation(int id);
        bool HasFixedData(ObservationSample *nextSample);
};
#endif // NAMEDPIPE_OBSERVATION_PROVIDER_H
