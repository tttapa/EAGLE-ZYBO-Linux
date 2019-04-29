#include <crypto.hpp>
#include <cryptoPoller.hpp>
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

/**
 * @brief   Check that the 20-bit values written to R0 can be read back 
 *          correctly from R1.
 */
TEST(Crypto, CryptoPoller) {
    const uint32_t mask = 0x00'0F'FF'F0;
    CryptoPoller cryptoPoller;

    cryptoPoller.write(0x9, 0x3c280);
    ASSERT_EQ(cryptoPoller.read(0x9) & mask, 0x3c280);

    ASSERT_EQ(cryptoPoller.hashAbsorb(0x3c280) & mask, 0x3c280);
    ASSERT_EQ(cryptoPoller.hashSqueeze() & mask, 0x1bef0);

    cryptoPoller.write(0x9, 0x18fe9);
    cryptoPoller.write(0x8, 0x730c9);
    cryptoPoller.write(0x7, 0x62fb9);
    cryptoPoller.write(0x6, 0x42dc6);
    cryptoPoller.write(0x5, 0x5ff89);
    cryptoPoller.write(0x4, 0x12ac3);
    cryptoPoller.write(0x3, 0x5cf58);
    cryptoPoller.write(0x2, 0xe27c0);
    cryptoPoller.write(0x1, 0x59f28);
    cryptoPoller.write(0x0, 0xb0181);

    cryptoPoller.start();

    cryptoPoller.normalStep(0x06008);
    cryptoPoller.normalStride(0x05008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);

    ASSERT_EQ(cryptoPoller.normalStep(0x02008) & mask, 0x3c280);
}