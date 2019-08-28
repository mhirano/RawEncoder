#include <iostream>
#include <filesystem> // checked on Ubuntu
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

#include "RawVideoEncoder.h"

#ifdef _OPENMP
#include "omp.h"
#endif

void help(){
    printf("Invalid arguments. \n");
    printf("\t Usage: ./RawEncoder [/path_to_the_data_dir] [frameWidth] [frameHeight] [(Option:) encodedFrameRate (default=10)] [(Option:) frameSkip (default=10)] \n");
    printf("\t Example: ./RawEncoder /path_to_the_data_dir 800 600 \n");
    printf("\t Example: ./RawEncoder /path_to_the_data_dir 800 600 10 \n");
    printf("\t Example: ./RawEncoder /path_to_the_data_dir 800 600 15 15\n");
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

    std::string rootDirString(argv[1]);
    int frameWidth = std::atoi(argv[2]);
    int frameHeight = std::atoi(argv[3]);

    printf("Run RawEncode under %s ...\n", rootDirString.c_str());
    printf("[Parameters] frame width: %d, frame height: %d, encoding frame rate: %d, frame skip: %d \n", frameWidth, frameHeight, frameRate, frameSkip);

    /**
     * videoFileDir に入っている動画を読み込む
     */

    std::vector<std::string> rawFileArray;
    std::vector<std::string> encodedFileArray;

    for(const fs::directory_entry& x : fs::recursive_directory_iterator(rootDirString)){
        std::string extension = x.path().extension().string();
        if (extension == ".raw"){
            rawFileArray.push_back(x.path().parent_path().string() + "/" + x.path().stem().string());
        } else if(extension == ".avi"){
            encodedFileArray.push_back(x.path().parent_path().string() + "/" + x.path().stem().string());
        }
    }

    int numFile = rawFileArray.size();

//#ifdef _OPENMP
//#pragma omp parallel for // サーバからの読み込みが律速になっているので早くならないどころかランダムアクセス度が高まるので遅くなる
//#endif
    for(int i = 0; i<numFile; i++){
        std::string rawFilePath = rawFileArray[i];
        printf("%s.raw", rawFilePath.c_str());

        auto isEncoded = [](std::vector<std::string> const &stringArray, std::string const &query) -> bool {
            auto contain = [](const std::string& s, const std::string& q) -> bool {
                return s.find(q) != std::string::npos;
            };
            bool isEncoded = false;
            for(const auto& itr : stringArray) isEncoded = isEncoded || contain(itr, query);
            return isEncoded;
        };

        if(isEncoded(encodedFileArray, rawFilePath)){
            printf("...encoded. Skip.");
        } else {
            printf("...not encoded.");
#ifdef _OPENMP
            printf("Encoding %s.raw in thread num: %d\n", rawFilePath.c_str(), omp_get_thread_num());
#else
            printf("Encoding %s.raw\n", rawFilePath.c_str());
#endif

            std::string rawFileFullPath = rawFilePath + ".raw";

            RawVideoEncoder vm(rawFileFullPath, frameWidth, frameHeight, frameSkip);
            vm.loadVideoFromFile();
            vm.encodeToAVI(30.0f);
        }

        printf("\n");
    }

    return 0;
}
