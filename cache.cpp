#include "cache.h"

#include <iomanip>
#include <iostream>

void Cache::printInfo() {
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

void Cache::put(TaggedCacheLine line) {}

void Cache::read(uint64_t addr) {}

void Cache::evict(int dir, int offset) {}
