#ifndef METIDATELEMETRY_MEMORYSTATE_H
#define METIDATELEMETRY_MEMORYSTATE_H


#include <string>
#include <fstream>

class memoryState {
public:
    struct memState{
        unsigned long physicTotal;
        unsigned long physicFree;
        unsigned long physicAvailable;
        unsigned long swapTotal;
        unsigned long swapAvailable;
    };

    static void getMemState(memState &state);
};


#endif //METIDATELEMETRY_MEMORYSTATE_H
