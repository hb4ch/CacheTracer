# CacheTracer
A CacheTrace eats a sequence of memory trace and spits out cache utility.

# Compilation
## Prerequiste
### Ubuntu
```
sudo apt install build-essential
```

```
cd CacheTracer
mkdir build
cd build
cmake .. && make -j4
```

# Usage
```
./CacheTracer [-c config_file] -t trace_file
```

# Develop
Must use clang-format to conform coding style.
```
clang-format --style=.clang-format xx.cpp
```