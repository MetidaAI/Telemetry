#ifndef TELEMETRY_PROCESSSTATE_H
#define TELEMETRY_PROCESSSTATE_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <logger.h>
#include <list>

class processState {
public:
    struct process{
        std::string name;
        double cpuPercent;
        double memPercent;
        int memoryRes;
        long pid;
        std::string state;
    };

    void static getProcessState(processState::process& process, const std::string &processName);
};


#endif //TELEMETRY_PROCESSSTATE_H
