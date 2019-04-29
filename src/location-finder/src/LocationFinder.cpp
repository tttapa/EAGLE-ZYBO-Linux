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
    Mask mask       = img;
    cv::Mat maskimg = {img.rows, img.cols, CV_8UC1, mask.ptr()};
    cv::imwrite("mask.bmp", maskimg);
    GridFinder gf = std::move(mask);
    Square sq     = gf.findSquare();
    angle_t angle = sq.getAngle();
    angle         = angleTracker.update(angle);
    std::cout << sq << std::endl;
    std::cout << angle << std::endl;

    using Vec2f = TColVector<float, 2>;

    Vec2f frameCenter = Point{gf.center()};
    Vec2f center;
    auto points = sq.points;
    if (!points[0] && !points[1])  // Just a line, not a single point
        return Point::invalid();
    angle_t diagAngle = angle + 45_deg;
    // If only the first point is valid
    if (points[0] && !points[1] && !points[2] && !points[3]) {
        if (sideLen == 0)
            return Point::invalid();
        center = sideLen / sqrt(2) * Vec2f{diagAngle.cosf(), diagAngle.sinf()} +
                 points[0]->vec();
    }
    // If only the second point is valid
    else if (!points[0] && points[1] && !points[2] && !points[3]) {
        if (sideLen == 0)
            return Point::invalid();
        center = sideLen / sqrt(2) * Vec2f{diagAngle.cosf(), diagAngle.sinf()} +
                 points[1]->vec();
    }
    // If only the first two points are valid
    else if (points[0] && points[1] && !points[2] && !points[3]) {
        sideLen = Point::distance(*points[0], *points[1]);
        if (sideLen == 0)
            return Point::invalid();
        center = sideLen / sqrt(2) * Vec2f{diagAngle.cosf(), diagAngle.sinf()} +
                 points[0]->vec();
    }
    // If we found all four points
    else if (points[0] && points[1] && points[2] && points[3]) {
        sideLen = (Point::distance(*points[0], *points[1]) +
                   Point::distance(*points[0], *points[2]) +
                   Point::distance(*points[1], *points[3]) +
                   Point::distance(*points[2], *points[3])) /
                  4;
        if (sideLen == 0)
            return Point::invalid();
        center = Point::average(Point::average(*points[0], *points[1]),
                                Point::average(*points[2], *points[3]));
    } else {
        std::cerr << ANSIColors::redb
                  << "Error: Somehow, we only found three points"
                  << ANSIColors::reset << std::endl;
        return Point::invalid();
    }
    Vec2f position = {0.5, 0.5};
    position += rotate(frameCenter - center, -angle).vec() / sideLen;
    return Point{position} % 1.0;
}