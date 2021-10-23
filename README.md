# CacheTracer
A CacheTrace eats a sequence of memory trace and spits out cache utility.

# Compilation
## Prerequiste
### Ubuntu
```
sudo apt install build-essential cmake
```

#### Release
```
cd CacheTracer
mkdir build
cd build
cmake .. && make -j4
```

#### Debug
```
cd CacheTracer
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j4
```

# Usage
```
./CacheTracer [-c config_file] -t trace_file [-m cache_miss_output] [-e evict_stat_output]
```

# Develop
Must use clang-format to conform coding style.
```
clang-format --style=.clang-format xx.cpp
```