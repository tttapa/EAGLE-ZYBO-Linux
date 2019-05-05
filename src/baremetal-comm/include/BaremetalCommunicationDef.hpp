#pragma once

#include <LogEntry.h>
#include <SharedStruct.hpp>
#include <cassert>
#include <cmath>  // NAN
#include <ostream>

/**
 * @brief   The states of the QR/Crypto state machine.
 */
enum class QRFSMState : int32_t {
    /// Not doing anything
    IDLE = 0,
    /// ANC requests a QR code read
    QR_READ_REQUEST = 1,
    /// The QR reader is busy reading
    QR_READING_BUSY = 2,
    /// Crypto read a new target
    NEW_TARGET = 3,
    /// Crypto read a land command
    LAND = 4,
    /// Crypto read an unknown QR code
    QR_UNKNOWN = 5,
    /// An error has occurred
    ERROR = -1,
};

/**
 * @brief   The different flight modes of the drone.
 */
enum class FlightMode : int32_t {
    /// Flying using just the attitude controller
    MANUAL        = 0,
    /// Flying using the attitude and altitude controllers
    ALTITUDE_HOLD = 1,
    /// Flying autonomously, using the camera as well
    AUTONOMOUS    = 2,
};

/**
 * @brief   A struct for x and y coordinates of vision/QR positions.
 */
struct Position {
    float x = NAN, y = NAN;
    explicit operator bool() const volatile {
        return !(std::isnan(x) || std::isnan(y));
    }
    Position() = default;
    Position(float x, float y) : x{x}, y{y} {}
    Position(const volatile Position &p) : x{p.x}, y{p.y} {}
    Position(const Position &p) : x{p.x}, y{p.y} {}
    void operator=(const Position &p) volatile {
        this->x = p.x;
        this->y = p.y;
    }
};

inline std::ostream &operator<<(std::ostream &os, Position pos) {
    return os << "(" << pos.x << ", " << pos.y << ")";
}

/**
 * @brief   The data format sent from Vision to ANC.
 */
struct VisionData {
    Position position;
    double yawAngle;
};

/**
 * @brief   The struct for communication between vision and ANC.
 */
using VisionCommStruct =
    AccessControlledSharedStruct<VisionData, Linux2Baremetal,
                                 SHARED_MEM_START_ADDRESS>;

/**
 * @brief   The struct for communication between QR/Crypto and ANC.
 */
struct QRCommStruct : SharedStruct<QRCommStruct> {
  private:
    mutable QRFSMState qrState = QRFSMState::IDLE;
    Position target;

  public:
    constexpr static uintptr_t address = VisionCommStruct::nextFreeAddress();

    /**
     * @brief   Get the current state of the QR/Crypto FSM.
     * 
     * @return  QRFSMState 
     *          The state of the QR/Crypto FSM.
     */
    QRFSMState getQRState() const volatile { return qrState; }

#ifndef BAREMETAL
    /**
     * @brief   Set the new target position, and change the state to NEW_TARGET.
     * 
     * @param   target 
     *          The new target position.
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     * @throws  std::runtime_error
     *          Baremetal has not yet finished reading the previous target 
     *          position.
     */
    void setTargetPosition(Position target) volatile {
        checkInitialized();
        if (getQRState() == QRFSMState::NEW_TARGET)
            throw std::runtime_error("Error: illegal setTargetPosition call: "
                                     "Baremetal not yet done reading");
        this->target = target;
        qrState      = QRFSMState::NEW_TARGET;
    }
    /// @see    setTargetPosition(Position)
    void setTargetPosition(float x, float y) volatile {
        setTargetPosition({x, y});
    }

    /**
     * @brief   Change the state to QR_READING_BUSY.
     * 
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     * @throws  std::logic_error
     *          The current state is not QR_READ_REQUEST.
     */
    void setQRStateBusy() volatile {
        checkInitialized();
        if (getQRState() != QRFSMState::QR_READ_REQUEST)
            throw std::logic_error("Error: illegal QR state transition: "
                                   "Only QR_READ_REQUEST → QR_READING_BUSY "
                                   "is allowed");
        qrState = QRFSMState::QR_READING_BUSY;
    }
    /** 
     * @brief   Change the state to ERROR.
     * 
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     */
    void setQRStateError() volatile {
        checkInitialized();
        qrState = QRFSMState::ERROR;
    }
    /**
     * @brief   Change the state to QR_UNKNOWN.
     * 
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     */
    void setQRStateUnkown() volatile {
        checkInitialized();
        qrState = QRFSMState::QR_UNKNOWN;
    }
    /** 
     * @brief   Change the state to LAND.
     * 
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     */
    void setQRStateLand() volatile {
        checkInitialized();
        qrState = QRFSMState::LAND;
    }
#else
    /**
     * @brief   Get the new target position, and change the state to IDLE.
     * 
     * @return  The new target position.
     * 
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     * @throws  std::logic_error
     *          No new target is available.
     */
    Position getTargetPosition() const volatile {
        checkInitialized();
        if (getQRState() != QRFSMState::NEW_TARGET)
            throw std::logic_error("Error: illegal getTargetPosition call: "
                                   "No new target available");
        Position target = this->target;
        qrState         = QRFSMState::IDLE;
        return target;
    }

    /**
     * @brief   Request read a new QR code.
     * 
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     */
    void setQRStateRequest() volatile {
        checkInitialized();
        if (getQRState() != QRFSMState::IDLE &&
            getQRState() != QRFSMState::ERROR)
            throw std::logic_error("Error: cannot request new QR code: "
                                   "QR state is not idle or error");
        qrState = QRFSMState::QR_READ_REQUEST;
    }

    /** 
     * @brief   Change the state to IDLE.
     *
     * @throws  std::runtime_error
     *          The communication struct is not initialized yet.
     */
    void setQRStateIdle() volatile {
        checkInitialized();
        qrState = QRFSMState::IDLE;
    }

    // TODO: should I check all FSM transitions?
#endif
};

/**
 * @brief   The struct for communicating logging data from Baremetal to Linux.
 */
using AccessControlledLogEntry =
    AccessControlledSharedStruct<LogEntry, Baremetal2Linux,
                                 SHARED_MEM_START_ADDRESS + 0x1000>;