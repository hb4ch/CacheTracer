#pragma once
#include <cstdlib>

constexpr size_t KB = 1024;
constexpr size_t MB = 1024 * KB;
constexpr size_t GB = 1024 * MB;

enum class CacheCoherenceProto { MSEI, MOSEI, MESIF };

struct TracerConfig {
    int nWay;
    size_t levelOneSize;
    size_t levelTwoSize;
    size_t levelThreeSize;

    CacheCoherenceProto cacheCoherenceProto;
    // TODO: 继续完善
};
// TracerConfig POD

int LoadConfig(int argc, const char *argv[], TracerConfig &tc);
