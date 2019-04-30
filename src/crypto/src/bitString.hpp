#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

class BitString {
  private:
    std::vector<uint8_t> bits;

  public:
    BitString(const std::vector<uint8_t> &input);
    BitString(uint8_t l);
    BitString(uint32_t data, uint8_t length);

    BitString &concatenate(const BitString &otherBitString);
    void split(std::array<uint32_t, 10> &array);
    void splitBlock(std::vector<BitString> &vector) const;

    BitString &addSimplePadding(int16_t r, int16_t MLength);
    BitString &addMultiRatePadding(int16_t r, int16_t MLength);

    BitString &concatenateAndAddMultiRatePadding(uint8_t concatenationBits,
                                                 int16_t r, int16_t MLength);

    uint32_t toUint32();

    void reserveBits(uint16_t nbBits);

    uint16_t getLength();

    friend bool operator==(const BitString &bitstring1,
                           const BitString &bitstring2);

    friend std::ostream &operator<<(std::ostream &out,
                                    const BitString &bitstring);
};