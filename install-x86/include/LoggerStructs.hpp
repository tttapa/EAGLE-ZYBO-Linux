#pragma once

#include <Quaternion.hpp>

// This is an automatically generated struct, edit it in the code generator
/**
 * Estimate of the state of the drone's attitude, consisting of the drone's
 * orientation (1 quaternion q), angular velocity in rad/s (3 components: wx,
 * wy, wz) and the angular velocity of the torque motors in rad/s (3 components:
 * nx, ny, nz).
 */
struct AttitudeState {
    AttitudeState() = default;
    AttitudeState(Quaternion q, Vec3f w, Vec3f n) : q{q}, w{w}, n{n} {}
    
    /**
     * The orientation of the drone
     */
    Quaternion q = Quaternion::unit();

    /**
     * The angular velocity of the drone
     */
    Vec3f w = {0.0, 0.0, 0.0};

    /**
     * The angular velocity of the motors
     */
    Vec3f n = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * PWM control signals sent to the torque motors (3 components: ux, uy, uz).
 */
struct AttitudeControlSignal {
    AttitudeControlSignal() = default;
    AttitudeControlSignal(Vec3f u) : u{u} {}
    
    /**
     * Motor signal.
     */
    Vec3f u = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Attitude reference to track, consisting of a single quaternion.
 */
struct AttitudeReference {
    AttitudeReference() = default;
    AttitudeReference(Quaternion q) : q{q} {}
    
    /**
     * Reference orientation.
     */
    Quaternion q = Quaternion::unit();
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Estimate of the state of the drone's altitude, consisting three components.
 * First is a float representing the marginal angular velocity of the 'common
 * motor', relative to the hovering angular velocity. This value is measured in
 * rad/s. Next is a float representing the height of the drone, measured in
 * meters. Finally is a float representing the vertical velocity of the drone,
 * measured in m/s.
 */
struct AltitudeState {
    AltitudeState() = default;
    AltitudeState(real_t nt, real_t z, real_t vz) : nt{nt}, z{z}, vz{vz} {}
    
    /**
     * Common motor marginal angular velocity (rad/s)
     */
    real_t nt = 0.0;

    /**
     * The height (m) of the drone
     */
    real_t z = 0.0;

    /**
     * Velocity (m/s) of the drone
     */
    real_t vz = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Marginal PWM control signal sent to the common motor.
 */
struct AltitudeControlSignal {
    AltitudeControlSignal() = default;
    AltitudeControlSignal(real_t ut) : ut{ut} {}
    
    /**
     * Common motor marginal signal.
     */
    real_t ut = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Altitude reference height to track, consisting of a single float.
 */
struct AltitudeReference {
    AltitudeReference() = default;
    AltitudeReference(real_t z) : z{z} {}
    
    /**
     * Reference height.
     */
    real_t z = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Estimate of the state of the drone's position, consisting six components. The
 * first two floats are the quaternion components q1 and q2. The next two floats
 * represent the drone's global position, measured in meters. Finally, two
 * floats represent the horizontal velocity of the drone in m/s.
 */
struct PositionState {
    PositionState() = default;
    PositionState(Vec2f q, Vec2f p, Vec2f v) : q{q}, p{p}, v{v} {}
    
    /**
     * Orientation q1 and q2 component (/).
     */
    Vec2f q = {0.0, 0.0};

    /**
     * Position (x,y) in meters.
     */
    Vec2f p = {0.0, 0.0};

    /**
     * Velocity (m/s).
     */
    Vec2f v = {0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Reference quaternion components q1 and q2 that will be sent to the attitude
 * controller.
 */
struct PositionControlSignal {
    PositionControlSignal() = default;
    PositionControlSignal(Vec2f q) : q{q} {}
    
    /**
     * Reference orientation q1 and q2 component.
     */
    Vec2f q = {0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Position (x,y) reference to track, consisting of a position. This value is
 * measured in meters.
 */
struct PositionReference {
    PositionReference() = default;
    PositionReference(Vec2f p) : p{p} {}
    
    /**
     * Reference position.
     */
    Vec2f p = {0.0, 0.0};
};

// This is an automatically generated enum, edit it in the code generator
/**
 * Struct containing the four different flight modes. First, the drone begins in
 * the UNINITIALIZED flight mode. After the first cycle, the drone will enter
 * the current flight mode, as specified by the RC. In MANUAL_MODE, the pilot
 * has full control over the drone's orientation and the common thrust. In
 * ALTITUDE_HOLD_MODE, the pilot still has control over the  drone's
 * orientation, but the altitude controller takes over the common thrust in
 * order to keep the drone at a constant height. Finally, in AUTONOMOUS_MODE,
 * the pilot has no control over the attitude or altitude of the drone. The
 * drone will navigate autonomously to successive QR codes and land at its final
 * code. As a safety precaution, if the pilot sets the throttle to zero during
 * the autonomous flight, the drone will land at its current location.
 */
enum class FlightMode : int32_t {
    /**
     * The drone is in its first clock cycle and has not yet entered a flight
     * mode.
     */
    UNINITIALIZED = 0,

    /**
     * The drone is in "manual mode". The pilot has control over the drone's
     * orientation and the common thrust.
     */
    MANUAL = 1,

    /**
     * The drone is in "altitude-hold mode". The pilot has control over drone's
     * orientation, but the altitude controller takes over the common thrust in
     * order to keep the drone at a constant altitude.
     */
    ALTITUDE_HOLD = 2,

    /**
     * The drone is in "autonomous mode". The pilot has no control over the
     * attitude or altitude of the drone. If the drone is grounded when entering
     * this flight mode, then it will take off as soon as the pilot raises the
     * throttle above the predetermined threshold (see Autonomous.hpp). If the
     * drone was already airborne when entering this flight mode, then this step
     * will be skipped. Then, the drone will loiter at its current position for
     * a predetermined time (see Autonomous.hpp). After that, it will navigate
     * autonomously to successive QR codes and finally land at its final code.
     * As a safety precaution, if the pilot sets the throttle to zero during the
     * autonomous flight, the drone will land at its current location.
     */
    AUTONOMOUS = 3,
};
