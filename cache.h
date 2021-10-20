#pragma once
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

#include "cacheline.h"
#include "config.h"
#include "bus.h"

#define LOG2(X)                                                                \
    ((unsigned)(8 * sizeof(unsigned long long) - __builtin_clzll((X)) - 1))

struct CacheSet {
    int nWay;
    std::vector<TaggedCacheLine> dir;

    int entryNum;
    int currEmpty;
    // CacheSet() {}

    CacheSet(int nWayIn) : nWay(nWayIn), dir(nWay), entryNum(0), currEmpty(0) {}
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

    size_t timeStamp_;
    SnoopBus* bus_;
  public:
    Cache(int totalSize, CacheCoherenceProto proto, int nWay, int totalSets,
          int lineSize, int addrLen)
        : totalSize_(totalSize), lineSize_(lineSize), proto_(proto),
          nWay_(nWay), totalSets_(totalSets), addrLen_(addrLen),
          sets(totalSets, nWay), nextLevelCache(nullptr), timeStamp_(0), bus_(nullptr) {
        totalCacheLines = totalSize / lineSize;
        linesPerWay = totalCacheLines / nWay;
        bitsForOffset = LOG2(lineSize);
        bitsForSet = LOG2(totalSets_);
        bitsForTag = addrLen - bitsForOffset - bitsForSet;
    }
    void setNextLevel(Cache *cache) { nextLevelCache = cache; }
    void setBus(SnoopBus *bus) { bus_ = bus; }
    void IncTime() { timeStamp_++; }
    void PrintInfo();
    void Put(uint64_t addr);
    void Read(uint64_t addr);
    bool Probe(uint64_t addr);
    void Invalidate(uint64_t addr);
};

class Processor {
  private:
    std::vector<Cache* > l1Cache_;
    std::vector<Cache* > l2Cache_;
    Cache *l3Cache_;

    SnoopBus *bus_;
  public:
    Processor(Cache *l1, Cache *l2, Cache *l3) : l3Cache_(l3) {
      l1Cache_.push_back(l1);
      l2Cache_.push_back(l2);
    }

    void ProcessorRead(uint64_t addr);
    void ProcessorWrite(uint64_t addr);
    void setBus(SnoopBus *bus) {
      bus_ = bus;
    }
    SnoopBus *getBus() {
      return bus_;
    }
};