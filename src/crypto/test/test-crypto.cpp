#include <cryptoPoller.hpp>
#include <gtest/gtest.h>

/**
 * @brief   Testbench provided by TAs.
 *          - Test if written value to R0 can be read back from R1.
 *          - Test hashing.
 *          - Test encryption.
 */
TEST(Crypto, CryptoPoller) {
    const uint32_t fullMask    = 0x00'0F'FF'FF;
    const uint32_t partialMask = 0x00'0F'FF'F0;
    CryptoPoller cryptoPoller;

    cryptoPoller.write(9, 0x3c280);
    ASSERT_EQ(cryptoPoller.read(9) & fullMask, 0x3c280);

    ASSERT_EQ(cryptoPoller.hashAbsorb(0x3c280) & partialMask, 0x3c280);
    ASSERT_EQ(cryptoPoller.hashSqueeze() & partialMask, 0x1bef0);

    cryptoPoller.write(9, 0x18fe9);
    cryptoPoller.write(8, 0x730c9);
    cryptoPoller.write(7, 0x62fb9);
    cryptoPoller.write(6, 0x42dc6);
    cryptoPoller.write(5, 0x5ff89);
    cryptoPoller.write(4, 0x12ac3);
    cryptoPoller.write(3, 0x5cf58);
    cryptoPoller.write(2, 0xe27c0);
    cryptoPoller.write(1, 0x59f28);
    cryptoPoller.write(0, 0xb0181);

    cryptoPoller.start();

    cryptoPoller.normalStep(0x06008);
    cryptoPoller.normalStride(0x05008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);

    ASSERT_EQ(cryptoPoller.normalStep(0x02008) & partialMask, 0xa5110);
}