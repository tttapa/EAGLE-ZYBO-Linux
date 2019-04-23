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

#define RGB2BGR(r, g, b) (b), (g), (r)

TEST(Mask, applyMask) {
    uint8_t colors[16 * 3] = {
        RGB2BGR(255, 0, 0),      // red
        RGB2BGR(200, 59, 239),   // purple
        RGB2BGR(46, 37, 145),    // dark blue
        RGB2BGR(173, 17, 53),    // dark rose red
        RGB2BGR(255, 239, 122),  // pastel yellow
        RGB2BGR(51, 42, 42),     // gray

    };

    std::array<uint8_t, 16> expected = {
        0xFF,  //
        0x00,  //
        0x00,  //
        0xFF,  //
        0x00,  //
        0x00,  //
    };
    std::array<uint8_t, 16> result;
    applyMask(colors, result.begin());
    ASSERT_EQ(result, expected);
}