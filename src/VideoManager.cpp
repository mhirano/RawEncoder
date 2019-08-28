//
// Created by Masahiro Hirano on 2018/03/07.
//

#include "VideoManager.h"

#define _MAX_MEMORY_ALLOCATION 16000000000

void VideoManager::loadVideoFromFile(const std::string videoFileName, int frameSkip) {
    /*
     * Clear image sequence
     */
    imageSequence.clear();

    /*
     * make sure videoFileName is raw video
     */
    std::string fileExtention = videoFileName.substr(videoFileName.find_last_of(".") + 1);
    if (fileExtention != "raw") {
        std::cerr << "Extension ." << fileExtention << " is not supported." << std::endl;
        abort();
    }

    /*
     * decide frameskip
     */
    uint64_t filesize = sizeInByte(videoFileName);
    std::cout << "filesize: " << filesize/1000000000.0f << "[GiB]" << std::endl;

    int autoFrameSkip = computeAutoFrameSkip(videoFileName);

    std::cout << "Loading video..." << std::endl;
    if (frameSkip < autoFrameSkip) {
        std::cout << "Video is too large to load on memory for frameSkip " << frameSkip << ". "
                  << "Set autoFrameSkip to " << autoFrameSkip << std::endl;
        loadVideoFromRawFile(videoFileName, autoFrameSkip);
    } else{
        std::cout << "Set frameSkip to " << frameSkip << std::endl;
        loadVideoFromRawFile(videoFileName, frameSkip);
    }
    std::cout << "Video loaded." << std::endl;

}


void VideoManager::loadVideoFromRawFile(const std::string rawFileName, int frameSkip) {
    std::ifstream ifs(rawFileName, std::ios::in | std::ios::binary);

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
}

bool VideoManager::encodeToAVI(float aviFrameRate, const std::string encodeFileName) const {

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
}


int VideoManager::totalFrames() const {
    if (isLoaded()){
        return imageSequence.size();
    } else {
        std::cout << "Video not loaded yet. Run loadVideoFromFile first." << std::endl;
        return -1;
    }
}


cv::Mat& VideoManager::imageAt(int index) {

    bool isIndexValid = Util::includes(0, totalFrames()-1, index);
    assert(isIndexValid);

    return imageSequence.at(index);
}

uint64_t VideoManager::sizeInByte(const std::string fileName) {
    std::ifstream ifs(fileName);

    ifs.seekg(0, std::fstream::end);
    uint64_t eofPos = ifs.tellg();

    ifs.clear();

    ifs.seekg(0, std::fstream::beg);
    uint64_t begPos = ifs.tellg();
    
    uint64_t fileSize = eofPos - begPos;
    ifs.close();
    
    return fileSize;
}

int VideoManager::computeAutoFrameSkip(const std::string videoFileName) {
    uint64_t filesize = sizeInByte(videoFileName);
    return std::ceil((float) filesize / _MAX_MEMORY_ALLOCATION);
}

bool VideoManager::isLoaded() const {
    if (imageSequence.empty()) {
        return false;
    } else {
        return true;
    }
}


