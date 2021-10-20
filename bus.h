#pragma once
#include "cache.h"
#include <vector>

enum class BusMessage {
    BUSRD,
    BUSRDX,
    BUSUPGR,
};

class SnoopBus {
    std::vector<Processor *> processors;
    int coreNum;
    
public:
    SnoopBus() : coreNum(0) {

    }

    void AddProcessor(Processor *processor) {
        processors.push_back(processor);
        coreNum++;
    }
    void SendMessage(BusMessage msg, uint64_t addr);
    bool OtherCacheHasValidCopy(Cache *cache, uint64_t addr);
    
    void UpdateBlock(uint64_t addr);
};