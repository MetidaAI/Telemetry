#include <logger.h>
#include "thermalState.h"

std::string getCPUPath(){
    for (const auto& entry : std::filesystem::directory_iterator("/sys/class/hwmon/")) {
        std::string nameFile = entry.path().generic_string() + "/name";
        std::ifstream file(nameFile);
        if (file.is_open()) {
            std::string line;
            getline(file, line);
            if(strcmp(line.c_str(), "coretemp") == 0){
               file.close();
               return entry.path();
            }
            file.close();
        }
    }
    return "";
}

void thermalState::searchTempFiles(std::list<thermalState::thermalSensor> &sensorDst) {
    std::string cpuPath = getCPUPath();
    if(!cpuPath.empty()) {
        for (const auto &entry: std::filesystem::directory_iterator(cpuPath)) {
            if (!entry.is_directory()) {
                std::string file_name = entry.path().filename().string();
                if (file_name.find("temp") != std::string::npos && file_name.find("_input") != std::string::npos) {
                    thermalState::thermalSensor t;
                    t.path = entry.path();

                    std::string labelPath = entry.path().parent_path();
                    labelPath += "/temp";
                    for(int i = 0; i < entry.path().filename().generic_string().length() - TEMP_FNAME_LENGTH+1; i++)
                        labelPath += entry.path().filename().c_str()[4+i];
                    labelPath += "_label";

                    std::ifstream label(labelPath);
                    if(label.is_open()){
                        std::string line;
                        getline(label, line);
                        t.name = line;
                        label.close();
                    }else{
                        logger::error("Failed to open file: ", labelPath);
                    }
                    sensorDst.emplace_back(t);
                }
            }
        }
    }else{
        logger::warn("Can't find CPU temperature path!!");
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
