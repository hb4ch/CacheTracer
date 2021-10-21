#pragma once
#include "cache.h"

#include <cstdlib>

enum class CacheLineState { MODIFIED, EXCLUSIVE, SHARED, INVALID };

class Cache;

struct Tag {
    CacheLineState state; // MESI state transitions
    uint64_t tag;         // higher part of address, used to do compare amongst
                          // cachelines of a cache set.
};

class TaggedCacheLine {
  private:
    size_t usedSize_;
    uint32_t birthTime_;
    uint32_t lastUseTime_;
    size_t lineSize_;
    Tag tag_;
    Cache *cache_;

  public:
    TaggedCacheLine()
        : usedSize_(0), birthTime_(0), lastUseTime_(0), lineSize_(64),
          tag_({CacheLineState::INVALID, 0}) {}
    TaggedCacheLine(size_t usedSize, uint32_t birthTime, size_t lineSize,
                    Tag tag)
        : usedSize_(usedSize), birthTime_(birthTime), lastUseTime_(birthTime),
          lineSize_(lineSize), tag_(tag) {}

    void setBirthTime(uint32_t birthTime) { birthTime_ = birthTime; }
    void setLastUseTime(uint32_t lastUseTime) { lastUseTime_ = lastUseTime; }
    void setTag(Tag tag) { tag_ = tag; }
    void setUsedSize(size_t usedSize) { usedSize_ = usedSize; }
    void setCache(Cache *cache) { cache_ = cache; }

    Tag &getTag() { return tag_; }
    uint32_t getBirthTime() { return birthTime_; }
    uint32_t getLastUseTime() { return lastUseTime_; }
    uint32_t getLineSize() { return lineSize_; }
    Cache *getCache() { return cache_; }
};