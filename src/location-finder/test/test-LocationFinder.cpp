#include <LocationFinder.hpp>
#include <gtest/gtest.h>

TEST(OpenCV, test) { ASSERT_EQ(test(), 0); }

#ifdef __SSE2__
#include <emmintrin.h>
TEST(SSE2, compareGT) {
    int8_t values[sizeof(__m128i)] = {
        63, -64, 63, -64,  //
        63, -64, 63, -64,  //
        63, -64, 63, -64,  //
        63, -64, 63, -64,  //
    };
    int8_t zeros[sizeof(__m128i)] = {};
    __m128i a = _mm_load_si128(reinterpret_cast<__m128i *>(values));
    __m128i b = _mm_load_si128(reinterpret_cast<__m128i *>(zeros));
    __m128i c = _mm_cmpgt_epi8(a, b);

    const uint8_t *result = reinterpret_cast<const uint8_t *>(&c);
    for (size_t i = 0; i < sizeof(__m128i); ++i)
        std::cout << +result[i] << ' ';
    std::cout << std::endl;
}
#endif

#ifdef __AVX__
__m128i mullo_epi8(__m128i a, __m128i b) {
    // unpack and multiply
    __m128i dst_even = _mm_mullo_epi16(a, b);
    __m128i dst_odd =
        _mm_mullo_epi16(_mm_srli_epi16(a, 8), _mm_srli_epi16(b, 8));
    // repack
#ifdef __AVX2__
    // only faster if have access to VPBROADCASTW
    return _mm_or_si128(_mm_slli_epi16(dst_odd, 8),
                        _mm_and_si128(dst_even, _mm_set1_epi16(0xFF)));
#else
    return _mm_or_si128(_mm_slli_epi16(dst_odd, 8),
                        _mm_srli_epi16(_mm_slli_epi16(dst_even, 8), 8));
#endif
}

TEST(AVX, mult8bit) {
    int8_t values[sizeof(__m128i)] = {
        63, -64, 63, -64,  //
        63, -64, 63, -64,  //
        63, -64, 63, -64,  //
        63, -64, 63, -64,  //
    };
    int8_t twos[sizeof(__m128i)] = {
        2, 2, 2, 2,  //
        2, 2, 2, 2,  //
        2, 2, 2, 2,  //
        2, 2, 2, 2,  //
    };
    __m128i a = _mm_load_si128(reinterpret_cast<__m128i *>(values));
    __m128i b = _mm_load_si128(reinterpret_cast<__m128i *>(twos));
    __m128i c = mullo_epi8(a, b);

    const int8_t *result = reinterpret_cast<const int8_t *>(&c);
    for (size_t i = 0; i < sizeof(__m128i); ++i)
        std::cout << +result[i] << '\t';
    std::cout << std::endl;
}

struct veccolu {
    uint8_t v[16];
    operator const __m128i *() const {
        return reinterpret_cast<const __m128i *>(v);
    }
};

struct veccol {
    int8_t v[16];
    operator const __m128i *() const {
        return reinterpret_cast<const __m128i *>(v);
    }
};

TEST(AVX, colortest) {
    veccolu values = {
        255, 255, 255,  //
        255, 0,   0,    //
        0,   255, 0,    //
        0,   0,   255,  //
        255, 20,  20,   //
        0,
    };
    __m128i shiftedvalues = _mm_srli_epi16(*values, 4);
    shiftedvalues         = _mm_and_si128(shiftedvalues, _mm_set1_epi8(0x0F));

    veccol weights = {
        1, -2, -3,  //
        1, -2, -3,  //
        1, -2, -3,  //
        1, -2, -3,  //
        1, -2, -3,  //
        1,
    };

    __m128i weighted = mullo_epi8(shiftedvalues, *weights);

    const int8_t *result = reinterpret_cast<const int8_t *>(&weighted);
    for (size_t i = 0; i < 15; ++i)
        std::cout << +result[i] << '\t';
    std::cout << std::endl;

    /*

    veccol weights = {
        1, -2, -3, //
        1, -2, -3, //
        1, -2, -3, //
        1, -2, -3, //
        1, -2, -3, //
        1, //
    };
    __m128i c = mullo_epi8(*values, *weights);

    const int8_t *result = reinterpret_cast<const int8_t *>(&c);
    for (size_t i = 0; i < sizeof(__m128i); ++i)
        std::cout << +result[i] << '\t';
    std::cout << std::endl;
    // */
}
#endif