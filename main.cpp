#include <iostream>
#include <logger.h>
#include <config.h>
#include <list>
#include <metida/core/core.h>
#include <metida/core/redis/heartBeatThread.h>

#include "cpuState.h"
#include "memoryState.h"
#include "thermalState.h"
#include "processState.h"
#include "configPaths.h"

#define WITH_DB false
#define WITH_DNN false
#define WITH_S3 false
#define WITH_REDIS true

int main() {
    logger::init("Telemetry");
    logger::setThread("MainThread", std::this_thread::get_id());

    config::init("Telemetry");
    core::init(WITH_DB, WITH_DNN, WITH_S3, WITH_REDIS);
    config::set(DEBUG, false);
    config::set(CLIENT_ID, 1);
    config::set(CPU_FULL_INFO, false);

    std::list<std::string> sensors;


    std::list<std::string> processes;
    processes.emplace_back("Detector");
    processes.emplace_back("Telemetry");
    processes.emplace_back("SQLCache");
    processes.emplace_back("DataCache");
    processes.emplace_back("ClientController");
    config::set(PROCESSES, processes);

    config::loadConfig();

    //Start monitoring
    std::thread hearthBeatT(heartBeatThread::run, "Telemetry-"+std::to_string(config::get<int>(CLIENT_ID)));

    std::list<thermalState::thermalSensor> sensorDst;
    thermalState::searchTempFiles(sensorDst);

    cpuState::cpuInfo cInfo{};
    cpuState::getCPUInfo(cInfo);
    cpuState::cpuStateInit(cInfo.cores);
    std::vector<double> cpuState;

    memoryState::memState mState{};

    std::map<std::string, double> thermalState;
    nlohmann::json json;
    processes.clear();
    processes = config::get<std::list<std::string>>(PROCESSES);


    while(true) {
        thermalState::getThermalState(sensorDst, thermalState);
        for (const auto &process: processes) {
            processState::process pState;
            processState::getProcessState(pState, process);

            if(config::get<bool>(DEBUG))
                logger::info("Process-", pState.name,
                             "pid-", pState.pid,
                             "mem-", pState.memoryRes,
                             "mem%-", pState.memPercent,
                             "cpu%-",pState.cpuPercent,
                             "state-",pState.state);

            json["processes"][pState.name]["pid"] = pState.pid;
            json["processes"][pState.name]["memUsage"] = pState.memoryRes;
            json["processes"][pState.name]["state"] = pState.state;
            json["processes"][pState.name]["cpuPercentUsage"] = pState.cpuPercent;
            json["processes"][pState.name]["memPercentUsage"] = pState.memPercent;
        }
        memoryState::getMemState(mState);
        cpuState.clear();
        cpuState::getCPUState(cInfo.cores, cpuState);

        if(config::get<bool>(CPU_FULL_INFO)) {
            json["cpu"]["name"] = cInfo.name;
            json["cpu"]["flags"] = cInfo.flags;
            if (config::get<bool>(DEBUG)) {
                logger::info("CPU", cInfo.name);
                logger::info("Flags", cInfo.flags);
            }
        }

        if (cpuState.size() == cInfo.cores)
            for (int i = 0; i < cpuState.size(); i++) {
                json["cpu"]["load"][i] = cpuState[i];

                if(config::get<bool>(DEBUG))
                    logger::info("Core " + std::to_string(i) + ": " + std::to_string(cpuState[i]) + "%");
            }

        for (const auto &item: thermalState) {
            json["thermal"][item.first] = item.second;

            if(config::get<bool>(DEBUG))
                logger::info(item.first + ": " + std::to_string(item.second));
        }

        json["mem"]["ram"]["physicAvailable"] = mState.physicAvailable;
        json["mem"]["ram"]["physicTotal"] = mState.physicTotal;
        json["mem"]["ram"]["swapTotal"] = mState.swapTotal;
        json["mem"]["ram"]["swapAvailable"] = mState.swapAvailable;
        long ts = time(nullptr);
        json["timestamp"] = ts;

        if(config::get<bool>(DEBUG)) {
            logger::info(
                    "Physical mem: " + std::to_string((mState.physicTotal - mState.physicAvailable) / 1024) + "Mb" +
                    "/" + std::to_string(mState.physicTotal / 1024) + "Mb");
            logger::info(
                    "Virtual mem: " + std::to_string((mState.swapTotal - mState.swapAvailable) / 1024) + "Mb" + "/" +
                    std::to_string(mState.swapTotal / 1024) + "Mb");
        }

        utils::writeSQLToFile("INSERT INTO telemetry SET "
            "clientID="+std::to_string(config::get<int>(CLIENT_ID))+
            ", data='"+nlohmann::to_string(json)+
            "', timestamp="+std::to_string(ts));
        sleep(5);
    }
}


