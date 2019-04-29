#include <GridFinder.hpp>
#include <LocationFinder.hpp>
#include <Mask.hpp>
#include <iostream>
#include <string>
#include <Angle.hpp>

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

std::optional<angle_t> LocationFinder::getAverageYaw(std::array<std::optional<LineResult>, 5> lines) {
    if (lines.empty())
        return std::nullopt;
    angle_t angle_line = angle_t::average(lines[0]->angle,lines[1]->angle);
    if ((angle_line > 45_deg && angle_line < 135_deg))
        angle_line = angle_line - 90_deg;
    if ((angle_line > 135_deg && angle_line < 225_deg))
        angle_line = angle_line - 180_deg;
    if ((angle_line > 225_deg && angle_line < 315_deg))
        angle_line = angle_line - 270_deg; 
    return angle_line;
}

Point LocationFinder::transformPoint(angle_t angle, Point point) {
    float x;
    float y;
    x = point.x - center.x;
    y = -(point.y - center.y);
    return {x*angle.cosf() + y*angle.sinf(), -x*angle.sinf() + y*angle.cosf()}
}

Point LocationFinder::findOrigin(std::array<std::optional<Point>, 4> points, angle_t angle) {
    std::array<std::optional<Point>, 4> transformed_points;
    std::optional<Point> bottom_left;
    std::optional<Point> top_left;
    std::optional<Point> bottom_right;
    std::optional<Point> top_right;
    for(auto point: points){
        transformed_point = transformPoint(angle, point);
        if (transformed_point[0]<0 && transformed_point[1]<0)
            bottom_left = transformed_point;
        else if (transformed_point[0]<0 && transformed_point[1]>0)
            top_left = transformed_point;
        else if (transformed_point[0]>0 && transformed_point[1]<0)
            bottom_right = transformed_point;
        else
            top_right = transformed_point;
    }
    if (bottom_left != std::nullopt)
        transformed_points.push_back(bottom_left);
    if (top_left != std::nullopt)
        transformed_points.push_back(top_left);
    if (bottom_right != std::nullopt)
        transformed_points.push_back(bottom_right);
    if (top_right != std::nullopt)
        transformed_points.push_back(top_right);
    setLength(transformed_points);
    return transformed_points[0];
}

Point LocationFinder::findLocation(Point origin) {
    float x;
    float y;
    if (origin[0] <= 0 && origin[1] <= 0){
        x = -origin[0];
        y = -origin[1];
    }
    else if (origin[0] <= 0 && origin[1] > 0){
        x = -origin[0];
        y = 1 - origin[1];
    }
    else{
        x = 1 - origin[0];
        y = -origin[1];
    }
    this->previous_location = {x/length, y/length};
    return {x/length, y/length};
}

bool LocationFinder::isUsableImage(Square square) {
    int size_points;
    int size_lines;
    for (auto point: square.points){
        if (point != std::nullopt)
            size_points += 1;
    }
    for (auto line: square.lines){
        if (line != std::nullopt)
            size_points += 1;
    }
    return size_points >= 2 && size_lines >= 2;
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
    Square fs = gf.findSquare();
    if (!previous_location){
        this->previous_location = {0.5,0.5};
    }
    if (!isUsableImage(fs)){
        return this->previous_location;
    }
    std::optional<angle_t> yaw = getAverageYaw(fs.lines);
    if (!yaw)
        return this->previous_location;
    Point origin = findOrigin(fs.points, *yaw);
    return findLocation(origin);
}

Point test1(Square square) {
    if (!previous_location){
        this->previous_location = {0.5,0.5};
    }
    if (!isUsableImage(square)){
        return this->previous_location;
    }
    std::optional<angle_t> yaw = getAverageYaw(square.lines);
    if (!yaw)
        return this->previous_location;
    Point origin = findOrigin(square.points, *yaw);
    return findLocation(origin);
}