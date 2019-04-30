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

Point LocationFinder::updateLocation() {
    // Read in the image, and convert to RGB if on x86
    // -----------------------------------------------
    if (!cap.isOpened())
        throw std::runtime_error("Capture is not opened");
    cap >> image;
    if (image.empty())
        throw std::runtime_error("Unable to capture frame");
#ifndef ZYBO
    // cv::imwrite("img.bmp", image);
    cv::Mat rgbimg;
    cv::cvtColor(image, rgbimg, cv::COLOR_BGR2RGB);
    image = rgbimg;
#endif

    // Mask the image, and save the mask as an image
    // ---------------------------------------------
    Mask mask = image;
    // Clone, so we don't keep a dangling pointer to mask
    maskImage = cv::Mat{image.rows, image.cols, CV_8UC1, mask.ptr()}.clone();
    // cv::imwrite("mask.bmp", maskImage);

    // Find a square of the grid using GridFinder
    // ------------------------------------------
    GridFinder gf     = std::move(mask);
    Square sq         = gf.findSquare();
    Point frameCenter = gf.center();

    return getLocation(sq, frameCenter);
}

Point LocationFinder::getLocation(Square &sq, Vec2f frameCenter) {
    // Find the angle of the square
    // ----------------------------
    try {
        angle_t newangle = sq.getAngle();
        angle            = angleTracker.update(newangle);
    } catch (std::runtime_error &) {
        // Fails if we can't determine the angle (when there are no lines)
    }

    // Find the center of the square and the average length of the sides
    // -----------------------------------------------------------------
    Vec2f center;
    auto points = sq.points;

    // === No points ===
    // If we found just one line, no points → nothing can be done
    if (!points[0] && !points[1])
        return Point::invalid();
    angle_t diagAngle = angle + 45_deg;
    // === One point ===
    // If only the first point is valid
    if (points[0] && !points[1] && !points[2] && !points[3]) {
        Vec2f diag = {diagAngle.cosf(), diagAngle.sinf()};
        center     = sideLen / sqrt(2) * diag + points[0]->vec();
    }
    // If only the second point is valid
    else if (!points[0] && points[1] && !points[2] && !points[3]) {
        Vec2f diag = {diagAngle.cosf(), diagAngle.sinf()};
        center     = sideLen / sqrt(2) * diag + points[1]->vec();
    }
    // === Two points ===
    // If only the first two points are valid
    else if (points[0] && points[1] && !points[2] && !points[3]) {
        sideLen    = Point::distance(*points[0], *points[1]);
        Vec2f diag = {diagAngle.cosf(), diagAngle.sinf()};
        center     = sideLen / sqrt(2) * diag + points[0]->vec();
    }
    // === Four points ===
    // If we found all four points
    else if (points[0] && points[1] && points[2] && points[3]) {
        sideLen = (Point::distance(*points[0], *points[1]) +
                   Point::distance(*points[0], *points[2]) +
                   Point::distance(*points[1], *points[3]) +
                   Point::distance(*points[2], *points[3])) /
                  4;
        center = Point::average(Point::average(*points[0], *points[1]),
                                Point::average(*points[2], *points[3]));
    } else {
        std::cerr << ANSIColors::redb
                  << "Error: Somehow, we only found three points"
                  << ANSIColors::reset << std::endl;
        return Point::invalid();
    }
    // Wie deelt door nul is een snul
    if (sideLen == 0)
        return Point::invalid();

    // Transform the position vector from the camera's frame of reference to
    // the grid axes
    // ---------------------------------------------------------------------
    Vec2f position = {0.5, 0.5};  // Center of square
    // Position vector of frame center relative to the square's center,
    // expressed in camera pixel coordinates
    Vec2f sqCenterToFrameCenter = frameCenter - center;
    // The y-axis of the camera is flipped
    sqCenterToFrameCenter[1] *= -1;
    // Transform the position vector of the frame center from the camera's
    // coordinate system to the grid's coordinate system, and normalize the side
    // length to one.
    position += rotate(sqCenterToFrameCenter, angle).vec() / sideLen;
    // Make sure that the coordinates are between 0.0 and 1.0
    // add 2.0 to handle negative case correctly
    return Point{position + Vec2f{2.0, 2.0}} % 1.0;
}