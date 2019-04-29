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

    Point getLocation();

  private:
    cv::VideoCapture cap;
    AngleTracker angleTracker;
    float sideLen = 0.0;
};