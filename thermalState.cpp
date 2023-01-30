#include <logger.h>
#include "thermalState.h"

void thermalState::parseConfig(const std::list<std::string>& sensors, std::list<thermalState::thermalSensor> &sensorDst) {
    for (const auto &item : sensors){
        thermalState::thermalSensor t;
        std::vector<std::string> v;
        utils::tokenize(item, ":", v);
        t.name = v[0];
        t.path = v[1];
        sensorDst.emplace_back(t);
    }
}

void thermalState::getThermalState(const std::list<thermalState::thermalSensor> &sensors, std::map<std::string, double> &state) {
    state.clear();
    for (const auto &item: sensors){
        if(std::filesystem::exists(item.path)){
            std::ifstream file(item.path);
            if(file.is_open()){
                std::string line;
                getline(file, line);
                state.insert(std::make_pair(item.name, std::stoi(line)/1000));
            }else{
                logger::error("Failed to open file: "+ item.path);
            }
        }else{
            logger::error("File not exists: "+ item.path);
        }
    }
}
