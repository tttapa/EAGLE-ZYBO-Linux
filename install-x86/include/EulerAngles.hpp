#pragma once

#include <Quaternion.hpp>

struct EulerAngles {
    real_t yaw;    ///< Z
    real_t pitch;  ///< Y'
    real_t roll;   ///< X"

    /** Convert a quaternion to Euler angles. */
    MATRIX_CONSTEXPR EulerAngles(Quaternion q = Quaternion::unit())
        : EulerAngles{quat2eul(q)} {}
    /** Initialize Euler Angles. */
    MATRIX_CONSTEXPR EulerAngles(real_t yaw, real_t pitch, real_t roll)
        : yaw{yaw}, pitch{pitch}, roll{roll} {}

    /** 
     * Convert a quaternion to Euler angles.
     */
    MATRIX_CONSTEXPR static EulerAngles quat2eul(const Quaternion &q) {
        const real_t phi   = atan2(2.0 * (q[0] * q[1] + q[2] * q[3]),
                                 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]));
        const real_t theta = asin(2.0 * (q[0] * q[2] - q[3] * q[1]));
        const real_t psi   = atan2(2.0 * (q[0] * q[3] + q[1] * q[2]),
                                 1.0 - 2.0 * (q[2] * q[2] + q[3] * q[3]));
        return {psi, theta, phi};
    }

    /** 
     * Convert Euler angles to a quaternion.
     */
    MATRIX_CONSTEXPR static Quaternion eul2quat(const EulerAngles &eulerAngles) {
        real_t cy = std::cos(eulerAngles.yaw / 2);
        real_t sy = std::sin(eulerAngles.yaw / 2);
        real_t cp = std::cos(eulerAngles.pitch / 2);
        real_t sp = std::sin(eulerAngles.pitch / 2);
        real_t cr = std::cos(eulerAngles.roll / 2);
        real_t sr = std::sin(eulerAngles.roll / 2);

        return {
            cy * cp * cr + sy * sp * sr,
            cy * cp * sr - sy * sp * cr,
            sy * cp * sr + cy * sp * cr,
            sy * cp * cr - cy * sp * sr,
        };
    }

    /**
     * Convert Euler angles to a quaternion.
     */
    MATRIX_CONSTEXPR operator Quaternion() const { return eul2quat(*this); }

    MATRIX_CONSTEXPR ColVector<3> asColVector() const {
        return {roll, pitch, yaw};
    }
};