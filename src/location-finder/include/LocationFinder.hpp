#pragma once

int test();

#include <HelperStructs.hpp>
#include <opencv2/videoio.hpp>

class LocationFinder {
  public:
    LocationFinder(cv::VideoCapture &&cap) : cap(cap) {}
    LocationFinder(const cv::VideoCapture &cap) : cap(cap) {}

    Point getLocation();

  private:
    cv::VideoCapture cap;
};