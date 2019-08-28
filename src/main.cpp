#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem> // checked on Ubuntu

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

#include "VideoManager.h"

#ifdef _OPENMP
#include "omp.h"
#endif

int main(int argc, char **argv) {

    /**
     * videoFileDir に入っている動画を読み込む
     */

    static int frameWidth = 800;
    static int frameHeight = 600;

    std::vector<std::string> rawFileArray;
    std::vector<std::string> parentPathFileArray;

    for(const fs::directory_entry& x : fs::recursive_directory_iterator(
            "/run/user/1000/gvfs/afp-volume:host=fortyeight.local,user=anonymous,volume=Project/Kotei/HSV_DATA/TruckData_20190822/20190808/")){
        std::string extension = x.path().extension().string();
        if(extension == ".raw"){
            parentPathFileArray.push_back(x.path().parent_path().string());
            rawFileArray.push_back(x.path().stem().string());
        }
    }

    int numFile = rawFileArray.size();

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for(int i = 0; i<numFile; i++){
        std::string fileStem = rawFileArray[i];
        std::string parentPath = parentPathFileArray[i];
        printf("Encoding %s/%s.raw in thread num: %d\n",
                parentPath.c_str(), fileStem.c_str(), omp_get_thread_num());

        VideoManager vm(frameWidth, frameHeight);
        vm.loadVideoFromFile(parentPath + "/" + fileStem+ ".raw");
        vm.encodeToAVI(parentPath + "/" + fileStem + ".avi", 10.0, 25);
    }

    return 0;
}
