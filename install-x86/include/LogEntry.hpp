#pragma once

#include <LoggerStructs.hpp>

/**
 * This is the struct for logging data between the ZYBO Baremetal ANC controller
 * and the computer.
 *
 * @note    For simplicity, we'll assume that the byte order and padding of both
 *          machines is the same.
 */
struct __attribute__((packed)) LogEntry {
    LogEntry() = default;

    /**
     * The altitude controller's control signal.
     */
    AltitudeControlSignal altitudeControlSignal;
    /**
     * The altitude controller's integral windup.
     */
    AltitudeIntegralWindup altitudeIntegralWindup;
    /**
     * The altitude controller's measurement.
     */
    AltitudeMeasurement altitudeMeasurement;
    /**
     * The altitude controller's reference.
     */
    AltitudeReference altitudeReference;
    /**
     * The altitude controller's state estimate.
     */
    AltitudeState altitudeStateEstimate;
    /**
     * The attitude controller's control signal.
     */
    AttitudeControlSignal attitudeControlSignal;
    /**
     * The attitude controller's integral windup.
     */
    AttitudeIntegralWindup attitudeIntegralWindup;
    /**
     * The attitude controller's measurement.
     */
    AttitudeMeasurement attitudeMeasurement;
    /**
     * The attitude controller's reference.
     */
    AttitudeReference attitudeReference;
    /**
     * The attitude controller's state estimate.
     */
    AttitudeState attitudeStateEstimate;
    /**
     * The position controller's control signal.
     */
    PositionControlSignal positionControlSignal;
    /**
     * The position controller's integral windup.
     */
    PositionIntegralWindup positionIntegralWindup;
    /**
     * The position controller's measurement.
     */
    PositionMeasurement positionMeasurement;
    /**
     * The position controller's reference.
     */
    PositionReference positionReference;
    /**
     * The position controller's state estimate.
     */
    PositionState positionStateEstimate;
    /**
     * The autonomous controller's output.
     */
    AutonomousOutput autonomousOutput;


    /**
     * The IMU's measurement.
     */
    IMUMeasurement imuMeasurement;
    /**
     * The most recent reading of the RC.
     */
    RCInput rcInput;
    /**
     * The current instruction playing on the buzzer.
     */
    BuzzerInstruction buzzerInstruction;
    /**
     * The current instruction displayed on the LEDs.
     */
    LEDInstruction ledInstruction;
    /**
     * The signal sent to the four motors (ESCs).
     */
    MotorSignals motorSignals;
    /**
     * The flight mode of the drone.
     */
    FlightMode flightMode;
    /**
     * The WPT mode of the drone.
     */
    WPTMode wptMode;
    /**
     * Time since startup in milliseconds.
     */
    uint64_t millis;
    /**
     * IMU tick count since startup.
     */
    uint32_t tickCount;
    /**
     * Height measurement from the sonar in meters (tilt corrected).
     */
    real_t sensorHeightMeasurement;
    /**
     * Position measurement from IMP in meters (tilt corrected).
     */
    Vec2f sensorPositionMeasurement;
    /**
     * Yaw measurement from IMP in radians.
     */
    real_t sensorYawMeasurement;
    /**
     * Pitch bias from the RC
     */
    float pitchBias;
    /**
     * Roll bias from the RC
     */
    float rollBias;
    /**
     * Thrust bias from the RC (hovering thrust)
     */
    float thrustBias;
    /**
     * Hovering thrust saved between flights for autonomous controller
     */
    float autonomousHoveringThrust;
    /**
     * Time since startup in seconds, calculated using getTickCount().
     */
    real_t time;
};
