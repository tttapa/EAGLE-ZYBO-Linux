#include <gtest/gtest.h>
#include <iostream>

#ifdef __ARM_NEON
#include <Mask-NEON.hpp>
#include <NEON-PrintHelpers.hpp>

TEST(Mask, applyMaskNEON) {
    uint8_t colors[16 * 3] = {
        255, 0, 0,    // red
        140, 4, 4,    // dark red
        255, 48, 48,  // light red
        //
        200, 59, 239,   // purple
        46, 37, 145,    // dark blue
        255, 239, 122,  // pastel yellow
        50, 45, 45,     // gray

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
    NEON::applyMask(colors, result.begin());
    ASSERT_EQ(result, expected);
}
#endif

#include <Mask-Fallback.hpp>
TEST(Mask, applyMaskFallback) {
    uint8_t colors[16 * 3] = {
        255, 0, 0,    // red
        140, 4, 4,    // dark red
        255, 48, 48,  // light red
        //
        200, 59, 239,   // purple
        46, 37, 145,    // dark blue
        255, 239, 122,  // pastel yellow
        50, 45, 45,     // gray

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
    Fallback::applyMask(colors, result.begin());
    ASSERT_EQ(result, expected);
}


#include <Mask.hpp>

TEST(Mask, Mask) {
    uint8_t colors[8 * 3] = {
        255, 0, 0,    // red
        140, 4, 4,    // dark red
        255, 48, 48,  // light red
        //
        200, 59, 239,   // purple
        46, 37, 145,    // dark blue
        255, 239, 122,  // pastel yellow
        50, 45, 45 ,     // gray
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