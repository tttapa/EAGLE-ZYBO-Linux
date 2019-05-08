#include <EagleCrypt.h>
#include <bitString.hpp>
#include <crypto.hpp>
#include <cryptoPoller.hpp>
#include <gtest/gtest.h>
#include <keccak.hpp>
#include <ketje.hpp>

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

    std::cout << std::hex << "\n" << keypack << "\n";
    std::cout << std::dec << keypack << "\n";

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

    const char *results[5] = {"b\n", "ab \n", "ab d\n", "ab cd \n",
                              "ab cd e\n"};
    for (int8_t i = 0; i < 5; i++) {
        BitString bitString(0xABCDE, 4 * (i + 1));
        stream << std::hex << bitString;
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
    stream << std::hex << vector2[0];
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
    bitString.concatenateAndAddMultiRatePadding(0x00, 20, 16);
    std::vector<BitString> vector2;
    bitString.splitBlocks(vector2);
    std::stringstream stream("");
    stream << std::hex << vector2[0];
    ASSERT_EQ(stream.str(), "12 34 \n");
    stream.str("");
    stream << vector2[1];
    ASSERT_EQ(stream.str(), "c\n");
}

/**
 * @brief   Test splitBlocks method of BitString class and 
 *          concatenate together with multi-rate padding.
 */
TEST(Crypto, BitStringSplitBlocks5) {
    std::vector<uint8_t> vector1({0x12, 0x34});
    BitString bitString(vector1);
    bitString.concatenateAndAddMultiRatePadding(0x01, 20, 16);
    std::vector<BitString> vector2;
    bitString.splitBlocks(vector2);
    std::stringstream stream("");
    stream << std::hex << vector2[0];
    ASSERT_EQ(stream.str(), "12 34 \n");
    stream.str("");
    stream << vector2[1];
    ASSERT_EQ(stream.str(), "d\n");
}

/**
 * @brief   Test splitBlocks method of BitString class.
 */
TEST(Crypto, BitStringSplitBlocks6) {
    std::vector<uint8_t> vector1({0x12, 0x34, 0x56});
    BitString bitString(vector1);
    std::vector<BitString> vector2;
    bitString.splitBlocks(vector2);
    std::stringstream stream("");
    stream << std::hex << vector2[0];
    ASSERT_EQ(stream.str(), "12 34 \n");
    stream.str("");
    stream << vector2[1];
    ASSERT_EQ(stream.str(), "56 \n");
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

/**
 * @brief   Test Ketje initialize and unwrap.
 */
TEST(Crypto, KetjeTest1) {
    Ketje ketje(BitString({0xfe, 0x97, 0x30, 0xc9, 0x62, 0xfb, 0x94, 0x2d,
                           0xc6, 0x5f, 0xf8, 0x91, 0x2a, 0xc3, 0x5c, 0xf5,
                           0x8e, 0x27, 0xc0, 0x59, 0xf2, 0x8b}));

    ketje.initialize(BitString());
    ASSERT_EQ(ketje.unwrap(
                  BitString(), BitString(),
                  BitString({0x1a, 0x9a, 0x90, 0xe0, 0x39, 0x37, 0x19, 0xf7,
                             0x29, 0xb0, 0x46, 0xa4, 0x98, 0x9a, 0xa5, 0x11})),
              BitString());

    std::cout << std::endl << std::endl;

    ASSERT_EQ(ketje.unwrap(
                  BitString(),
                  BitString({0xf7, 0x37, 0xdf, 0x7b, 0x9e, 0x0f, 0x80, 0xb4,
                             0x79, 0xce, 0xb7}),
                  BitString({0xf5, 0xf5, 0x91, 0x4f, 0x1e, 0x10, 0x68, 0x5e,
                             0x6b, 0xc5, 0x84, 0xdd, 0x28, 0x14, 0xf7, 0x21})),
              BitString({0x4b, 0x6c, 0xec, 0x0d, 0x8d, 0xae, 0x2e, 0x4f, 0xcf,
                         0xf0, 0x70}));

    ASSERT_EQ(
        ketje.unwrap(
            BitString(),
            BitString({0xcc, 0x7d, 0x26, 0x44, 0x20, 0xd9, 0xc9, 0xf1, 0xed,
                       0xcf, 0x54, 0xf8, 0x93, 0x9b, 0x04, 0x34, 0x2f, 0xd1,
                       0x7b, 0x54, 0xa0, 0xe9, 0x44, 0xb6, 0xeb, 0x37, 0xcf}),
            BitString({0xdc, 0x52, 0xc8, 0x99, 0xb1, 0x75, 0x00, 0x75, 0xf9,
                       0xf7, 0x8b, 0x29, 0xaf, 0x5c, 0xa9, 0xb0})),
        BitString({0x6b, 0x8c, 0x0c, 0x2d, 0xad, 0xce, 0x4e, 0x6f, 0xef,
                   0x10, 0x90, 0xb1, 0x31, 0x52, 0xd2, 0xf3, 0x73, 0x94,
                   0x14, 0x35, 0xb5, 0xd6, 0x56, 0x77, 0xf7, 0x18, 0x98}));

    ASSERT_EQ(ketje.unwrap(
                  BitString({0x3a, 0xcb, 0x5c, 0xed, 0x7e, 0x0f, 0xa0, 0x31,
                             0xc2, 0x53, 0xe4}),
                  BitString(),
                  BitString({0xe4, 0x85, 0x40, 0x90, 0xed, 0xf6, 0x2a, 0x84,
                             0x9a, 0x6e, 0xe2, 0x4b, 0x85, 0x96, 0x1b, 0x4d})),
              BitString());

    ASSERT_EQ(ketje.unwrap(
                  BitString({0x51, 0xe2, 0x73, 0x04, 0x95, 0x26, 0xb7, 0x48,
                             0xd9, 0x6a, 0xfb}),
                  BitString({0x3f, 0x78, 0x85, 0xb4, 0x38, 0x3f, 0xa3, 0xef,
                             0x6e, 0x8d, 0xb5, 0xb8, 0x2a, 0x72, 0xdf, 0xe7,
                             0x20, 0x68, 0x06, 0x80, 0x12, 0x79, 0xae}),
                  BitString({0x41, 0xe3, 0xda, 0x56, 0x2c, 0x81, 0x9e, 0xec,
                             0xdd, 0x3f, 0x84, 0xd0, 0xb5, 0xdb, 0xe1, 0xfb})),
              BitString({0xee, 0x0f, 0x8f, 0xb0, 0x30, 0x51, 0xd1, 0xf2,
                         0x72, 0x93, 0x13, 0x34, 0xb4, 0xd5, 0x55, 0x76,
                         0xf6, 0x17, 0x97, 0xb8, 0x38, 0x59, 0xd9}));
}

/**
 * @brief   Test Ketje initialize and unwrap.
 */
TEST(Crypto, KetjeTest2) {
    Ketje ketje(BitString({0xb4, 0x4d, 0xe6, 0x7f, 0x18, 0xb1, 0x4a, 0xe3, 0x7c,
                           0x15, 0xae, 0x47}));

    ketje.initialize(BitString(
        {0x83, 0xe4, 0x45, 0xa6, 0x66, 0xc7, 0x28, 0x89, 0x49, 0xaa}));
    ASSERT_EQ(ketje.unwrap(
                  BitString(), BitString(),
                  BitString({0x6a, 0x16, 0xbf, 0xa5, 0x2a, 0xdc, 0x43, 0x5b,
                             0x4a, 0x0d, 0xab, 0x59, 0x5c, 0x65, 0xa2, 0x26})),
              BitString());

    std::vector<uint8_t> cipherText({0xe0});
    std::vector<uint8_t> plainText({0xf7});
    ASSERT_EQ(ketje.unwrap(
                  BitString(), BitString(cipherText),
                  BitString({0xab, 0xde, 0x67, 0x18, 0x8f, 0x43, 0x72, 0xe1,
                             0xfc, 0x83, 0x5d, 0xe2, 0x5e, 0x9f, 0x82, 0xb3})),
              BitString(plainText));
}

/**
 * @brief   Test Ketje initialize and unwrap with QR
 *          code data.
 */
TEST(Crypto, KetjeTest3) {
    Ketje ketje(BitString({0x0a, 0x62, 0x85, 0xc7, 0x27, 0x98, 0x10, 0x22, 0x66,
                           0x1f, 0x7c, 0x04}));

    ketje.initialize(BitString(
        {0x53, 0x9d, 0x08, 0x8b, 0xc0, 0x13, 0xfe, 0x89, 0x96, 0x39}));

    std::vector<uint8_t> cipherText({0x9c});
    std::vector<uint8_t> plainText({0x7f});
    ASSERT_EQ(ketje.unwrap(BitString({0x04, 0x04, 0xfc, 0x00, 0x08}),
                           BitString(cipherText),
                           BitString({0x94, 0xcb, 0x04, 0x8a})),
              BitString(plainText));
}