#pragma once

#include <cmath>
#include <cstdint>

/// The minimum saturation of the lines @f$ s \in {2, 4, 8, 16, 32, 64, 128} @f$.
const int SATURATION_THRES       = 64;
const int SATURATION_THRES_SHIFT = 8 - round(log2(SATURATION_THRES));
/// The minimum brightness of the lines @f$ b \in [0, 255] @f$.
const int BRIGHTNESS_THRES = 32;
/// The maximum absolute hue angle of the lines in degrees
/// @f$ h \in {3.75, 7.5, 15, 30, 60} @f$.
const float HUE_THRES_DEGREES = 30;
const int HUE_THRES_SHIFT     = round(log2(60 / HUE_THRES_DEGREES));
