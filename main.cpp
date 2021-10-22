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
        l1Caches[i] = std::make_shared<Cache>(
            tc.l1Size, CacheCoherenceProto::MESI, tc.nWay, l1TotalSets,
            tc.lineSize, tc.addrLen);
        l2Caches[i] = std::make_shared<Cache>(
            tc.l2Size, CacheCoherenceProto::MESI, tc.nWay, l2TotalSets,
            tc.lineSize, tc.addrLen);
        l1Caches[i]->SetCacheForAllSet();
        l1Caches[i]->SetCacheForAllSet();
    }
    // TODO: pmr
    Cache l3Cache(tc.l3Size, CacheCoherenceProto::MESI, tc.nWay, l3TotalSets,
                  tc.lineSize, tc.addrLen);
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
    cout << "L1 info: \n";
    l1Caches[0]->PrintInfo();
    cout << "---------------------------------\n";
    cout << "L2 info: \n";
    l2Caches[0]->PrintInfo();
    cout << "---------------------------------\n";
    cout << "L3 info: \n";
    l3Cache.PrintInfo();

    // std::fstream traceStream(tc.traceFile);

    // while (traceStream) {
    // }

    return 0;
}