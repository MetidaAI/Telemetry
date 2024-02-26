#ifndef METIDATELEMETRY_THERMALSTATE_H
#define METIDATELEMETRY_THERMALSTATE_H


#include <string>
#include <list>
#include <map>
#include <vector>
#include <utils.h>
#include <filesystem>
#include <fstream>

#define TEMP_FNAME_LENGTH 11

class thermalState {
public:
    struct thermalSensor{
        std::string name;
        std::string path;
    };

    static void getThermalState(const std::list<thermalState::thermalSensor> &sensors, std::map<std::string, double> &state);
    static void searchTempFiles(std::list<thermalState::thermalSensor> &sensorDst);
};


#endif //METIDATELEMETRY_THERMALSTATE_H
