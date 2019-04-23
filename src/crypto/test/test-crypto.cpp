#include <crypto.hpp>
#include <gtest/gtest.h>

/**
 * @brief   Check that the values written to R0 can be read back correctly.
 */
TEST(Crypto, readWriteR0) {
    uint32_t testVals[] = {
        0x55'55'55'55,
        0xAA'AA'AA'AA,
        0x00'00'00'00,
        0xFF'FF'FF'FF,
    };
    for (uint32_t testVal : testVals) {
        writeR0(testVal);
        EXPECT_EQ(readR0(), testVal);
    }
}

/**
 * @brief   Check that the 20-bit values written to R0 can be read back 
 *          correctly from R1.
 */
TEST(Crypto, writeR0ReadR1) {
    const uint32_t mask = 0x00'0F'FF'FF;
    uint32_t data       = 0x00'0A'AA'AA;
    writeR0(0x10'10'00'00 | (data & mask));
    ASSERT_EQ(readR1() & mask, data & mask);
}