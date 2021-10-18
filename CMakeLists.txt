cmake_minimum_required(VERSION 3.1)
project(CacheTracer VERSION 1.0 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)
add_executable(CacheTracer main.cpp l1Cache.cpp config.cpp)