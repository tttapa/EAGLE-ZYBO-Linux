#include <cstdint>
#include <vector>
#include <array>

class BitString {
  private:
    std::vector<uint8_t> bits;

  public:
    BitString(std::vector<uint8_t> input);

    void concatenate(const BitString &otherBitString);

    void split(uint32_t array[]);
    void split(uint32_t (&array)[10]);
    void split(std::array<uint32_t, 10> &array);
};