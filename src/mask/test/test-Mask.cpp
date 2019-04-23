#include <gtest/gtest.h>
#include <iostream>

#ifdef __ARM_NEON
#include <Mask-NEON.hpp>
#include <NEON-PrintHelpers.hpp>

TEST(Mask, applyMaskFunctionNEON) {
    uint8_t colors[16 * 3] = {
        0,   0,   255,  // BGR
        0,   255, 0,    //
        255, 0,   0,    //
        0,   255, 255,  //
        255, 255, 0,    //
        255, 255, 255,  //
        20,  20,  255,  //
        20,  20,  128,  //
        10,  10,  100,  //
        20,  20,  0,    //
        0,   0,   50,   //
        0,   50,  0,    //
        50,  0,   0,    //
        20,  150, 100,  //
        100, 150, 20,   //
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

#ifndef __ARM_NEON
#include <Mask-Fallback.hpp>
#endif

TEST(Mask, applyMask) {
    uint8_t colors[16 * 3] = {
        255, 0, 0,    // red
        102, 4, 4,    // dark red
        255, 48, 48,  // light red
        //
        200, 59, 239,   // purple
        46, 37, 145,    // dark blue
        255, 239, 122,  // pastel yellow
        51, 42, 42,     // gray

    };

    std::array<uint8_t, 16> expected = {
        0xFF,  //
        0xFF,  //
        0xFF,  //
        //
        0x00,  //
        0x00,  //
        0x00,  //
        0x00,  //
    };
    std::array<uint8_t, 16> result;
    applyMask(colors, result.begin());
    ASSERT_EQ(result, expected);
}

#include <Mask.hpp>

TEST(Mask, Mask) {
    uint8_t colors[8 * 3] = {
        255, 0, 0,    // red
        102, 4, 4,    // dark red
        255, 48, 48,  // light red
        //
        200, 59, 239,   // purple
        46, 37, 145,    // dark blue
        255, 239, 122,  // pastel yellow
        51, 42, 42,     // gray
        255, 255, 255,  // white
    };

    Mask mask = {colors, 4, 2};

    std::array<uint8_t, 16> result = {
        mask.get({0, 0}), mask.get({1, 0}),  //
        mask.get({0, 1}), mask.get({1, 1}),  //
        mask.get({0, 2}), mask.get({1, 2}),  //
        mask.get({0, 3}), mask.get({1, 3}),  //
    };

    std::array<uint8_t, 16> expected = {
        0xFF,  //
        0xFF,  //
        0xFF,  //
        //
        0x00,  //
        0x00,  //
        0x00,  //
        0x00,  //
    };
    ASSERT_EQ(result, expected);
}