#include <Degrees.hpp>
#include <Line.hpp>
#include <gtest/gtest.h>

#include <iostream>

using namespace std;

TEST(Line, intersect) {
    Line a       = {{3, 7, -5}};
    Line b       = {{2, -11, 13}};
    Point result = Line::intersect(a, b);
    Point expect = {-36.0 / 47.0, 49.0 / 47.0};
    EXPECT_EQ(result, expect);
}

TEST(Line, pointRelativePosition) {
    Pixel point = {5, 5};
    Line l1     = {{7, 3}, angle_t(225_deg)};
    Line l2     = {{3, 7}, angle_t(45_deg)};
    Line r1     = {{7, 3}, angle_t(45_deg)};
    Line r2     = {{3, 7}, angle_t(225_deg)};

    EXPECT_TRUE(l1.rightOfPoint(point));
    EXPECT_TRUE(l2.rightOfPoint(point));
    EXPECT_FALSE(r1.rightOfPoint(point));
    EXPECT_FALSE(r2.rightOfPoint(point));
}

TEST(Point, distance) {
    Point p1       = {2.68, 5.28};
    Point p2       = {10.5, -6.7};
    float result   = Point::distance(p1, p2);
    float expected = 14.30639018;
    ASSERT_FLOAT_EQ(result, expected);
}