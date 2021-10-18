#include "config.h"
#include <fstream>
#include <iostream>

int LoadConfig(int argc, const char *argv[], TracerConfig &tc) {
    if (argc == 1) {
        tc.nWay = 8;
        tc.levelOneSize = 64 * KB;
        tc.levelTwoSize = 256 * KB;
        tc.levelThreeSize = 2 * MB;
        return 0;
    } else if (argc == 2) {
        std::fstream fs(argv[1]);
        fs >> tc.nWay >> tc.levelOneSize >> tc.levelTwoSize >>
            tc.levelThreeSize;
        return 0;
    }
    return 1;
}