#pragma once
#include <cstdlib>

enum class CacheLineState {
    MODIFIED, EXCLUSIVE, SHARED, INVALID
};

struct Tag {
    CacheLineState state;   // MESI state transitions
    uint32_t tag;           // higher part of address, used to do compare amongst
                            // cachelines of a cache set.
};

class TaggedCacheLine {
private:
    size_t usedSize_;
    uint32_t birthTime_;
    size_t lineSize_;
    Tag tag_;
public:
    TaggedCacheLine(size_t usedSize, uint32_t birthTime, size_t lineSize, Tag tag) 
        : usedSize_(usedSize), birthTime_(birthTime), lineSize_(lineSize),
          tag_(tag) { }
    TaggedCacheLine() : usedSize_(0), birthTime_(0), lineSize_(64), 
        tag_({CacheLineState::SHARED, 0}) {}

    void setBirthTime(uint32_t birthTime) {
        birthTime_ = birthTime;
    }
    
    void setTag(Tag tag) {
        tag_ = tag;
    }

    void setUsedSize(size_t usedSize) {
        usedSize_ = usedSize;
    }

    Tag getTag() {
        return tag_;
    }

    uint32_t getBirthTime() {
        return birthTime_;
    }

    uint32_t getLineSize() {
        return lineSize_;
    }

};