#pragma once
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

#include "cacheline.h"
#include "config.h"

#define LOG2(X)                                                                \
    ((unsigned)(8 * sizeof(unsigned long long) - __builtin_clzll((X)) - 1))

class TaggedCacheLine;
class Processor;
struct CacheSet;
class Cache;
class SnoopBus;

struct CacheSet {
    int nWay;
    std::vector<TaggedCacheLine> dir;

    int entryNum;
    int currEmpty;
    Cache *cache;
    // CacheSet() {}

    CacheSet(int nWayIn)
        : nWay(nWayIn), dir(nWayIn), entryNum(0), currEmpty(0) {}

    void setCache() {
        for (TaggedCacheLine &cl : dir) {
            cl.setCache(cache);
        }
    }
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
    SnoopBus *bus_;

  public:
    Cache(int totalSize, CacheCoherenceProto proto, int nWay, int totalSets,
          int lineSize, int addrLen)
        : totalSize_(totalSize), lineSize_(lineSize), proto_(proto),
          nWay_(nWay), totalSets_(totalSets), addrLen_(addrLen),
          sets(totalSets, nWay), nextLevelCache(nullptr), timeStamp_(0),
          bus_(nullptr) {
        totalCacheLines = totalSize / lineSize;
        linesPerWay = totalCacheLines / nWay;
        bitsForOffset = LOG2(lineSize);
        bitsForSet = LOG2(totalSets_);
        bitsForTag = addrLen - bitsForOffset - bitsForSet;
    }
    void SetCacheForAllSet() {
        for (CacheSet &cs : sets) {
            cs.cache = this;
            for (TaggedCacheLine &cl : cs.dir) {
                cl.setCache(this);
            }
        }
    }
    void setNextLevel(Cache *cache) { nextLevelCache = cache; }
    void setBus(SnoopBus *bus) { bus_ = bus; }
    void IncTime() { timeStamp_++; }
    void PrintInfo();
    void Put(uint64_t addr);
    void Read(uint64_t addr);
    bool Probe(uint64_t addr, TaggedCacheLine **cl);
    void Invalidate(uint64_t addr);

    uint64_t GetOffset(uint64_t addr) {
        uint64_t offsetMask = (1 << bitsForOffset) - 1;
        return addr & offsetMask;
    }
    uint64_t GetSet(uint64_t addr) {
        addr = addr >> bitsForOffset;
        uint64_t setMask = (1 << bitsForSet) - 1;
        return addr & setMask;
    }
    uint64_t GetTag(uint64_t addr) {
        addr = addr >> (bitsForOffset + bitsForSet);
        uint64_t tagMask = (1 << bitsForTag) - 1;
        return addr & tagMask;
    }

    SnoopBus *GetBus() { return bus_; }
};

class Processor {
  private:
    std::vector<Cache *> l1Cache_;
    std::vector<Cache *> l2Cache_;
    Cache *l3Cache_;

    SnoopBus *bus_;
    int numCore = 0;

  public:
    Processor(Cache *l1, Cache *l2, Cache *l3) : l3Cache_(l3) {
        l1Cache_.push_back(l1);
        l2Cache_.push_back(l2);
        numCore++;
    }
    void PrRdMachine(TaggedCacheLine *cl, uint64_t);
    void ProcessorRead(int processNum, uint64_t addr);

    void PrWrMachine(TaggedCacheLine *cl, uint64_t addr);
    void ProcessorWrite(int processNum, uint64_t addr);
    void setBus(SnoopBus *bus) { bus_ = bus; }

    SnoopBus *getBus() { return bus_; }
    std::vector<Cache *> *getL1Cache() { return &l1Cache_; }
    std::vector<Cache *> *getL2Cache() { return &l2Cache_; }
    Cache *getL3Cache() { return l3Cache_; }
};

class SnoopBus {
    std::vector<Processor *> processors;
    int coreNum;

  public:
    SnoopBus() : coreNum(0) {}

    void AddProcessor(Processor *processor) {
        processors.push_back(processor);
        coreNum++;
    }
    bool OtherCacheHasValidCopy(Cache *selfCache, uint64_t addr);
    void InvalidOtherCache(Cache *selfCache, uint64_t addr);
    void UpdateBlock(uint64_t addr);
};