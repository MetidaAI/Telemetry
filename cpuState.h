#ifndef METIDATELEMETRY_CPUSTATE_H
#define METIDATELEMETRY_CPUSTATE_H


#include <string>
#include <fstream>
#include <vector>
#include <cstring>
#include <utils.h>


class cpuState {
private:
    static std::vector<unsigned long long> lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
public:
    struct cpuInfo{
        std::string name;
        int cores;
        std::string flags;
    };

    static void cpuStateInit(int cores);
    static void getCPUState(int cores, std::vector<double> &percent);
    static void getCPUInfo(cpuInfo &info);


};


#endif //METIDATELEMETRY_CPUSTATE_H
