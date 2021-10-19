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
        std::cerr << "Error loading cache config!" << std::endl;
        return -1;
    }
    cout << tc.l1Size << " " << tc.l2Size << " " << tc.l3Size << " " << endl;

    size_t l1TotalSets = tc.l1Size / tc.addrLen / tc.nWay;
    size_t l2TotalSets = tc.l2Size / tc.addrLen / tc.nWay;
    size_t l3TotalSets = tc.l3Size / tc.addrLen / tc.nWay;

    Cache l1Cache(tc.l1Size, CacheCoherenceProto::MESI, tc.nWay, l1TotalSets,
                  tc.lineSize, tc.addrLen);
    Cache l2Cache(tc.l2Size, CacheCoherenceProto::MESI, tc.nWay, l2TotalSets,
                  tc.lineSize, tc.addrLen);
    Cache l3Cache(tc.l3Size, CacheCoherenceProto::MESI, tc.nWay, l2TotalSets,
                  tc.lineSize, tc.addrLen);
    return 0;
}