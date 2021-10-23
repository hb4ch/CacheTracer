#include "cache.h"

#include <fstream>
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
    CacheSet &hitSet = sets.at(GetSet(addr));

    // Find first empty and put
    for (TaggedCacheLine &cl : hitSet.dir) {
        if (cl.getTag().state == CacheLineState::INVALID) {
            cl.setTag({CacheLineState::EXCLUSIVE, addr});
            cl.setBirthTime(GetProcessor()->getTimeStamp());
            cl.setLastUseTime(GetProcessor()->getTimeStamp());
            return;
        }
    }
    // There is no empty slots, now we need to evict least recently used.
    int victimEntry = 0;
    uint32_t leastUsedTime = UINT32_MAX;
    for (size_t i = 0; i < hitSet.dir.size(); i++) {
        TaggedCacheLine &cl = hitSet.dir[i];
        if (cl.getLastUseTime() < leastUsedTime) {
            victimEntry = i;
            leastUsedTime = cl.getLastUseTime();
        }
    }
    hitSet.dir[victimEntry].setTag({CacheLineState::EXCLUSIVE, addr});
    if (nextLevelCache) {
        nextLevelCache->Put(addr);
    }
}

void Cache::Read(uint64_t addr) {
    CacheSet &hitSet = sets.at(GetSet(addr));

    // Find first invalid and put
    for (TaggedCacheLine &cl : hitSet.dir) {
        if (cl.getTag().state == CacheLineState::INVALID) {
            cl.setTag({CacheLineState::EXCLUSIVE, addr});
            cl.setBirthTime(GetProcessor()->getTimeStamp());
            cl.setLastUseTime(GetProcessor()->getTimeStamp());
            return;
        }
    }
    // none empty, evict
    int victimEntry = 0;
    uint32_t leastUsedTime = UINT32_MAX;
    for (int i = 0; i < hitSet.entryNum; i++) {
        TaggedCacheLine &cl = hitSet.dir[i];
        if (cl.getLastUseTime() < leastUsedTime) {
            victimEntry = i;
            leastUsedTime = cl.getLastUseTime();
        }
    }
    TaggedCacheLine &cl = hitSet.dir[victimEntry];
    uint64_t victimAddr = cl.getTag().addr;
    cl.setTag({CacheLineState::EXCLUSIVE, addr});
    cl.setBirthTime(GetProcessor()->getTimeStamp());
    cl.setLastUseTime(GetProcessor()->getTimeStamp());
    if (nextLevelCache) {
        nextLevelCache->Read(victimAddr);
    }
}

bool Cache::Probe(uint64_t addr, TaggedCacheLine **cl) {
    uint64_t tag = GetTag(addr);
    CacheSet &hitSet = sets.at(GetSet(addr));

    for (TaggedCacheLine &tcl : hitSet.dir) {
        if (tcl.getTag().state != CacheLineState::INVALID &&
            GetTag(tcl.getTag().addr) == tag) {
            tcl.setLastUseTime(GetProcessor()->getTimeStamp());
            *cl = &tcl;
            return true;
        }
    }
    *cl = nullptr;
    return false;
}

void Cache::Invalidate(uint64_t addr) {
    uint64_t tag = GetTag(addr);
    CacheSet &hitSet = sets[GetSet(addr)];

    for (TaggedCacheLine &cl : hitSet.dir) {
        if (GetTag(cl.getTag().addr) == tag) {
            cl.setTag({CacheLineState::INVALID, addr});
            return;
        }
    }
}

void Processor::PrRdMachine(TaggedCacheLine *cl, uint64_t addr) {
    if (cl->getTag().state == CacheLineState::INVALID) {
        if (!bus_->OtherCacheHasValidCopy(cl->getCache(), addr)) {
            cl->getTag().state = CacheLineState::SHARED;
        } else {
            cl->getTag().state = CacheLineState::EXCLUSIVE;
        }
    } else if (cl->getTag().state == CacheLineState::EXCLUSIVE) {
        // do nothing
    } else if (cl->getTag().state == CacheLineState::SHARED) {
        // do nothing
    } else if (cl->getTag().state == CacheLineState::MODIFIED) {
        // do nothing
    }
}

void Processor::ProcessorRead(int processNum, uint64_t addr) {
    TaggedCacheLine *cl = nullptr;
    if (l1Cache_[processNum]->Probe(addr, &cl)) {
        PrRdMachine(cl, addr);
    } else if (l2Cache_[processNum]->Probe(addr, &cl)) {
        l1Cache_[processNum]->IncMiss();
        PrRdMachine(cl, addr);
    } else if (l3Cache_->Probe(addr, &cl)) {
        l1Cache_[processNum]->IncMiss();
        l2Cache_[processNum]->IncMiss();
        PrRdMachine(cl, addr);
    } else {
        l1Cache_[processNum]->IncMiss();
        l2Cache_[processNum]->IncMiss();
        l3Cache_->IncMiss();

        l1Cache_[processNum]->Read(addr);
    }
    timeStamp_++;
}

void Processor::PrWrMachine(TaggedCacheLine *cl, uint64_t addr) {
    if (cl->getTag().state == CacheLineState::INVALID) {
        cl->getTag().state = CacheLineState::MODIFIED;
        bus_->InvalidOtherCache(cl->getCache(), addr);
        // tell bus to invalidate all other cache;
    } else if (cl->getTag().state == CacheLineState::EXCLUSIVE) {
        cl->getTag().state = CacheLineState::MODIFIED;
    } else if (cl->getTag().state == CacheLineState::SHARED) {
        cl->getTag().state = CacheLineState::MODIFIED;
        bus_->InvalidOtherCache(cl->getCache(), addr);
    } else if (cl->getTag().state == CacheLineState::MODIFIED) {
        // do nothing
    }
}

void Processor::ProcessorWrite(int processNum, uint64_t addr) {
    TaggedCacheLine *cl = nullptr;
    if (l1Cache_[processNum]->Probe(addr, &cl)) {
        PrWrMachine(cl, addr);
    } else if (l2Cache_[processNum]->Probe(addr, &cl)) {
        l1Cache_[processNum]->IncMiss();
        PrWrMachine(cl, addr);
    } else if (l3Cache_->Probe(addr, &cl)) {
        l1Cache_[processNum]->IncMiss();
        l2Cache_[processNum]->IncMiss();
        PrWrMachine(cl, addr);
    } else {
        l1Cache_[processNum]->IncMiss();
        l2Cache_[processNum]->IncMiss();
        l3Cache_->IncMiss();

        l1Cache_[processNum]->Put(addr);
    }
    timeStamp_++;
}

void Processor::OutputCacheMissOneLine(std::ofstream &fs) {
    uint64_t l1Miss = 0, l2Miss = 0, l3Miss = 0;
    for (size_t i = 0; i < l1Cache_.size(); i++) {
        l1Miss += l1Cache_[i]->GetMiss();
        l2Miss += l2Cache_[i]->GetMiss();
    }
    l3Miss += l3Cache_->GetMiss();
    fs << l1Miss << " " << l2Miss << " " << l3Miss << "\n";
}