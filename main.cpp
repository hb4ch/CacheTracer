#include <iostream>
#include <map>
// std

// other libs
#include "config.h"

// our header
using std::cout;
using std::endl;

int main(int argc, char **argv) {
    TracerConfig tc;
    if (LoadConfig(argc, argv, tc)) {
        std::cerr << "Error loading cache config!" << std::endl;
        return -1;
    }
    cout << tc.levelOneSize << " " << tc.levelTwoSize << " " 
         << tc.levelThreeSize << " " << std::endl;
    return 0;
}