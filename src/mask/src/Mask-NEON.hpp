#pragma once

#include <Threshold.hpp>
#include <arm_neon.h>

namespace NEON {

/**
 * @brief   Checks whether the given RGB pixels are within a certain range in 
 *          HSV space.
 * 
 * @f$
 * \begin{align}
 * M &= \max(R, G, B) \\
 * m &= \min(R, G, B) \\
 * \Delta &= M - m \\
 * \end{align}
 * @f$
 * 
 * **Hue**  
 * @f$
 * H = 
 * \begin{cases} 
 *    60° (G - B) / \Delta        & M = R \\
 *    60° (B - R) / \Delta + 120° & M = G \\
 *    60° (R - G) / \Delta + 240° & M = B
 * \end{cases} \\
 * \begin{align}
 * \left|H\right| \quad<&\quad H_{thres} \\
 * \Leftrightarrow&\quad M = R \quad\wedge\quad
 *      60° \frac{\left|G - B\right|}{\Delta} \;<\; H_{thres} \\
 * \Leftrightarrow&\quad M = R \quad\wedge\quad
 *      \left|G - B\right| \;<\; \frac{\Delta}{60° / H_{thres}} \\
 * \Leftrightarrow&\quad M = R \quad\wedge\quad
 *      \left|G - B\right| 
 *      \;<\; \Delta \gg \log_2\left(\frac{60°}{H_{thres}}\right) \\
 * \end{align}
 * @f$  
 * 
 * **Saturation**  
 * @f$
 * \begin{align}
 * S \quad=&\quad 255 \frac{\Delta}{M} \;>\; S_{thres} \\
 * \Leftrightarrow&\quad \Delta \;>\; \frac{M}{255/S_{thres}} \\
 * \Leftrightarrow&\quad \Delta \;>\; M \gg \left(8 - \log_2\left(S_{thres}\right)\right)
 * \end{align}
 * @f$  
 * 
 * **Value**  
 * @f$
 * V \quad=\quad M \;>\; V_{thres}
 * @f$
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
    uint8x16_t r     = rgb.val[0];
    uint8x16_t g     = rgb.val[1];
    uint8x16_t b     = rgb.val[2];

    // Calculate the maximum and minimum of the RGB values, and their difference
    uint8x16_t max   = vmaxq_u8(vmaxq_u8(r, g), b);
    uint8x16_t min   = vminq_u8(vminq_u8(r, g), b);
    uint8x16_t delta = vsubq_u8(max, min);

    // Condition for hue: M == R && Δ / (60 / thres) > |G - B|
    // bool hue_cond = (r == max) && (delta >> HUE_THRES_SHIFT > abs(g - b));
    uint8x16_t hue_cond =
        vandq_u8(vceqq_u8(r, max),
                 vcgtq_u8(vshrq_n_u8(delta, HUE_THRES_SHIFT), vabdq_u8(g, b)));

    // Condition for saturation: Δ > M / (256 / thres)
    // bool sat_cond = delta > (max >> SATURATION_THRES_SHIFT);
    uint8x16_t sat_cond =
        vcgtq_u8(delta, vshrq_n_u8(max, SATURATION_THRES_SHIFT));

    // Condition for value/brightness: V > thres
    // bool val_cond = max > BRIGHTNESS_THRES;
    uint8x16_t val_cond = vcgtq_u8(max, vdupq_n_u8(BRIGHTNESS_THRES));

    // All conditions must be satisfied, so AND them all together.
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
 * 
 * @see     applyMask(const uint8_t *)
 */
inline void applyMask(const uint8_t *colors, uint8_t *mask) {
    vst1q_u8(mask, applyMask(colors));
}

}  // namespace NEON