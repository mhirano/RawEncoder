//
// Created by Masahiro Hirano <masahiro.dll@gmail.com>
//

#ifndef RAWVIDEOENCODER_H
#define RAWVIDEOENCODER_H

#include <fstream>
#include <opencv2/opencv.hpp>

#include "Utility.h"

#define _MAX_MEMORY_ALLOCATION 16000000000

class RawVideoEncoder {

public:
    RawVideoEncoder(std::string videoFullPath_, int frameWidth_, int frameHeight_, int frameSkip_=1)
    : videoFullPath(videoFullPath_), frameWidth(frameWidth_), frameHeight(frameHeight_),
      frameSkip (computeFrameSkip(frameSkip_)) {
        /*
         * Clear image sequence
         */
        imageSequence.clear();

        /*
         * make sure videoFileName is raw video
         */
        std::string fileExtention = videoFullPath.substr(videoFullPath.find_last_of(".") + 1);
        if (fileExtention != "raw") {
            std::cerr << "Extension ." << fileExtention << " is not supported." << std::endl;
            abort();
        }

        /*
         * output file size
         */
        uint64_t filesize = sizeInByte(videoFullPath);
        std::cout << "filesize: " << filesize/1000000000.0f << "[GiB]" << std::endl;
    };

    void loadVideoFromFile(){
        std::cout << "Video load start." << std::endl;
        loadVideoFromRawFile();
        std::cout << "Video loaded." << std::endl;
    };

    bool encodeToAVI(float aviFrameRate) {
        std::string videoStem = videoFullPath.substr(0, videoFullPath.find_last_of(".") );
        std::cout << videoStem + "_frameSkip_" + std::to_string(frameSkip) +".avi" << std::endl;
        return encodeToAVI(10.0, videoStem + "_frameSkip_" + std::to_string(frameSkip) +".avi");
    };

    bool encodeToAVI(float aviFrameRate, const std::string encodeFileName) const{
        cv::VideoWriter writer(encodeFileName,
                               cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),
                               aviFrameRate,
                               cv::Size(imageSequence.at(0).size().width, imageSequence.at(0).size().height),
                               false);

        if (!writer.isOpened()){
            std::cerr << "Video open failed " << std::endl;
            return -1;
        }

        std::cout << "Encode start: " << encodeFileName << std::endl << "Encoding..." << std::endl;

        for(auto itr = imageSequence.begin(); itr != imageSequence.end(); ++itr) {
            writer.write(*itr);
            cv::waitKey(1);
        }

        std::cout << "Encode finished: " << encodeFileName << std::endl;

        writer.release();

        return true;
    };

    int totalFrames() const {
        if (isLoaded()){
            return imageSequence.size();
        } else {
            std::cout << "Video not loaded yet. Run loadVideoFromFile first." << std::endl;
            return -1;
        }
    };

    cv::Mat& imageAt(int index){
        bool isIndexValid = Util::includes(0, totalFrames()-1, index);
        assert(isIndexValid);

        return imageSequence.at(index);
    };

    static uint64_t sizeInByte(std::string fileName){
        std::ifstream ifs(fileName);

        ifs.seekg(0, std::fstream::end);
        uint64_t eofPos = ifs.tellg();

        ifs.clear();

        ifs.seekg(0, std::fstream::beg);
        uint64_t begPos = ifs.tellg();

        uint64_t fileSize = eofPos - begPos;
        ifs.close();

        return fileSize;
    };

    int computeFrameSkip(int frameSkip){
        uint64_t filesize = sizeInByte(videoFullPath);
        int minFrameSkip = std::ceil((float) filesize / _MAX_MEMORY_ALLOCATION);

        if (frameSkip >= minFrameSkip){
            std::cout << "Set frameSkip to " << frameSkip << std::endl;
            return frameSkip;
        } else {
            // framSkip < minFrameSkip
            std::cout << "Video is too large to load on memory for frameSkip " << frameSkip << ". "
                      << "Set frameSkip to " << minFrameSkip << std::endl;
            return minFrameSkip;
        }
    }

    bool isLoaded() const{
        return imageSequence.empty() ? false : true;
//        if (imageSequence.empty()) {
//            return false;
//        } else {
//            return true;
//        }
    };

private:

    void loadVideoFromRawFile(){
        std::ifstream ifs(videoFullPath, std::ios::in | std::ios::binary);

        if( ifs.fail() ) {
            std::cout << "Failed to open the raw file..." << std::endl;
            abort();
        }

        int fileSizePerFrame = sizeof(uint8_t) * frameWidth * frameHeight; // assume bayer image

        uint8_t *imageDataBuffer = (uint8_t *) malloc(fileSizePerFrame);
        cv::Mat image(frameHeight, frameWidth, CV_8UC1);

        int frameCount = 0;
        while( !ifs.eof() ) {
            ifs.read(reinterpret_cast<char *>(imageDataBuffer), fileSizePerFrame);
            frameCount++;
            if(frameCount % frameSkip == 0) {
                memcpy(image.data, imageDataBuffer, fileSizePerFrame);
                cv::cvtColor(image, image, cv::COLOR_BayerBG2GRAY); //Bayer conversion: BayerBG8 to gray
                imageSequence.push_back(image.clone());
            }
        }

        free(imageDataBuffer);
        ifs.close();
    };

    std::vector<cv::Mat> imageSequence; // stores all images in video file

    /*
     * initialized at construction
     */
    const std::string videoFullPath;
    const int frameWidth;
    const int frameHeight;
    const int frameSkip;
};

#endif
