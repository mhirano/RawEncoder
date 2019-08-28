//
// Created by Masahiro Hirano on 2018/03/07.
//

#ifndef ACCEL_IMAGEMANAGER_H
#define ACCEL_IMAGEMANAGER_H

#include <fstream>
#include <opencv2/opencv.hpp>

#include "Utility.h"


class VideoManager {

public:
    VideoManager(int frameWidth_, int frameHeight_):frameWidth(frameWidth_), frameHeight(frameHeight_) {};

    void loadVideoFromFile(const std::string videoFileName);

    bool encodeToAVI(std::string videoFileName, float aviFrameRate, int frameInterval) const;

    int totalFrames() const;
    cv::Mat& imageAt(int index);

    static uint64_t sizeInByte(const std::string fileName);

    bool isLoaded() const;

private:

    void loadVideoFromAviFile(const std::string aviFileName);
    void loadVideoFromRawFile(const std::string rawFileName);

    int frameWidth;
    int frameHeight;

    cv::VideoCapture videoCapture;
    std::vector<cv::Mat> imageSequence; // stores all images in video file
};

#endif //ACCEL_IMAGEMANAGER_H
