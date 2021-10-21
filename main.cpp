#include <iostream>
#include <map>
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
    cout << tc.l1Size << " " << tc.l2Size << " " << tc.l3Size << " " << endl;

    size_t l1TotalSets = tc.l1Size / tc.lineSize / tc.nWay;
    size_t l2TotalSets = tc.l2Size / tc.lineSize / tc.nWay;
    size_t l3TotalSets = tc.l3Size / tc.lineSize / tc.nWay;

    Cache l1Cache(tc.l1Size, CacheCoherenceProto::MESI, tc.nWay, l1TotalSets,
                  tc.lineSize, tc.addrLen);
    Cache l2Cache(tc.l2Size, CacheCoherenceProto::MESI, tc.nWay, l2TotalSets,
                  tc.lineSize, tc.addrLen);
    Cache l3Cache(tc.l3Size, CacheCoherenceProto::MESI, tc.nWay, l3TotalSets,
                  tc.lineSize, tc.addrLen);

    l1Cache.SetCacheForAllSet();
    l2Cache.SetCacheForAllSet();
    l3Cache.SetCacheForAllSet();

    l1Cache.setNextLevel(&l2Cache);
    l2Cache.setNextLevel(&l3Cache);

    Processor processor(&l1Cache, &l2Cache, &l3Cache);
    SnoopBus bus;
    bus.AddProcessor(&processor);
    processor.setBus(&bus);

    l1Cache.setBus(&bus);
    l2Cache.setBus(&bus);
    l3Cache.setBus(&bus);

    // Now we are set...
    // cout << "L1 info: \n";
    // l1Cache.PrintInfo();
    // cout << "---------------------------------\n";
    // cout << "L2 info: \n";
    // l2Cache.PrintInfo();
    // cout << "---------------------------------\n";
    // cout << "L3 info: \n";
    // l3Cache.PrintInfo();

    return 0;
}