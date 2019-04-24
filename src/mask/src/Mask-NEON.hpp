#pragma once

#include <Threshold.hpp>
#include <arm_neon.h>
#include <cmath>

const int SATURATION_THRES       = 16;
const int SATURATION_THRES_SHIFT = round(log2(SATURATION_THRES));
const int BRIGHTNESS_THRES       = 16;
const int HUE_THRES_DEGREES      = 15;
const int HUE_THRES_SHIFT        = round(log2(60 / HUE_THRES_DEGREES));

/**
 * @brief   Checks whether the given RGB pixels are within a certain range in 
 *          HSV space.
 * 
 * @image   html Colors-Mask-Light.png
 * @image   html Colors-Mask-Dark.png
 * @image   html Colors-Mask-Gradient.png
 * 
 * @param   colors
 *          A pointer to an array of at least 16 RGB pixels (or 48 bytes).
 */
inline uint8x16_t applyMask(const uint8_t *colors) {
    // First, load 48 bytes of strided RGB data from the image in three vector
    // registers, one for red, one for green, and one for blue.
    uint8x16x3_t rgb = vld3q_u8(colors);
    uint8x16_t r     = colors[0];
    uint8x16_t g     = colors[1];
    uint8x16_t b     = colors[2];

    uint8x16_t max   = vmaxq_u8(vmaxq_u8(r, g), b);
    uint8x16_t min   = vminq_u8(vminq_u8(r, g), b);
    uint8x16_t delta = vsubq_u8(max, min);

    // Condition for saturation: thres < s = delta / max
    uint8x16_t sat_cond =
        vcgtq_u8(delta, vshrq_n_u8(max, SATURATION_THRES_SHIFT));
    // Condition for value/brightness
    uint8x16_t val_cond = vcgtq_u8(max, vdupq_n_u8(BRIGHTNESS_THRES));
    // Condition for hue
    uint8x16_t hue_cond =
        vandq_u8(vceqq_u8(r, max),
                 vcgtq_u8(vshrq_n_u8(delta, HUE_THRES_SHIFT), vabdq_u8(g, b)));

    return vandq_u8(vandq_u8(sat_cond, val_cond), hue_cond);
}

/**
 * @brief   Calculate the mask of 16 pixels of the RGB image, and store the mask
 *          in second parameter.
 * 
 * @param[in]   colors 
 *              A pointer to an array of at least 16 RGB pixels (or 48 bytes).
 * @param[out]  mask 
 *              A pointer to write the mask to.
 */
inline void applyMask(const uint8_t *colors, uint8_t *mask) {
    vst1q_u8(mask, applyMask(colors));
}