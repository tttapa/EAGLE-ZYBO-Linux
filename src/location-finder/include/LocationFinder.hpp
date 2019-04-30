#pragma once

int test();

#include <HelperStructs.hpp>
#include <opencv2/videoio.hpp>

class LocationFinder {
  public:
    LocationFinder(cv::VideoCapture &&cap) : cap(cap) {}
    LocationFinder(const cv::VideoCapture &cap) : cap(cap) {}

    std::optional<angle_t> getAverageYaw(std::array<std::optional<LineResult>, 5> lines);
    Point transformPoint(angle_t angle, Point point);
    void setLength(Point point1, Point point2);
    Point findOrigin(std::array<std::optional<Point>, 4> points, angle_t angle);
    Point findLocation(Point origin);
    bool isUsableImage(Square square);   
    Point getLocation();
  
    Point test1(Square square);

  private:
    cv::VideoCapture cap;
    const Point center = {320,240};
    //TODO 640,480
    float length;
    Point previous_location;
};