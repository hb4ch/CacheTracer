cmake_minimum_required(VERSION 3.1)
project(CacheTracer VERSION 1.0 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# include_directories(test)

add_executable(CacheTracer main.cpp cache.cpp config.cpp bus.cpp)

enable_testing()
add_executable(
    mainTest
    test/mainTest.cpp
    config.cpp
)
target_link_libraries(
    mainTest
    gtest_main
)
include(GoogleTest)
gtest_discover_tests(mainTest)