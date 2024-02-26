#include <utils.h>
#include "processState.h"

void processState::getProcessState(processState::process& process, const std::string& processName) {
    std::string cmd = "top -n 1 -b | grep " + processName;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        logger::error("popen() failed!");
        return;
    }

    char buffer[128];
    bzero(buffer, 128);
    process.name = processName;
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != nullptr) {
            //парсим полученную строку
            std::vector<std::string> parsed;
            utils::tokenize(buffer,  " ", parsed);
            process.pid = std::stol(parsed[0]);
            process.memoryRes = std::stoi(parsed[5]);
            process.state = parsed[7];
            process.cpuPercent = std::stod(parsed[8]);
            process.memPercent = std::stod(parsed[9]);
        }
    }
    if(strlen(buffer) == 0){
        process.pid = -1;
        process.state = "NF";
        process.memoryRes = 0;
        process.cpuPercent = 0;
        process.memPercent = 0;
        logger::warn("Process", processName, "not found");
    }
}
