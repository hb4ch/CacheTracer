#pragma once
#include <cstdlib>
#include <string>

constexpr size_t KB = 1024;
constexpr size_t MB = 1024 * KB;
constexpr size_t GB = 1024 * MB;

enum class CacheCoherenceProto { MESI, MOSEI, MESIF };

struct TracerConfig {
    int nWay;
    size_t l1Size;
    size_t l2Size;
    size_t l3Size;
    size_t addrLen;
    int lineSize;
    // Input

    CacheCoherenceProto cacheCoherenceProto;
    std::string traceFile;
    // TODO: 继续完善
};
// TracerConfig POD

int LoadConfig(int argc, char **argv, TracerConfig &tc);
