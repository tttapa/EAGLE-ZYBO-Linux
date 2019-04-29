#include <vector>
#include <cstdint>

class BitString {
    private:
    std::vector<uint8_t> bits;

    public:
    BitString(std::vector<uint8_t> input);

    void concatenate(const BitString &otherBitString);

    void split(uint32_t array[]);
}