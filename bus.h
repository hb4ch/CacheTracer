#pragma once
#include "cache.h"

enum class BusMessage {
    BUSRD,
    BUSRDX,
    BUSUPGR,
};

class SnoopBus {
    Cache *l1Cache;
    Cache *l2Cache;
    Cache *l3Cache;

    void SendMessage(BusMessage msg);
};