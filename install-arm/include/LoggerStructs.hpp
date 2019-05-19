#pragma once

#include <cstdint>
#include <Quaternion.hpp>
#include <real_t.h>

// This is an automatically generated enum, edit it in the code generator
/**
 * Enumeration containing the four different flight modes. First, the drone
 * begins in the UNINITIALIZED flight mode. After the first cycle, the drone
 * will enter the current flight mode, as specified by the RC. In MANUAL_MODE,
 * the pilot has full control over the drone's orientation and the common
 * thrust. In ALTITUDE_HOLD_MODE, the pilot still has control over the drone's
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
     * The drone is in MANUAL mode. The pilot has control over the drone's
     * orientation and the common thrust.
     */
    MANUAL = 1,

    /**
     * The drone is in ALTITUDE-HOLD mode. The pilot has control over drone's
     * orientation, but the altitude controller takes over the common thrust in
     * order to keep the drone at a constant altitude.
     */
    ALTITUDE_HOLD = 2,

    /**
     * The drone is in AUTONOMOUS mode. The pilot has no control over the
     * attitude or altitude of the drone. If the drone is grounded when entering
     * this flight mode, then it will take off as soon as the pilot raises the
     * throttle above 50%. If the drone was already airborne when entering this
     * flight mode, then this step will be skipped. Then, the drone will loiter
     * at its current position for 15 seconds. After that,it will navigate
     * autonomously to successive QR codes and finally land at its final code.
     * As a safety precaution, if the pilot sets the throttle to zero during the
     * autonomous flight, the drone will land at its current location. See
     * Autonomous.hpp for more details.
     */
    AUTONOMOUS = 3,
};

// This is an automatically generated enum, edit it in the code generator
/**
 * Enumeration containing the two states of wireless power transfer, namely OFF
 * and ON. However, wireless power transfer cannot be initiated at any time.
 * Only if the drone is in MANUAL mode or if it is grounded in AUTONOMOUS mode
 * with zero thrust.
 */
enum class WPTMode : int32_t {
    /**
     * Wireless power transfer (WPT) is off.
     */
    OFF = 0,

    /**
     * Wireless power transfer (WPT) is on.
     */
    ON = 1,
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Marginal PWM control signal sent to the 'common motor' (float).
 */
struct __attribute__((packed)) AltitudeControlSignal {
    AltitudeControlSignal() = default;
    AltitudeControlSignal(real_t ut) : ut{ut} {}
    
    /**
     * 'Common motor' marginal signal.
     */
    real_t ut = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Integral of the error of the height of the drone in meters.
 */
struct __attribute__((packed)) AltitudeIntegralWindup {
    AltitudeIntegralWindup() = default;
    AltitudeIntegralWindup(real_t z) : z{z} {}
    
    /**
     * Integral of the height estimate in meters.
     */
    real_t z = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Measurement of the drone height by the sonar in meters, corrected for the
 * tilt of the drone, represented by a float.
 */
struct __attribute__((packed)) AltitudeMeasurement {
    AltitudeMeasurement() = default;
    AltitudeMeasurement(real_t z) : z{z} {}
    
    /**
     * Measurement height in meters.
     */
    real_t z = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Reference height to track in meters, represented by a float.
 */
struct __attribute__((packed)) AltitudeReference {
    AltitudeReference() = default;
    AltitudeReference(real_t z) : z{z} {}
    
    /**
     * Reference height in meters.
     */
    real_t z = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Estimate of the altitude state, consisting of the marginal angular velocity
 * of the 'common motor' in rad/s (float), the height of the drone in meters
 * (float) and the vertical velocity of the drone in m/s (float).
 */
struct __attribute__((packed)) AltitudeState {
    AltitudeState() = default;
    AltitudeState(real_t nt, real_t z, real_t vz) : nt{nt}, z{z}, vz{vz} {}
    
    /**
     * 'Common motor' marginal angular velocity in rad/s.
     */
    real_t nt = 0.0;

    /**
     * Height of the drone in meters.
     */
    real_t z = 0.0;

    /**
     * Vertical velocity of the drone in m/s.
     */
    real_t vz = 0.0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * PWM control signals sent to the 'torque motors', consisting of a Vec3f. Each
 * component will be sent to a different 'torque motor'.
 */
struct __attribute__((packed)) AttitudeControlSignal {
    AttitudeControlSignal() = default;
    AttitudeControlSignal(Vec3f uxyz) : uxyz{uxyz} {}
    
    /**
     * Signal sent to the 'torque motors'.
     */
    Vec3f uxyz = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Integral of the error of the quaternion components q1, q2 and q3, represented
 * by a Vec3f.
 */
struct __attribute__((packed)) AttitudeIntegralWindup {
    AttitudeIntegralWindup() = default;
    AttitudeIntegralWindup(Vec3f q123) : q123{q123} {}
    
    /**
     * Integral of the error of the quaternion components q1 q2 and q3.
     */
    Vec3f q123 = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Measurement from the IMU, consisting of a quaternion for the drone's
 * orientation and a Vec3f for the drone's angular velocity, measured in rad/s.
 */
struct __attribute__((packed)) AttitudeMeasurement {
    AttitudeMeasurement() = default;
    AttitudeMeasurement(Quaternion q, Vec3f w) : q{q}, w{w} {}
    
    /**
     * Measurement orientation from the IMU.
     */
    Quaternion q = Quaternion::identity();

    /**
     * Measurement angular velocity from the IMU.
     */
    Vec3f w = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Reference orientation to track, represented by a quaternion.
 */
struct __attribute__((packed)) AttitudeReference {
    AttitudeReference() = default;
    AttitudeReference(Quaternion q) : q{q} {}
    
    /**
     * Reference orientation.
     */
    Quaternion q = Quaternion::identity();
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Estimate of the attitude state, consisting of the drone's orientation
 * (quaterion), angular velocity in rad/s (Vec3f) and the angular velocity of
 * the 'torque motors' in rad/s (Vec3f).
 */
struct __attribute__((packed)) AttitudeState {
    AttitudeState() = default;
    AttitudeState(Quaternion q, Vec3f w, Vec3f n) : q{q}, w{w}, n{n} {}
    
    /**
     * The orientation of the drone.
     */
    Quaternion q = Quaternion::identity();

    /**
     * The angular velocity of the drone in rad/s.
     */
    Vec3f w = {0.0, 0.0, 0.0};

    /**
     * The angular velocity of the 'torque motors' in rad/s.
     */
    Vec3f n = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Reference quaternion components q1 and q2 that will be sent to the attitude
 * controller, represented by a Vec2f.
 */
struct __attribute__((packed)) PositionControlSignal {
    PositionControlSignal() = default;
    PositionControlSignal(Vec2f q12) : q12{q12} {}
    
    /**
     * Reference orientation, q1 and q2 components.
     */
    Vec2f q12 = {0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Integral of the error of the position in meters, represented by a Vec2f.
 */
struct __attribute__((packed)) PositionIntegralWindup {
    PositionIntegralWindup() = default;
    PositionIntegralWindup(Vec2f p) : p{p} {}
    
    /**
     * Integral of the position in meters.
     */
    Vec2f p = {0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Measurement of the position in meters, represented by a Vec2f.
 */
struct __attribute__((packed)) PositionMeasurement {
    PositionMeasurement() = default;
    PositionMeasurement(Vec2f p) : p{p} {}
    
    /**
     * Measurement position in meters.
     */
    Vec2f p = {0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Reference position to track in meters, represented by a Vec2f.
 */
struct __attribute__((packed)) PositionReference {
    PositionReference() = default;
    PositionReference(Vec2f p) : p{p} {}
    
    /**
     * Reference position in meters.
     */
    Vec2f p = {0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Estimate of the position state, consisting of the quaternion components q1
 * and q2 (Vec2f), the global position of the drone in meters (Vec2f) and the
 * horizontal velocity of the drone in m/s (Vec2f).
 */
struct __attribute__((packed)) PositionState {
    PositionState() = default;
    PositionState(Vec2f q, Vec2f p, Vec2f v) : q{q}, p{p}, v{v} {}
    
    /**
     * Drone orientation, q1 and q2 components.
     */
    Vec2f q = {0.0, 0.0};

    /**
     * Position of the drone in meters.
     */
    Vec2f p = {0.0, 0.0};

    /**
     * Horizontal velocity of the drone in m/s.
     */
    Vec2f v = {0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Output of the autonomous control system, which contains information about
 * whether the altitude/position controllers should be used and whether their
 * observers should be updated. If the controller(s) should be bypassed, the
 * struct also contains signals to pass to the next part of the code. Finally,
 * if the position controller should be updated, should it use IMP's measurement
 * or data from the accelerometer to determine the position of the drone?
 */
struct __attribute__((packed)) AutonomousOutput {
    AutonomousOutput() = default;
    AutonomousOutput(bool useAltitudeController, AltitudeReference referenceHeight, AltitudeControlSignal commonThrust, bool updateAltitudeObserver, bool usePositionController, PositionReference referencePosition, PositionControlSignal q12ref, bool updatePositionObserver, bool trustIMPForPosition) : useAltitudeController{useAltitudeController}, referenceHeight{referenceHeight}, commonThrust{commonThrust}, updateAltitudeObserver{updateAltitudeObserver}, usePositionController{usePositionController}, referencePosition{referencePosition}, q12ref{q12ref}, updatePositionObserver{updatePositionObserver}, trustIMPForPosition{trustIMPForPosition} {}
    
    /**
     * Whether the altitude controller should be used. If this is false, then
     * this AutonomousOutput's commonThrust should be used instead of the
     * altitude controller's common thrust.
     */
    bool useAltitudeController = false;

    /**
     * Reference height to be sent to the altitude controller, if it is not
     * bypassed.
     */
    AltitudeReference referenceHeight = {};

    /**
     * Control signal to send to the 'common motor' if the altitude controller
     * is bypassed.
     */
    AltitudeControlSignal commonThrust = {};

    /**
     * Whether the altitude observer should be updated.
     */
    bool updateAltitudeObserver = false;

    /**
     * Whether the position controller should be used. If this is false, then
     * this AutonomousOutput's reference orientation should be used instead of
     * the position controller's reference orientation.
     */
    bool usePositionController = false;

    /**
     * Reference position to be sent to the position controller, if it is not
     * bypassed.
     */
    PositionReference referencePosition = {};

    /**
     * Control signal to send to the attitude controller, if the position
     * controller is bypassed.
     */
    PositionControlSignal q12ref = {};

    /**
     * Whether the position observer should be updated.
     */
    bool updatePositionObserver = false;

    /**
     * If this is true, then the drone should trust IMP's measurement to
     * determine the position of the drone. Otherwise, it will use the
     * accelerometer's ax and ay data to determine the position, according to
     * the mathematical model.
     */
    bool trustIMPForPosition = false;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Accelerometer measurement acceleration in g (Vec3f).
 */
struct __attribute__((packed)) AccelMeasurement {
    AccelMeasurement() = default;
    AccelMeasurement(Vec3f a) : a{a} {}
    
    /**
     * Accelerometer measurement of the acceleration in g.
     */
    Vec3f a = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Gyroscope measurement angular velocity in rad/s (Vec3f).
 */
struct __attribute__((packed)) GyroMeasurement {
    GyroMeasurement() = default;
    GyroMeasurement(Vec3f g) : g{g} {}
    
    /**
     * Gyroscope measurement of the angular velocity in rad/s.
     */
    Vec3f g = {0.0, 0.0, 0.0};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * IMU measurement angular velocity in rad/s (Vec3f) and acceleration in g
 * (Vec3f).
 */
struct __attribute__((packed)) IMUMeasurement {
    IMUMeasurement() = default;
    IMUMeasurement(GyroMeasurement gyro, AccelMeasurement accel) : gyro{gyro}, accel{accel} {}
    
    /**
     * Gyroscope measurement of the angular velocity in rad/s.
     */
    GyroMeasurement gyro = {};

    /**
     * Accelerometer measurement of the acceleration in g.
     */
    AccelMeasurement accel = {};
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Struct containing the values from the RC transmitter. This includes the value
 * of the throttle, which ranges from 0 to 1. It also contains the values of the
 * roll, pitch, yaw and tuner knob, which range from -1 to +1. Lastly, there are
 * switches for the flight mode and the wireless power transfer. These are
 * represented by their respective enumerations.
 */
struct __attribute__((packed)) RCInput {
    RCInput() = default;
    RCInput(real_t throttle, real_t roll, real_t pitch, real_t yaw, real_t tuner, FlightMode flightMode, WPTMode wptMode) : throttle{throttle}, roll{roll}, pitch{pitch}, yaw{yaw}, tuner{tuner}, flightMode{flightMode}, wptMode{wptMode} {}
    
    /**
     * Value of the RC throttle in [0, 1].
     */
    real_t throttle = 0.0;

    /**
     * Value of the RC roll in [-1, 1].
     */
    real_t roll = 0.0;

    /**
     * Value of the RC pitch in [-1, 1].
     */
    real_t pitch = 0.0;

    /**
     * Value of the RC yaw in [-1, 1].
     */
    real_t yaw = 0.0;

    /**
     * Value of the RC tuner knob in [-1, 1].
     */
    real_t tuner = 0.0;

    /**
     * Value of the RC flight mode (as a FlightMode).
     */
    FlightMode flightMode = FlightMode::UNINITIALIZED;

    /**
     * Value of the RC WPT mode (as a WPTMode).
     */
    WPTMode wptMode = WPTMode::OFF;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Instruction to be sent to the buzzer containing a duration in seconds
 * (float), a buzzer period in ? (int) and a buzzer volume in ? (int).
 */
struct __attribute__((packed)) BuzzerInstruction {
    BuzzerInstruction() = default;
    BuzzerInstruction(real_t duration, int period, int volume) : duration{duration}, period{period}, volume{volume} {}
    
    /**
     * Duration of the instruction in seconds.
     */
    real_t duration = 0.0;

    /**
     * Period of sound, represented as an integer.
     */
    int period = 0;

    /**
     * Volume of sound, represented as an integer.
     */
    int volume = 0;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Instruction to be sent to the LEDs, containing four booleans, representing
 * whether each of the four LEDs should be lit.
 */
struct __attribute__((packed)) LEDInstruction {
    LEDInstruction() = default;
    LEDInstruction(bool led1, bool led2, bool led3, bool led4) : led1{led1}, led2{led2}, led3{led3}, led4{led4} {}
    
    /**
     * Whether the first LED should be lit.
     */
    bool led1 = false;

    /**
     * Whether the second LED should be lit.
     */
    bool led2 = false;

    /**
     * Whether the third LED should be lit.
     */
    bool led3 = false;

    /**
     * Whether the fourth LED should be lit.
     */
    bool led4 = false;
};

// This is an automatically generated struct, edit it in the code generator
/**
 * Four floats representing the duty cycles to be sent to the four motors
 * (front-left, front-right, back-left, back-right). These four must be in the
 * interval [0.0, 1.0].
 */
struct __attribute__((packed)) MotorSignals {
    MotorSignals() = default;
    MotorSignals(real_t v0, real_t v1, real_t v2, real_t v3) : v0{v0}, v1{v1}, v2{v2}, v3{v3} {}
    
    /**
     * Front-left motor duty cycle in [0.0, 1.0].
     */
    real_t v0 = 0.0;

    /**
     * Front-right motor duty cycle in [0.0, 1.0].
     */
    real_t v1 = 0.0;

    /**
     * Back-left motor duty cycle in [0.0, 1.0].
     */
    real_t v2 = 0.0;

    /**
     * Back-right motor duty cycle in [0.0, 1.0].
     */
    real_t v3 = 0.0;
};
