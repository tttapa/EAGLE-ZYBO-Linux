#include <bitString.hpp>

#include <algorithm>
#include <deque>

BitString::BitString(const std::vector<uint8_t> &input) {
    bits.reserve(2 * input.size());
    for (uint8_t byte : input) {
        bits.push_back(byte & 0x0F);
        bits.push_back((byte & 0xF0) >> 4);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
BitString::BitString(uint32_t data, uint8_t nbBits) {
    uint8_t length = nbBits / 4;
    std::deque<uint8_t> temporaryBits;

    static const uint32_t masks[5] = {0xF0000, 0x0F000, 0x00F00, 0x000F0,
                                      0x0000F};

    // Note: fallthrough is on purpose.
    switch (length) {
        case 5: temporaryBits.push_front(data & masks[4]);
        case 4: temporaryBits.push_front((data & masks[2]) >> 8);
        case 3: temporaryBits.push_front((data & masks[3]) >> 4);
        case 2: temporaryBits.push_front((data & masks[0]) >> 16);
        case 1: temporaryBits.push_front((data & masks[1]) >> 12);
        default: break;
    }

    bits.reserve(length);
    for (uint8_t halfByte : temporaryBits)
        bits.push_back(halfByte);
}
#pragma GCC diagnostic pop

BitString::BitString(uint16_t l) {
    bits.reserve(2);
    bits.push_back((l / 8) & 0x0F);
    bits.push_back(((l / 8) & 0xF0) >> 4);
}

void BitString::split(std::array<uint32_t, 10> &array) const {
    for (uint16_t i = 0;
         i < std::min((uint16_t) 5, (uint16_t)(bits.size() / 10)); i++) {
        array[2 * i] = bits[10 * i + 5] | (bits[10 * i + 2] << 4) |
                       (bits[10 * i + 3] << 8) | (bits[10 * i] << 12) |
                       (bits[10 * i + 1] << 16);
        array[2 * i + 1] = bits[10 * i + 8] | (bits[10 * i + 9] << 4) |
                           (bits[10 * i + 6] << 8) | (bits[10 * i + 7] << 12) |
                           (bits[10 * i + 4] << 16);
    }
}

void BitString::splitBlocks(std::vector<BitString> &vector) const {
    if (bits.empty()) {
        vector.push_back(BitString());
    } else {
        vector.reserve((bits.size() / 4) + 1);
        for (uint16_t i = 0; i < bits.size() / 4; i++) {
            BitString block;
            block.bits.reserve(4);
            block.bits.push_back(bits[4 * i]);
            block.bits.push_back(bits[4 * i + 1]);
            block.bits.push_back(bits[4 * i + 2]);
            block.bits.push_back(bits[4 * i + 3]);
            vector.push_back(block);
        }

        if (bits.size() % 4) {
            BitString lastBlock;
            lastBlock.bits.reserve(bits.size() % 4);

            for (uint8_t i = (bits.size() % 4); i > 0; i--)
                lastBlock.bits.push_back(bits[bits.size() - i]);

            vector.push_back(lastBlock);
        }
    }
}

void BitString::xorWith(const BitString &otherBitString) {
    for (uint16_t i = 0; i < otherBitString.bits.size(); i++)
        bits[i] ^= otherBitString.bits[i];
}

BitString &BitString::concatenate(const BitString &otherBitString) {
    bits.insert(bits.end(), otherBitString.bits.begin(),
                otherBitString.bits.end());

    return *this;
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
    int16_t q           = (((-MLength - 2) % r) + r) % r;
    int16_t nbHalfBytes = (q + 2) / 4;

    bits.reserve(bits.size() + nbHalfBytes);

    if (nbHalfBytes == 1)
        bits.push_back(0x09);
    else {
        bits.push_back(0x01);
        for (int16_t i = 0; i < nbHalfBytes - 2; i++)
            bits.push_back(0x00);

        bits.push_back(0x08);
    }

    return *this;
}

BitString &
BitString::concatenateAndAddMultiRatePadding(uint8_t concatenationBits,
                                             int16_t r, int16_t MLength) {
    concatenationBits &= 0x03;

    int16_t q           = (((-MLength - 4) % r) + r) % r;
    int16_t nbHalfBytes = (q + 4) / 4;

    bits.reserve(bits.size() + nbHalfBytes);

    if (nbHalfBytes == 1)
        bits.push_back(concatenationBits | 0x0C);
    else {
        bits.push_back(concatenationBits | 0x04);
        for (int16_t i = 0; i < nbHalfBytes - 2; i++)
            bits.push_back(0x00);

        bits.push_back(0x08);
    }

    return *this;
}

uint16_t BitString::getLength() const { return 4 * bits.size(); }

void BitString::reserve(uint16_t nbBits) { bits.reserve(nbBits / 4); }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
uint32_t BitString::toUint32() const {
    uint32_t result = 0;

    switch (bits.size()) {
        case 5: result |= (uint32_t) bits[4];
        case 4: result |= (((uint32_t) bits[3]) << 8);
        case 3: result |= (((uint32_t) bits[2]) << 4);
        case 2: result |= (((uint32_t) bits[1]) << 16);
        case 1: return result | (((uint32_t) bits[0]) << 12);
        default: return 0; break;
    }
}
#pragma GCC diagnostic pop

bool operator==(const BitString &bitString1, const BitString &bitString2) {
    return bitString1.bits == bitString2.bits;
}

bool operator!=(const BitString &bitString1, const BitString &bitString2) {
    return bitString1.bits != bitString2.bits;
}

std::ostream &operator<<(std::ostream &out, const BitString &bitString) {
    if (out.flags() & std::ios::hex) {
        for (uint16_t i = 0; i < bitString.bits.size() / 2; i++) {
            uint8_t firstHalfByte  = bitString.bits[2 * i + 1] << 4;
            uint8_t secondHalfByte = bitString.bits[2 * i];
            if (firstHalfByte == 0)
                out << '0';

            out << ((unsigned int) (firstHalfByte | secondHalfByte))
                << (((i + 1) % 16) ? " " : "\n");
        }

        if (bitString.bits.size() % 2) {
            out << ((unsigned int) bitString.bits[bitString.bits.size() - 1])
                << "\n";
        } else if (bitString.bits.size() % 16) {
            out << "\n";
        }
    } else {
        for (uint16_t i = 0; i < bitString.bits.size(); i++) {
            static const uint8_t masks[4] = {0x01, 0x02, 0x04, 0x08};

            for (uint8_t j = 0; j < 4; j++) {
                if (bitString.bits[i] & masks[j]) {
                    out << '1';
                } else {
                    out << '0';
                }
            }

            if (!((i + 1) % 8)) {
                out << "\n";
            } else if (i % 2) {
                out << " ";
            }
        }

        if (bitString.bits.size() % 8) {
            out << "\n";
        }
    }

    return out;
}

BitString &BitString::operator=(const BitString bitString) {
    bits = bitString.bits;
    return *this;
}