#include <bitString.hpp>
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

    // FIRST ENCRYPTION:

    cryptoPoller.normalStep(0x06008);
    cryptoPoller.normalStride(0x05008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);

    ASSERT_EQ(cryptoPoller.normalStep(0x02008) & partialMask, 0xa5110);

    // SECOND ENCRYPTION:

    cryptoPoller.normalStep(0x06008);
    cryptoPoller.xorStep(0x4b6cf);
    cryptoPoller.xorStep(0xec0df);
    cryptoPoller.xorStep(0x8daef);
    cryptoPoller.xorStep(0x2e4ff);
    cryptoPoller.xorStep(0xcff0f);
    cryptoPoller.normalStride(0x70058);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);

    ASSERT_EQ(cryptoPoller.normalStep(0x02008) & partialMask, 0xf7210);

    // THIRD ENCRYPTION:

    cryptoPoller.normalStep(0x06008);
    cryptoPoller.xorStep(0x6b8cf);
    cryptoPoller.xorStep(0x0c2df);
    cryptoPoller.xorStep(0xadcef);
    cryptoPoller.xorStep(0x4e6ff);
    cryptoPoller.xorStep(0xef10f);
    cryptoPoller.xorStep(0x90b1f);
    cryptoPoller.xorStep(0x3152f);
    cryptoPoller.xorStep(0xd2f3f);
    cryptoPoller.xorStep(0x7394f);
    cryptoPoller.xorStep(0x1435f);
    cryptoPoller.xorStep(0xb5d6f);
    cryptoPoller.xorStep(0x5677f);
    cryptoPoller.xorStep(0xf718f);
    cryptoPoller.normalStride(0x98058);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);

    ASSERT_EQ(cryptoPoller.normalStep(0x02008) & partialMask, 0xa9b00);

    // FOURTH ENCRYPTION:

    cryptoPoller.normalStep(0x3acbc);
    cryptoPoller.normalStep(0x5cedc);
    cryptoPoller.normalStep(0x7e0fc);
    cryptoPoller.normalStep(0xa031c);
    cryptoPoller.normalStep(0xc253c);
    cryptoPoller.normalStep(0xe4068);
    cryptoPoller.normalStride(0x05008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);

    ASSERT_EQ(cryptoPoller.normalStep(0x02008) & partialMask, 0x1b4d0);

    // FIFTH ENCRYPTION:

    cryptoPoller.normalStep(0x51e2c);
    cryptoPoller.normalStep(0x7304c);
    cryptoPoller.normalStep(0x9526c);
    cryptoPoller.normalStep(0xb748c);
    cryptoPoller.normalStep(0xd96ac);
    cryptoPoller.normalStep(0xfb068);
    cryptoPoller.xorStep(0xee0ff);
    cryptoPoller.xorStep(0x8fb0f);
    cryptoPoller.xorStep(0x3051f);
    cryptoPoller.xorStep(0xd1f2f);
    cryptoPoller.xorStep(0x7293f);
    cryptoPoller.xorStep(0x1334f);
    cryptoPoller.xorStep(0xb4d5f);
    cryptoPoller.xorStep(0x5576f);
    cryptoPoller.xorStep(0xf617f);
    cryptoPoller.xorStep(0x97b8f);
    cryptoPoller.xorStep(0x3859f);
    cryptoPoller.normalStride(0xd9058);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);
    cryptoPoller.normalStep(0x02008);

    ASSERT_EQ(cryptoPoller.normalStep(0x02008) & partialMask, 0xe1fb0);
}

TEST(Crypto, CryptoKeypack) {
    BitString bitstring(176 + 16);
    bitstring.concatenate(BitString(
        {0xfe, 0x97, 0x30, 0xc9, 0x62, 0xfb, 0x94, 0x2d, 0xc6, 0x5f, 0xf8,
         0x91, 0x2a, 0xc3, 0x5c, 0xf5, 0x8e, 0x27, 0xc0, 0x59, 0xf2, 0x8b}));
    bitstring.addSimplePadding(176 + 16 - 8, 176);
    bitstring.addMultiRatePadding(200, bitstring.getLength());
    ASSERT_TRUE(bitstring ==
                BitString({0x18, 0xfe, 0x97, 0x30, 0xc9, 0x62, 0xfb, 0x94, 0x2d,
                           0xc6, 0x5f, 0xf8, 0x91, 0x2a, 0xc3, 0x5c, 0xf5, 0x8e,
                           0x27, 0xc0, 0x59, 0xf2, 0x8b, 0x01, 0x81}));
    std::array<uint32_t, 10> keypack;
    bitstring.split(keypack);
    ASSERT_EQ(keypack[0], 0x18fe9);
    ASSERT_EQ(keypack[1], 0x730c9);
    ASSERT_EQ(keypack[2], 0x62fb9);
    ASSERT_EQ(keypack[3], 0x42dc6);
    ASSERT_EQ(keypack[4], 0x5ff89);
    ASSERT_EQ(keypack[5], 0x12ac3);
    ASSERT_EQ(keypack[6], 0x5cf58);
    ASSERT_EQ(keypack[7], 0xe27c0);
    ASSERT_EQ(keypack[8], 0x59f28);
    ASSERT_EQ(keypack[9], 0xb0181);
}
