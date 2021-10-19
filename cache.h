#pragma once
#include <cstddef>
#include <vector>

#include "cacheline.h"
#include "config.h"

#define LOG2(X)                                                                \
    ((unsigned)(8 * sizeof(unsigned long long) - __builtin_clzll((X)) - 1))

struct CacheSet {
    int nWay;
    std::vector<TaggedCacheLine> dir;

    CacheSet() {}

    CacheSet(int nWayIn) : nWay(nWayIn), dir(nWay) {}
};

class Cache {
  private:
    size_t totalSize_;
    CacheCoherenceProto proto_;
    int nWay_;
    int totalSets_;
    size_t lineSize_;
    int addrLen_;
    // c'tor

    std::vector<CacheSet> sets;
    Cache *nextLevelCache;
    // setter

    size_t totalCacheLines;
    size_t linesPerWay;
    size_t bitsForOffset;
    size_t bitsForSet;
    size_t bitsForTag;
    // calculated
  public:
    Cache(int totalSize, CacheCoherenceProto proto, int nWay, int totalSets,
          int lineSize, int addrLen)
        : totalSize_(totalSize), lineSize_(lineSize), proto_(proto),
          nWay_(nWay), totalSets_(totalSets), addrLen_(addrLen),
          sets(totalSets), nextLevelCache(nullptr) {
        totalCacheLines = totalSize / lineSize;
        linesPerWay = totalCacheLines / nWay;
        bitsForOffset = LOG2(lineSize);
        bitsForSet = LOG2(totalSets_);
        bitsForTag = addrLen - bitsForOffset - bitsForSet;
    }

    void put(TaggedCacheLine line);
    void read(uint64_t addr);
    void evict(int dir, int offset);
};
