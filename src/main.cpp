#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem> // checked on Ubuntu
#include <algorithm>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

#include "VideoManager.h"

#ifdef _OPENMP
#include "omp.h"
#endif

int main(int argc, char **argv) {
    char* rootDir = argv[1];
    std::string rootDirString(rootDir);

    if (argc == 2){
        // OK. do nothing
        std::cout << "Run RawEncode under " << rootDirString << "..." << std::endl;
    } else if (argc == 1){
        std::cerr << "Directory unspecified!" << std::endl
                  <<"Usage: ./RawEncoder /path_to_the_data_direcotry/" << std::endl;
        return -1;
    } else {
        std::cerr << "Too many arguments" << std::endl;
        return -1;
    }
    /**
     * videoFileDir に入っている動画を読み込む
     */
    static int frameWidth = 800;
    static int frameHeight = 600;

    std::vector<std::string> rawFileArray;
    std::vector<std::string> encodedFileArray;

    for(const fs::directory_entry& x : fs::recursive_directory_iterator(rootDirString)){
        std::string extension = x.path().extension().string();
        if (extension == ".raw"){
            rawFileArray.push_back(x.path().parent_path().string() + "/" + x.path().stem().string());
            // std::cout << ".raw file: " << rawFileArray.back() << ".raw" << std::endl;
        } else if(extension == ".avi"){
            encodedFileArray.push_back(x.path().parent_path().string() + "/" + x.path().stem().string());
            // std::cout << ".avi file: " << encodedFileArray.back() << ".avi" << std::endl;
        }
    }

    // for(const fs::directory_entry& x : fs::recursive_directory_iterator(rootDirString)){
    //     std::string extension = x.path().extension().string();
    //     if(extension == ".raw"){
    //         //parentPathFileArray.push_back(x.path().parent_path().string());
    //         //rawFileArray.push_back(x.path().stem().string());
    //     }
    // }

    int numFile = rawFileArray.size();

//#ifdef _OPENMP
//#pragma omp parallel for // サーバからの読み込みが律速になっているので早くならないどころかランダムアクセス度が高まるので遅くなる
//#endif
    for(int i = 0; i<numFile; i++){
        //std::string fileStem = rawFileArray[i];
        //std::string parentPath = parentPathFileArray[i];
        std::string rawFilePath = rawFileArray[i];
        printf("%s.raw", rawFilePath.c_str());
        auto rawItr = find(encodedFileArray.cbegin(), encodedFileArray.cend(), rawFilePath);
        if(rawItr != encodedFileArray.cend()){
            printf("...skipped\n");
        } else{
            printf("...encode\n");
            printf("Encoding %s.raw in thread num: %d\n",
                   rawFilePath.c_str(), omp_get_thread_num());

            VideoManager vm(frameWidth, frameHeight);
            vm.loadVideoFromFile(rawFilePath + ".raw");
            vm.encodeToAVI(rawFilePath + ".avi", 10.0, 25);
        }
        printf("\n");


    }

    return 0;
}
