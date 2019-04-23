#include <LocationFinder.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(OpenCV, test) { ASSERT_EQ(test(), 0); }

TEST(LocationFinder, getLocation) {
    LocationFinder loc(0 + cv::CAP_V4L2);
    loc.getLocation();
}