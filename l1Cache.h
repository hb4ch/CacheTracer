#pragma once
#include <cstddef>
#include <vector>

#include "config.h"

class CacheLine {
    bool tag;
    char *cacheLineData;

public:
    CacheLine() : cacheLineData(new char[tc.levelOneSize]) {}
    static TracerConfig tc;
    void print();
    ~CacheLine() { delete[] cacheLineData; }
};
