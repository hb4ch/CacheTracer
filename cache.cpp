#include "cache.h"

#include <iomanip>
#include <iostream>

void Cache::PrintInfo() {
    std::cout << "Cache Info: \n"
              << std::setw(20) << std::left
              << "Total Cache lines: " << totalCacheLines << "\n"
              << std::setw(20) << std::left << "Cache TotalSize: " << totalSize_
              << "\n"
              << std::setw(20) << std::left << "Cache line size: " << lineSize_
              << "\n"
              << std::setw(20) << std::left << "n-Way: " << nWay_ << "\n"
              << std::setw(20) << std::left << "Number of sets: " << totalSets_
              << "\n"
              << std::setw(20) << std::left << "address length: " << addrLen_
              << "\n"
              << std::setw(20) << std::left
              << "bits for offset: " << bitsForOffset << "\n"
              << std::setw(20) << std::left << "bits for set: " << bitsForSet
              << "\n"
              << std::setw(20) << std::left << "bits for tag: " << bitsForTag
              << std::endl;
}

void Cache::Put(uint64_t addr) {
    uint64_t addrCopy = addr;
    uint64_t offsetMask = (1u << (bitsForOffset)) - 1;
    uint64_t offset = addr & offsetMask;
    addr = addr >> bitsForOffset;

    uint64_t setMask = (1u << (bitsForSet)) - 1;
    uint64_t set = addr & setMask;
    addr = addr >> bitsForSet;

    uint64_t tagMask = (1u << (bitsForTag)) - 1;
    uint64_t tag = addr & tagMask;

    CacheSet &hitSet = sets.at(set);

    for (int i = 0; i < hitSet.entryNum; i++) {
        TaggedCacheLine &cl = hitSet.dir[i];
        if (!cl.getIsEmpty() && cl.getTag().tag == tag) {
            cl.setTag({CacheLineState::MODIFIED, tag});
            cl.setLastUseTime(timeStamp_);
            // TODO: Snoop bus for other cores
            return;
        }
    }
    // Find first empty and put
    for (TaggedCacheLine &cl : hitSet.dir) {
        if (cl.getIsEmpty()) {
            cl.setTag({CacheLineState::EXCLUSIVE, tag});
            cl.setBirthTime(timeStamp_);
            cl.setLastUseTime(timeStamp_);
            cl.setEmpty(false);
            return;
        }
    }
    // There is no empty slots, now we need to evict least recently used.
    int victimEntry = 0;
    uint32_t leastUsedTime = UINT32_MAX;
    for (int i = 0; i < hitSet.entryNum; i++) {
        TaggedCacheLine &cl = hitSet.dir[i];
        if (!cl.getIsEmpty()) {
            if (cl.getLastUseTime() < leastUsedTime) {
                victimEntry = i;
                leastUsedTime = cl.getLastUseTime();
            }
        }
    }
    TaggedCacheLine victim = hitSet.dir[victimEntry];
    hitSet.dir[victimEntry].setTag({CacheLineState::EXCLUSIVE, tag});
    if (nextLevelCache) {
        nextLevelCache->Put(addrCopy);
    }
}

void Cache::Read(uint64_t addr) {
    uint64_t addrCopy = addr;
    uint64_t offsetMask = (1u << (bitsForOffset)) - 1;
    uint64_t offset = addr & offsetMask;
    addr = addr >> bitsForOffset;

    uint64_t setMask = (1u << (bitsForSet)) - 1;
    uint64_t set = addr & setMask;
    addr = addr >> bitsForSet;

    uint64_t tagMask = (1u << (bitsForTag)) - 1;
    uint64_t tag = addr & tagMask;

    CacheSet &hitSet = sets[set];
    for (TaggedCacheLine &cl : hitSet.dir) {
        if (cl.getTag().tag == tag) {
            cl.setLastUseTime(timeStamp_);
            return;
        }
    }
    // Find first empty and put
    for (TaggedCacheLine &cl : hitSet.dir) {
        if (cl.getIsEmpty()) {
            cl.setTag({CacheLineState::EXCLUSIVE, tag});
            cl.setBirthTime(timeStamp_);
            cl.setLastUseTime(timeStamp_);
            cl.setEmpty(false);
            return;
        }
    }
    // none empty, evict
    int victimEntry = 0;
    uint32_t leastUsedTime = UINT32_MAX;
    for (int i = 0; i < hitSet.entryNum; i++) {
        TaggedCacheLine &cl = hitSet.dir[i];
        if (!cl.getIsEmpty()) {
            if (cl.getLastUseTime() < leastUsedTime) {
                victimEntry = i;
                leastUsedTime = cl.getLastUseTime();
            }
        }
    }
    TaggedCacheLine victim = hitSet.dir[victimEntry];
    hitSet.dir[victimEntry].setTag({CacheLineState::EXCLUSIVE, tag});
    if (nextLevelCache) {
        nextLevelCache->Put(addrCopy);
    }
}

void Cache::Invalidate(uint64_t addr) {
    uint64_t addrCopy = addr;
    uint64_t offsetMask = (1u << (bitsForOffset)) - 1;
    uint64_t offset = addr & offsetMask;
    addr = addr >> bitsForOffset;

    uint64_t setMask = (1u << (bitsForSet)) - 1;
    uint64_t set = addr & setMask;
    addr = addr >> bitsForSet;

    uint64_t tagMask = (1u << (bitsForTag)) - 1;
    uint64_t tag = addr & tagMask;

    CacheSet &hitSet = sets[set];
    for (TaggedCacheLine &cl : hitSet.dir) {
        if (cl.getTag().tag == tag) {
            cl.setEmpty(true);
            cl.setTag({CacheLineState::INVALID, tag});
            return;
        }
    }
}
