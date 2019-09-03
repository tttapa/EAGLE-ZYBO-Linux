#pragma once

#include <math/cmath>  // M_PI

/// Convert radians to degrees
template <class T>
constexpr inline T rad2deg(T r) {
    return r * 180 * M_1_PI;
}
/// Convert degrees to radians
template <class T>
constexpr inline T deg2rad(T d) {
    return d / 180 * M_PI;
}

/// Convert degrees to radians
constexpr long double operator"" _deg(long double deg) { return deg2rad(deg); }
/// Convert degrees to radians
constexpr long double operator"" _deg(unsigned long long deg) {
    return deg2rad<long double>(deg);
}
