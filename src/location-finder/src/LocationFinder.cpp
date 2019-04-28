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

std::optional<angle_t> LocationFinder::getAverageYaw(std::array<std::optional<LineResult>, 5> lines;) {
    if (lines.empty())
        return std::nullopt;
    angle_t angle_line = average(lines[0].angle,lines[1].angle);
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
    x = point[0] - center[0];
    y = -(point[1] - center[1]);
    return {x*cosf(angle) + y*sinf(angle), -x*sinf(angle) + y*cosf(angle)}
}

void setLength(std::array<std::optional<Point>, 4> points) {
    if (points.size() < 2)
        return;
    if (points.size() == 2){
        if ((points[0][1] > 0 && points[1][1] > 0)||(points[0][1] < 0 && points[1][1] < 0)
            this->length = points[1][0] - points[0][0];
        else
            this->length = points[1][1] - points[0][1];
    }
}

Point LocationFinder::findOrigin(std::array<std::optional<Point>, 4> points, angle_t angle) {
    std::array<std::optional<Point>, 4> transformed_points;
    Point bottom_left;
    Point top_left;
    Point bottom_right;
    Point top_right;
    for(Point point: points){
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
    if (bottom_left != null)
        transformed_points.push_back(bottom_left);
    if (top_left != null)
        transformed_points.push_back(top_left);
    if (bottom_right != null)
        transformed_points.push_back(bottom_right);
    if (top_right != null)
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

boolean LocationFinder::isUsableImage(Square square) {
    if (square == null || square.lines == null || square.points == null){
        return false;
    }
    return square.lines.size() >= 2 && square.points.size() >= 2;
}

Point LocationFinder::getLocation() {
    if (!cap.isOpened())
        throw std::runtime_error("Capture is not opened");
    cv::Mat img;
    cap >> img;
    Mask mask = img;
    GridFinder gf = std::move(mask);
    std::cout << gf.findSquare() << std::endl;
    Square fs = gf.findSquare();
    if (previous_location == null){
        this->previous_location = {0.5,0.5};
    }
    if (!isUsableImage(fs)){
        return this->previous_location;
    }
    angle_t yaw = getAverageYaw(fs.lines);
    Point origin = findOrigin(fs.points, yaw);
    return findLocation(origin);
}

Point test1(Square square) {
    if (previous_location == null){
        this->previous_location = {0.5,0.5};
    }
    if (!isUsableImage(fs)){
        return this->previous_location;
    }
    angle_t yaw = getAverageYaw(fs.lines);
    Point origin = findOrigin(fs.points, yaw);
    return findLocation(origin);
}