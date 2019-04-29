#pragma once

#include <Threshold.hpp>

namespace Fallback {

/**
 * @brief   Calculate the mask of 16 pixels of the RGB image, and store the mask
 *          in second parameter.
 * 
 * A fallback version of @ref NEON#applyMask(const uint8_t *, uint8_t *) that
 * doesn't use ARM NEON intrinsics.
 * 
 * @param[in]   colors 
 *              A pointer to an array of at least 16 RGB pixels (or 48 bytes).
 * @param[out]  mask 
 *              A pointer to write the mask to.
 */
inline void applyMask(const uint8_t *colors, uint8_t *mask) {
    for (uint8_t i = 0; i < 16; ++i) {
        uint8_t r = colors[3 * i + 0];
        uint8_t g = colors[3 * i + 1];
        uint8_t b = colors[3 * i + 2];

        uint8_t max   = std::max(std::max(r, g), b);
        uint8_t min   = std::min(std::min(r, g), b);
        uint8_t delta = max - min;

        // Condition for saturation: thres < s = delta / max
        bool sat_cond = delta > (max >> SATURATION_THRES_SHIFT);
        // Condition for value/brightness
        bool val_cond = max > BRIGHTNESS_THRES;
        // Condition for hue
        bool hue_cond = (r == max) && (delta >> HUE_THRES_SHIFT > abs(g - b));

        mask[i] = sat_cond && val_cond && hue_cond ? 0xFF : 0x00;
    }
}

}  // namespace Fallback