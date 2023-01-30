#include "memoryState.h"

void memoryState::getMemState(memState &state) {
    std::string token;
    std::ifstream file("/proc/meminfo");
    while(file >> token) {
        if(token == "MemTotal:")
            file >> state.physicTotal;
        if(token == "MemFree:")
            file >> state.physicFree;
        if(token == "MemAvailable:")
            file >> state.physicAvailable;
        if(token == "SwapTotal:")
            file >> state.swapTotal;
        if(token == "SwapFree:")
            file >> state.swapAvailable;

        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    file.close();
}