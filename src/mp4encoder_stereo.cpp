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
    printf("\t Usage: ./mp4encoder_stereo [left.raw] [right.raw] [frameWidth] [frameHeight] [(Option:) encodingFrameRate (default=10)] [(Option:) frameSkip (default=10)] \n");
    printf("\t Example: ./mp4encoder_stereo left.raw right.raw 800 600 \n");
    printf("\t Example: ./mp4encoder_stereo left.raw right.raw 800 600 10 \n");
    printf("\t Example: ./mp4encoder_stereo left.raw right.raw 800 600 15 15\n");
}

int main(int argc, char **argv) {

    int frameSkip = 10;
    int frameRate = 10;

    if (argc == 7){
        frameSkip = std::atoi(argv[6]);
        frameRate = std::atoi(argv[5]);
    } else if (argc == 6){
        frameRate = std::atoi(argv[5]);
    } else if (argc == 5){
        // do nothing
    } else {
        help();
        return -1;
    }

    std::string leftVideoPath(argv[1]);
    std::string rightVideoPath(argv[2]);
    int frameWidth = std::atoi(argv[3]);
    int frameHeight = std::atoi(argv[4]);

    printf("Run mp4encoder_stereo\n left: %s\n right: %s \n...\n", leftVideoPath.c_str(), rightVideoPath.c_str());
    printf("[Parameters] frame width: %d, frame height: %d, encoding frame rate: %d, frame skip: %d \n", frameWidth, frameHeight, frameRate, frameSkip);


    RawVideo leftVideo(leftVideoPath, frameWidth, frameHeight, 0.0, 1.0, frameSkip);
    RawVideo rightVideo(rightVideoPath, frameWidth, frameHeight, 0.0, 1.0, frameSkip);
    leftVideo.loadVideoFromFile();
    rightVideo.loadVideoFromFile();

    assert(leftVideo.totalFrames() == rightVideo.totalFrames() && "Left and right video have same number of frames");

    cv::Mat canvas(frameHeight, frameWidth * 2, CV_8UC1);
    cv::Mat canvas_color(frameHeight, frameWidth * 2, CV_8UC3);

    std::string encodeFileName("out.mp4");

    cv::VideoWriter writer(encodeFileName,
                           cv::VideoWriter::fourcc('M', 'P', '4', 'V'),
                           frameRate,
                           cv::Size(frameWidth*2, frameHeight),
                           true);

    if (!writer.isOpened()) {
        std::cerr << "Video open failed: " << encodeFileName << std::endl;
        return -1;
    }

    std::cout << "Encode start." << std::endl
              << "Encoding to " << encodeFileName << std::endl
              << "Encoding..." << std::endl;

    for (int i = 0; i < leftVideo.totalFrames(); i++) {
        cv::hconcat(leftVideo.imageAt(i), rightVideo.imageAt(i), canvas);

        cv::cvtColor(canvas, canvas_color, cv::COLOR_GRAY2BGR);

        cv::putText(canvas_color, "Time: " + std::to_string((float)i*frameSkip/500.0), cv::Point(50,50),
                    cv::FONT_HERSHEY_SIMPLEX, 1, {0, 0, 0});

        writer.write(canvas_color);

        cv::waitKey(1);
    }

    writer.release();

    std::cout << "Encode finished: " << encodeFileName << std::endl;

    return 0;
}
