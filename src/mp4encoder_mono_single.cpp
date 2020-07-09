#include <iostream>
#include <filesystem> // checked on Ubuntu
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

#include <cvut/RawVideo.h>
using namespace cvut;

#ifdef _OPENMP
#include "omp.h"
#endif

void help(){
    printf("Invalid arguments. \n");
    printf("\t Usage: ./mp4encoder_mono_single [/path_to_the_raw] [frameWidth] [frameHeight] [(Option:) encodingFrameRate (default=10)] [(Option:) frameSkip (default=10)] \n");
    printf("\t Example: ./mp4encoder_mono_single /path_to_the_raw 800 600 \n");
    printf("\t Example: ./mp4encoder_mono_single /path_to_the_raw 800 600 10 \n");
    printf("\t Example: ./mp4encoder_mono_single /path_to_the_raw 800 600 15 15\n");
}

int main(int argc, char **argv) {

    int frameSkip = 10;
    int frameRate = 10;

    if (argc == 6){
        frameSkip = std::atoi(argv[5]);
        frameRate = std::atoi(argv[4]);
    } else if (argc == 5){
        frameRate = std::atoi(argv[4]);
    } else if (argc == 4){
        // do nothing
    } else {
        help();
        return -1;
    }

    std::string rawFile(argv[1]);
    int frameWidth = std::atoi(argv[2]);
    int frameHeight = std::atoi(argv[3]);

    printf("Run mp4encoder_mono_single under %s ...\n", rawFile.c_str());
    printf("[Parameters] frame width: %d, frame height: %d, encoding frame rate: %d, frame skip: %d \n", frameWidth, frameHeight, frameRate, frameSkip);
    printf("Encoding... %s\n", rawFile.c_str());

    RawVideo vm(rawFile, frameWidth, frameHeight, 0.0, 1.0, frameSkip);
    vm.loadVideoFromFile();
    vm.encodeToMP4(frameRate);
    printf("Encoding... %s\n", rawFile.c_str());

    return 0;
}
