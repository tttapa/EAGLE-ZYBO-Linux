#include <bitString.hpp>
#include <crypto.hpp>
#include <cryptoPoller.hpp>
#include <gtest/gtest.h>
#include <keccak.hpp>
#include <ketje.hpp>
#include <swImplementation.hpp>

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
 * @brief   Test BitString conversion to byte array.
 */
TEST(Crypto, BitStringToByteArray) {
    BitString string({0x08, 0x59, 0xd4, 0x56, 0x78, 0xfd});
    unsigned char buffer[6];
    string.toByteArray(buffer, 6);

    std::stringstream stream;
    stream << std::hex << string;

    ASSERT_EQ(stream.str(), "08 59 d4 56 78 fd \n");
}

/**
 * @brief   Test SW implementation.
 */
TEST(Crypto, TestSWImplementation) {
    std::vector<uint8_t> message = {0xae, 0xb6, 0x93, 0x1d, 0x55, 0xdf, 0x17,
                                    0x2e, 0xac, 0x78, 0x0f, 0xa6, 0x7e, 0xe4,
                                    0xdd, 0xf3, 0x03, 0x00, 0xf0, 0x00};
    BitString messageBitString   = SWImplementation::hash(BitString(message));
    ASSERT_EQ(messageBitString,
              BitString({0x23, 0xdc, 0x24, 0xd4, 0x24, 0x81, 0x33, 0x52, 0x82,
                         0x30, 0x9a, 0x6e}));

    ASSERT_EQ(SWImplementation::decrypt(
                  BitString({0x23, 0xdc, 0x24, 0xd4, 0x24, 0x81, 0x33, 0x52,
                             0x82, 0x30, 0x9a, 0x6e}),
                  BitString({0x5d, 0xa4, 0xa2, 0x8f, 0x6b, 0x13, 0x19, 0xe4,
                             0x77, 0x1c}),
                  BitString({0x03, 0x00, 0xf0, 0x00, 0x08}),
                  BitString({0x8d, 0x8b, 0x92}),
                  BitString({0x2a, 0x0f, 0xde, 0xe9})),
              BitString({0x70, 0x06, 0x02}));
}