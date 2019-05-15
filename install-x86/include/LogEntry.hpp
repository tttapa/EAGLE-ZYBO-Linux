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
  private:
    size_t size;

  public:
    LogEntry() : size(sizeof(LogEntry)) {}

    AttitudeState attitudeObserverState;
    AttitudeControlSignal attitudeControlSignals;
    AttitudeReference referenceOrientation;
    AltitudeState altitudeObserverState;
    AltitudeControlSignal altitudeControlSignal;
    AltitudeReference referenceHeight;
    PositionState navigationObserverState;
    PositionControlSignal positionControlSignal;
    /**
     * The reference for the position controller
     */
    PositionReference referenceLocation;
    /**
     * The flight mode of the drone
     */
    int32_t mode;
};
