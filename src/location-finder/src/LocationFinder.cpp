#include <LocationFinder.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>

#include <opencv2/core/utils/logger.hpp>

using namespace cv;

int test() {
    utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_VERBOSE);
    std::cout << "Trying to create capture" << std::endl;
    VideoCapture cap(0 + CAP_V4L2);  // open the default camera
    std::cout << "VideoCapture created" << std::endl;
    cap.set(CAP_PROP_FRAME_WIDTH, 410);
    cap.set(CAP_PROP_FRAME_HEIGHT, 308);
    std::cout << "VideoCapture dimensions set" << std::endl;
    if (!cap.isOpened())  // check if we succeeded
        return -1;
    std::cout << "VideoCapture is open" << std::endl;
    Mat frame;
    cap >> frame;  // get a new frame from camera
    std::cout << "VideoCapture captured" << std::endl;
    Mat edges;
    cvtColor(frame, edges, COLOR_BGR2GRAY);
    GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
    Canny(edges, edges, 0, 30, 3);
    imwrite("test.bmp", edges);
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}