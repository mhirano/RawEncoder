//
// Created by Masahiro Hirano on 2018/03/07.
//

#include "VideoManager.h"



void VideoManager::loadVideoFromFile(const std::string videoFileName) {
    /*
     * Clear image sequence
     */
    imageSequence.clear();
    
    std::string fileExtention = videoFileName.substr(videoFileName.find_last_of(".") + 1);

    std::cout << "Loading video..." << std::endl;

    if(fileExtention == "avi") { 
        loadVideoFromAviFile(videoFileName); 
    } else if (fileExtention == "raw") { 
        loadVideoFromRawFile(videoFileName);
    } else {
        std::cout << "Extension " << fileExtention << " is not supported." << std::endl;
        abort(); 
    }

    std::cout << "Video loaded." << std::endl;
}

void VideoManager::loadVideoFromAviFile(const std::string aviFileName) {
    if (!videoCapture.open(aviFileName)) {
        std::cout << "Failed to open the avi file..." << std::endl;
        abort();
    }

    cv::Mat image;
    while (true) {
        videoCapture >> image;
        if (image.empty()) break;
        imageSequence.push_back(image.clone());
    }
}

void VideoManager::loadVideoFromRawFile(const std::string rawFileName) {

    std::ifstream ifs(rawFileName, std::ios::in | std::ios::binary);

    if( ifs.fail() ) {
        std::cout << "Failed to open the raw file..." << std::endl;
        abort();
    }

    uint64_t fileSize = sizeInByte(rawFileName);
    int fileSizePerFrame = sizeof(uint8_t) * frameWidth * frameHeight;
    int totalFrames = static_cast<int>(fileSize / fileSizePerFrame);

    uint8_t *imageDataBuffer = (uint8_t *) malloc(fileSizePerFrame);
    cv::Mat image(frameHeight, frameWidth, CV_8UC1);

    while( !ifs.eof() ) {
        ifs.read(reinterpret_cast<char *>(imageDataBuffer), fileSizePerFrame);
        memcpy(image.data, imageDataBuffer, fileSizePerFrame);
        /*
         * Bayer conversion: BayerBG8 to gray
         */
        cv::cvtColor(image,image,cv::COLOR_BayerBG2GRAY); // TODO: check if mode is correct

        imageSequence.push_back(image.clone());
    }

    free(imageDataBuffer);
    ifs.close();
}

bool VideoManager::encodeToAVI(std::string videoFileName, float aviFrameRate, int frameInterval) const {

    cv::VideoWriter writer(videoFileName,
                           cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),
                           aviFrameRate,
                           cv::Size(imageSequence.at(0).size().width, imageSequence.at(0).size().height),
                           false);

    if (!writer.isOpened()){
        std::cerr << "Video open failed " << std::endl;
        return -1;
    }

    std::cout << "Encode start: " << videoFileName << std::endl << "Encoding..." << std::endl;

    int i = 0;
    for(auto itr = imageSequence.begin(); itr != imageSequence.end(); ++itr) {
        if (i % frameInterval == 0){ // set every 25 frame as keyframes
            writer.write(imageSequence.at(i));
            cv::waitKey(1);
        }
        i++;
    }

    std::cout << "Encode finished: " << videoFileName << std::endl;

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

bool VideoManager::isLoaded() const {
    if (imageSequence.empty()) {
        return false;
    } else {
        return true;
    }
}


