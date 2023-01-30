#include <iostream>
#include <logger.h>
#include <config.h>
#include <list>
#include "cpuState.h"
#include "memoryState.h"
#include "thermalState.h"


int main() {
    logger::init("MTD-telemetry");
    logger::setThread("MainThread", std::this_thread::get_id());

    config::init("MTD-telemetry");
    config::set("debug", false);
    config::set("clientID", 1);

    std::list<std::string> sensors;

    //NAME:PATH
    sensors.emplace_back("CPU:/sys/class/hwmon/hwmon2/temp1_input");
    sensors.emplace_back("CORE0:/sys/class/hwmon/hwmon2/temp2_input");

    config::set("thermalSensors", sensors);
    config::loadConfig();

    std::list<thermalState::thermalSensor> sensorDst;
    thermalState::parseConfig(config::get<std::list<std::string>>("thermalSensors"), sensorDst);

    cpuState::cpuInfo cInfo{};
    cpuState::getCPUInfo(cInfo);
    cpuState::cpuStateInit(cInfo.cores);
    std::vector<double> cpuState;

    memoryState::memState mState{};

    std::map<std::string, double> thermalState;
    nlohmann::json json;
    while(true) {
        thermalState::getThermalState(sensorDst, thermalState);
        memoryState::getMemState(mState);
        cpuState.clear();
        cpuState::getCPUState(cInfo.cores, cpuState);

        if (cpuState.size() == cInfo.cores)
            for (int i = 0; i < cpuState.size(); i++) {
                json["cpu"]["load"][i] = cpuState[i];

                if(config::get<bool>("debug"))
                    logger::info("Core " + std::to_string(i) + ": " + std::to_string(cpuState[i]) + "%");
            }

        for (const auto &item: thermalState) {
            json["thermal"][item.first] = item.second;

            if(config::get<bool>("debug"))
                logger::info(item.first + ": " + std::to_string(item.second));
        }

        json["mem"]["ram"]["physicAvailable"] = mState.physicAvailable;
        json["mem"]["ram"]["physicTotal"] = mState.physicTotal;
        json["mem"]["ram"]["swapTotal"] = mState.swapTotal;
        json["mem"]["ram"]["swapAvailable"] = mState.swapAvailable;
        json["timestamp"] = std::time(nullptr);

        if(config::get<bool>("debug")) {
            logger::info(
                    "Physical mem: " + std::to_string((mState.physicTotal - mState.physicAvailable) / 1024) + "Mb" +
                    "/" + std::to_string(mState.physicTotal / 1024) + "Mb");
            logger::info(
                    "Virtual mem: " + std::to_string((mState.swapTotal - mState.swapAvailable) / 1024) + "Mb" + "/" +
                    std::to_string(mState.swapTotal / 1024) + "Mb");
        }

        utils::writeSQLToFile("INSERT INTO telemetry SET clientID="+std::to_string(config::get<int>("clientID"))+", data='"+nlohmann::to_string(json)+"'");
        sleep(5);
    }
}


