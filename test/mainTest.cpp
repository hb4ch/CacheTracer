#include <gtest/gtest.h>

#include "../config.h"

// Demonstrate some basic assertions.
TEST(mainTest, LoadConfig1) {
    // Expect two strings not to be equal.
    const char *argv[3] = {"./CacheTracer", "-t", "trace.txt"};
    TracerConfig cf;
    int ret = LoadConfig(3, (char**)argv, cf);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(cf.nWay, 8);
    // Expect equality.

}