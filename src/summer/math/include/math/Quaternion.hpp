#pragma once

#include <limits>           // std::numeric_limits
#include <math/Vector.hpp>  // Vec3f
#include <math/cmath>       // std::sqrt
#include <ostream>          // std::ostream

/**
 *  @brief  Type for quaternions of floating point numbers.
 * 
 *  Quaternions can be multiplied (hamiltonian product), normalized and can
 *  perform rotations of vectors. Quaternion also has an implementation of the
 *  following operators:
 *      "-" (negation);
 *      "+", "+=", "-", "-=" (quat-quat);
 *      "*", "*=", "/", "/=" (quat-scalar);
 *      "==", "!=" (equality);
 *      "*" (scalar-quat);
 *      "<<" (printing).
 */
struct Quaternion {
    float w = 1.0;
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    /// Create a quaternion that is initialized to the identity quaternion.
    Quaternion() = default;
    /// Create a quaterion with the given values for w, x, y and z.
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

    /// Sum of two quaterions uses quaternion multiplication.
    Quaternion &operator+=(Quaternion rhs) {
        return *this = hamiltonianProduct(*this, rhs);
    }
    /// Sum of two quaternions uses quaternion multiplication.
    Quaternion operator+(Quaternion rhs) const {
        return hamiltonianProduct(*this, rhs);
    }

    /// Complex conjugate (doesn't change the original quaternion).
    Quaternion conjugated() const { return {w, -x, -y, -z}; }
    /// Negated quaternion is its conjugate.
    Quaternion operator-() const { return conjugated(); }
    /// Difference of two quaternions `a` and `b` is the quaternion
    /// multiplication of `a` and the conjugate of `b`.
    Quaternion &operator-=(Quaternion rhs) { return *this += -rhs; }
    /// Difference of two quaternions `a` and `b` is the quaternion
    /// multiplication of `a` and the conjugate of `b`.
    Quaternion operator-(Quaternion rhs) const {
        Quaternion result = *this;
        result -= rhs;
        return result;
    }

    /// Scalar multiplication.
    Quaternion &operator*=(float rhs) {
        w *= rhs;
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }
    /// Scalar multiplication.
    Quaternion operator*(float rhs) const {
        Quaternion result = *this;
        result *= rhs;
        return result;
    }

    /// Scalar division.
    Quaternion &operator/=(float rhs) {
        w /= rhs;
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }
    /// Scalar division.
    Quaternion operator/(float rhs) const {
        Quaternion result = *this;
        result /= rhs;
        return result;
    }

    /// Norm squared.
    float normSquared() const { return w * w + x * x + y * y + z * z; }
    /// Norm.
    float norm() const { return std::sqrt(normSquared()); }
    /// Normalize this quaternion.
    Quaternion &normalize() { return *this /= norm(); }
    /// Normalize a copy of this quaternion (doesn't change the original
    /// quaternion).
    Quaternion normalized() const { return *this / norm(); }

    /**
     * @brief   Rotate vector by this quaternion.
     * 
     * @note    This function uses the normalized version of this quaternion.
     * 
     * @note    This function is not the same as `quatrotate` in MATLAB!
     *          MATLAB rotates by the conjugate of the quaternion, while this
     *          function rotates by the quaternion itself.
     */
    Vec3f rotate(Vec3f v) const {
        // Source:
        // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Quaternion-derived_rotation_matrix

        // Compare to the MATLAB convetions:
        // https://www.mathworks.com/matlabcentral/answers/352465-what-is-the-aerospace-blockset-quaternion-convention
        // https://www.mathworks.com/help/aerotbx/ug/quatrotate.html#mw_f4c2628b-30a7-4b62-a227-7fb9ef158187

        // Normalized quaternion.
        Quaternion q = normalized();

        // Rotation matrix.
        float M11 = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
        float M12 = 2.0 * (q.x * q.y - q.w * q.z);
        float M13 = 2.0 * (q.x * q.z + q.w * q.y);
        float M21 = 2.0 * (q.x * q.y + q.w * q.z);
        float M22 = 1.0 - 2.0 * q.x * q.x - 2.0 * q.z * q.z;
        float M23 = 2.0 * (q.y * q.z - q.w * q.x);
        float M31 = 2.0 * (q.x * q.z - q.w * q.y);
        float M32 = 2.0 * (q.y * q.z + q.w * q.x);
        float M33 = 1.0 - 2.0 * q.x * q.x - 2.0 * q.y * q.y;

        return Vec3f{
            M11 * v.x + M12 * v.y + M13 * v.z,
            M21 * v.x + M22 * v.y + M23 * v.z,
            M31 * v.x + M32 * v.y + M33 * v.z,
        };
    }

    /// Equality check.
    bool operator==(Quaternion rhs) const {
        return this->w == rhs.w && this->x == rhs.x && this->y == rhs.y &&
               this->z == rhs.z;
    }
    /// Inequality check.
    bool operator!=(Quaternion rhs) const { return !(*this == rhs); }

    /// Identity quaternion (1,0,0,0).
    static Quaternion identity() { return {1, 0, 0, 0}; }

    /** 
     *  @brief  Calculate the quaternion that satisfies the following: 
     *          `result.rotate(Vec3f{0, 0, 1}) == v.normalized()`.
     */
    static Quaternion fromDirection(Vec3f v) {
        /*
         * Formula:
    	 * q = cos(ϑ / 2) + sin(ϑ / 2)·(x·i + y·j + z·k)
    	 * where (x y z) is a unit vector representing the axis about which
    	 * the body is rotated; ϑ is the angle by which it is rotated.
    	 * 
         * Source: 
         * https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Using_quaternion_as_rotations
         * 
    	 * The rotational axis (x y z) can be calcuated by taking the normalized
         * cross product of (0 0 1) and the given vector. The angle of rotation
         * ϑ can be found using |A×B| = |A||B|·sin(ϑ).
    	 */

        // First check the edge case, where v ~= (0 0 1).
        float eps = std::numeric_limits<float>::epsilon();
        if (std::abs(v.x) <= eps && std::abs(v.y) <= eps)
            return Quaternion::identity();

        // Calculate the cross product and its norm.
        Vec3f cross     = {-v.y, v.x, 0};
        float crossNorm = cross.norm();
        cross /= crossNorm;

        // Calculate the angle ϑ.
        float angle = std::asin(crossNorm / v.norm());

        // Calculate the resulting quaternion.
        return {
            std::cos(angle / 2),            //
            std::sin(angle / 2) * cross.x,  //
            std::sin(angle / 2) * cross.y,  //
            std::sin(angle / 2) * cross.z,  //
        };
    }

    /// Quaternion multiplication.
    static Quaternion hamiltonianProduct(Quaternion q, Quaternion r) {
        return {
            r.w * q.w - r.x * q.x - r.y * q.y - r.z * q.z,
            r.w * q.x + r.x * q.w - r.y * q.z + r.z * q.y,
            r.w * q.y + r.x * q.z + r.y * q.w - r.z * q.x,
            r.w * q.z - r.x * q.y + r.y * q.x + r.z * q.w,
        };
    }
};

/// Scalar multiplication.
inline Quaternion operator*(float lhs, Quaternion rhs) {
    return {lhs * rhs.w, lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

/// Printing.
inline std::ostream &operator<<(std::ostream &os, Quaternion q) {
    return os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
}

/**
 *  @brief  Struct for Euler angles of floating point numbers.
 * 
 *  EulerAngles provides the conversions between Euler angles and quaternions.
 *  It also has an implementation of the following operators:
 *      "==", "!=" (equality);
 *      "<<" (printing).
 */
struct EulerAngles {
    float yaw   = 0.0;  ///< Z : drone Z = world +Z.
    float pitch = 0.0;  ///< Y': drone Y = world -X.
    float roll  = 0.0;  ///< X": drone X = world +Y.

    /// Create Euler angles that are initialized to (0 0 0), or upright.
    EulerAngles() = default;
    /// Create Euler angles with the given values for yaw, pitch and roll.
    EulerAngles(float yaw, float pitch, float roll)
        : yaw(yaw), pitch(pitch), roll(roll) {}
    /// Create Euler angles from the given quaternion.
    EulerAngles(Quaternion q) : EulerAngles{quat2eul(q)} {}

    /// Implicitly convert these Euler angles to a quaternion.
    operator Quaternion() const { return eul2quat(*this); }

    /// Equality check.
    bool operator==(EulerAngles rhs) const {
        return this->yaw == rhs.yaw && this->pitch == rhs.pitch &&
               this->roll == rhs.roll;
    }
    /// Inequality check.
    bool operator!=(EulerAngles rhs) const { return !(*this == rhs); }

    /// Convert the given quaternion to Euler angles.
    static EulerAngles quat2eul(Quaternion q) {

        // Source: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_Angles_Conversion

        // Normalize quaternion (q is passed by value, so this is a copy of q).
        q.normalize();

        float phi   = atan2(2.0 * (q.w * q.x + q.y * q.z),
                          1.0 - 2.0 * (q.x * q.x + q.y * q.y));
        float theta = asin(2.0 * (q.w * q.y - q.z * q.x));
        float psi   = atan2(2.0 * (q.w * q.z + q.x * q.y),
                          1.0 - 2.0 * (q.y * q.y + q.z * q.z));
        return {psi, theta, phi};
    }

    /// Convert the given Euler angles to a quaternion.
    static Quaternion eul2quat(EulerAngles eulerAngles) {

        // Source: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_Angles_to_Quaternion_Conversion

        float cy = std::cos(eulerAngles.yaw / 2);
        float sy = std::sin(eulerAngles.yaw / 2);
        float cp = std::cos(eulerAngles.pitch / 2);
        float sp = std::sin(eulerAngles.pitch / 2);
        float cr = std::cos(eulerAngles.roll / 2);
        float sr = std::sin(eulerAngles.roll / 2);

        return {
            cy * cp * cr + sy * sp * sr,
            cy * cp * sr - sy * sp * cr,
            sy * cp * sr + cy * sp * cr,
            sy * cp * cr - cy * sp * sr,
        };
    }
};

#include <math/Degrees.hpp>

/// Printing.
inline std::ostream &operator<<(std::ostream &os, EulerAngles e) {
    return os << "(" << rad2deg(e.yaw) << "°, " << rad2deg(e.pitch) << "°, "
              << rad2deg(e.roll) << "°)";
}
