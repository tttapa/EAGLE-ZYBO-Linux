#include <LocationFinder.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(OpenCV, test) { ASSERT_EQ(test(), 0); }
TEST(OpenCV, test) { ASSERT_EQ(test1(null), {0.5,0.5}); }
TEST(OpenCV, test) { 
    ASSERT_NEAR(test1(null)[0], 0.5, 0.02);
    ASSERT_NEAR(test1(null)[1], 0.5, 0.02); }
TEST(OpenCV, test) { 
    ASSERT_NEAR(test1(null)[0], 0.9, 0.02);
    ASSERT_NEAR(test1(null)[1], 0.1, 0.02); }