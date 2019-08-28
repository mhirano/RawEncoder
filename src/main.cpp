#include <iostream>
#include <filesystem> // checked on Ubuntu
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

#include "RawVideoEncoder.h"

#ifdef _OPENMP
#include "omp.h"
#endif


int main(int argc, char **argv) {
    if (argc == 2){
        // OK. do nothing
    } else if (argc == 1){
        std::cerr << "Directory unspecified!" << std::endl
                  <<"Usage: ./RawEncoder /path_to_the_data_direcotry/" << std::endl;
        return -1;
    } else {
        std::cerr << "Too many arguments" << std::endl;
        return -1;
    }

    char* rootDir = argv[1];
    std::string rootDirString(rootDir);

    printf("Run RawEncode under %s ...", rootDirString.c_str());

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
            printf("Encoding %s.raw in thread num: %d\n",
                   rawFilePath.c_str(), omp_get_thread_num());

            std::string rawFileFullPath = rawFilePath + ".raw";

            int frameSkip = 10;

            RawVideoEncoder vm(rawFileFullPath, frameWidth, frameHeight, frameSkip);
            vm.loadVideoFromFile();
            vm.encodeToAVI(10.0f);
        }

        printf("\n");
    }

    return 0;
}
