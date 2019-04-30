#pragma once

#include <array>
#include <cstdint>
#include <vector>

class BitString {
  private:
    std::vector<uint8_t> bits;

  public:
    BitString(const std::vector<uint8_t> &input);

    void concatenate(const BitString &otherBitString);
    void split(std::array<uint32_t, 10> &array);
};