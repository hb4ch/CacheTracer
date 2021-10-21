#include "cache.h"

void SnoopBus::InvalidOtherCache(Cache *selfCache, uint64_t addr) {
    for (Processor *p : processors) {
        for (int i = 0; p->getL1Cache()->size(); i++) {
            Cache *c = p->getL1Cache()->at(i);
            if (c != selfCache) {
                c->Invalidate(addr);
            }
        }
        for (int i = 0; p->getL2Cache()->size(); i++) {
            Cache *c = p->getL2Cache()->at(i);
            if (c != selfCache) {
                c->Invalidate(addr);
            }
        }
        if (p->getL3Cache() != selfCache) {
            p->getL3Cache()->Invalidate(addr);
        }
    }
}

bool SnoopBus::OtherCacheHasValidCopy(Cache *selfCache, uint64_t addr) {
    TaggedCacheLine *cl = nullptr;
    // no need
    Cache *c;
    for (Processor *p : processors) {
        for (int i = 0; p->getL1Cache()->size(); i++) {
            c = p->getL1Cache()->at(i);
            if (c != selfCache && c->Probe(addr, &cl)) {
                return true;
            }
        }
        for (int i = 0; p->getL2Cache()->size(); i++) {
            c = p->getL2Cache()->at(i);
            if (c != selfCache && c->Probe(addr, &cl)) {
                return true;
            }
        }
        c = p->getL3Cache();
        if (c != selfCache && c->Probe(addr, &cl)) {
            return true;
        }
    }
    return false;
}