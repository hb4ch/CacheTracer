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

    l1Cache.setNextLevel(&l2Cache);
    l2Cache.setNextLevel(&l3Cache);

    // cout << "L1 info: \n";
    // l1Cache.printInfo();
    // cout << "---------------------------------\n";
    // cout << "L2 info: \n";
    // l2Cache.printInfo();
    // cout << "---------------------------------\n";
    // cout << "L3 info: \n";
    // l3Cache.printInfo();

    return 0;
}