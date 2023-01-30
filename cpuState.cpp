#include "cpuState.h"

std::vector<unsigned long long> cpuState::lastTotalUser, cpuState::lastTotalUserLow, cpuState::lastTotalSys, cpuState::lastTotalIdle;

void cpuState::cpuStateInit(int cores){
    std::ifstream file("/proc/stat");

    std::string line;
    std::vector<std::string> vecToken;

    lastTotalUser.clear();
    lastTotalUserLow.clear();
    lastTotalSys.clear();
    lastTotalIdle.clear();

    while(getline(file, line)){
        vecToken.clear();
        utils::tokenize(line, " ", vecToken);
        for(int i = 0; i < cores; i++) {
            if (vecToken[0] == "cpu"+std::to_string(i)){
                lastTotalUser.emplace_back(std::stoll(vecToken[1]));
                lastTotalUserLow.emplace_back(std::stoll(vecToken[2]));
                lastTotalSys.emplace_back(std::stoll(vecToken[3]));
                lastTotalIdle.emplace_back(std::stoll(vecToken[4]));
            }
        }
    }
}

void cpuState::getCPUState(int cores, std::vector<double> &percent){
    std::vector<unsigned long long> totalUser, totalUserLow, totalSys, totalIdle, total;

    std::string line;
    std::vector<std::string> vecToken;

    totalUser.clear();
    totalUserLow.clear();
    totalSys.clear();
    totalIdle.clear();
    total.clear();

    std::ifstream file("/proc/stat");
    if(file.is_open()){
        while(getline(file, line)){
            vecToken.clear();
            utils::tokenize(line, " ", vecToken);
            for(int i = 0; i < cores; i++) {
                if (vecToken[0] == "cpu"+std::to_string(i)){
                    totalUser.emplace_back(std::stoll(vecToken[1]));
                    totalUserLow.emplace_back(std::stoll(vecToken[2]));
                    totalSys.emplace_back(std::stoll(vecToken[3]));
                    totalIdle.emplace_back(std::stoll(vecToken[4]));
                }
            }
        }
    }
    file.close();

    percent.clear();
    for(int i = 0; i < cores; i++){
        if (totalUser[i] < lastTotalUser[i] || totalUserLow[i] < lastTotalUserLow[i] ||
            totalSys[i] < lastTotalSys[i] || totalIdle[i] < lastTotalIdle[i]) {
            percent.emplace_back(-1.0);
        } else {
            total.emplace_back((totalUser[i] - lastTotalUser[i]) + (totalUserLow[i] - lastTotalUserLow[i]) + (totalSys[i] - lastTotalSys[i]));
            percent.emplace_back(total[i]);
            total[i] += (totalIdle[i] - lastTotalIdle[i]);
            if(total[i] > 0) {
                percent[i] /= total[i];
                percent[i] *= 100;
            }else
                percent[i] = -1;
        }

        lastTotalUser[i] = totalUser[i];
        lastTotalUserLow[i] = totalUserLow[i];
        lastTotalSys[i] = totalSys[i];
        lastTotalIdle[i] = totalIdle[i];
    }
}

void cpuState::getCPUInfo(cpuInfo &info){
    std::string token, line;
    std::ifstream file("/proc/cpuinfo");

    std::vector<std::string> vecTokens;
    bool read = false;
    while (getline(file, line)){
        token = line;
        vecTokens.clear();
        utils::tokenize(token, ":", vecTokens);
        if(strstr(vecTokens[0].c_str(), "model name") && !read)
            info.name = vecTokens[1];
        if(strstr(vecTokens[0].c_str(), "processor"))
            info.cores = std::stoi(vecTokens[1])+1;
        if(strstr(vecTokens[0].c_str(), "flags") && !read){
            info.flags = vecTokens[1];
            read = true;
        }
    }
    file.close();
}