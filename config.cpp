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

    while ((c = getopt(argc, argv, "c:t:m:e:")) != -1) {
        switch (c) {
        case 'c':
            cFlag = 1;
            configFile = std::string(optarg);
            break;
        case 't':
            tFlag = 1;
            tc.traceFile = std::string(optarg);
            break;
        case 'm':
            tc.missFile = std::string(optarg);
            break;
        case 'e':
            tc.evictFile = std::string(optarg);
            break;
        case '?':
            if (optopt == 'c') {
                std::cerr << "-c option requires config file!" << std::endl;
            } else if (optopt == 't') {
                std::cerr << "-t option requires trace file!" << std::endl;
            } else {
                std::cerr << "Unknown option!" << std::endl;
            }
            return -1;
        default:
            abort();
        }
    }
    if (!tFlag) {
        std::cerr << "No trace file provided!" << std::endl;
        return -1;
    }

    if (!cFlag) {
        tc.nCore = 2;
        tc.nWay = 8;
        tc.l1Size = 64 * KB;
        tc.l2Size = 256 * KB;
        tc.l3Size = 2 * MB;
        tc.addrLen = 36;
        tc.lineSize = 64;
    } else {
        try {
            std::fstream fs(configFile);
            fs >> tc.nCore >> tc.nWay >> tc.l1Size >> tc.l2Size >> tc.l3Size >>
                tc.addrLen >> tc.lineSize;
        } catch (const std::exception &e) {
            std::cerr << "Error reading config File!" << std::endl;
            return -1;
        }
        if (tc.nCore < 0 || tc.nCore > 31) {
            std::cerr << "Wrong number of cores!" << std::endl;
            return -1;
        }
        if (tc.nWay < 2 || tc.nWay > 32) {
            std::cerr << "Wrong number of n-Way!" << std::endl;
            return -1;
        }
        if (tc.l1Size > 16 * MB) {
            std::cerr << "L1 Cache size is too large!" << std::endl;
            return -1;
        }
        if (tc.l2Size > 32 * MB) {
            std::cerr << "L2 Cache size is too large!" << std::endl;
            return -1;
        }
        if (tc.l3Size > 128 * MB) {
            std::cerr << "L3 Cache size is too large!" << std::endl;
            return -1;
        }
        if (tc.addrLen > 64) {
            std::cerr << "Address bus cannot exceed 64-bit!" << std::endl;
            return -1;
        }
    }
    return 0;
}