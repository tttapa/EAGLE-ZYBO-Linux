#pragma once

#include <cstdint>

using bool32 = uint32_t;

enum class QRFSMState : int32_t {
    IDLE            = 0,
    QR_READ         = 1,
    QR_READING_BUSY = 2,
    CRYPTO_BUSY     = 3,
    NEW_TARGET      = 4,
    LAND            = 5,
    QR_UNKNOWN      = 6,
    ERROR           = 7,
};

enum class FlightMode : int32_t {
    MANUAL        = 0,
    ALTITUDE_HOLD = 1,
    AUTONOMOUS    = 2,
};

constexpr uintptr_t SHARED_MEM_START_ADDRESS = 0xFFFF0000;
constexpr uintptr_t SHARED_MEM_LAST_ADDRESS  = 0xFFFFFFFF;

constexpr uintptr_t BAREMETAL_COM_ADDRESS = SHARED_MEM_START_ADDRESS + 0x1000;

struct __attribute((packed)) BaremetalCommStruct {
    bool32 psi_lock;
    bool32 log_request;

    FlightMode mode;
    bool32 init;
    bool32 inductive;

    bool32 land;
    float positionX, positionY;
    float yawAngle;

    QRFSMState qrState;
    float targetX, targetY;
};