#pragma once

#include <array>
#include <cryptoInstruction.hpp>
#include <cstdint>
#include <iostream>
#include <vector>

class BitString {

  private:
    std::vector<uint8_t> bits;

  public:
    BitString() {}
    BitString(const std::vector<uint8_t> &input, int16_t start = 0,
              int16_t end = -1);
    BitString(uint32_t data, uint8_t nbBits);
    BitString(uint16_t l);

    void split(std::array<uint32_t, 10> &array) const;
    void splitBlocks(std::vector<BitString> &vector) const;

    void xorWith(const BitString &otherBitString);

    BitString &concatenate(const BitString &otherBitString);
    BitString &addSimplePadding(int16_t r, int16_t MLength);
    BitString &addMultiRatePadding(int16_t r, int16_t MLength);
    BitString &concatenateAndAddMultiRatePadding(uint8_t concatenationBits,
                                                 int16_t r, int16_t MLength);

    uint16_t getLength() const;
    void reserve(uint16_t nbBits);

    uint32_t toUint32() const;
    void toByteArray(unsigned char *buffer, uint16_t bufferLength) const;
    void toVector(std::vector<uint8_t> &vector) const;
    CryptoInstruction toCryptoInstruction(uint8_t currentX, uint8_t currentY);

    friend bool operator==(const BitString &bitString1,
                           const BitString &bitString2);

    friend bool operator!=(const BitString &bitString1,
                           const BitString &bitString2);

    friend std::ostream &operator<<(std::ostream &out,
                                    const BitString &bitString);

    BitString &operator=(const BitString bitString);
};