#include <LocationFinder.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(OpenCV, test) { ASSERT_EQ(test(), 0); }

#ifdef __ARM_NEON
#include <Mask-NEON.hpp>
#include <NEON-PrintHelpers.hpp>

TEST(NEON, color) {
    uint8_t colors[16 * 3] = {
        255, 0,   0,    //
        0,   255, 0,    //
        0,   0,   255,  //
        255, 255, 0,    //
        0,   255, 255,  //
        255, 255, 255,  //
        255, 20,  20,   //
        128, 20,  20,   //
        100, 10,  10,   //
        0,   20,  20,   //
        50,  0,   0,    //
        0,   50,  0,    //
        0,   0,   50,   //
        100, 150, 20,   //
        20,  150, 100,  //
        0,   0,   0,    //
    };

    std::array<int8_t, 16> expected = {31, -64, -64, -32, -128, -96, 21,  6,
                                       7,  -10, 6,   -13, -13,  -30, -60, 0};
    int8x16_t resultv               = applyMaskFunction(colors);
    std::array<int8_t, 16> result;
    vst1q_s8(result.begin(), resultv);
    ASSERT_EQ(result, expected);
}
#endif