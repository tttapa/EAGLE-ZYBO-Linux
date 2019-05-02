#include <bitString.hpp>
#include <crypto.hpp>
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

/**
 * @brief   Test simple operations on crypto keypack.
 *          - Enc function.
 *          - Concatenation.
 *          - Padding.
 */
TEST(Crypto, CryptoKeypack) {
    const uint16_t keyLength = 176;
    const uint16_t l         = keyLength + 16;
    BitString keypack(l);
    keypack.reserve(200);

    keypack
        .concatenate(BitString({0xfe, 0x97, 0x30, 0xc9, 0x62, 0xfb, 0x94, 0x2d,
                                0xc6, 0x5f, 0xf8, 0x91, 0x2a, 0xc3, 0x5c, 0xf5,
                                0x8e, 0x27, 0xc0, 0x59, 0xf2, 0x8b}))
        .addSimplePadding(l - 8, keyLength)
        .addMultiRatePadding(200, keypack.getLength());

    std::cout.flags(std::cout.flags() | std::ios::hex);
    std::cout << "\n" << keypack << "\n";
    std::cout.flags(std::cout.flags() & ~std::ios::hex);
    std::cout << keypack << "\n";

    ASSERT_EQ(keypack,
              BitString({0x18, 0xfe, 0x97, 0x30, 0xc9, 0x62, 0xfb, 0x94, 0x2d,
                         0xc6, 0x5f, 0xf8, 0x91, 0x2a, 0xc3, 0x5c, 0xf5, 0x8e,
                         0x27, 0xc0, 0x59, 0xf2, 0x8b, 0x01, 0x81}));

    std::array<uint32_t, 10> splittedKeypack;
    keypack.split(splittedKeypack);

    ASSERT_EQ(splittedKeypack[0], 0x18fe9);
    ASSERT_EQ(splittedKeypack[1], 0x730c9);
    ASSERT_EQ(splittedKeypack[2], 0x62fb9);
    ASSERT_EQ(splittedKeypack[3], 0x42dc6);
    ASSERT_EQ(splittedKeypack[4], 0x5ff89);
    ASSERT_EQ(splittedKeypack[5], 0x12ac3);
    ASSERT_EQ(splittedKeypack[6], 0x5cf58);
    ASSERT_EQ(splittedKeypack[7], 0xe27c0);
    ASSERT_EQ(splittedKeypack[8], 0x59f28);
    ASSERT_EQ(splittedKeypack[9], 0xb0181);
}

/**
 * @brief   Test BitString constructor and output operator.
 */
TEST(Crypto, BitStringConstructorOutput) {
    std::stringstream stream("");
    stream.flags(stream.flags() | std::ios::hex);

    const char *results[5] = {"b\n", "ab \n", "ab d\n", "ab cd \n",
                              "ab cd e\n"};
    for (int8_t i = 0; i < 5; i++) {
        BitString bitString(0xABCDE, 4 * (i + 1));
        stream << bitString;
        ASSERT_EQ(stream.str(), results[i]);
        stream.str("");
    }
}

/**
 * @brief   Test splitBlocks method of BitString class.
 */
TEST(Crypto, BitStringSplitBlocks1) {
    BitString bitString({0x01, 0x23, 0x45, 0x67});
    std::vector<BitString> vector;
    bitString.splitBlocks(vector);
    BitString result1(std::vector<uint8_t>({0x01, 0x23}));
    BitString result2(std::vector<uint8_t>({0x45, 0x67}));
    ASSERT_EQ(vector[0], result1);
    ASSERT_EQ(vector[1], result2);
}

/**
 * @brief   Test splitBlocks method of BitString class.
 */
TEST(Crypto, BitStringSplitBlocks2) {
    BitString bitString;
    std::vector<BitString> vector;
    bitString.splitBlocks(vector);
    BitString result;
    ASSERT_EQ(vector[0], result);
}

/**
 * @brief   Test splitBlocks method of BitString class and 
 *          concatenate together with multi-rate padding.
 */
TEST(Crypto, BitStringSplitBlocks3) {
    std::vector<uint8_t> vector1({0x12, 0x34});
    BitString bitString(vector1);
    bitString.concatenateAndAddMultiRatePadding(0x03, 20, 16);
    std::vector<BitString> vector2;
    bitString.splitBlocks(vector2);
    std::stringstream stream("");
    stream.flags(stream.flags() | std::ios::hex);
    stream << vector2[0];
    ASSERT_EQ(stream.str(), "12 34 \n");
    stream.str("");
    stream << vector2[1];
    ASSERT_EQ(stream.str(), "f\n");
}

/**
 * @brief   Test splitBlocks method of BitString class and 
 *          concatenate together with multi-rate padding.
 */
TEST(Crypto, BitStringSplitBlocks4) {
    std::vector<uint8_t> vector1({0x12, 0x34});
    BitString bitString(vector1);
    bitString.concatenateAndAddMultiRatePadding(0x01, 20, 16);
    std::vector<BitString> vector2;
    bitString.splitBlocks(vector2);
    std::stringstream stream("");
    stream.flags(stream.flags() | std::ios::hex);
    stream << vector2[0];
    ASSERT_EQ(stream.str(), "12 34 \n");
    stream.str("");
    stream << vector2[1];
    ASSERT_EQ(stream.str(), "d\n");
}

/**
 * @brief   Test xorWith method of BitString class.
 */
TEST(Crypto, BitStringXor) {
    std::vector<uint8_t> vector1({0x01, 0x4A});
    BitString bitString1(vector1);

    std::vector<uint8_t> vector2({0x10, 0x7F});
    BitString bitString2(vector2);

    bitString1.xorWith(bitString2);

    std::vector<uint8_t> vector3({0x11, 0x35});
    BitString bitString3(vector3);

    ASSERT_EQ(bitString1, bitString3);
}

/**
 * @brief   Test toUint32 method of BitString class.
 */
TEST(Crypto, BitStringtoUint32) {
    std::vector<uint8_t> vector1({0x01, 0x4A});
    BitString bitString1(vector1);

    ASSERT_EQ(bitString1.toUint32(), 0x014A0);

    std::vector<uint8_t> vector2({0x01, 0x4A});
    BitString bitString2(vector2);
    bitString2.concatenateAndAddMultiRatePadding(0x03, 20, 16);

    ASSERT_EQ(bitString2.toUint32(), 0x014AF);

    std::vector<uint8_t> vector3({0x01});
    BitString bitString3(vector3);
    bitString3.concatenateAndAddMultiRatePadding(0x03, 12, 8);

    ASSERT_EQ(bitString3.toUint32(), 0x010F0);
}

/**
 * @brief   Test fictional QR codes.
 */
TEST(Crypto, TemporaryCryptoTest) {
    CryptoInstruction instruction = decrypt(
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x07, 0x04, 0x00, 0x02, 0x13,
         0xf9, 0x34, 0x5b, 0x49, 0x1e, 0xa1, 0xdf, 0xe2, 0x25, 0x8c, 0xbc,
         0x6d, 0xeb, 0x85, 0x01, 0x80, 0xf4, 0x9a, 0x3b, 0x13, 0xa1, 0x63,
         0x6a, 0xfb, 0x9b, 0xc3, 0x71, 0xbe, 0x99, 0x5d, 0x6e, 0x4e, 0xf5,
         0xe6, 0x25, 0xfa, 0x04, 0x21, 0x4e});

    ASSERT_EQ(instruction.getInstructionType(),
              CryptoInstruction::InstructionType::GOTO);
    ASSERT_EQ(instruction.getXCoordinate(), 8);
    ASSERT_EQ(instruction.getYCoordinate(), 6);
}