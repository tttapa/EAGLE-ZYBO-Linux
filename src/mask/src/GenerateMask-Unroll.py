from sys import argv
import os 
dir_path = os.path.dirname(os.path.realpath(__file__))

if len(argv) != 2:
    print("Usage: {} <unroll factor>".format(argv[0]))
    exit()

with open(dir_path + "/GenerateMask-Unroll.ipp", 'w') as f:
    f.write("""\
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

""")

    f.write("""\
    // https://en.wikipedia.org/wiki/Duff%27s_device
    constexpr size_t UNROLL = {};
    size_t i                = (n + UNROLL - 1) / UNROLL;
    switch (n % UNROLL) {{
        case 0: do {{ applyMaskImpl(rgb_p++, result_p++);\n""".format(argv[1]))
    for i in range(int(argv[1]) - 1, 0, -1):
        f.write("                case {}: applyMaskImpl(rgb_p++, result_p++);\n"
                .format(i))

    f.write("""\
            } while (--i > 0);
    }
    return result;
}

#pragma GCC diagnostic pop
""")