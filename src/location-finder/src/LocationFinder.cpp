#include <GridFinder.hpp>
#include <LocationFinder.hpp>
#include <Mask.hpp>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include <opencv2/core/utils/logger.hpp>

using ::Point;

int test() {
    using namespace cv;
    utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_VERBOSE);
    std::cout << "Trying to create capture" << std::endl;
    VideoCapture cap(0 + CAP_V4L2);  // open the default camera
    std::cout << "VideoCapture created" << std::endl;
    // cap.set(CAP_PROP_FRAME_WIDTH, 410);
    // cap.set(CAP_PROP_FRAME_HEIGHT, 308);
    // std::cout << "VideoCapture dimensions set" << std::endl;
    if (!cap.isOpened())  // check if we succeeded
        return -1;
    std::cout << "VideoCapture is open" << std::endl;
    Mat frame;
    cap >> frame;  // get a new frame from camera
    std::cout << "VideoCapture captured" << std::endl;

    Mat blur;
    GaussianBlur(frame, blur, Size(7, 7), 1.5, 1.5);
    imwrite("test.bmp", frame);
    imwrite("blur.bmp", blur);
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

Point LocationFinder::getLocation() {
    if (!cap.isOpened())
        throw std::runtime_error("Capture is not opened");
    cv::Mat img;
    cap >> img;
    cv::Mat rgbimg;
    cv::cvtColor(img, rgbimg, cv::COLOR_BGR2RGB);
    cv::imwrite("img.bmp", rgbimg);
    uint px = 3 * 32;
    std::cout << +img.ptr()[px + 0] << ", "
              << +img.ptr()[px + 1] << ", "
              << +img.ptr()[px + 2] << std::endl;
    Mask mask       = img;
    cv::Mat maskimg = {img.rows, img.cols, CV_8UC1, mask.ptr()};
    cv::imwrite("mask.bmp", maskimg);
    GridFinder gf = std::move(mask);
    std::cout << gf.findSquare() << std::endl;
    return {0, 0};  // TODO
}