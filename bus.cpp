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

void SnoopBus::BusRd(Cache *selfCache, uint64_t addr, BusRdResp &resp) {
    TaggedCacheLine *ocl = nullptr;
    size_t nCore = selfCache->GetProcessor()->getL1Cache()->size();
    size_t selfCoreNum = 0;
    bool l3 = false;
    while (selfCoreNum++) {
        if (selfCache->GetProcessor()->getL1Cache()->at(selfCoreNum) ==
            selfCache) {
            break;
        }
        if (selfCache->GetProcessor()->getL2Cache()->at(selfCoreNum) ==
            selfCache) {
            break;
        }
        if (selfCache->GetProcessor()->getL3Cache() == selfCache) {
            l3 = true;
            break;
        }
    }
    if (l3) {
        resp = BusRdResp::HAS_NO_VALID_OTHER_COPY;
        return;
    }
    assert(selfCoreNum < nCore);
    resp = BusRdResp::HAS_NO_VALID_OTHER_COPY;
    // if broatcastee is l3, then no need to broadcast to other cores.
    for (size_t i = 0; i < processors.size(); i++) {
        for (size_t j = 0; j < processors[i]->getL1Cache()->size(); j++) {
            if (j != selfCoreNum) {
                bool otherOneCorehasValidCopy =
                    processors[i]->getL1Cache()->at(j)->ProbeNoStat(addr, &ocl);
                if (otherOneCorehasValidCopy) {
                    ocl->setTag({CacheLineState::SHARED, ocl->getTag().addr});
                    resp = BusRdResp::HAS_VALID_OTHER_COPY;
                }
                otherOneCorehasValidCopy =
                    processors[i]->getL2Cache()->at(j)->ProbeNoStat(addr, &ocl);
                if (otherOneCorehasValidCopy) {
                    ocl->setTag({CacheLineState::SHARED, ocl->getTag().addr});
                    resp = BusRdResp::HAS_VALID_OTHER_COPY;
                }
            }
        }
    }
}

void SnoopBus::BusRdX(Cache *selfCache, uint64_t addr) {
    InvalidOtherCache(selfCache, addr);
}
