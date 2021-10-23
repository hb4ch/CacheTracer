#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
// std

// other libs
#include "cache.h"
#include "cacheline.h"
#include "config.h"

// our header
using std::cout;
using std::endl;

int main(int argc, char **argv) {
    TracerConfig tc;
    if (LoadConfig(argc, argv, tc)) {
        return -1;
    }
    // cout << tc.l1Size << " " << tc.l2Size << " " << tc.l3Size << " " << endl;

    std::vector<std::shared_ptr<Cache>> l1Caches(tc.nCore);
    std::vector<std::shared_ptr<Cache>> l2Caches(tc.nCore);

    size_t l1TotalSets = tc.l1Size / tc.lineSize / tc.nWay;
    size_t l2TotalSets = tc.l2Size / tc.lineSize / tc.nWay;
    size_t l3TotalSets = tc.l3Size / tc.lineSize / tc.nWay;

    for (int i = 0; i < tc.nCore; i++) {
        l1Caches[i] = std::make_shared<Cache>(tc.l1Size, tc.nWay, l1TotalSets,
                                              tc.lineSize, tc.addrLen, "l1");
        l2Caches[i] = std::make_shared<Cache>(tc.l2Size, tc.nWay, l2TotalSets,
                                              tc.lineSize, tc.addrLen, "l2");
        l1Caches[i]->SetCacheForAllSet();
        l1Caches[i]->SetCacheForAllSet();
    }
    // TODO: pmr
    Cache l3Cache(tc.l3Size, tc.nWay, l3TotalSets, tc.lineSize, tc.addrLen,
                  "l3");
    l3Cache.SetCacheForAllSet();
    for (int i = 0; i < tc.nCore; i++) {
        l1Caches[i]->setNextLevel(l2Caches[i].get());
        l2Caches[i]->setNextLevel(&l3Cache);
    }

    Processor processor;
    for (int i = 0; i < tc.nCore; i++) {
        processor.AddL1L2CacheForCore(l1Caches[i].get(), l2Caches[i].get());
    }
    processor.SetL3Cache(&l3Cache);

    SnoopBus bus;
    bus.AddProcessor(&processor);
    processor.setBus(&bus);

    for (int i = 0; i < tc.nCore; i++) {
        l1Caches[i]->setBus(&bus);
        l1Caches[i]->setProcessor(&processor);
        l2Caches[i]->setBus(&bus);
        l2Caches[i]->setProcessor(&processor);
    }

    l3Cache.setBus(&bus);
    l3Cache.setProcessor(&processor);

    // Now we are set...
    // cout << "L1 info: \n";
    // l1Caches[0]->PrintInfo();
    // cout << "---------------------------------\n";
    // cout << "L2 info: \n";
    // l2Caches[0]->PrintInfo();
    // cout << "---------------------------------\n";
    // cout << "L3 info: \n";
    // l3Cache.PrintInfo();
    std::ofstream missFileStream;
    std::ofstream evictFileStream;
    try {
        if (!tc.missFile.empty()) {
            missFileStream.open(tc.missFile, std::ios::out | std::ios::trunc);
        }
        if (!tc.evictFile.empty()) {
            evictFileStream.open(tc.evictFile, std::ios::out | std::ios::trunc);
        }
        std::fstream traceStream(tc.traceFile);
        while (traceStream) {
            uint32_t threadNum;
            uint64_t addr;
            char op;
            size_t opSize;
            traceStream >> std::dec >> threadNum >> op >> std::hex >> addr >>
                std::dec >> opSize;
            if (opSize > 128) {
                std::cerr << "Invalid op size!" << std::endl;
                return -1;
            }
            if (op == 'w') {
                for (size_t i = 0; i < opSize; i++) {
                    processor.ProcessorWrite(threadNum % tc.nCore, addr + i,
                                             evictFileStream);
                }
                processor.OutputCacheMissOneLine(missFileStream);
            } else if (op == 'r') {
                for (size_t i = 0; i < opSize; i++) {
                    processor.ProcessorRead(threadNum % tc.nCore, addr + i,
                                            evictFileStream);
                }
                processor.OutputCacheMissOneLine(missFileStream);
            } else {
                std::cerr << "Invalid operation type!" << std::endl;
                return -1;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error reading trace file, data exception." << std::endl;
        return -1;
    }
    missFileStream.flush();
    fflush(stdout);

    return 0;
}