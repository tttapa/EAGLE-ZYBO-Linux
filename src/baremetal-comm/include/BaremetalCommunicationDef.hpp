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
    MANUAL        = 0,
    ALTITUDE_HOLD = 1,
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
    mutable QRFSMState qrState;
    Position target;

  public:
    constexpr static uintptr_t address = VisionCommStruct::nextFreeAddress();

    QRFSMState getQRState() const volatile { return qrState; }

#ifndef BAREMETAL
    void setTargetPosition(Position target) volatile {
        checkInitialized();
        if (getQRState() == QRFSMState::NEW_TARGET)
            throw std::runtime_error("Error: illegal setTargetPosition call: "
                                     "Baremetal not yet done reading");
        this->target = target;
        qrState      = QRFSMState::NEW_TARGET;
    }
    void setTargetPosition(float x, float y) volatile {
        setTargetPosition({x, y});
    }

    void setQRStateBusy() volatile {
        if (getQRState() != QRFSMState::QR_READ_REQUEST)
            throw std::runtime_error("Error: illegal QR state transition: "
                                     "Only QR_READ_REQUEST → QR_READING_BUSY "
                                     "is allowed");
        qrState = QRFSMState::QR_READING_BUSY;
    }
    void setQRStateError() volatile { qrState = QRFSMState::ERROR; }
    void setQRStateUnkown() volatile { qrState = QRFSMState::QR_UNKNOWN; }
    void setQRStateLand() volatile { qrState = QRFSMState::LAND; }
#else
    Position getTargetPosition() const volatile {
        if (getQRState() != QRFSMState::NEW_TARGET)
            throw std::runtime_error("Error: illegal getTargetPosition call: "
                                     "No new target available");
        Position target = this->target;
        qrState         = QRFSMState::IDLE;
        return target;
    }
#endif
};

using AccessControlledLogEntry =
    AccessControlledSharedStruct<LogEntry, Baremetal2Linux,
                                 SHARED_MEM_START_ADDRESS + 0x1000>;