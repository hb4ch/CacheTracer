#include "config.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
// getopt

int LoadConfig(int argc, char **argv, TracerConfig &tc) {
    int c;
    int cFlag = 0; // config file flag
    int tFlag = 0; // trace file flag
    std::string configFile;

    while ((c = getopt(argc, argv, "c:t:")) != -1) {
        switch (c) {
        case 'c':
            cFlag = 1;
            configFile = std::string(optarg);
            break;
        case 't':
            tFlag = 1;
            tc.traceFile = std::string(optarg);
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
        tc.l1Size = 64 * KB;
        tc.l2Size = 256 * KB;
        tc.l3Size = 2 * MB;
        tc.addrLen = 36;
        tc.lineSize = 64;
    } else {
        try {
            std::fstream fs(configFile);
            fs >> tc.nWay >> tc.l1Size >> tc.l2Size >> tc.l3Size >>
                tc.addrLen >> tc.lineSize;
        } catch (const std::exception &e) {
            std::cerr << "Error reading config File!" << std::endl;
            return 1;
        }
    }
    return 0;
}