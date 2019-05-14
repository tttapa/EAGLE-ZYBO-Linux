#include <Mask.hpp>

#ifdef __ARM_NEON
#include <Mask-NEON.hpp>
void (&applyMaskImpl)(const uint8_t *, uint8_t *) = NEON::applyMask;
#else
#include <Mask-Fallback.hpp>
void (&applyMaskImpl)(const uint8_t *, uint8_t *) = Fallback::applyMask;
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

std::vector<uint8_t> Mask::generateMask(const uint8_t *imgRGB,
                                        size_t numberOfPixels) {
    if (numberOfPixels == 0)
        return {};

    std::vector<uint8_t> result;
    result.resize(getPaddedLength(numberOfPixels));

    struct {
        auto operator++(int) {
            auto res = *this;
            p += 3 * 16;
            return res;
        }
        operator const uint8_t *() const { return p; }
        const uint8_t *p;
    } rgb_p = {imgRGB};

    struct {
        auto operator++(int) {
            auto res = *this;
            p += 16;
            return res;
        }
        operator uint8_t *() const { return p; }
        uint8_t *p;
    } result_p = {result.data()};

    size_t n = result.size() / 16;  // The number of mask applications

    // https://en.wikipedia.org/wiki/Duff%27s_device
    constexpr size_t UNROLL = 64;
    size_t i                = (n + UNROLL - 1) / UNROLL;
    switch (n % UNROLL) {
        case 0: do { applyMaskImpl(rgb_p++, result_p++);
                case 63: applyMaskImpl(rgb_p++, result_p++);
                case 62: applyMaskImpl(rgb_p++, result_p++);
                case 61: applyMaskImpl(rgb_p++, result_p++);
                case 60: applyMaskImpl(rgb_p++, result_p++);
                case 59: applyMaskImpl(rgb_p++, result_p++);
                case 58: applyMaskImpl(rgb_p++, result_p++);
                case 57: applyMaskImpl(rgb_p++, result_p++);
                case 56: applyMaskImpl(rgb_p++, result_p++);
                case 55: applyMaskImpl(rgb_p++, result_p++);
                case 54: applyMaskImpl(rgb_p++, result_p++);
                case 53: applyMaskImpl(rgb_p++, result_p++);
                case 52: applyMaskImpl(rgb_p++, result_p++);
                case 51: applyMaskImpl(rgb_p++, result_p++);
                case 50: applyMaskImpl(rgb_p++, result_p++);
                case 49: applyMaskImpl(rgb_p++, result_p++);
                case 48: applyMaskImpl(rgb_p++, result_p++);
                case 47: applyMaskImpl(rgb_p++, result_p++);
                case 46: applyMaskImpl(rgb_p++, result_p++);
                case 45: applyMaskImpl(rgb_p++, result_p++);
                case 44: applyMaskImpl(rgb_p++, result_p++);
                case 43: applyMaskImpl(rgb_p++, result_p++);
                case 42: applyMaskImpl(rgb_p++, result_p++);
                case 41: applyMaskImpl(rgb_p++, result_p++);
                case 40: applyMaskImpl(rgb_p++, result_p++);
                case 39: applyMaskImpl(rgb_p++, result_p++);
                case 38: applyMaskImpl(rgb_p++, result_p++);
                case 37: applyMaskImpl(rgb_p++, result_p++);
                case 36: applyMaskImpl(rgb_p++, result_p++);
                case 35: applyMaskImpl(rgb_p++, result_p++);
                case 34: applyMaskImpl(rgb_p++, result_p++);
                case 33: applyMaskImpl(rgb_p++, result_p++);
                case 32: applyMaskImpl(rgb_p++, result_p++);
                case 31: applyMaskImpl(rgb_p++, result_p++);
                case 30: applyMaskImpl(rgb_p++, result_p++);
                case 29: applyMaskImpl(rgb_p++, result_p++);
                case 28: applyMaskImpl(rgb_p++, result_p++);
                case 27: applyMaskImpl(rgb_p++, result_p++);
                case 26: applyMaskImpl(rgb_p++, result_p++);
                case 25: applyMaskImpl(rgb_p++, result_p++);
                case 24: applyMaskImpl(rgb_p++, result_p++);
                case 23: applyMaskImpl(rgb_p++, result_p++);
                case 22: applyMaskImpl(rgb_p++, result_p++);
                case 21: applyMaskImpl(rgb_p++, result_p++);
                case 20: applyMaskImpl(rgb_p++, result_p++);
                case 19: applyMaskImpl(rgb_p++, result_p++);
                case 18: applyMaskImpl(rgb_p++, result_p++);
                case 17: applyMaskImpl(rgb_p++, result_p++);
                case 16: applyMaskImpl(rgb_p++, result_p++);
                case 15: applyMaskImpl(rgb_p++, result_p++);
                case 14: applyMaskImpl(rgb_p++, result_p++);
                case 13: applyMaskImpl(rgb_p++, result_p++);
                case 12: applyMaskImpl(rgb_p++, result_p++);
                case 11: applyMaskImpl(rgb_p++, result_p++);
                case 10: applyMaskImpl(rgb_p++, result_p++);
                case 9: applyMaskImpl(rgb_p++, result_p++);
                case 8: applyMaskImpl(rgb_p++, result_p++);
                case 7: applyMaskImpl(rgb_p++, result_p++);
                case 6: applyMaskImpl(rgb_p++, result_p++);
                case 5: applyMaskImpl(rgb_p++, result_p++);
                case 4: applyMaskImpl(rgb_p++, result_p++);
                case 3: applyMaskImpl(rgb_p++, result_p++);
                case 2: applyMaskImpl(rgb_p++, result_p++);
                case 1: applyMaskImpl(rgb_p++, result_p++);
            } while (--i > 0);
    }
    return result;
}

#pragma GCC diagnostic pop
