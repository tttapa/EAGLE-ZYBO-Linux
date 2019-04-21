#include <LocationFinder.hpp>
#include <gtest/gtest.h>

TEST(OpenCV, test) { ASSERT_EQ(test(), 0); }

#include <immintrin.h>

TEST(AVX, compareGT) {
    int8_t values[sizeof(__m256i)] = {
        63, -64, 63, -64, //
        63, -64, 63, -64, //
        63, -64, 63, -64, //
        63, -64, 63, -64, //
        63, -64, 63, -64, //
        63, -64, 63, -64, //
        63, -64, 63, -64, //
        63, -64, 63, -64, //
    };
    int8_t zeros[sizeof(__m256i)] = {};
    __m256i a = _mm256_load_si256(reinterpret_cast<__m256i*>(values));
    __m256i b=_mm256_load_si256(reinterpret_cast<__m256i*>(zeros));;
    __m256i c = _mm256_cmpgt_epi8(a, b);

    const int8_t *result = reinterpret_cast<const int8_t*>(&c);
    for (size_t i = 0; i < sizeof(__m256i); ++i)
        std::cout << +result[i] << ' ';
    std::cout << std::endl;
}