#include <bitString.hpp>

BitString::BitString(std::vector<uint8_t> input) {
    bits.reserve(2 * input.size());
    for (uint8_t byte : input) {
        bits.push_back(byte & 0x0F);
        bits.push_back((byte & 0xF0) >> 4);
    }
}

void BitString::concatenate(const BitString &otherBitString) {
    bits.insert(bits.end(), otherBitString.bits.begin(),
                otherBitString.bits.end());
}

void BitString::split(std::array<uint32_t, 10> &array) {
    for (uint16_t i = 0; i < bits.size() / 10; i++) {
        array[2 * i] = bits[10 * i + 1] | (bits[10 * i] << 4) |
                       (bits[10 * i + 3] << 8) | (bits[10 * i + 2] << 12) |
                       (bits[10 * i + 5] << 16);
        array[2 * i + 1] = bits[10 * i + 4] | (bits[10 * i + 7] << 4) |
                           (bits[10 * i + 6] << 8) | (bits[10 * i + 9] << 12) |
                           (bits[10 * i + 8] << 16);
    }
}