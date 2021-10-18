#include "config.h"

#include <fstream>
#include <iostream>
#include <cctype>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <unistd.h>
int LoadConfig(int argc, char **argv, TracerConfig &tc) {
    int c;
    int cFlag = 0;  // config file flag
    int tFlag = 0;  // trace file flag
    std::string configFile;
    std::string traceFile;
    while ((c = getopt(argc, argv, "c:t:")) != -1) {
        switch (c) {
            case 'c':
                cFlag = 1;
                configFile = std::string(optarg);
                break;
            case 't':
                tFlag = 1;
                traceFile = std::string(optarg);
                break;
            case '?':
                if (optopt == 'c') {
                    std::cerr << "-c option requires config file!" << std::endl;
                } else if (optopt == 't') {
                    std::cerr << "-t option requires trace file!" << std::endl;
                } else {
                    std::cerr << "Unknown option!" << std::endl;
                }
                return 1;
            default:
                abort();
        }
    }
    if (!tFlag) {
        std::cerr << "No trace file provided!" << std::endl;
        return 1;
    }

    if (!cFlag) {
        tc.nWay = 8;
        tc.levelOneSize = 64 * KB;
        tc.levelTwoSize = 256 * KB;
        tc.levelThreeSize = 2 * MB;
    } else {
        try {
            std::fstream fs(configFile);
            fs >> tc.nWay >> tc.levelOneSize >> tc.levelTwoSize >>
                tc.levelThreeSize;
        } catch (const std::exception & e) {
            std::cerr << "Error reading config File!" << std::endl;
            return 1;
        }
    }
    return 0;
}