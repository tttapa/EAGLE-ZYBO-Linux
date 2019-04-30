#pragma once

int test();

#include <AngleTracker.hpp>
#include <HelperStructs.hpp>
#include <opencv2/videoio.hpp>

class LocationFinder {
  public:
    LocationFinder(cv::VideoCapture &&cap) : cap(cap) {}
    LocationFinder(const cv::VideoCapture &cap) : cap(cap) {}
    LocationFinder(int i) : cap(i) {}
    LocationFinder(std::string s) : cap(s) {}

    Point getLocation();
    angle_t getAngle() const { return angle; }
    cv::Mat getImage() const { return image; }
    cv::Mat getMaskImage() const { return maskImage; }
    cv::VideoCapture &getCapture() { return cap; }

  private:
    cv::VideoCapture cap;
    cv::Mat image;
    cv::Mat maskImage;
    bool hasImage = false;
    AngleTracker angleTracker;
    float sideLen = 0.0;
    angle_t angle;
};