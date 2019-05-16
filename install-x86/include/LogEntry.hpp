#pragma once

#include <LoggerStructs.hpp>

/**
 * This is the struct for logging data between the ZYBO Baremetal ANC controller
 * and the computer.
 *
 * @note    For simplicity, we'll assume that the byte order and padding of both
 *          machines is the same.
 */
struct LogEntry {
  // private:
  //   size_t size;
  //
  // public:
  //   LogEntry() : size(sizeof(LogEntry)) {}

    AltitudeControlSignal altitudeControlSignal;
    AltitudeIntegralWindup altitudeIntegralWindup;
    AltitudeMeasurement altitudeMeasurement;
    AltitudeReference altitudeReference;
    AltitudeState altitudeStateEstimate;
    AttitudeControlSignal attitudeControlSignal;
    AttitudeIntegralWindup attitudeIntegralWindup;
    AttitudeMeasurement attitudeMeasurement;
    AttitudeReference attitudeReference;
    AttitudeState attitudeStateEstimate;
    PositionControlSignal positionControlSignal;
    PositionIntegralWindup positionIntegralWindup;
    PositionMeasurement referenceLocation;
    PositionReference positionReference;
    PositionState positionStateEstimate;
    AutonomousOutput autonomousOutput;
    AccelMeasurement accelMeasurement;
    GyroMeasurement gyroMeasurement;
    IMUMeasurement imuMeasurement;
    RCInput rcInput;
    BuzzerInstruction buzzerInstruction;
    LEDInstruction ledInstruction;
    MotorSignals motorSignals;
    /**
     * The flight mode of the drone.
     */
    FlightMode flightMode;
    /**
     * The flight mode of the drone.
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
     * Time since startup in seconds, calculated using getTickCount().
     */
    real_t time;
};
