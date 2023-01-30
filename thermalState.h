#ifndef METIDATELEMETRY_THERMALSTATE_H
#define METIDATELEMETRY_THERMALSTATE_H


#include <string>
#include <list>
#include <map>
#include <vector>
#include <utils.h>
#include <filesystem>
#include <fstream>

class thermalState {
public:
    struct thermalSensor{
        std::string name;
        std::string path;
    };

    static void parseConfig(const std::list<std::string> &sensors, std::list<thermalState::thermalSensor> &sensorDst);
    static void getThermalState(const std::list<thermalState::thermalSensor> &sensors, std::map<std::string, double> &state);
};


#endif //METIDATELEMETRY_THERMALSTATE_H
