#pragma once

#include <arm_neon.h>

constexpr int8_t THRESHOLD = 6;

/**
 * @brief   Calculates @f$ \frac{R}{8} - \frac{G+B}{4} @f$ for eight BGR pixels.
 * 
 * This value is greater than @ref THRESHOLD in the grey triangle in the
 * image below (it reaches its maximum value of 63 for `#FF0000`).  
 * When the color is too dark (R is low), or when the color contains 
 * too much R and B, the value is lower than the threshold, the mask is
 * black, and the actual color is shown in the image below.
 * 
 * @image   html Colors-Mask.png
 * 
 * @param   colors
 *          A pointer to an array of at least 16 BGR pixels (or 48 bytes).
 */
inline int8x16_t applyMaskFunction(const uint8_t *colors) {
    // First, load 48 bytes of strided BGR data from the image in three vector
    // registers, one for red, one for green, and one for blue.
    uint8x16x3_t bgr = vld3q_u8(colors);
    // Shift the red vector 2 bits to the right, to divide by 4.
    uint8x16_t r_over4 = vshrq_n_u8(bgr.val[2], 2);
    // Calculate the average of the green and blue vectors.
    uint8x16_t g_plus_b_over2 = vhaddq_u8(bgr.val[1], bgr.val[0]);
    // Subtract the average of green and blue from the scaled red vector, and
    // halve the result so it fits in an 8-bit integer.
    return reinterpret_cast<int8x16_t>(vhsubq_u8(r_over4, g_plus_b_over2));
}

/**
 * @brief   Applies the mapping function, and checks if the result is greater
 *          than the threshold.
 * 
 * @param   colors 
 *          A pointer to an array of at least 16 BGR pixels (or 48 bytes).
 * @return  A vector register containing 0xFF for red pixels and 0x00 for other
 *          colors.
 */
inline uint8x16_t applyMask(const uint8_t *colors) {
    int8x16_t maskfn     = applyMaskFunction(colors);
    int8x16_t thresholdv = vdupq_n_s8(THRESHOLD);
    return reinterpret_cast<uint8x16_t>(vcgeq_s8(maskfn, thresholdv));
}

/**
 * @brief   Calculate the mask of 16 pixels of the BGR image, and store the mask
 *          in second parameter.
 * 
 * @param[in]   colors 
 *              A pointer to an array of at least 16 BGR pixels (or 48 bytes).
 * @param[out]  mask 
 *              A pointer to write the mask to.
 */
inline void applyMask(const uint8_t *colors, uint8_t *mask) {
    vst1q_u8(mask, applyMask(colors));
}