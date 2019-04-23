#pragma once

#include <Threshold.hpp>

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
    for (uint8_t i = 0; i < 16; ++i) {
        uint8_t r              = colors[3 * i + 2];
        uint8_t g              = colors[3 * i + 1];
        uint8_t b              = colors[3 * i + 0];
        uint8_t r_over4        = r >> 2;
        uint8_t g_plus_b_over2 = ((uint16_t) g + b) / 2;
        int8_t diff            = ((int16_t) r_over4 - g_plus_b_over2) / 2;
        mask[i]                = diff >= THRESHOLD ? 0xFF : 0x00;
    }
}