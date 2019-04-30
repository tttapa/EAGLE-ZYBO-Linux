#include <bitString.hpp>

BitString::BitString(const std::vector<uint8_t> &input) {
    bits.reserve(2 * input.size());
    for (uint8_t byte : input) {
        bits.push_back(byte & 0x0F);
        bits.push_back((byte & 0xF0) >> 4);
    }
}

BitString::BitString(uint8_t l) {
    bits.push_back((l / 8) & 0x0F);
    bits.push_back((l / 8) >> 4);
}

BitString::BitString(uint32_t data, uint8_t length) {
    uint32_t mask[5] = {0xF0000, 0x0F000, 0x00F00, 0x000F0, 0x0000F};
    if (length > 0)
        (data & mask[1]) >> 12;
    if (length > 1)
        (data & mask[0]) >> 16;
    if (length > 2)
        (data & mask[2]) >> 12;
    if (length > 3)
        (data & mask[2]) >> 12;
}

BitString &BitString::concatenate(const BitString &otherBitString) {
    bits.insert(bits.end(), otherBitString.bits.begin(),
                otherBitString.bits.end());

    return *this;
}

void BitString::split(std::array<uint32_t, 10> &array) {
    for (uint16_t i = 0; i < bits.size() / 10; i++) {
        array[2 * i] = bits[10 * i + 5] | (bits[10 * i + 2] << 4) |
                       (bits[10 * i + 3] << 8) | (bits[10 * i] << 12) |
                       (bits[10 * i + 1] << 16);
        array[2 * i + 1] = bits[10 * i + 8] | (bits[10 * i + 9] << 4) |
                           (bits[10 * i + 6] << 8) | (bits[10 * i + 7] << 12) |
                           (bits[10 * i + 4] << 16);
    }
}

void BitString::splitBlock(std::vector<BitString> &vector) const {
    if (bits.empty())
        vector.push_back(BitString({0x00}));
    else {
        for (uint16_t i = 0; i < bits.size() / 4; i++) {
            vector.push_back(BitString({bits[4 * i], bits[4 * i + 1],
                                        bits[4 * i + 2], bits[4 * i + 3]}));
        }
        switch (bits.size() % 4) {
            case 1: vector.push_back(BitString({bits[bits.size() - 1]})); break;

            case 2:
                vector.push_back(
                    BitString({bits[bits.size() - 2], bits[bits.size() - 1]}));
                break;

            case 3:
                vector.push_back(
                    BitString({bits[bits.size() - 3], bits[bits.size() - 2],
                               bits[bits.size() - 1]}));
                break;

            default: break;
        }
    }
}

BitString &BitString::addSimplePadding(int16_t r, int16_t MLength) {
    int16_t q           = (((-MLength - 1) % r) + r) % r;
    int16_t nbHalfBytes = (q + 1) / 4;

    bits.reserve(bits.size() + nbHalfBytes);
    bits.push_back(0x01);
    for (int16_t i = 0; i < nbHalfBytes - 1; i++)
        bits.push_back(0x00);

    return *this;
}

BitString &BitString::addMultiRatePadding(int16_t r, int16_t MLength) {
    uint16_t q           = (((-MLength - 2) % r) + r) % r;
    uint16_t nbHalfBytes = (q + 2) / 4;

    bits.reserve(bits.size() + nbHalfBytes);

    if (nbHalfBytes == 1)
        bits.push_back(0x09);
    else {
        bits.push_back(0x01);
        for (int i = 0; i < nbHalfBytes - 2; i++)
            bits.push_back(0x00);

        bits.push_back(0x08);
    }

    return *this;
}

BitString &
BitString::concatenateAndAddMultiRatePadding(uint8_t concatenationBits,
                                             int16_t r, int16_t MLength) {
    concatenationBits &= 0x03;

    uint16_t q           = (((-MLength - 4) % r) + r) % r;
    uint16_t nbHalfBytes = (q + 4) / 4;

    bits.reserve(bits.size() + nbHalfBytes);

    if (nbHalfBytes == 1)
        bits.push_back(concatenationBits | 0x0C);
    else {
        bits.push_back(concatenationBits | 0x04);
        for (int i = 0; i < nbHalfBytes - 2; i++)
            bits.push_back(0x00);

        bits.push_back(0x08);
    }

    return *this;
}

uint32_t BitString::toUint32() {
    for (int i = bits.size(); i < 5; i++) {
        bits.push_back(0x0);
    }
    return bits[5] | (bits[2] << 4) | (bits[3] << 8) | (bits[0] << 12) |
           (bits[1] << 16);
}

void BitString::reserveBits(uint16_t nbBits) { bits.reserve(nbBits / 4); }

uint16_t BitString::getLength() { return 4 * bits.size(); }

bool operator==(const BitString &bitstring1, const BitString &bitstring2) {
    return bitstring1.bits == bitstring2.bits;
}

std::ostream &operator<<(std::ostream &out, const BitString &bitstring) {
    out << std::hex;
    out << bitstring.bits.size();
    out << "\n";
    for (uint16_t i = 0; i < bitstring.bits.size() / 2; i++) {
        uint8_t firstHalfByte  = bitstring.bits[2 * i + 1] << 4;
        uint8_t secondHalfByte = bitstring.bits[2 * i];
        out << (firstHalfByte | secondHalfByte) << " ";
    }
    if (bitstring.bits.size() % 2) {
        out << bitstring.bits[bitstring.bits.size() - 1];
    }
    out << std::dec;
    return out;
}
